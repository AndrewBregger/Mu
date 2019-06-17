//
// Created by Andrew Bregger on 2019-06-07.
//

#ifndef MU_TYPER_HPP
#define MU_TYPER_HPP

class Interpreter;

#include <parser/ast/module.hpp>
#include "common.hpp"

#include "types/type.hpp"
#include "scope.hpp"
#include "entity.hpp"

#include "value.hpp"

#include "parser/ast/expr.hpp"
#include "parser/ast/stmt.hpp"
#include "parser/ast/decl.hpp"
#include "parser/ast/specs.hpp"
#include "parser/ast/pattern.hpp"


namespace mu {
    const Val NO_VALUE;

    enum AccessType {
        LValue,
        RValue,
    };

    // the result of an expression
    // when an expression is resolved, either a type or a type with a value is
    // the result. This struct encapsulates both results.
    // if val is invalid, then the value is computed at run time
    // if val is constant, then we known the value at
    // compile time.
    // AccessType determines how this expression can be used.
    //  If it it is an LValue can be on the left side of an assign.
    //  If is it is an RValue then it must be on the right side.
    struct Operand {
        types::Type* type;
        ast::Expr* expr;
        AccessType access{RValue};
        Val val{NO_VALUE};
        bool error{false};

        // Valid run time expression
        Operand(mu::types::Type *type, ast::Expr *expr, AccessType access_type);

        // Valid compile time expression
        Operand(mu::types::Type* type, ast::Expr* expr, const Val& val);

        // error on expression
        // will have been reported by this point.
        // this is so the error can be propagated
        explicit Operand(ast::Expr* expr);
    };

    class Typer {
        public:
            Typer(Interpreter* interp);

            // loads a module according to given use declaration;
            // the function will check what type of use is given.
            void load_module(ast::Decl* use_decl);

            // processes the main file, it must have main function.
            Module * resolve_main_module(ast::ModuleFile *main_module);

            // adds entity to the current active scope.
            void add_entity(Entity* entity);

            void push_scope(Scope* scope);

            void pop_scope();

            Entity* build_top_level_entity(ast::Decl* decl);

            Entity* resolve_entity(Entity* entity);

            bool is_redeclaration(ast::Ident* name);

            Entity* resolve(Global* global);
            Entity* resolve(Local* local);
            Entity* resolve(Type* type);
            Entity* resolve(Function* funct);
            Entity* resolve(Alias* alias);
            Entity* resolve(Constant* constant);



            // resolves an expression with an expected type.
            // if a type is known (function parameter) the resulting type will
            // be check against it.
            Operand resolve_expr(ast::Expr* expr, mu::types::Type* expected_type = nullptr);

            // resolves binary expression wrapper
            Operand resolve_binary(ast::Binary *expr, types::Type *expected_type);

            // resolves binary expression when the lhs is an arithmetic type.
            Operand resolve_arithmetic_binary(TokenKind op, Operand lhs, Operand rhs, ast::Expr *expr,
                                              types::Type *expected_type);

            // resolves binary expression when the lhs is not an arithmetic type.
            // this is for when a struct overloads a binary operator
            Operand resolve_binary_overload(TokenKind op, Operand rhs, Operand lhs, ast::Expr *expr,
                                            types::Type *expected_type);

            // resolve unary expression
            Operand resolve_unary(ast::Unary *expr, types::Type *expected_type);

            Operand resolve_arithmetic_unary(TokenKind op, Operand operand, ast::Expr* expr, types::Type* expectd_type);

            // resolves unary expression on non primitive types
            // this is for when a struct overloads a unary operator
            Operand resolve_unary_overload(TokenKind op, Operand operand, ast::Expr *expr, types::Type *expected_type);

            Operand resolve_call_or_curry(ast::Call* expr);

            // a wrapper for 'resolve_name_expr' and 'resolve_name_generic_expr'
            Operand resolve_name(ast::Expr* expr);

            // resolves a name, when used in an expression
            Operand resolve_name_expr(ast::Name* expr);

            // resolves a name with type parameters.
            Operand resolve_name_generic_expr(ast::NameGeneric* expr);

            Operand resolve_cast(ast::Cast* expr);

            Operand resolve_literals(ast::Expr* expr);

//            template <typename... Args>
//            void report(const mu::Pos& pos, const std::string& msg, Args... args) {
//                interp->report_error(pos, msg, args...);
//            }

        private:
            Interpreter* interp{nullptr};

            Scope* current_scope{nullptr};
    };
}


#endif //MU_TYPER_HPP
