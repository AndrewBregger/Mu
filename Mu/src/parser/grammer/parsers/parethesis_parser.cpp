//
// Created by Andrew Bregger on 2019-05-22.
//

#include "parethesis_parser.hpp"
#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::ParethesisParser::lud(mu::Parser &parser, mu::Token token) {
    parser.advance();
    auto pos = token.pos();

    auto expr = parser.parse_expr();
    std::vector<ast::ExprPtr> elements = {expr};
    pos.extend(expr->pos());

    if(parser.allow(mu::Tkn_Comma)) {
        pos.span += 1;

        parser.many<ast::ExprPtr>(
                    [&parser]() {
                        return parser.parse_expr();
                    },
                    [&parser]() {
                        bool val = parser.allow(mu::Tkn_Comma);
                        if(val and parser.check(mu::Tkn_CloseParen)) {
                            parser.report(parser.current().pos(), "expecting expression following ',', found ')");
                            return false;
                        }
                        return val;
                    },
                    [&parser, &pos, &elements](std::vector<ast::ExprPtr>&, ast::ExprPtr expr) {
                        if(expr) {
                            elements.push_back(expr);
                            pos.extend(expr->pos());
                        }
                    }
                );
    }
    parser.expect(mu::Tkn_CloseParen);
    pos.span += 1;

    switch(elements.size()) {
        case 1:
            return elements.front();
        default:
            return ast::make_expr<ast::TupleExpr>(elements, pos);
    }



}
