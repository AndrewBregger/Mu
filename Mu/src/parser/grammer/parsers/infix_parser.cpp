//
// Created by Andrew Bregger on 2019-05-20.
//

#include "parser/ast/expr.hpp"
#include "infix_parser.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::InfixParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
    parser.advance();
    auto rhs = parser.parse_expr(op.prec());
    auto pos = left->pos();

    pos = pos.extend(op.pos()).extend(rhs->pos());
    return ast::make_expr<ast::Binary>(op.kind(), left, rhs, pos);
}
