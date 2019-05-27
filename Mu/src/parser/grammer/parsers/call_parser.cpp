//
// Created by Andrew Bregger on 2019-05-22.
//

#include "call_parser.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::CallParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
//    parser.advance();
    return parser.parse_call(nullptr, op, left);
}
