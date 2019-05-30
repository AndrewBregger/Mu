//
// Created by Andrew Bregger on 2019-05-21.
//

#include "period_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::PeriodParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
    parser.advance();

    if(parser.check(mu::Tkn_Identifier)) {
        auto name = parser.current();
        parser.advance();
        if(parser.check(mu::Tkn_OpenBrace) or parser.check(mu::Tkn_OpenParen)) {
            return parser.parse_call(name.ident, parser.current(), left);
        }
        else {
            auto pos = left->pos();
            pos.extend(name.pos());
            return ast::make_expr<ast::Accessor>(left, name.ident, pos);
        }
    }
    else if(parser.check(mu::Tkn_IntLiteral)) {
        auto index = parser.current();
        parser.advance();

        auto pos = left->pos();
        pos.extend(index.pos());
        return ast::make_expr<ast::TupleAcessor>(left, index.integer, pos);
    }
    else {
        parser.report(parser.current().pos(), "expecting identifier or an integer, found: '%s'",
                parser.current().get_string().c_str());
        return left;
    }
}
