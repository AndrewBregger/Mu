//
// Created by Andrew Bregger on 2019-05-20.
//

#include "parser/ast/expr.hpp"
#include "infix_parser.hpp"
#include "parser/parser.hpp"

ast::ExprPtr parse::InfixParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
    parser.advance();

    if(op.is_assignment()) {
        if(left->kind == ast::ast_assign) {
            parser.report(left->pos(), "left expression of an asigned is an assigned");
            return ast::ExprPtr();
        }

        auto rhs = parser.parse_expr(op.prec());
        auto pos = left->pos();

        pos = pos.extend(op.pos()).extend(rhs->pos());
        return ast::make_expr<ast::Assign>(op.kind(), left, rhs, pos);
    }
    else {
        auto rhs = parser.parse_expr(op.prec());
        auto pos = left->pos();

        if(rhs->kind == ast::ast_assign) {
            parser.report(rhs->pos(), "the right expression is an assigned");
            return ast::ExprPtr();
        }

        pos = pos.extend(op.pos()).extend(rhs->pos());
        return ast::make_expr<ast::Binary>(op.kind(), left, rhs, pos);
    }
}
