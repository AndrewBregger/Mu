//
// Created by Andrew Bregger on 2019-05-20.
//

#include "name_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"
#include "parser/ast/specs.hpp"

ast::ExprPtr parse::NameParser::lud(mu::Parser &parser, mu::Token token) {
    ast::ExprPtr expr;
    bool is_self = false;
    if(token.kind() != mu::Tkn_SelfType) {
        expr = parser.parse_expr_spec(false);
    }
    else {
        is_self = true;
        parser.advance();
    }

    if(is_self and !parser.check(mu::Tkn_OpenBracket)) {
        parser.report(parser.current().pos(), "'Self' must be followed by '{' in an expression");
        return expr;
    }

    if(parser.check_restriction(mu::NoStructExpr) and is_self) {
        parser.report(token.pos(), "'Self' is not a valid expression, perhaps 'self' was desired");
        return expr;
    }

    if(!parser.check_restriction(mu::NoStructExpr) and parser.allow(mu::Tkn_OpenBracket)) {
        auto spec = (is_self ? ast::make_spec<ast::SelfSpec>(token.pos()) : ast::make_spec<ast::ExprSpec>(expr, expr->pos()));
        auto pos = spec->pos();
        pos.span++;

        std::vector<ast::ExprPtr> members;
        if(!parser.check(mu::Tkn_CloseBracket)) {
            members = parser.many<ast::ExprPtr>(
                    [&parser]() {
                        if (parser.check(mu::Tkn_Identifier) and parser.peek().kind() == mu::Tkn_Colon) {
                            auto token = parser.current();
                            auto pos = token.pos();
                            parser.advance();
                            pos.span++;
                            auto expr = parser.parse_expr();
                            pos.extend(expr->pos());
                            return ast::make_expr<ast::BindingExpr>(token.ident, expr, pos);
                        } else {
                            return parser.parse_expr();
                        }
                    },
                    [&parser]() {
                        bool val = parser.allow(mu::Tkn_Comma);
                        if (val and parser.check(mu::Tkn_CloseBracket)) {
                            parser.report(parser.current().pos(), "expecting expression following comma");
                            return false;
                        }
                        return val;
                    },
                    mu::Parser::append<ast::ExprPtr>
            );
        }
        parser.expect(mu::Tkn_CloseBracket);
        return ast::make_expr<ast::StructExpr>(spec, members, pos);
    }
    else return expr;
}