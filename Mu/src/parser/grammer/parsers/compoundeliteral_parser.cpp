//
// Created by Andrew Bregger on 2019-05-22.
//

#include "compoundeliteral_parser.hpp"
#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"

ast::ExprPtr parse::CompoundedLiteralParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(token.kind() == mu::Tkn_OpenBrace);
    auto pos = token.pos();

    parser.advance();
    auto first = parser.parse_expr();

    auto condition =
            [&parser]() {
                return parser.allow(mu::Tkn_Comma) and !parser.check(mu::Tkn_CloseBrace);
            };

    if(parser.allow(mu::Tkn_Colon)) {
        auto second = parser.parse_expr();
        std::vector<std::tuple<ast::ExprPtr, ast::ExprPtr>> elements;
        elements.emplace_back(first, second);

        parser.many<std::tuple<ast::ExprPtr, ast::ExprPtr>>(
                [&parser]() {
                    auto key = parser.parse_expr();
                    parser.expect(mu::Tkn_Colon);
                    auto value = parser.parse_expr();
                    return std::make_tuple(key, value);
                },
                condition,
                [&pos, &elements](std::vector<std::tuple<ast::ExprPtr, ast::ExprPtr>>&,
                        std::tuple<ast::ExprPtr, ast::ExprPtr> element) {
                    elements.push_back(element);
                    pos.extend(std::get<0>(element)->pos());
                    pos.extend(std::get<1>(element)->pos());
                }
        );
        parser.expect(mu::Tkn_CloseBrace);
        return ast::make_expr<ast::Map>(elements, pos);
    }
    else {
        std::vector<ast::ExprPtr> elements = {first};
        parser.many<ast::ExprPtr>(
                [&parser]() {
                    return parser.parse_expr();
                },
                condition,
                [&pos, &elements](std::vector<ast::ExprPtr>&,
                                  ast::ExprPtr element) {
                    elements.push_back(element);
                    pos.extend(element->pos());
                }
        );
        pos.extend(parser.expect(mu::Tkn_CloseBrace).first.pos());

        return ast::make_expr<ast::List>(elements, pos);
    }

}
