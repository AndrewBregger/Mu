//
// Created by Andrew Bregger on 2019-05-22.
//

#include "if_parser.hpp"

#include "parser/ast/expr.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::IfParser::lud(mu::Parser &parser, mu::Token token) {

    switch(token.kind()) {
        case mu::Tkn_If:
        case mu::Tkn_Elif: {
            parser.advance();

            auto cond = parser.parse_expr();

            if(parser.check(mu::Tkn_OpenBracket)) {
                auto body = parser.parse_expr();
                if(parser.check(mu::Tkn_NewLine))
                    parser.advance(true);

                auto tok = parser.current();

                auto else_if = lud(parser, tok);

                auto pos = token.pos();
                pos.extend(cond->pos())
                        .extend(body->pos())
                        .extend(tok.pos());

                if(else_if)
                    pos.extend(else_if->pos());

                return ast::make_expr<ast::If>(cond, body, else_if, pos);
            }
            else {
                parser.report(parser.current().pos(), "expecting '{' in %s expression", token.get_string().c_str());
            }
        }
        case mu::Tkn_Else: {
            parser.advance();
            if(parser.check(mu::Tkn_OpenBracket)) {
                return parser.parse_expr();
            }
            else {
                parser.report(parser.current().pos(), "expecting '{' in else branch");
            }
        }
        default:
            return nullptr;
    }
}
