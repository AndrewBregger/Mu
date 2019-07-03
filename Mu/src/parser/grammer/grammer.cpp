//
// Created by Andrew Bregger on 2019-05-20.
//

#include "grammer.hpp"

#include "parsers/infix_parser.hpp"
#include "parsers/range_parser.hpp"
#include "parsers/cast_parser.hpp"



parse::Grammar::Grammar() {
    // register all of the parser
    register_parser(mu::Tkn_Plus, new InfixParser);
    register_parser(mu::Tkn_Minus, new InfixParser);
    register_parser(mu::Tkn_Slash, new InfixParser);
    register_parser(mu::Tkn_Astrick, new InfixParser);

    register_parser(mu::Tkn_Less, new InfixParser);
    register_parser(mu::Tkn_Greater, new InfixParser);
    register_parser(mu::Tkn_LessEqual, new InfixParser);
    register_parser(mu::Tkn_GreaterEqual, new InfixParser);

    register_parser(mu::Tkn_LessLess, new InfixParser);
    register_parser(mu::Tkn_GreaterGreater, new InfixParser);
    register_parser(mu::Tkn_LessLessEqual, new InfixParser);
    register_parser(mu::Tkn_GreaterGreaterEqual, new InfixParser);

    register_parser(mu::Tkn_Percent, new InfixParser);
    register_parser(mu::Tkn_AstrickAstrick, new InfixParser);

    register_parser(mu::Tkn_And, new InfixParser);
    register_parser(mu::Tkn_Or, new InfixParser);
    register_parser(mu::Tkn_Ampersand, new InfixParser);
    register_parser(mu::Tkn_Pipe, new InfixParser);

	register_parser(mu::Tkn_PeriodPeriod, new RangeParser);
    register_parser(mu::Tkn_As, new CastParser);
}

void parse::Grammar::register_parser(mu::TokenKind tok, parse::InfixParser *parser) {
    infix_parser.emplace(tok, parser);
}

void parse::Grammar::register_parser(mu::TokenKind tok, parse::PrefixParser *parser) {
    prefix_parser.emplace(tok, parser);
}

parse::InfixParser *parse::Grammar::get_infix(const mu::Token &token) {
    auto iter = infix_parser.find(token.kind());
    if(iter == infix_parser.end())
        return nullptr;
    else
        return iter->second.get();
}

parse::PrefixParser *parse::Grammar::get_prefix(const mu::Token &token) {
    auto iter = prefix_parser.find(token.kind());
    if(iter == prefix_parser.end())
        return nullptr;
    else
        return iter->second.get();
}

