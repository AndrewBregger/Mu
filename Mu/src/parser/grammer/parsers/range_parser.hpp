//
// Created by Andrew Bregger on 2019-06-07.
//

#ifndef MU_RANGE_PARSER_HPP
#define MU_RANGE_PARSER_HPP

#include "infix_parser.hpp"

#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class RangeParser : public InfixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) override;
    };
}
#endif //MU_RANGE_PARSER_HPP
