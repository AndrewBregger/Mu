//
// Created by Andrew Bregger on 2019-05-19.
//

#ifndef MU_PARSER_HPP
#define MU_PARSER_HPP

#include <type_traits>
#include <stack>

#include "common.hpp"
#include "parser/ast/decl.hpp"
#include "ast/ast_common.hpp"
#include "grammer/grammer.hpp"
#include "scanner/scanner.hpp"
#include "utils/file.hpp"
#include "interpreter.hpp"



namespace mu {
    class Module;

    typedef u32 Restriction;
    const Restriction Default = 0;
    const Restriction NoStructExpr = 1;

    class Parser {
    public:
        Parser(Interpreter* interp);

        bool advance(bool ignore_newline = false);

        inline bool has_error() { return error_count > 0; }

        ast::ModuleFile* process(io::File* file);

        std::pair<Token, bool> expect(TokenKind tok);

        bool allow(TokenKind tok);

        bool check(TokenKind tok);

        inline Token peek() { return scanner.token(); }

        void passert(bool term);

        void remove_newlines();

        template <bool r = true, typename... Args>
        void report(const mu::Pos& pos, const std::string& fmt, Args... args) {
            interp->report_error(pos, fmt, args...);
			
			interp->quit();
            // if(r and !sync_after_error()) {
            //     interp->fatal("unrecoverable syntax error");
            // }

            ++error_count;
        }

        inline mu::Token current() {
            return t;
        }

        template<typename Ret>
        static void append(std::vector<Ret>& result, Ret val) {
            if(val)
                result.push_back(val);
        }

        template<typename Ret>
        static void append_value(std::vector<Ret>& result, Ret val) {
            result.push_back(val);
        }

        template<typename Ret, typename Fn, typename Cond, typename Process>
        std::vector<Ret> many(Fn fn, Cond cond, Process process = Parser::append<Ret>) {
            std::vector<Ret> result;
            do {
                auto res = fn();
                process(result, res);
            } while(cond());
            return result;
        }

        ast::ModuleFile * parse_module();

        inline mu::Scanner::State save_state() { return scanner.save(); }
        inline void reset(const mu::Scanner::State& state) { scanner.restore(state); }

        // expression parsing
        ast::ExprPtr parse_expr();

        ast::ExprPtr parse_expr(i32 prec_min);

        ast::ExprPtr parse_primary_expr();

		ast::ExprPtr parse_bottom_expr();

        ast::ExprPtr parse_call(ast::ExprPtr& name, Token token);

		ast::ExprPtr parse_method(ast::ExprPtr operand, ast::ExprPtr name, Token token);

        ast::ExprPtr parse_expr_spec(bool is_spec);

        ast::ExprPtr parse_suffix(ast::ExprPtr& expr, bool is_spec = false);

        ast::ExprPtr parse_name();

		ast::ExprPtr parse_unary();

		ast::ExprPtr try_struct_expr(ast::ExprPtr operand);

		ast::ExprPtr try_tuple_or_expr(); 

		ast::ExprPtr parse_compound_literal();

		ast::ExprPtr parse_for();

		ast::ExprPtr parse_if();

		ast::ExprPtr parse_lambda();
	
		ast::ExprPtr parse_match();

		ast::ExprPtr parse_arm();



		// stmt parsing
        ast::StmtPtr parse_stmt();

        ast::DeclPtr parse_toplevel();

        ast::DeclPtr parse_decl(mu::Token token, bool toplevel = false);

        ast::DeclPtr parse_variable(mu::TokenKind kind);

        ast::DeclPtr parse_global(mu::TokenKind kind, ast::Visibility vis);

        ast::DeclPtr parse_type_decl(ast::Ident *name, const ast::AttributeList &attributes, ast::Visibility vis);

        ast::AttributeList parse_attributes();

        ast::DeclPtr parse_procedure_parameter();

        std::shared_ptr<ast::ProcedureSigniture> parse_procedure_signiture();

        ast::DeclPtr parse_procedure(ast::Ident *name, const ast::AttributeList &attributes, ast::Visibility vis);

        ast::DeclPtr parse_struct(ast::Ident *name, ast::Visibility vis);

        ast::DeclPtr parse_type(ast::Ident *name, ast::Visibility vis);

        ast::DeclPtr parse_trait(ast::Ident *name, ast::Visibility vis);

        ast::DeclPtr parse_member_variable();

        ast::DeclPtr parse_generic_group();

        ast::DeclPtr parse_generic();

        ast::DeclPtr parse_generic_and_bounds();

        ast::DeclPtr parse_impl(ast::Ident *name, ast::Visibility vis);

        ast::DeclPtr parse_type_member();

        ast::DeclPtr parse_use(ast::Visibility vis);

        ast::DeclPtr parse_usepath();

        ast::PatternPtr parse_pattern(bool bind_pattern = false);

        ast::SpecPtr parse_spec(bool allow_infer);

        ast::Visibility parse_visability();

        bool sync_after_error();

        void push_restriction(Restriction res);

        bool check_restriction(Restriction res);

        void pop_restriction();

    private:
        Interpreter* interp;
        Scanner scanner;
        Token t;
        parse::Grammar grammar;
        Restriction restriction{Default};
        std::stack<Restriction> prev_res;

        u64 error_count{0};
    };
}

#endif //MU_PARSER_HPP
