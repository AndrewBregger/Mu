#ifndef MU_RENDERER_HPP_
#define MU_RENDERER_HPP_

#include "common.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "pattern.hpp"
#include "specs.hpp"
#include "stmt.hpp"
#include "module.hpp"

namespace ast {
    class AstRenderer {
    public:
        AstRenderer(bool all, std::ostream& stream);
        void render(AstNode* node);

        void render(Name* node);
        void render(NameGeneric* node);
        void render(Integer* node);
        void render(Float* node);
        void render(Char* node);
        void render(Str* node);
        void render(Bool* node);
        void render(Nil* node);
        void render(Unit* node);
        void render(Self* node);
        void render(Lambda* node);
        void render(TupleExpr* node);
        void render(List* node);
        void render(Map* node);
        void render(Unary* node);
        void render(Binary* node);
        void render(Accessor* node);
        void render(TupleAcessor* node);
        void render(Method* node);
        void render(Cast* node);
        void render(Block* node);
        void render(Call* node);
        void render(If* node);
        void render(While* node);
        void render(MatchArm* node);
        void render(Match* node);
        void render(For* node);
        void render(Defer* node);
        void render(Return* node);
        void render(BindingExpr* node);
        void render(StructExpr* node);
        void render(Range* node);
        void render(Assign* node);
        void render(ExprStmt* node);
        void render(DeclStmt* node);
        void render(EmptyStmt* node);
        void render(ExprSpec* node);
        void render(TupleSpec* node);
        void render(ListSpec* node);
        void render(DynListSpec* node);
        void render(ProcedureSpec* node);
        void render(PtrSpec* node);
        void render(RefSpec* node);
        void render(MutSpec* node);
        void render(SelfSpec* node);
        void render(InferSpec* node);
        void render(TypeLitSpec* node);
        void render(UnitSpec* node);
        void render(Local* node);
        void render(Mutable* node);
        void render(Global* node);
        void render(GlobalMut* node);
        void render(AttributeList* node);
        void render(ProcedureSigniture* node);
        void render(Procedure* node);
        void render(ProcedureParameter* node);
        void render(SelfParameter* node);
        void render(CVariadicParameter* node);
        void render(VariadicParameter* node);
        void render(Structure* node);
        void render(Type* node);
        void render(TypeClass* node);
        void render(UsePath* node);
        void render(UsePathList* node);
        void render(UsePathAlias* node);
        void render(Use* node);
        void render(Alias* node);
        void render(BoundedGeneric* node);
        void render(GenericGroup* node);
        void render(MemberVariable* node);
        void render(Impl* node);
        void render(TypeMember* node);
        void render(TraitElementType* node);
        void render(IdentPattern* node);
        void render(MultiPattern* node);
        void render(TuplePattern* node);
        void render(StructPattern* node);
        void render(ListPattern* node);
        void render(TypePattern* node);
        void render(IgnorePattern* node);
        void render(BindPattern* node);
        void render(IntPattern* node);
        void render(FloatPattern* node);
        void render(CharPattern* node);
        void render(StringPattern* node);
        void render(BoolPattern* node);
        void render(RangePattern* node);

        void render(ModuleFile* node);
        void render(ModuleDirectory* node);

        void render(Ident* name);

        void indent();
        void unindent();

        void render_indent();

    private:
        u32 num_tabs{0};
        bool all{false};
        std::ostream& stream;
    };
}

#endif