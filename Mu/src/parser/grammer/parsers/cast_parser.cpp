//
// Created by Andrew Bregger on 2019-06-13.
//

#include "cast_parser.hpp"
#include "parser/parser.hpp"
#include "parser/ast/specs.hpp"

ast::ExprPtr parse::CastParser::lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) {
    // assert that the token given is the expected token.
    parser.passert(op.kind() == mu::Tkn_As);
    parser.advance();

    auto type = parser.parse_spec(false);

    if(!type) {
        parser.report(parser.current().pos(), "expecting type following 'as', found: '%s'",
                parser.current().get_string().c_str());
        return ast::ExprPtr();
    }

    if(type->kind == ast::ast_infer_type) {
        parser.report(type->pos(), "expecting type, found infer type, unable to infer type from context");
        return ast::ExprPtr();
    }

    auto pos = left->pos();
    pos.span += 2;
    pos.extend(type->pos());

    return ast::make_expr<ast::Cast>(left, type, pos);
}
