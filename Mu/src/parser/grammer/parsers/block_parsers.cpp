//
// Created by Andrew Bregger on 2019-05-20.
//

#include "block_parsers.hpp"


#include "parser/ast/ast_common.hpp"
#include "parser/ast/expr.hpp"
#include "parser/ast/stmt.hpp"

#include "parser/parser.hpp"

#include <iostream>

ast::ExprPtr parse::BlockParsers::lud(mu::Parser &parser, mu::Token token) {
    std::cout << parser.current() << std::endl;
    parser.advance(true);
//    std::cout << parser.current() << std::endl;
//    parser.allow(mu::Tkn_NewLine);


    auto pos = token.pos();
    std::vector<ast::StmtPtr> elements;
    if(!parser.check(mu::Tkn_CloseBracket)) {
        parser.many<ast::StmtPtr>([&parser]() {
            std::cout << parser.current() << std::endl;
            auto stmt = parser.parse_stmt();
            return stmt;
        }, [&parser]() {
            return !parser.check(mu::Tkn_CloseBracket);
        }, [&pos, &elements](std::vector<ast::StmtPtr> &, ast::StmtPtr stmt) {
            mu::Parser::append(elements, stmt);
            pos.extend(stmt->pos());
        });
    }

    pos.extend(parser.current().pos());
    parser.expect(mu::Tkn_CloseBracket);

    return ast::make_expr<ast::Block>(std::move(elements), pos);
}
