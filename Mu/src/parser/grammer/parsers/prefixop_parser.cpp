//
// Created by Andrew Bregger on 2019-05-20.
//

#include "parser/ast/expr.hpp"

#include "prefixop_parser.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::PrefixOpParser::lud(mu::Parser &parser, mu::Token token) {
    parser.advance();
    auto expr = parser.parse_expr();
    auto pos = token.pos();
    return ast::make_expr<ast::Unary>(token.kind(), expr, pos.extend(expr->pos()));
}
