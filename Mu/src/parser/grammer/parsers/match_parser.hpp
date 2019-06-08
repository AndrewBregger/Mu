//
// Created by Andrew Bregger on 2019-06-06.
//

#ifndef MU_MATCH_PARSER_HPP
#define MU_MATCH_PARSER_HPP

#include "prefix_parser.hpp"
#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace parse {
    class MatchParser : public PrefixParser {
    public:
        ast::ExprPtr lud(mu::Parser &parser, mu::Token token) override;

        ast::ExprPtr parse_arm(mu::Parser& parser);
    };
}


#endif //MU_MATCH_PARSER_HPP
