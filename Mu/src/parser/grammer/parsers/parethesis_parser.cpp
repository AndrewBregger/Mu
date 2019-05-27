//
// Created by Andrew Bregger on 2019-05-22.
//

#include "parethesis_parser.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::ParethesisParser::lud(mu::Parser &parser, mu::Token token) {
    parser.advance();
    auto expr = parser.parse_expr();
    parser.expect(mu::Tkn_CloseParen);
    return expr;
}
