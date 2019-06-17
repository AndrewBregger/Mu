#include "pattern.hpp"

namespace ast {
    IdentPattern::IdentPattern(Ident* name, const mu::Pos& pos) : Pattern(ast_ident, pos), name(name) {}

    MultiPattern::MultiPattern(std::vector<PatternPtr>& patterns, const mu::Pos& pos) : Pattern(ast_multi, pos),
    patterns(std::move(patterns)) {}

    TuplePattern::TuplePattern(std::vector<PatternPtr>& patterns, const mu::Pos& pos) : Pattern(ast_tuple_desc, pos),
    patterns(std::move(patterns)) {}

    StructPattern::StructPattern(SpecPtr& type, std::vector<PatternPtr>& elements, const mu::Pos& pos) :
    Pattern(ast_struct_desc, pos), type(std::move(type)), elements(std::move(elements)) {}

    ListPattern::ListPattern(std::vector<PatternPtr>& elements, const mu::Pos& pos) : Pattern(ast_list_desc, pos),
    elements(std::move(elements)) {}

TypePattern::TypePattern(SpecPtr& type, std::vector<PatternPtr>& elements, const mu::Pos& pos) : Pattern(ast_type_desc, pos),
type(std::move(type)), elements(std::move(elements)) {}

IgnorePattern::IgnorePattern(const mu::Pos& pos) : Pattern(ast_ignore_pattern, pos) {}

BindPattern::BindPattern(Ident* name, PatternPtr& patterns, const mu::Pos& pos) : Pattern(ast_bind_pattern, pos),
name(name), patterns(std::move(patterns)) {}

IntPattern::IntPattern(i64 value, const mu::Pos& pos) : Pattern(ast_int_pattern, pos), value(value) {}

FloatPattern::FloatPattern(f64 value, const mu::Pos& pos) : Pattern(ast_float_pattern, pos), value(value) {}

CharPattern::CharPattern(char value, const mu::Pos& pos) : Pattern(ast_char_pattern, pos), value(value) {}

StringPattern::StringPattern(const std::string& value, const mu::Pos& pos) : Pattern(ast_string_pattern, pos), value(value) {}

BoolPattern::BoolPattern(bool value, const mu::Pos& pos) : Pattern(ast_bool_pattern, pos), value(value) {}

RangePattern::RangePattern(PatternPtr& start, PatternPtr& end, const mu::Pos& pos) : Pattern(ast_range_pattern, pos),
start(std::move(start)), end(std::move(end)) {}
}