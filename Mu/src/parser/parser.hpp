//
// Created by Andrew Bregger on 2019-05-19.
//

#ifndef MU_PARSER_HPP
#define MU_PARSER_HPP

#include "common.hpp"

#include "ast/ast_common.hpp"
#include "grammer/grammer.hpp"
#include "scanner/scanner.hpp"
#include "utils/file.hpp"
#include "interpreter.hpp"



namespace mu {
    class Module;

    class Parser {
    public:
        Parser(Interpreter* interp);

        bool advance(bool ignore_newline = false);

        Module* process(io::File* file);

        std::pair<Token, bool> expect(TokenKind tok);

        bool allow(TokenKind tok);

        bool check(TokenKind tok);

        inline Token peek() { return scanner.token(); }

        void passert(bool term);

        void remove_newlines();

        template <typename... Args>
        void report(const mu::Pos& pos, const std::string& fmt, Args... args) {
            interp->report_error(pos, fmt, args...);
        }

        inline mu::Token current() {
            return t;
        }

        template<typename Ret>
        static void append(std::vector<Ret>& result, Ret val) {
            if(val)
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

        mu::Module* parse_module();

        // expression parsing
        ast::ExprPtr parse_expr();

        ast::ExprPtr parse_expr(i32 prec_min);

        ast::ExprPtr parse_call(ast::Ident* name, Token token, ast::ExprPtr operand = nullptr);

        // stmt parsing
        ast::StmtPtr parse_stmt();


        ast::DeclPtr parse_toplevel();

        ast::DeclPtr parse_decl(mu::Token token, bool toplevel = false);

        ast::DeclPtr parse_variable(mu::TokenKind kind);

        ast::DeclPtr parse_global(mu::TokenKind kind);

        ast::DeclPtr parse_type_decl(ast::Ident* name);

        ast::DeclPtr parse_procedure(ast::Ident* name);

        ast::DeclPtr parse_struct(ast::Ident* name);

        ast::DeclPtr parse_type(ast::Ident* name);

        ast::DeclPtr parse_member_variable();

        ast::DeclPtr parse_generic_group();

        ast::DeclPtr parse_generic();

        ast::DeclPtr parse_generic_and_bounds();




        ast::SpecPtr parse_spec();

    private:
        Interpreter* interp;
        Scanner scanner;
        Token t;
        parse::Grammar grammar;
    };
}

#endif //MU_PARSER_HPP
