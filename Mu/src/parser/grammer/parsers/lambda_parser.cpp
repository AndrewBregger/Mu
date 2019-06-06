//
// Created by Andrew Bregger on 2019-06-06.
//

#include "lambda_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"
#include "parser/ast/specs.hpp"


ast::ExprPtr parse::LambdaParser::lud(mu::Parser &parser, mu::Token token) {
    parser.expect(mu::Tkn_BackSlash);

    auto pos = token.pos();

    auto params = parser.many<ast::DeclPtr>(
            [&parser]() {
                return parser.parse_procedure_parameter();
            },
            [&parser]() {
                return parser.allow(mu::Tkn_Comma);
            },
            mu::Parser::append<ast::DeclPtr>
        );

    for(auto& p : params)
        pos.extend(p->pos());

    ast::SpecPtr ret;
    if(parser.allow(mu::Tkn_Colon))
        ret = parser.parse_spec(false);
    else
        ret = ast::make_spec<ast::InferSpec>(parser.current().pos());

    pos.extend(ret->pos());

    auto [_, valid] = parser.expect(mu::Tkn_MinusGreater);

    if(valid) {
        auto body = parser.parse_expr();
        pos.extend(body->pos());

        return ast::make_expr<ast::Lambda>(params, ret, body, pos);
    }
    else {
        ast::ExprPtr expr;
        return expr;
    }
}
