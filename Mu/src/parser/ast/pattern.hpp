//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_PATTERN_HPP
#define MU_PATTERN_HPP

#include "ast_common.hpp"

namespace ast {

    struct IdentPattern : public Pattern {
        Ident* name;

        IdentPattern(Ident* name, const mu::Pos& pos);
    };

    struct MultiPattern : public Pattern {
        std::vector<PatternPtr> patterns;

        MultiPattern(std::vector<PatternPtr>& patterns, const mu::Pos& pos);
    };

    struct TuplePattern : public Pattern {
        std::vector<PatternPtr> patterns;

        TuplePattern(std::vector<PatternPtr>& patterns, const mu::Pos& pos);
    };

    struct StructPattern : public Pattern {
        SpecPtr type;
        std::vector<PatternPtr> elements;

        StructPattern(SpecPtr& type, std::vector<PatternPtr>& elements, const mu::Pos& pos);
    };

    struct ListPattern : public Pattern {
        std::vector<PatternPtr> elements;

        ListPattern(std::vector<PatternPtr>& elements, const mu::Pos& pos);
    };

    struct TypePattern : public Pattern {
        SpecPtr type;
        std::vector<PatternPtr> elements;

        TypePattern(SpecPtr& type, std::vector<PatternPtr>& elements, const mu::Pos& pos);
    };

    struct IgnorePattern : public Pattern {
        IgnorePattern(const mu::Pos& pos);
    };

    struct BindPattern : public Pattern {
        Ident* name;
        PatternPtr patterns;

        BindPattern(Ident* name, PatternPtr& patterns, const mu::Pos& pos);
    };

    struct IntPattern : public Pattern {
        i64 value;

        IntPattern(i64 value, const mu::Pos& pos);
    };

    struct FloatPattern : public Pattern {
        f64 value;

        FloatPattern(f64 value, const mu::Pos& pos);
    };

    struct CharPattern : public Pattern {
        char value;

        CharPattern(char value, const mu::Pos& pos);
    };

    struct StringPattern : public Pattern {
        std::string value;

        StringPattern(const std::string& value, const mu::Pos& pos);
    };

    struct BoolPattern : public Pattern {
        bool value;

        BoolPattern(bool value, const mu::Pos& pos);
    };

    struct RangePattern : public Pattern {
        PatternPtr start;
        PatternPtr end;

        RangePattern(PatternPtr& start, PatternPtr& end, const mu::Pos& pos);
    };
}

#endif //MU_PATTERN_HPP
