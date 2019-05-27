//
// Created by Andrew Bregger on 2019-05-22.
//

#include "for_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::ForParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(token.kind() == mu::Tkn_For);
    return ast::ExprPtr();
}
