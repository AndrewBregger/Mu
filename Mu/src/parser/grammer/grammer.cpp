//
// Created by Andrew Bregger on 2019-05-20.
//

#include "grammer.hpp"

#include "parser/grammer/parsers/name_parser.hpp"
#include "parser/grammer/parsers/prefixop_parser.hpp"
#include "parser/grammer/parsers/literal_parser.hpp"
#include "parser/grammer/parsers/block_parsers.hpp"
#include "parser/grammer/parsers/period_parser.hpp"
#include "parser/grammer/parsers/call_parser.hpp"
#include "parser/grammer/parsers/parethesis_parser.hpp"
#include "parser/grammer/parsers/compoundeliteral_parser.hpp"
#include "parser/grammer/parsers/if_parser.hpp"
#include "parser/grammer/parsers/for_parser.hpp"
#include "parser/grammer/parsers/while_parser.hpp"
#include "parser/grammer/parsers/lambda_parser.hpp"
#include "parser/grammer/parsers/match_parser.hpp"
#include "parser/grammer/parsers/defer_parser.hpp"
#include "parser/grammer/parsers/return_parser.hpp"
#include "parser/grammer/parsers/cast_parser.hpp"



parse::Grammar::Grammar() {
    // register all of the parser

    register_parser(mu::Tkn_Identifier, new NameParser);
    register_parser(mu::Tkn_Self, new NameParser);
    register_parser(mu::Tkn_SelfType, new NameParser);

    register_parser(mu::Tkn_Minus, new PrefixOpParser);
    register_parser(mu::Tkn_Tilde, new PrefixOpParser);
    register_parser(mu::Tkn_Bang, new PrefixOpParser);
    register_parser(mu::Tkn_Ampersand, new PrefixOpParser);
    register_parser(mu::Tkn_Astrick, new PrefixOpParser);

    register_parser(mu::Tkn_IntLiteral, new LiteralParser);
    register_parser(mu::Tkn_FloatLiteral, new LiteralParser);
    register_parser(mu::Tkn_CharLiteral, new LiteralParser);
    register_parser(mu::Tkn_StringLiteral, new LiteralParser);

    register_parser(mu::Tkn_Nil, new LiteralParser);
    register_parser(mu::Tkn_True, new LiteralParser);
    register_parser(mu::Tkn_False, new LiteralParser);

    register_parser(mu::Tkn_OpenBracket, new BlockParsers);
    register_parser(mu::Tkn_OpenBrace, new CompoundedLiteralParser);
    register_parser(mu::Tkn_OpenParen, new ParethesisParser);

    register_parser(mu::Tkn_If, new IfParser);
    register_parser(mu::Tkn_For, new ForParser);
    register_parser(mu::Tkn_While, new WhileParser);
    register_parser(mu::Tkn_Defer, new DeferParser);
    register_parser(mu::Tkn_Match, new MatchParser);
    register_parser(mu::Tkn_Return, new ReturnParser);


    register_parser(mu::Tkn_BackSlash, new LambdaParser);

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

    register_parser(mu::Tkn_As, new CastParser);

    register_parser(mu::Tkn_Period, new PeriodParser);
//    register_parser(mu::Tkn_OpenParen, new CallParser);

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

