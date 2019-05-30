//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_PATTERN_HPP
#define MU_PATTERN_HPP

#include "ast_common.hpp"

namespace ast {
    struct Pattern : public AstNode {
        Pattern(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}
    };

    struct IdentPattern : public Pattern {
        Ident* name;

        IdentPattern(Ident* name, const mu::Pos& pos) : Pattern(ast_ident, pos), name(name) {}
    };

    struct MultiPattern : public Pattern {
        std::vector<PatternPtr> patterns;

        MultiPattern(std::vector<PatternPtr>& patterns, const mu::Pos& pos) : Pattern(ast_multi, pos),
            patterns(std::move(patterns)) {}
    };

    struct TuplePattern : public Pattern {
        std::vector<PatternPtr> patterns;

        TuplePattern(std::vector<PatternPtr>& patterns, const mu::Pos& pos) : Pattern(ast_tuple_desc, pos),
        patterns(std::move(patterns)) {}
    };

    struct StructPattern : public Pattern {
        SpecPtr type;
        std::vector<PatternPtr> elements;

        StructPattern(SpecPtr& type, std::vector<PatternPtr>& elements, const mu::Pos& pos) :
            Pattern(ast_struct_desc, pos), type(std::move(type)), elements(std::move(elements)) {}
    };

    struct ListPattern : public Pattern {
        std::vector<PatternPtr> elements;

        ListPattern(std::vector<PatternPtr>& elements, const mu::Pos& pos) : Pattern(ast_list_desc, pos),
        elements(std::move(elements)) {}
    };
//    ast_type_desc,
}

#endif //MU_PATTERN_HPP
