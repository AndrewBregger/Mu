//
// Created by Andrew Bregger on 2019-06-07.
//

#include "range_parser.hpp"

#include "parser/parser.hpp"
#include "parser/ast/expr.hpp"


ast::ExprPtr parse::RangeParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
    auto pos = left->pos();
    if(left->kind == ast::ast_range) {
        parser.report(parser.current().pos(), "the start of a range must not be a range");
        return ast::ExprPtr();
    }

    auto expr = parser.parse_expr();

    if(!expr)
        return expr;

    if(expr->kind == ast::ast_range) {
        parser.report(parser.current().pos(), "the end of a range must not be a range");
        return ast::ExprPtr();
    }

    ast::ExprPtr step;
    if(parser.allow(mu::Tkn_Comma)) {
        step = parser.parse_expr();
        if (step)
            pos.extend(step->pos());
        else {
            parser.report(parser.current().pos(), "expecting expression following ','");
            return step;
        }
    }
    return ast::make_expr<ast::Range>(left, expr, step, pos);

}
