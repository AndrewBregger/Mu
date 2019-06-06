//
// Created by Andrew Bregger on 2019-06-06.
//

#include "return_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::ReturnParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(mu::Tkn_Return);
    parser.advance();

    auto expr = parser.parse_expr();
    if(expr)
        return ast::make_expr<ast::Return>(expr, expr->pos());
    else
        return expr;
}
