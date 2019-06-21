//
// Created by Andrew Bregger on 2019-06-07.
//
#include "interpreter.hpp"

#include "typer.hpp"


#include "typer_op_eval.hpp"


#define report(pos, msg, ...) { \
    interp->report_error(pos, msg, __VA_ARGS__); \
    increment_error(); \
}

#define report_str(pos, msg) { \
    interp->report_error(pos, msg); \
    increment_error(); \
}

// these are not well defined. they owuld only be used
// once in a single scope.
#define push_context_state(var, new_state) \
    auto _old_##var = context.var; \
    context.var = new_state;


#define pop_context_state(var) context.var = _old_##var;



extern mu::types::Type* type_u8;
extern mu::types::Type* type_u16;
extern mu::types::Type* type_u32;
extern mu::types::Type* type_u64;
extern mu::types::Type* type_i8;
extern mu::types::Type* type_i16;
extern mu::types::Type* type_i32;
extern mu::types::Type* type_i64;
extern mu::types::Type* type_f32;
extern mu::types::Type* type_f64;
extern mu::types::Type* type_char;
extern mu::types::Type* type_bool;
extern mu::types::Type* type_unit;

// this is not implemented yet
extern mu::types::Type* type_string;

namespace mu {

    Operand::Operand(mu::types::Type *type, ast::Expr *expr, AccessType access_type) : type(type),
        expr(expr), access(access_type) {
    }

    Operand::Operand(mu::types::Type *type, ast::Expr *expr, const Val &val) : type(type), expr(expr),
        val(val) {
    }

    Operand::Operand(ast::Expr* expr) : type(nullptr), expr(expr), error(true) {}

    Typer::Typer(Interpreter *interp) : interp(interp), prelude(interp->get_prelude()) {
    }

    void Typer::increment_error() {
        errors_num++;
    }

    ScopePtr Typer::active_scope() {
        return context.current_scope;
    }

    AddressType Typer::get_addressing_by_type(types::Type* type) {
        if(type->is_array() || type->is_ptr() || type->is_ref() || type->is_function()) 
            return AddressType::Reference;
        else
            return AddressType::Value;
    }

    bool Typer::is_redeclaration(ast::Ident* name) {
        auto entity = search_scope(active_scope(), name);
        return entity != nullptr;
    }


    Entity* Typer::search_active_scope(ast::Ident* name) {
        auto e = search_scope(active_scope(), name);
        if(e) {
            return e;
        }
        else {
            e = search_scope(prelude, name);
            if(e)
                return e;

            report(name->pos, "unable to find name '%s'", name->val->value.c_str())
            return nullptr;
        }
    }

    Entity* Typer::search_scope(ScopePtr  scope, ast::Ident* name) {
        auto curr = scope;

        while(curr) {
            auto [entity, valid] = curr->find(name);
            if(valid) {
                return entity;
            }
            curr = curr->get_parent();
        }

        return nullptr;
    }

    Module * Typer::resolve_main_module(ast::ModuleFile *main_module) {
        // this will become the executable name.
        auto name = main_module->get_name();

        auto module_scope = make_scope<ModuleScope>(name, main_module, active_scope());

        push_scope(module_scope);


        std::vector<Entity*> entities;
        for(auto& decl : *main_module) {
            auto entity = build_top_level_entity(decl);
            add_entity(entity);
            entities.emplace_back(entity);
        }

        for(auto entity : entities) {
            auto e = resolve_entity(entity);
    
            if(e) e->debug_print(interp->out_stream());
        }

        pop_scope();

//        auto main_module_type = interp->new_type<ModuleType>();

        return nullptr;
    }

    void Typer::add_entity(Entity *entity) {
        active_scope()->insert(entity->get_name(), entity);
    }

    void Typer::push_scope(ScopePtr scope) {
        if(active_scope()) active_scope()->add_child(scope);
        if(scope->get_parent() != active_scope())
            interp->fatal("Compiler Error: Pushing a scope where the parent isn't the active scope");

        context.current_scope = scope;
    }

    void Typer::pop_scope() {
        context.current_scope = active_scope()->get_parent(); 
    }

    Entity *Typer::build_top_level_entity(ast::DeclPtr decl) {
        switch(decl->kind) {
            case ast::ast_global: {
                auto g = decl->as<ast::Global>();
                return interp->new_entity<Global>(g->name, active_scope(), decl);
            }
            case ast::ast_global_mut: {
                auto g = decl->as<ast::GlobalMut>();
               return interp->new_entity<Global>(g->name, active_scope(), decl);
            } break;
            case ast::ast_procedure: {
                auto p = decl->as<ast::Procedure>();
                return interp->new_entity<Function>(p->name, active_scope(), decl);
            } break;
            case ast::ast_structure: {
                auto s = decl->as<ast::Structure>();
                return interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_type: {
                auto s = decl->as<ast::Type>();
                return interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_type_class: {
                auto s = decl->as<ast::TypeClass>();
                return interp->new_entity<Type>(s->name, active_scope(), decl);
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
                return interp->new_entity<Module>(name, active_scope(), decl);
            } break;
            case ast::ast_alias: {
                auto a = decl->as<ast::Alias>();
                return interp->new_entity<Alias>(a->name, (types::Type*) nullptr, active_scope(), decl);
            } break;
            case ast::ast_impl:  {
                // think about how this should be handled.
                // maybe what I can do is search for the struct this impl block is associated with and
                // attach it so that it is resolved when the structure is.
            } break;
            default:
                report_str(decl->pos(), "invalid global declaration");
                return nullptr;
        }
    }

    Entity *Typer::resolve_entity(Entity *entity) {
        interp->message("Resolving: %s", entity->str().c_str());
        if(entity->is_resolved()) {
            interp->message("\tAlready Resolved: Returning");
            return entity;
        }

        // If an entity is resolved while it is
        // actively being resolved.
        if(entity->status() == Resolving) {
            report(entity->node()->pos(), "'%s' has ciclic dependency", entity->str().c_str());

            // return null if this happens.
            return nullptr;
        }

        entity->update_status(Resolving);
        context.active_entity = entity;

        auto e = entity->resolve(this);

        if(e != entity)
            interp->message("The Entity was changed during resolution");

        context.active_entity = nullptr;
        return e;
    }

    Entity *Typer::resolve(Global *global) {
        
        ast::Ident* name{nullptr};
        ast::Spec* spec{nullptr};
        ast::Expr* init{nullptr};

        if(global->is_mutable()) {
            auto declaration = global->get_decl_as<ast::GlobalMut>();

            name = declaration->name;
            spec = declaration->type.get();
            init = declaration->init.get();

            if(!spec and !init) {
                report(name->pos, "global '%s' must have a type or be initialized", name->value().c_str());
            }
        }
        else {
            auto declaration = global->get_decl_as<ast::Global>();

            name = declaration->name;
            spec = declaration->type.get();
            init = declaration->init.get();

            if(!init) {
                report(name->pos, "constant global '%s' must be initialized", name->value().c_str());
                return nullptr;
            }
        }

        types::Type* resolved_type{nullptr};
        Operand expr_type(init);

        if(spec)
            resolved_type = resolve_spec(spec);

        if(init)
            expr_type = resolve_expr(init, resolved_type);
        
        if(expr_type.error) {
            return nullptr;
        }

        if(resolved_type and expr_type.type) {
            interp->message("Global Types: Annotation: %s, Expression: %s",
                            resolved_type->str().c_str(),
                            expr_type.type->str().c_str());
        }

        if(!resolved_type)
            resolved_type = expr_type.type;

        if(!resolved_type)
            return nullptr;


        interp->message("Global resoved type: %s", resolved_type->str().c_str());

        // if the value is mutable the valarable should be converted to
        // a constant
        if(expr_type.val.is_constant and !global->is_mutable()) {
            interp->message("Global is a constant, is tranformed into a constant entity");
            // convert this entity to a constant.
            // interp->remove_entity(global);

            auto constant = interp->new_entity<Constant>(name, resolved_type, expr_type.val,
                                                         active_scope(), global->decl_ptr());

            constant->resolve_to(resolved_type);
            
            active_scope()->replace_name(constant->get_name(), constant);
            return constant;
        }
        else {
            if(init) {
                global->set_initialized();
            }

            global->set_addressing(get_addressing_by_type(resolved_type));
            global->resolve_to(resolved_type);
            return global;
        }
    }

    Entity *Typer::resolve(Local *local) {
        return local;
    }

    Entity *Typer::resolve(Type *type) {
        interp->message("Resolving %s", type->str().c_str());

        switch(type->get_decl()->kind) {
            case ast::ast_structure:
                return resolve_struct(type, type->get_decl());
            case ast::ast_type:
                return resolve_sumtype(type, type->get_decl());
            case ast::ast_type_class:
                return resolve_trait(type, type->get_decl());
            default:
                return nullptr;
        }
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

    Entity* Typer::resolve_struct(Type* entity, ast::DeclPtr decl_ptr) {
        auto decl = decl_ptr->as<ast::Structure>();
        // I will duplicate code to reduce complexity.
        if(decl->generics)
            return resolve_poly_struct(entity, decl_ptr);

        // creates the member scope for the struct.
        auto member_scope = mu::make_scope<mu::MemberScope>(entity->get_name(), decl, active_scope());        

        push_scope(member_scope);

        std::unordered_map<ast::Ident*, Entity*> members;

        u64 sz = 0;
        for(auto local : decl->members) {
            switch(local->kind) {
                case ast::ast_member_variable: {
                    auto entities = resolve_member_variable(local);
                    // there was an error resolving the members
                    if(entities.empty())
                        return nullptr;

                    for(auto e : entities) {
                        sz += e->get_type()->size();
                        members.emplace(e->get_name(), e);
                    }

                    break;
                }
                default:
                    break;
            }
        }

        auto type = interp->checked_new_type<types::StructType>(entity->get_name(),
            members, member_scope, sz, entity);


        interp->message("Structure Resolution");
        for(auto& [name, entity] : members) {
            interp->message("%s -> %s", name->value().c_str(), entity->get_type()->str().c_str());
        }

        entity->resolve_to(type);

        pop_scope();


        return entity;
    }

    Entity* Typer::resolve_poly_struct(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

    types::FunctionType* Typer::resolve_function_signiture(ast::ProcedureSigniture* sig) {
        return nullptr;
    }

    Entity* Typer::resolve_function(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

    Entity* Typer::resolve_poly_function(Type* entity, ast::DeclPtr decl_ptr) {

        return entity;
    }

    Entity* Typer::resolve_sumtype(Type* entity, ast::DeclPtr decl_ptr) {

        return entity;
    }

    Entity* Typer::resolve_poly_sumtype(Type* entity, ast::DeclPtr decl_ptr) {

        return entity;
    }

    Entity* Typer::resolve_trait(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

    Entity* Typer::resolve_poly_trait(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

    std::vector<Local*> Typer::resolve_member_variable(ast::DeclPtr decl_ptr) {
        auto member = decl_ptr->as<ast::MemberVariable>();
        interp->message("Resolving Memeber");

        std::vector<Local*> entities;

        u32 num_names = member->names.size();
        u32 num_exprs = member->init.size();

        types::Type* type{nullptr};

        if(member->type)
            type = resolve_spec(member->type.get());


        if(num_names == num_exprs) {
            std::vector<Operand> init_types;

            // resolve the type of the expression
            for(auto& i : member->init) {
                auto op = resolve_expr(i.get(), type);

                if(op.error)
                    return entities;

                init_types.push_back(op);
            }

            // build the entity
            for(u32 i = 0; i < num_exprs; ++i) {
                auto e = interp->new_entity<Local>(member->names[i], init_types[i].type,
                        get_addressing_by_type(init_types[i].type),
                        active_scope(),
                        decl_ptr);
                        
                
                if(member->vis == ast::Visibility::Public)
                    e->set_visable();

                e->set_initialized();
                entities.push_back(e);
            }


        }
        else if(num_exprs == 1) {
            auto op = resolve_expr(member->init.front().get(), type);

            for(u32 i = 0; i < num_names; ++i) {
                auto e = interp->new_entity<Local>(member->names[i], op.type,
                        get_addressing_by_type(op.type),
                        active_scope(),
                        decl_ptr);

                if(member->vis == ast::Visibility::Public)
                    e->set_visable();

                e->set_initialized();
                entities.push_back(e);
            }
        }
        else if(num_exprs == 0) {
            if(!type) {
                report_str(member->pos(), "member variable must have type annotation or initialized");
                return {};
            }

            for(u32 i = 0; i < num_names; ++i) {
                auto e = interp->new_entity<Local>(member->names[i], type,
                        get_addressing_by_type(type),
                        active_scope(),
                        decl_ptr);
                        

                if(member->vis == ast::Visibility::Public)
                    e->set_visable();

                entities.push_back(e);
            }
        }
        else {
            report(member->pos(), "unexected number of initialization expression, expecting '%u' or '1' "
                                "found %u", num_names, num_exprs);
        }

        for(auto e : entities) { 
            if(is_redeclaration(e->get_name())) {
                // check for redeclaration
                report(e->get_name()->pos, "redeclaration of member variable '%s'",  e->get_name()->value().c_str());

                auto entity = search_scope(active_scope(), e->get_name());

                interp->message("'%s' previously declared here:", e->get_name()->value().c_str());
                interp->print_file_section(entity->get_name()->pos);

                return {};
            }
            else {
                // add it otherwise
                add_entity(e);
            }
        }

        return entities;
    }

    Entity* Typer::resolve_local_from_decl(ast::DeclPtr decl_ptr) {
        
    }

    Operand Typer::resolve_expr(ast::Expr *expr, types::Type *expected_type) {
        Operand result(expr);
        switch(expr->kind) {
            case ast::ast_name:
            case ast::ast_name_generic:
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
            case ast::ast_tuple_expr: {
                auto t = expr->as<ast::TupleExpr>();
                u64 sz = 0;
                std::vector<types::Type*> types;
                for(auto& element : t->elements) {
                    auto op = resolve_expr(element.get());
                    if(op.error)
                        return Operand(expr);

                    types.push_back(op.type);
                    sz += op.type->size();
                }
                auto res_type = interp->checked_new_type<types::Tuple>(types, sz);

                // Rvalue because this is a tuple literal.
                result = Operand(res_type, expr, RValue);
                break;

            }
            default:
                break;
        }

        if(expected_type) {
            // check for compatibility of the resulting type and the expected type.
            // if(!compatable_types(expected_type, result.type)) {
            //     report(expr->pos(), "incompatable type: '%s' expected: '%s'",
            //         result.type->str().c_str(),
            //         expected_type->str().c_str());
            //     return Operand(expr);
            // }
        }
        expr->type = result.type;
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
                default:
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
                    auto type = interp->checked_new_type<types::Pointer>(operand.type);
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
        switch(expr->kind) {
            case ast::ast_name:
                return resolve_name_expr(expr->as<ast::Name>());
            case ast::ast_name_generic:
                return resolve_name_generic_expr(expr->as<ast::NameGeneric>());
            default:
                interp->fatal("Invalid name expression");
                return Operand(expr);
        }
    }

    Operand Typer::resolve_name_expr(ast::Name *expr) {
        auto entity = search_active_scope(expr->name);

        if(!entity)
            return Operand(expr);

        if(entity->is_local()) {
            auto local = entity->as<Local>();
            if(!local->is_initialized()) {
                report(expr->pos(), "use of an uninitialized variable '%s'", expr->name->value().c_str());
                interp->message("'%s' is declared here:", expr->name->value().c_str());
                interp->print_file_section(local->get_decl()->pos());
            }
            return Operand(local->get_type(), expr, LValue);
            // local->get_type();
        }
        else if(entity->is_global()) {
            auto global = entity->as<Global>(); 
            // resolve the entity anyway.
            global->resolve(this);
            if(!global->is_initialized()) {
                report(expr->pos(), "use of an uninitialized variable '%s'", expr->name->value().c_str());
                interp->message("'%s' is declared here:", expr->name->value().c_str());
                // interp->out_stream() << "\t";
                interp->print_file_section(global->get_decl()->pos());
            }
            return Operand(global->get_type(), expr, LValue);
        }
    }

    Operand Typer::resolve_name_generic_expr(ast::NameGeneric *expr) {
        return Operand(nullptr, nullptr, LValue);
    }

    // how to make sure multiple of the same type are not created
    types::Type *Typer::resolve_spec(ast::Spec *spec) {
        switch(spec->kind) {
            case ast::ast_expr_type: {
                auto e = spec->as<ast::ExprSpec>();

                auto entity = resolve_expr_spec(e->type.get());

                // this is for pointers and reference with in a struct to itself.
                if(context.allow_incomplete_types and !entity->is_resolved()) {
                }

                if(!entity->is_resolved())
                    entity->resolve(this);
                
                if(entity->is_type())
                    return entity->get_type();
                else {
                    report(e->pos(), "'%s' does not refer to a type", entity->str().c_str());
                    return nullptr;
                }
            } break;
            case ast::ast_tuple: {
                auto s = spec->as<ast::TupleSpec>();
                std::vector<types::Type*> types;
                u64 sz = 0;
                for(auto& t : s->elements) {
                    auto e = resolve_spec(t.get());
                    if(e) {
                        types.push_back(e);
                        e += e->size();
                    }
                    return nullptr;
                }
                return interp->checked_new_type<types::Tuple>(types, sz);
            }
            case ast::ast_list_spec:
            case ast::ast_list_spec_dyn:
                break;
            case ast::ast_ptr: {
                auto s = spec->as<ast::PtrSpec>();

                push_context_state(allow_incomplete_types, true)
                
                auto base_type = resolve_spec(s->type.get());

                pop_context_state(allow_incomplete_types);

                return interp->checked_new_type<types::Pointer>(base_type);
            }
            case ast::ast_ref: {
                auto s = spec->as<ast::RefSpec>();
                push_context_state(allow_incomplete_types, true)

                auto base_type = resolve_spec(s->type.get());

                pop_context_state(allow_incomplete_types);

                return interp->checked_new_type<types::Reference>(base_type);
            }
            case ast::ast_mut: {
                auto s = spec->as<ast::MutSpec>();
                auto base_type = resolve_spec(s->type.get());
                return interp->checked_new_type<types::Mutable>(base_type);
            }
            case ast::ast_self_type: {
                // check if we are resolving an
                // impl block. If we are, then return the type
                // of the impl type being implemented.
            }
                //
            case ast::ast_procedure_spec: {
                auto p = spec->as<ast::ProcedureSpec>();
            } break;
            case ast::ast_type_lit:
                return nullptr;
            case ast::ast_infer_type:
                return nullptr;
            case ast::ast_unit_type:
                return type_unit;
            default:
                // report the error
                break;
        }
    }

    Entity *Typer::resolve_expr_spec(ast::Expr *expr) {
        switch(expr->kind) {
            case ast::ast_accessor: {
                auto e = expr->as<ast::Accessor>();
                return resolve_accessor_spec(e);
            } break;
            case ast::ast_name: {
                auto e = expr->as<ast::Name>();
                return search_active_scope(e->name);
            } break;
            case ast::ast_name_generic: {
                auto e = expr->as<ast::NameGeneric>();
                interp->message("Generics is not implemented");
                // resolves the name and 
                return nullptr;
            } break;
            default:
                report_str(expr->pos(), "invalid type expression");
                return nullptr;
        }
        return nullptr;
    }

    Entity* Typer::resolve_accessor_spec(ast::Accessor* expr) {
        auto root_entity = resolve_expr_spec(expr->operand.get());

        // if there was an error above then propogate it through the rest.
        if(!root_entity) return nullptr;

        ScopePtr  scope = nullptr;
        switch(root_entity->kind()) {
            case mu::ModuleEntity: {
                auto mod = CAST_PTR(Module, root_entity);
                interp->message("Modules are not implemented");
                return nullptr;
                // return search_scope(mod->)
            }
            case mu::TypeEntity: {
                auto type = CAST_PTR(Type, root_entity);

                if(type->is_struct()) {
                    auto struct_type = CAST_PTR(types::StructType, type->get_type());
                    scope = struct_type->get_scope();
                }
                else if(type->is_sumtype()) {
                    auto sum_type = CAST_PTR(types::SumType, type->get_type());
                    scope = sum_type->get_scope();
                }
                else if(type->is_trait()) {
                    auto trait_type = CAST_PTR(types::TraitType, type->get_type());
                    scope = trait_type->get_scope();
                }
                else {
                    report_str(expr->operand->pos(), "type does doesn't have a scope");
                    return nullptr;
                }

            } break;
            default:
                report_str(expr->operand->pos(), "is not a module, structure, or Sum Type");
                return nullptr;
        }

        auto entity = search_scope(scope, expr->name);                    
        if(!entity) {
            report(expr->name->pos, "'%s' is not a member of '%s'",
                    expr->name->value().c_str(), entity->get_name()->value().c_str())
        }
        return entity;
    }
}
