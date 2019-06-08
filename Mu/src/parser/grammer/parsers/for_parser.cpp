//
// Created by Andrew Bregger on 2019-05-22.
//

#include "for_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"
#include "parser/ast/pattern.hpp"

ast::ExprPtr parse::ForParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(token.kind() == mu::Tkn_For);

    auto pos = parser.current().pos();
    parser.advance();

    auto pattern = parser.parse_pattern(false);
    pos.extend(pattern->pos());

    if(pattern)
        return ast::ExprPtr();

    parser.push_restriction(mu::NoStructExpr);
    auto expr = parser.parse_expr();
    parser.pop_restriction();
    pos.extend(expr->pos());

    parser.remove_newlines();
    if(parser.check(mu::Tkn_OpenBracket)) {
        auto body = parser.parse_expr();
        pos.extend(body->pos());
        return ast::make_expr<ast::For>(pattern, expr, body, pos);
    }
    else {
        parser.report(parser.current().pos(), "expecting '{' for expression body");
        return ast::ExprPtr();
    }
}
