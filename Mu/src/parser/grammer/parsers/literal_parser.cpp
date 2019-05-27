//
// Created by Andrew Bregger on 2019-05-20.
//

#include "literal_parser.hpp"
#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::LiteralParser::lud(mu::Parser &parser, mu::Token token) {
    parser.advance();
    switch(token.kind()) {
        case mu::Tkn_IntLiteral:
            return ast::make_expr<ast::Integer>(token.integer, token.pos());
        case mu::Tkn_FloatLiteral:
            return ast::make_expr<ast::Float>(token.floating, token.pos());
        case mu::Tkn_CharLiteral:
            return ast::make_expr<ast::Char>(token.character, token.pos());
        case mu::Tkn_StringLiteral:
            return ast::make_expr<ast::Str>(token.str, token.pos());
        default:
            return nullptr;
    }
}
