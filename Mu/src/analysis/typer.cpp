//
// Created by Andrew Bregger on 2019-06-07.
//
#include "interpreter.hpp"

#include "typer.hpp"


#include "typer_op_eval.hpp"


#define report(pos, msg, ...) { \
    interp->report_error(pos, msg, __VA_ARGS__); \
}

#define report_str(pos, msg) { \
    interp->report_error(pos, msg); \
}

using namespace mu::types;

extern Type* type_u8;
extern Type* type_u16;
extern Type* type_u32;
extern Type* type_u64;
extern Type* type_i8;
extern Type* type_i16;
extern Type* type_i32;
extern Type* type_i64;
extern Type* type_f32;
extern Type* type_f64;
extern Type* type_char;
extern Type* type_bool;
extern Type* type_unit;

// this is not implemented yet
extern Type* type_string;

namespace mu {

    Operand::Operand(mu::types::Type *type, ast::Expr *expr, AccessType access_type) : type(type),
        expr(expr), access(access_type) {
    }

    Operand::Operand(mu::types::Type *type, ast::Expr *expr, const Val &val) : type(type), expr(expr),
        val(val) {
    }

    Operand::Operand(ast::Expr* expr) : type(nullptr), expr(expr), error(true) {}

    Typer::Typer(Interpreter *interp) : interp(interp), current_scope(interp->get_prelude()) {

    }

    Module * Typer::resolve_main_module(ast::ModuleFile *main_module) {
        // this will become the executable name.
        auto name = main_module->get_name();

        auto module_scope = make_scope<ModuleScope>(name, main_module, current_scope);

        push_scope(module_scope.get());


        std::vector<Entity*> entities;
        for(auto& decl : *main_module)
            entities.emplace_back(build_top_level_entity(decl.get()));

        for(auto entity : entities)
            resolve_entity(entity);

        pop_scope();

//        auto main_module_type = interp->new_type<ModuleType>();

        return nullptr;
    }

    void Typer::add_entity(Entity *entity) {
        current_scope->insert(entity->get_name(), entity);
    }

    void Typer::push_scope(Scope *scope) {
        current_scope->add_child(scope);
        if(scope->get_parent() != current_scope)
            interp->fatal("Compiler Error: Pushing a scope where the parent isn't the active scope");
        current_scope = scope;
    }

    void Typer::pop_scope() {
        current_scope = current_scope->get_parent();
    }

    Entity *Typer::build_top_level_entity(ast::Decl *decl) {
        switch(decl->kind) {
            case ast::ast_global: {
                auto g = decl->as<ast::Global>();
                return interp->new_entity<Global>(g->name, current_scope, decl);
            }
            case ast::ast_global_mut: {
                auto g = decl->as<ast::GlobalMut>();
               return interp->new_entity<Global>(g->name, current_scope, decl);
            } break;
            case ast::ast_procedure: {
                auto p = decl->as<ast::Procedure>();
                return interp->new_entity<Function>(p->name, current_scope, decl);
            } break;
            case ast::ast_structure: {
                auto s = decl->as<ast::Structure>();
                return interp->new_entity<Type>(s->name, current_scope, decl);
            } break;
            case ast::ast_type: {
                auto s = decl->as<ast::Type>();
                return interp->new_entity<Type>(s->name, current_scope, decl);
            } break;
            case ast::ast_type_class: {
                auto s = decl->as<ast::TypeClass>();
                return interp->new_entity<Type>(s->name, current_scope, decl);
            } break;
            case ast::ast_use: {
                auto s = decl->as<ast::Use>();
                ast::Ident* name = nullptr;
                switch(s->use_path->kind) {
                    case ast::ast_use_path:
                    case ast::ast_use_path_list:
                    case ast::ast_use_path_alias:
                    default:
                        report_str(s->use_path->pos(), "Compiler Error: parser bug in use declaration");
                }
                interp->fatal("Use are not implemented at this moment");
                return interp->new_entity<Module>(name, current_scope, decl);
            } break;
            case ast::ast_alias: {
                auto a = decl->as<ast::Alias>();
                return interp->new_entity<Alias>(a->name, nullptr, current_scope, decl);
            } break;
            case ast::ast_impl:  {
                // think about how this should be handled.
                // maybe what I can do is search for the struct this impl block is associated with and
                // attach it so that it is resolved when the structure is.
            } break;
            default:
                report_str(decl->pos(), "invalid global declaration");
        }
    }

    Entity *Typer::resolve_entity(Entity *entity) {
        if(entity->is_resolved())
            return entity;

        if(entity->status() == Resolving) {
            report(entity->node()->pos(), "'%s' has ciclic dependency", entity->str().c_str());
        }

        entity->resolve(this);
    }

    Entity *Typer::resolve(Global *global) {
        return global;
    }

    Entity *Typer::resolve(Local *local) {
        return local;
    }

    Entity *Typer::resolve(Type *type) {
        return type;
    }

    Entity *Typer::resolve(Function *funct) {
        return funct;
    }

    Entity *Typer::resolve(Alias *alias) {
        return alias;
    }

    Entity *Typer::resolve(Constant *constant) {
        return constant;
    }

    Operand Typer::resolve_expr(ast::Expr *expr, types::Type *expected_type) {
        Operand result(expr);
        switch(expr->kind) {
            case ast::ast_name:
                result = resolve_name(expr);
                break;
            case ast::ast_integer:
            case ast::ast_fl:
            case ast::ast_bool:
            case ast::ast_ch:
            case ast::ast_str:
                result = resolve_literals(expr);
                break;
            case ast::ast_binary:
                result = resolve_binary(expr->as<ast::Binary>(), nullptr);
                break;
            case ast::ast_unary:
                result = resolve_unary(expr->as<ast::Unary>(), nullptr);
                break;
        }

        if(expected_type) {
            // check for compatibility of the resulting type and the expected type.
        }
        return result;
    }

    Operand Typer::resolve_binary(ast::Binary *expr, types::Type *expected_type) {
        // resolve sub expression
        auto lhs = resolve_expr(expr->lhs.get());
        auto rhs = resolve_expr(expr->rhs.get());

        // check for errors.
        if(lhs.error)
            return lhs;
        else if(rhs.error)
            return rhs;

        if(lhs.type->is_arithmetic() and lhs.type->is_primative()) {
            return resolve_arithmetic_binary(expr->op, lhs, rhs, expr, expected_type);
        }
        else if(lhs.type->is_ptr()) {

        }
        else return resolve_binary_overload(Tkn_Mut, rhs, lhs, nullptr, nullptr);
    }

    Operand Typer::resolve_arithmetic_binary(TokenKind op, Operand lhs, Operand rhs, ast::Expr *expr,
                                             types::Type *expected_type) {
        auto lhs_type = lhs.type;
        auto rhs_type = rhs.type;

        assert(lhs_type->is_arithmetic());

        bool invalid_lhs = lhs_type->is_bool() || lhs_type->is_char();
        if(lhs_type->is_ptr()) {
            switch(op) {
                // standard arithmitic
                case mu::Tkn_Plus:
                case mu::Tkn_Minus: {
                    if(rhs_type->is_integer())  {
                        // this should be checked to make sure it is still valid.
                        // *T + 1 -> *T
                        // *T - 1 -> *T
                        return Operand(lhs_type, expr, LValue);
                    }
                }
                default:
                    break;
            }
        }
        else if(!invalid_lhs and rhs_type->is_arithmetic() and rhs_type->is_primative()) {
            switch(op) {
                // standard arithmitic
                case mu::Tkn_Plus:
                case mu::Tkn_Minus:
                case mu::Tkn_Astrick:
                case mu::Tkn_Slash:
                case mu::Tkn_AstrickAstrick:
                    // translate this to a call to powf
                case mu::Tkn_Percent:
                    // translate this to a call to modf
                // equality operations
                case mu::Tkn_EqualEqual:
                case mu::Tkn_BangEqual:
                case mu::Tkn_LessEqual:
                case mu::Tkn_GreaterEqual:
                    if(lhs_type->kind() == rhs_type->kind()) {
                        return eval_binary_op(this, op, lhs, rhs, expr, lhs_type);
                    }
                    break;

                // boolean logic
                case mu::Tkn_And:
                case mu::Tkn_Or:
                    if(lhs_type->is_bool() and rhs_type->is_bool()) {
                        return eval_binary_op(this, op, lhs, rhs, expr, lhs_type);
                    }
                    break;
                // bitwise operations
                case mu::Tkn_LessLess:
                case mu::Tkn_GreaterGreater:
                case mu::Tkn_Ampersand:
                case mu::Tkn_Pipe:
                    if(lhs_type->is_integer() and rhs_type->is_integer()) {
                        if(lhs_type->kind() == rhs_type->kind()) {
                            return eval_binary_op(this, op, lhs, rhs, nullptr, lhs_type);
                        }
                    }
                    else {
                        report(expr->pos(), "bitwise operation '%s' must be an integer type, found '%s' and '%s'",
                            Token::get_string(op).c_str(),
                            lhs_type->str().c_str(),
                            rhs_type->str().c_str())
                        return Operand(expr);
                    }
                    break;
            }
        }
        report(expr->pos(), "invalid operands for binary operation '%s' with types: '%s' and '%s'",
               Token::get_string(op).c_str(),
               lhs_type->str().c_str(),
               rhs_type->str().c_str())
        return Operand(expr);
    }

    Operand Typer::resolve_binary_overload(TokenKind op, Operand rhs, Operand lhs, ast::Expr *expr,
                                           types::Type *expected_type) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_unary(ast::Unary *expr, types::Type *expected_type) {
        auto operand = resolve_expr(expr->expr.get(), nullptr);
        if(operand.type->is_ptr()) {
            switch(expr->op) {
                case mu::Tkn_Bang: {
                    return Operand(type_bool, expr, RValue);
                } break;
                case mu::Tkn_Ampersand: {
                    auto type = interp->new_type<types::Pointer>(operand.type);
                    // I think
                    return Operand(type, expr, LValue);
                } break;
                case mu::Tkn_Astrick: {
                    return Operand(operand.type->base_type(), expr, LValue);
                } break;
                default:
                    break;
            }
            report(expr->pos(), "invalid type for unary operation '%s' and type '%s'",
                    Token::get_string(expr->op).c_str(),
                    operand.type->str().c_str());
            return Operand(expr);
        }
        else if(operand.type->is_arithmetic()) {
            return resolve_arithmetic_unary(expr->op, operand, expr, expected_type);
        }
        else {
            return Operand(expr);
        }
    }

    Operand Typer::resolve_arithmetic_unary(TokenKind op, Operand operand, ast::Expr *expr, types::Type *expectd_type) {

        auto type = operand.type;
        bool invalid_type = type->is_char();

        if(!invalid_type and type->is_arithmetic()) {
            switch(op) {
                case mu::Tkn_Minus:
                    if(type->is_bool()) break;
                    else if(type->is_unsigned()) {
                        report(expr->pos(), "attempting to negate an unsigned type '%s'",
                                type->str().c_str());
                        interp->message("cast '%s' to signed type", type->str().c_str());
                    }
                    return eval_unary_op(this, op, operand, expr, type);
                case mu::Tkn_Tilde:
                    if(type->is_integer())
                        return eval_unary_op(this, op, operand, expr, type);
                    break;
                case mu::Tkn_Bang:
                    if(type->is_bool() or type->is_integer() or type->is_float())
                        return eval_unary_op(this, op, operand, expr, type);
                    break;
                case mu::Tkn_Ampersand:
                    if(operand.val.is_constant) {
                        report_str(operand.expr->pos(), "unable to take the address of a literal");
                        return Operand(expr);
                    }
                case mu::Tkn_Astrick:
                    report(operand.expr->pos(), "attempting to dereference a non-pointer type '%s'",
                            type->str().c_str());
                    return Operand(expr);
                default:
                    break;
            }
        }
        else {
            report(expr->pos(), "invalid type for unary operation '%s' and type '%s'",
                   Token::get_string(op).c_str(),
                   operand.type->str().c_str());
            return Operand(expr);
        }
    }

    Operand Typer::resolve_unary_overload(TokenKind op, Operand operand, ast::Expr *expr, types::Type *expected_type) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_literals(ast::Expr *expr) {
        switch(expr->kind) {
            case ast::ast_integer: {
                // defaults to i32 unless larger
                auto integer = expr->as<ast::Integer>();
                if(integer->value < MAX_I32)
                    return Operand(type_i32, expr, Val((i32) integer->value));
                else
                    return Operand(type_i64, expr, Val((i64) integer->value));
            }
            case ast::ast_fl: {
                // defaults to f32
                auto fl = expr->as<ast::Float>();
                if(fl->value < MAX_F32)
                    return Operand(type_f32, expr, Val((f32) fl->value));
                else
                    return Operand(type_f64, expr, Val(fl->value));
            }
            case ast::ast_bool: {
                auto boolean = expr->as<ast::Bool>();
                return Operand(type_bool, expr, Val(boolean->value));
            }
            case ast::ast_ch: {
                auto ch = expr->as<ast::Char>();
                return Operand(type_char, expr, Val(ch->value));
            }
            case ast::ast_str: {
                report_str(expr->pos(), "string literals are not implemented at this time");
            }
            default:
                report_str(expr->pos(), "Compiler Error: attempting to resolve a non-literal expression as literal");
        }
    }

    Operand Typer::resolve_name(ast::Expr *expr) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_name_expr(ast::Name *expr) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_name_generic_expr(ast::NameGeneric *expr) {
        return Operand(nullptr, nullptr, LValue);
    }
}