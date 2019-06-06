//
// Created by Andrew Bregger on 2019-06-06.
//

#ifndef MU_LAMBDA_PARSER_HPP
#define MU_LAMBDA_PARSER_HPP

#include "prefix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class LambdaParser : public PrefixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, mu::Token token) override;
    };
}


#endif //MU_LAMBDA_PARSER_HPP
