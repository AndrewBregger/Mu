//
// Created by Andrew Bregger on 2019-06-06.
//

#include "match_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"
#include "parser/ast/pattern.hpp"

ast::ExprPtr parse::MatchParser::lud(mu::Parser &parser, mu::Token token) {
    parser.passert(token.kind() == mu::Tkn_Match);
    auto pos = parser.current().pos();

    parser.advance();
    parser.push_restriction(mu::NoStructExpr);
    auto condition = parser.parse_expr();
    parser.pop_restriction();

    pos.extend(condition->pos());

    parser.remove_newlines();
    parser.expect(mu::Tkn_OpenBracket);
    parser.remove_newlines();
    pos.span += 1;

    auto arms = parser.many<ast::ExprPtr>(
                [&parser, this]() {
                    return parse_arm(parser);
                },
                [&parser]() {
                    bool val = parser.allow(mu::Tkn_Comma);
                    parser.remove_newlines();
                    if(val and parser.check(mu::Tkn_CloseBracket)) {
                        return false;
                    }
                    return val;
                },
                mu::Parser::append<ast::ExprPtr>
            );

    for(auto& a : arms)
        pos.extend(a->pos());

    parser.expect(mu::Tkn_CloseBracket);
    pos.span += 1;

    return ast::make_expr<ast::Match>(condition, arms, pos);
}

ast::ExprPtr parse::MatchParser::parse_arm(mu::Parser &parser) {
    auto pos = parser.current().pos();
    pos.span = 0;

    auto patterns = parser.many<ast::PatternPtr>(
                [&parser]() {
                    return parser.parse_pattern(false);
                },
                [&parser]() {
                    parser.remove_newlines();
                    bool val = parser.allow(mu::Tkn_Pipe);
                    parser.remove_newlines();
                    if(val and parser.check(mu::Tkn_Arrow)) {
                        parser.report(parser.current().pos(), "expecting pattern following '|'");
                        return false;
                    }
                    return val;
                },
                mu::Parser::append<ast::PatternPtr>
            );

    for(auto& p : patterns)
        pos.extend(p->pos());

    auto [_, valid] = parser.expect(mu::Tkn_Arrow);
    if(valid) {
        auto body = parser.parse_expr();
        return ast::make_expr<ast::MatchArm>(patterns, body, pos);
    }
    else {
        ast::ExprPtr expr;
        return expr;
    }
}
