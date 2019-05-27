//
// Created by Andrew Bregger on 2019-05-20.
//

#ifndef MU_GRAMMER_HPP
#define MU_GRAMMER_HPP

#include "parser/grammer/parsers/infix_parser.hpp"
#include "parser/grammer/parsers/prefix_parser.hpp"

#include <unordered_map>
#include <memory>

namespace parse {
    class Grammar {
    public:
        Grammar();

        void register_parser(mu::TokenKind tok, parse::InfixParser *parser);

        void register_parser(mu::TokenKind tok, parse::PrefixParser *parser);

        parse::InfixParser *get_infix(const mu::Token &token);

        parse::PrefixParser *get_prefix(const mu::Token &token);

    private:
        std::unordered_map<mu::TokenKind, std::unique_ptr<parse::InfixParser>> infix_parser;
        std::unordered_map<mu::TokenKind, std::unique_ptr<parse::PrefixParser>> prefix_parser;
    };
}


#endif //MU_GRAMMER_HPP
