//
// Created by Andrew Bregger on 2019-05-20.
//

#ifndef MU_BLOCK_PARSERS_HPP
#define MU_BLOCK_PARSERS_HPP

#include "prefix_parser.hpp"


namespace mu {
    class Parser;
}

namespace parse {
    class BlockParsers : public PrefixParser {
        public:
        ast::ExprPtr lud(mu::Parser &parser, mu::Token token) override;
    };
}


#endif //MU_BLOCK_PARSERS_HPP
