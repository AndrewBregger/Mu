//
// Created by Andrew Bregger on 2019-05-20.
//

#ifndef MU_PREFIXPARSER_HPP
#define MU_PREFIXPARSER_HPP

#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"

namespace mu {
    class Parser;
}

namespace parse {
    class PrefixParser {
    public:
        virtual ~PrefixParser() = default;

        virtual ast::ExprPtr lud(mu::Parser& parser, mu::Token token) = 0;
    };
}


#endif //MU_PREFIXPARSER_HPP
