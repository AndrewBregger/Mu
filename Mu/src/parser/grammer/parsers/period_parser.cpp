//
// Created by Andrew Bregger on 2019-05-21.
//

#include "period_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::PeriodParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
    parser.advance();

    auto [name, valid] = parser.expect(mu::Tkn_Identifier);

    if(valid) {
        if(parser.check(mu::Tkn_OpenBrace) or parser.check(mu::Tkn_OpenParen)) {
            return parser.parse_call(name.ident, parser.current(), left);
        }
        else {
            auto pos = left->pos();
            pos.extend(name.pos());
            return ast::make_expr<ast::Accessor>(left, name.ident, pos);
        }
    }
    else {
        return nullptr;
    }
}
