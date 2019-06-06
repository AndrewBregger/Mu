//
// Created by Andrew Bregger on 2019-06-06.
//

#include <parser/ast/expr.hpp>
#include "defer_parser.hpp"

#include "parser/parser.hpp"

ast::ExprPtr parse::DeferParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(parser.current().kind() == token.kind());

    auto [_, valid] = parser.expect(mu::Tkn_Defer);
    if(valid) {
        auto expr = parser.parse_expr();
        if(expr)
            return ast::make_expr<ast::Defer>(expr, token.position.extend(expr->pos()));
        else
            return expr;
    }
    else return ast::ExprPtr();
}
