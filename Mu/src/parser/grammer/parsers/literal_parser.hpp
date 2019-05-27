//
// Created by Andrew Bregger on 2019-05-20.
//

#ifndef MU_LITERAL_PARSER_HPP
#define MU_LITERAL_PARSER_HPP

#include "prefix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class LiteralParser : public PrefixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, mu::Token token) override;

    };
}


#endif //MU_LITERAL_PARSER_HPP
