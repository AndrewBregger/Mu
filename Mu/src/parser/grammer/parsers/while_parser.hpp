//
// Created by Andrew Bregger on 2019-05-22.
//

#ifndef MU_WHILE_PARSER_HPP
#define MU_WHILE_PARSER_HPP

#include "prefix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class WhileParser : public PrefixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, mu::Token token) override;
    };
}


#endif //MU_WHILE_PARSER_HPP
