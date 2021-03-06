#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include "common.hpp"
#include <string>
#include <fstream>
#include "parser/ast/ast_common.hpp"

#define TOKEN_KINDS \
    TOKEN_KIND(Error, "error") \
	TOKEN_KIND(None, "none") \
    TOKEN_KIND(Comment, "Comment") \
    TOKEN_KIND(Eof, "__EOF__") \
    TOKEN_KIND(IntLiteral, "integer literal") \
    TOKEN_KIND(FloatLiteral, "float literal") \
    TOKEN_KIND(StringLiteral, "string literal") \
    TOKEN_KIND(CharLiteral, "character literal") \
    TOKEN_KIND(Identifier, "") \
	TOKEN_KIND(NewLine, "") \
    TOKEN_KIND(OpenParen, "(") \
    TOKEN_KIND(CloseParen, ")") \
    TOKEN_KIND(OpenBrace, "[") \
    TOKEN_KIND(CloseBrace, "]") \
    TOKEN_KIND(OpenBracket, "{") \
    TOKEN_KIND(CloseBracket, "}") \
    TOKEN_KIND(BackSlash, "\\") \
    TOKEN_KIND(Period, ".") \
    TOKEN_KIND(PeriodPeriod, "..") \
    TOKEN_KIND(PeriodPeriodPeriod, "...") \
    TOKEN_KIND(Comma, ",") \
    TOKEN_KIND(Colon, ":") \
    TOKEN_KIND(Semicolon, ";") \
    TOKEN_KIND(ColonEqual, ":=") \
    TOKEN_KIND(ColonColon, "::") \
    TOKEN_KIND(MinusGreater, "->") \
    TOKEN_KIND(Unit, "<>") \
    TOKEN_KIND(Hash, "#") \
    TOKEN_KIND(Dollar, "$") \
    TOKEN_KIND(At, "@") \
    TOKEN_KIND(Arrow, "=>") \
    TOKEN_KIND(Plus, "+") \
    TOKEN_KIND(Minus, "-") \
    TOKEN_KIND(Slash, "/") \
    TOKEN_KIND(Percent, "%") \
    TOKEN_KIND(Astrick, "*") \
    TOKEN_KIND(AstrickAstrick, "**") \
    TOKEN_KIND(LessLess, "<<") \
    TOKEN_KIND(GreaterGreater, ">>") \
    TOKEN_KIND(Ampersand, "&") \
    TOKEN_KIND(Pipe, "|") \
    TOKEN_KIND(Carrot, "^") \
    TOKEN_KIND(Tilde, "~") \
    TOKEN_KIND(Bang, "!") \
    TOKEN_KIND(Less, "<") \
    TOKEN_KIND(Greater, ">") \
    TOKEN_KIND(LessEqual, "<=") \
    TOKEN_KIND(GreaterEqual, ">=") \
    TOKEN_KIND(EqualEqual, "==") \
    TOKEN_KIND(BangEqual, "!=") \
    TOKEN_KIND(Equal, "=") \
    TOKEN_KIND(PlusEqual, "+=") \
    TOKEN_KIND(MinusEqual, "-=") \
    TOKEN_KIND(AstrickEqual, "*=") \
    TOKEN_KIND(SlashEqual, "/=") \
    TOKEN_KIND(PercentEqual, "%=") \
    TOKEN_KIND(AstrickAstrickEqual, "**=") \
    TOKEN_KIND(LessLessEqual, "<<=") \
    TOKEN_KIND(GreaterGreaterEqual, ">>=") \
    TOKEN_KIND(CarrotEqual, "^=") \
    TOKEN_KIND(AmpersandEqual, "&=") \
    TOKEN_KIND(PipeEqual, "|=") \
    TOKEN_KIND(Underscore, "_") \
    TOKEN_KIND(If, "if") \
    TOKEN_KIND(Else, "else") \
    TOKEN_KIND(Elif, "elif") \
    TOKEN_KIND(Let, "let") \
    TOKEN_KIND(Mut, "mut") \
    TOKEN_KIND(Type, "type") \
    TOKEN_KIND(TypeLit, "Type") \
    TOKEN_KIND(Struct, "struct") \
    TOKEN_KIND(Break, "break") \
    TOKEN_KIND(Continue, "continue") \
    TOKEN_KIND(Return, "return") \
    TOKEN_KIND(While, "while") \
    TOKEN_KIND(Derive, "derive") \
    TOKEN_KIND(Where, "where") \
    TOKEN_KIND(Inline, "inline") \
	TOKEN_KIND(Defer, "defer") \
    TOKEN_KIND(For, "for") \
    TOKEN_KIND(Match, "match") \
    TOKEN_KIND(Loop, "loop") \
    TOKEN_KIND(Sizeof, "sizeof") \
    TOKEN_KIND(Alignof, "alignof") \
    TOKEN_KIND(Async, "async") \
    TOKEN_KIND(Await, "await") \
    TOKEN_KIND(Impl, "impl") \
    TOKEN_KIND(Use, "use") \
    TOKEN_KIND(And, "and") \
    TOKEN_KIND(Or, "or") \
    TOKEN_KIND(In, "in") \
    TOKEN_KIND(True, "true") \
    TOKEN_KIND(Ref, "ref") \
    TOKEN_KIND(False, "false") \
    TOKEN_KIND(Nil, "nil") \
    TOKEN_KIND(Self, "self") \
    TOKEN_KIND(SelfType, "Self") \
    TOKEN_KIND(Pub, "pub") \
    TOKEN_KIND(Alias, "alias") \
    TOKEN_KIND(Trait, "trait") \
    TOKEN_KIND(UnitName, "Unit") \
    TOKEN_KIND(As, "as")

struct Atom;

namespace mu {

    enum TokenKind {
#define TOKEN_KIND(n, ...) Tkn_##n,
        TOKEN_KINDS
#undef TOKEN_KIND
    };

#define TOKEN_CONSTRUCTOR_DEF(Type) Token(Type elem, const mu::Pos& pos)

    enum Associative {
        Right,
        Left,
        None
    };

    struct Token {
        Token();

		Token(TokenKind kind, const Pos& pos);

		TOKEN_CONSTRUCTOR_DEF(u64);
		TOKEN_CONSTRUCTOR_DEF(f64);
		TOKEN_CONSTRUCTOR_DEF(char);
		TOKEN_CONSTRUCTOR_DEF(const std::string&);
		TOKEN_CONSTRUCTOR_DEF(ast::Ident*);

        ~Token();

        const std::string& get_string();

        static const std::string& get_string(TokenKind kind);
        static TokenKind keyword(const std::string& str);

        inline TokenKind kind() const { return tokenKind; }
		inline TokenKind kind() { return tokenKind; }
		inline const Pos& pos() { return position; }

        bool is_operator();

        bool is_assignment();

        i32 prec();
        Associative acc();

        TokenKind tokenKind;
		Pos position;

		friend std::ostream& operator<< (std::ostream& out, const Token& t);
    
        std::string str;
//		enum {
			u64 integer;
            f64 floating;
            char character;
			ast::Ident* ident;
 //       };
    };
}

#endif