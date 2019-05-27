//
// Created by Andrew Bregger on 2019-05-20.
//

#ifndef MU_INFIX_PARSER_HPP
#define MU_INFIX_PARSER_HPP

#include "parser/ast/ast_common.hpp"
#include "parser/scanner/token.hpp"


namespace mu {
    class Parser;
}

namespace parse {
    class InfixParser {
    public:
        virtual ~InfixParser() = default;
        virtual ast::ExprPtr lud(mu::Parser& parser, ast::ExprPtr left, mu::Token op);
    };
}


#endif //MU_INFIX_PARSER_HPP
