//
// Created by Andrew Bregger on 2019-05-22.
//

#include "while_parser.hpp"
#include "parser/ast/expr.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::WhileParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(token.kind() == mu::Tkn_While);
    parser.advance();

    auto cond = parser.parse_expr();

    if(parser.check(mu::Tkn_OpenBracket)) {
        auto body = parser.parse_expr();
        auto pos = token.pos();
        pos.extend(cond->pos());
        pos.extend(body->pos());

        return ast::make_expr<ast::While>(cond, body, pos);
    }
    else {
        parser.report(parser.current().pos(), "expecting '{' in %s expression", token.get_string().c_str());
        return nullptr;
    }
}
