//
// Created by Andrew Bregger on 2019-05-20.
//

#include "name_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::NameParser::lud(mu::Parser &parser, mu::Token token) {
    parser.advance();
    return ast::make_expr<ast::Name>(token.ident, token.pos());
}
