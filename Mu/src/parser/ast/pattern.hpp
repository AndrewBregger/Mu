//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_PATTERN_HPP
#define MU_PATTERN_HPP

#include "ast_common.hpp"

namespace ast {

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

    struct TypePattern : public Pattern {
        SpecPtr type;
        std::vector<PatternPtr> elements;

        TypePattern(SpecPtr& type, std::vector<PatternPtr>& elements, const mu::Pos& pos) : Pattern(ast_type_desc, pos),
            type(std::move(type)), elements(std::move(elements)) {}
    };

    struct IgnorePattern : public Pattern {
        IgnorePattern(const mu::Pos& pos) : Pattern(ast_ignore_pattern, pos) {}
    };

    struct BindPattern : public Pattern {
        Ident* name;
        PatternPtr patterns;

        BindPattern(Ident* name, PatternPtr& patterns, const mu::Pos& pos) : Pattern(ast_bind_pattern, pos),
            name(name), patterns(std::move(patterns)) {}
    };

    struct IntPattern : public Pattern {
        i64 value;

        IntPattern(i64 value, const mu::Pos& pos) : Pattern(ast_int_pattern, pos), value(value) {}
    };

    struct FloatPattern : public Pattern {
        f64 value;

        FloatPattern(f64 value, const mu::Pos& pos) : Pattern(ast_float_pattern, pos), value(value) {}
    };

    struct CharPattern : public Pattern {
        char value;

        CharPattern(char value, const mu::Pos& pos) : Pattern(ast_char_pattern, pos), value(value) {}
    };

    struct StringPattern : public Pattern {
        std::string value;

        StringPattern(const std::string& value, const mu::Pos& pos) : Pattern(ast_string_pattern, pos), value(value) {}
    };

    struct BoolPattern : public Pattern {
        bool value;

        BoolPattern(bool value, const mu::Pos& pos) : Pattern(ast_bool_pattern, pos), value(value) {}
    };

    struct RangePattern : public Pattern {
        PatternPtr start;
        PatternPtr end;

        RangePattern(PatternPtr& start, PatternPtr& end, const mu::Pos& pos) : Pattern(ast_range_pattern, pos),
            start(std::move(start)), end(std::move(end)) {}
    };
}

#endif //MU_PATTERN_HPP
