//
// Created by Andrew Bregger on 2019-05-22.
//

#ifndef MU_FUNCTION_PARSER_HPP
#define MU_FUNCTION_PARSER_HPP

#include "infix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"


namespace parse {
    class CallParser : public InfixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, ast::ExprPtr left, mu::Token op) override;
    };
}


#endif //MU_FUNCTION_PARSER_HPP
