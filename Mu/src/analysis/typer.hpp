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



            /*-----------------------------Misc------------------------------*/
            // checks for a ren
            bool is_redeclaration(ast::Ident* name);

            AddressType get_addressing_by_type(types::Type* type);

            types::Type* existing_type(types::Type* type);

            /*-----------------------Module Handling-------------------------*/

            // processes the main file, it must have main function.
            Module * resolve_main_module(ast::ModuleFile *main_module);

            // loads a module according to given use declaration;
            // the function will check what type of use is given.
            void load_module(ast::Decl* use_decl);

            /*-----------------------Scope Handling--------------------------*/

            // search the typers active scope for the name.
            Entity* search_active_scope(ast::Ident* name);

            void push_scope(ScopePtr  scope);

            void pop_scope();

            // adds entity to the current active scope.
            void add_entity(Entity* entity);

            // search the given scope for the name.
            Entity* search_scope(ScopePtr  scope, ast::Ident* name);


            /*-----------------------Entity Handling-------------------------*/
            Entity* build_top_level_entity(ast::DeclPtr decl);

            Entity* resolve_entity(Entity* entity);

            Entity* resolve(Global* global);
            Entity* resolve(Local* local);
            Entity* resolve(Type* type);
            Entity* resolve(Function* funct);
            Entity* resolve(Alias* alias);
            Entity* resolve(Constant* constant);


            /*--------------------Declaration Handling-----------------------*/

            Entity* resolve_struct(Type* entity, ast::DeclPtr decl_ptr);
            Entity* resolve_poly_struct(Type* entity, ast::DeclPtr decl_ptr);

            types::FunctionType* resolve_function_signiture(ast::ProcedureSigniture* sig);
            Entity* resolve_function(Type* entity,ast::DeclPtr decl_ptr); 
            Entity* resolve_poly_function(Type* entity, ast::DeclPtr decl_ptr);


            Entity* resolve_sumtype(Type* entity, ast::DeclPtr decl_ptr);
            Entity* resolve_poly_sumtype(Type* entity, ast::DeclPtr decl_ptr);

            Entity* resolve_trait(Type* entity, ast::DeclPtr decl_ptr);
            Entity* resolve_poly_trait(Type* entity, ast::DeclPtr decl_ptr);

            Entity* resolve_local_from_decl(ast::DeclPtr local);

            std::vector<Local*> resolve_member_variable(ast::DeclPtr decl_ptr);

            /*--------------------Expression Handling-----------------------*/

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

            /*--------------------------Spec Handling----------------------------*/
            // resolve a type spec to the type it specifies.
            types::Type* resolve_spec(ast::Spec* spec);

            // resolves the expression as an a type.
            // this will be expanded to allow arbitrary expressions
            // as types.
            Entity* resolve_expr_spec(ast::Expr* expr);

            Entity* resolve_accessor_spec(ast::Accessor* expr);

            types::Type* resolve_function_spec(ast::ProcedureSpec* proc_spec);

//            template <typename... Args>
//            void report(const mu::Pos& pos, const std::string& msg, Args... args) {
//                interp->report_error(pos, msg, args...);
//            }

        private:

            struct Context {
                Entity*     impl_block_entity{nullptr}; // the Self entity when resolving an impl block
                Entity*     active_entity{nullptr};     // current entity being resolved
                bool resolving_loop{false};             // true when resolving for, while, loop
                ScopePtr current_scope{nullptr};  // the current scope being resolved.
                bool allow_incomplete_types{false};     // this is to allow incomplete pointer or references (ie. in linked lists).
            };

            void increment_error();
            ScopePtr active_scope();

            Interpreter* interp{nullptr};   // pointer to the interpreter object.
            ScopePtr prelude{nullptr};        // a pointer to the prelude scope.

            Context context;

            u32 errors_num{0};              // the number of errors
    };
}


#endif //MU_TYPER_HPP
