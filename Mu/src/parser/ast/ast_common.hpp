#ifndef AST_COMMON_HPP_
#define AST_COMMON_HPP_

#include "common.hpp"

namespace mu {
    struct Pos {
        u64 line;
        u64 column;
        u64 span;
        u64 fid;


        inline Pos() = default;

        inline Pos(u64 line, u64 column, u64 span, u64 fid) : line(line), column(column), span(span), fid(fid) {
        }

        inline Pos extend(const Pos& pos) {
            span += pos.span;
            return *this;
        }

        bool operator== (const mu::Pos& pos) {
            return line == pos.line && column == pos.column &&
                   span == pos.span && fid == pos.fid;
        }
    };
}

namespace ast {
    struct Ident {
        mu::Pos pos;
        std::string value;

        inline Ident(const std::string& value, const mu::Pos& pos) : value(value), pos(pos) {
        }
   };
}

#endif