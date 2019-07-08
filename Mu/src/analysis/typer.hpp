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
#include "operand.hpp"

#include "value.hpp"

#include "parser/ast/expr.hpp"
#include "parser/ast/stmt.hpp"
#include "parser/ast/decl.hpp"
#include "parser/ast/specs.hpp"
#include "parser/ast/pattern.hpp"
#include "parser/ast/renderer.hpp"


namespace mu {

    class Typer {
        public:
            Typer(Interpreter* interp);



            /*-----------------------------Misc------------------------------*/
            // checks for a ren
            bool is_redeclaration(ast::Ident* name);

            AddressType get_addressing_by_type(types::Type* type);

            types::Type* existing_type(types::Type* type);

            Local* new_padding(const std::string& name, u32 size);

            // check if the given type is compatible with the expected.
            bool compatible_types(types::Type *expected, types::Type *given, bool casting = false);

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

            std::tuple<u64, u64, std::vector<ast::Ident*>> construct_member_order(std::vector<Entity*>& members);

            Entity* resolve_struct(Type* entity, ast::DeclPtr decl_ptr);
            Entity* resolve_poly_struct(Type* entity, ast::DeclPtr decl_ptr);

            bool resolve_impl_blocks(Type* entity);

            // this function builds the local pramameters and generates the function type to be used by the entity.
            std::tuple<std::vector<Local *>, bool> resolve_function_members(ast::ProcedureSigniture *sig);

            Entity* resolve_function(Type* entity,ast::DeclPtr decl_ptr); 
            Entity* resolve_poly_function(Type* entity, ast::DeclPtr decl_ptr);


            Entity* resolve_sumtype(Type* entity, ast::DeclPtr decl_ptr);
            Entity* resolve_poly_sumtype(Type* entity, ast::DeclPtr decl_ptr);

            Entity* resolve_trait(Type* entity, ast::DeclPtr decl_ptr);
            Entity* resolve_poly_trait(Type* entity, ast::DeclPtr decl_ptr);

            void resolve_local_from_decl(ast::DeclPtr decl_ptr, bool mut);

			std::tuple<std::vector<Local*>, bool> resolve_member_variable(ast::DeclPtr decl_ptr);

            // resolves an expression that is expected to return an entity.
            // Null if it fails.
            Entity* resolve_expr_to_entity(ast::Expr* expr);
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

            // this is used when the function entity is known.
            std::tuple<std::vector<Operand>, bool> resolve_call_actuals(Function *fn,
                    const std::vector<ast::ExprPtr> &actuals, const mu::Pos &call_pos);

            // this is used when calling a function pointer.
            std::tuple<std::vector<Operand>, bool> resolve_actauls(types::FunctionType* fn,
                    const std::vector<ast::ExprPtr>& actuals, const mu::Pos& call_pos);

            Operand resolve_call_or_curry(ast::Call* expr);

			Operand resolve_method_call(ast::Expr* expr);
	

			// resolves a static method of Entity owner 
			// The owner is the function that has fn.
			Operand resolve_static_method(Entity* owner, Entity* fn,
					const std::vector<ast::ExprPtr>& actuals, Operand operand,
					ast::Expr* name);
		
			
			// resolves a method 
			// The receiver is the structure that is receive the function call.
			Operand resolve_received_method(Entity* receiver, Entity* fn,
					const std::vector<ast::ExprPtr>& actuals, Operand operand,
					ast::Expr* name);

            // a wrapper for 'resolve_name_expr' and 'resolve_name_generic_expr'
            Operand resolve_name(ast::Expr* expr);

            // resolves a name, when used in an expression
            Entity * resolve_name_expr(ast::Name *expr);

            // resolves a name with type parameters.
            std::tuple<Operand, Entity *> resolve_name_generic_expr(ast::NameGeneric *expr);

            Operand resolve_cast(ast::Cast* expr);

            Operand resolve_literals(ast::Expr* expr);

            Operand resolve_accessor(ast::Expr* expr);

            Operand resolve_tuple_accessor(ast::Expr* expr);

            Operand resolve_struct_expr(ast::Expr* expr);

            Operand resolve_block(ast::Expr* expr);

            /*--------------------------Stmt Handling----------------------------*/

            Operand resolve_stmt(ast::Stmt* stmt);

            Operand resolve_decl_stmt(ast::DeclStmt* stmt);

            /*--------------------------Pattern Handling------------------------*/

            // resovles a pattern, the pattern is matched with the expected type
            // if they do not match then there is an error otherwise, the entities are created.
            void resolve_pattern(ast::Pattern *pattern, Operand expected_type, ast::DeclPtr decl);

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
                bool resolving_trait_body{false};       // resolving trait body, this means that function do not have to have a body.
                bool function_body{false};              // resolving a function body, this is to be used when resolving parameter names.
                bool resolving_local{false};            // resolving local, this is for pattern resolution
                bool resolving_match{false};            // resolving match, this is for pattern resolution
            };

            void increment_error();
            ScopePtr active_scope();

            Interpreter* interp{nullptr};   // pointer to the interpreter object.
            ScopePtr prelude{nullptr};        // a pointer to the prelude scope.

            Context context;

            u32 errors_num{0};              // the number of errors

            ast::AstRenderer renderer;
    };
}


#endif //MU_TYPER_HPP
