//
// Created by Andrew Bregger on 2019-05-20.
//

#ifndef MU_NAME_PARSER_HPP
#define MU_NAME_PARSER_HPP

#include "prefix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class NameParser : public PrefixParser {
    public:
        virtual ast::ExprPtr lud(mu::Parser& parser, mu::Token token);
    };
}


#endif //MU_NAME_PARSER_HPP
