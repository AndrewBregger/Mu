//
// Created by Andrew Bregger on 2019-05-21.
//

#ifndef MU_PERIOD_PARSER_HPP
#define MU_PERIOD_PARSER_HPP


#include "infix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class PeriodParser : public InfixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) override;
    };
}


#endif //MU_PERIOD_PARSER_HPP
