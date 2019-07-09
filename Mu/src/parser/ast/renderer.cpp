#include "renderer.hpp"
#include "parser/scanner/token.hpp"

ast::AstRenderer::AstRenderer(bool all, std::ostream& stream) : all(all), stream(stream) {
}

void ast::AstRenderer::indent() {
    ++num_tabs;
}

void ast::AstRenderer::unindent() {
    --num_tabs;
}

void ast::AstRenderer::render_indent() {
    stream << std::string(num_tabs, '\t');
}

void ast::AstRenderer::render(ast::Ident *name) {
    render_indent();
    stream << "Identifier";
    if(all) {
        stream << "(" << name->value() << ")";
    }
    stream << std::endl;
}

void ast::AstRenderer::render(AstNode *node) {
}

void ast::AstRenderer::render(Name *node) {
    render_indent();
    stream << "Name";

    if(all) {
        stream << "(" << node->name->value() << ")";
    }

    stream << std::endl;
}

void ast::AstRenderer::render(NameGeneric *node) {
    render_indent();
    stream << "Generic Name";

    if(all) {
        stream << "(" << node->name->value() << ")";
    }

    stream << std::endl;

    indent();
    for(auto& params : node->type_params)
        params->renderer(this);
    unindent();

}

void ast::AstRenderer::render(Integer *node) {
    render_indent();
    stream << "Integer";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;
}

void ast::AstRenderer::render(Float *node) {
    render_indent();
    stream << "Float";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;

}

void ast::AstRenderer::render(Char *node) {
    render_indent();
    stream << "Character";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;

}

void ast::AstRenderer::render(Str *node) {
    render_indent();
    stream << "String";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;

}

void ast::AstRenderer::render(Bool *node) {
    render_indent();
    stream << "Boolean";
    if(all) {
        stream << "(" << (node->value ? "true" : "false") << ")";
    }
    stream << std::endl;

}

void ast::AstRenderer::render(Nil *node) {
    render_indent();
    stream << "Nil";
    stream << std::endl;
}

void ast::AstRenderer::render(Unit *node) {
    render_indent();
    stream << "Unit";
    stream << std::endl;
}

void ast::AstRenderer::render(Self *node) {
    render_indent();
    stream << "Self Literal";
    stream << std::endl;
}

void ast::AstRenderer::render(Lambda *node) {
    render_indent();
    stream << "Lambda";
    stream << std::endl;
    indent();
    for(auto& p : node->parameters)
        p->renderer(this);
    indent();
    node->body->renderer(this);
    unindent();
    unindent();
}

void ast::AstRenderer::render(TupleExpr *node) {
    render_indent();
    stream << "Tuple Literal";
    stream << std::endl;
    indent();
    for(auto& e : node->elements)
        e->renderer(this);
    unindent();
}

void ast::AstRenderer::render(List *node) {
    render_indent();
    stream << "List Literal";
    stream << std::endl;
    indent();
    for(auto& e : node->elements)
        e->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Map *node) {
    render_indent();
    stream << "Map Literal";
    stream << std::endl;
    indent();
    for(auto& [k, v] : node->elements) {
        k->renderer(this);
        indent();
        v->renderer(this);
        unindent();
    }
    unindent();
}

void ast::AstRenderer::render(Unary *node) {
    render_indent();
    stream << "Unary Expression";
    if(all) {
        stream << "(" << mu::Token::get_string(node->op) << ")";
    }
    stream << std::endl;
    indent();
    node->expr->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Binary *node) {
    render_indent();
    stream << "Binary Expression";
    if(all) {
        stream << "(" << mu::Token::get_string(node->op) << ")";
    }
    stream << std::endl;
    indent();
    node->lhs->renderer(this);
    node->rhs->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Accessor *node) {
    render_indent();
    stream << "Accessor";
    stream << std::endl;
    indent();
    node->operand->renderer(this);
    indent();
    render(node->name);
    unindent();
    unindent();
}

void ast::AstRenderer::render(TupleAcessor *node) {
    render_indent();
    stream << "Tuple Accessor";
    stream << std::endl;
    indent();
    node->operand->renderer(this);
    if(all) {
        indent();
        render_indent();
        stream << node->value << std::endl;
        unindent();
    }
    unindent();
}

void ast::AstRenderer::render(Method *node) {
    render_indent();
    stream << "Method Call";
    stream << std::endl;
    indent();
    node->name->renderer(this);
	indent();
    for(auto& a : node->actuals)
        a->renderer(this);
	unindent();
    unindent();
}

void ast::AstRenderer::render(Cast *node) {
    render_indent();
    stream << "Cast";
    stream << std::endl;
    indent();
    node->operand->renderer(this);
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Block *node) {
    render_indent();
    stream << "Block";
    stream << std::endl;
    indent();
    for(auto& s : node->elements)
        s->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Call *node) {
    render_indent();
    stream << "Function Call";
    stream << std::endl;
    indent();
    node->name->renderer(this);
    for(auto& a : node->actuals)
        a->renderer(this);
    unindent();
}

void ast::AstRenderer::render(If *node) {
    render_indent();
    stream << "If";
    stream << std::endl;
    indent();
    node->cond->renderer(this);
    node->body->renderer(this);
    unindent();
}

void ast::AstRenderer::render(While *node) {
    render_indent();
    stream << "While";
    stream << std::endl;
    indent();
    node->cond->renderer(this);
    node->body->renderer(this);
    unindent();
}

void ast::AstRenderer::render(MatchArm *node) {
    render_indent();
    stream << "Match Arm";
    stream << std::endl;
    indent();
    for(auto& p : node->patterns)
        p->renderer(this);
    node->body->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Match *node) {
    render_indent();
    stream << "Match";
    stream << std::endl;
    indent();
    node->cond->renderer(this);
    for(auto& e : node->members)
        e->renderer(this);
    unindent();
}

void ast::AstRenderer::render(For *node) {
    render_indent();
    stream << "For";
    stream << std::endl;
    indent();
    node->pattern->renderer(this);
    node->expr->renderer(this);
    node->body->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Defer *node) {
    render_indent();
    stream << "Defer";
    stream << std::endl;
    indent();
    node->body->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Return *node) {
    render_indent();
    stream << "Return";
    stream << std::endl;
    indent();
    if(node->body)
        node->body->renderer(this);
    unindent();
}

void ast::AstRenderer::render(BindingExpr *node) {
    render_indent();
    stream << "Binding";
    if(all) {
        stream << "(" << node->name->value() << ")";
    }
    stream << std::endl;
    indent();
    node->expr->renderer(this);
    unindent();
}

void ast::AstRenderer::render(StructExpr *node) {
    render_indent();
    stream << "Struct Expression";
    stream << std::endl;
    indent();
    node->spec->renderer(this);
    for(auto& m : node->members)
        m->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Range *node) {
    render_indent();
    stream << "Range";
    stream << std::endl;
    indent();
    node->start->renderer(this);
    node->end->renderer(this);
    node->step->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Assign *node) {
    render_indent();
    stream << "Assign";

    if(all) {
        stream << "(" << mu::Token::get_string(node->op) << ")";
    }
    stream << std::endl;

    indent();
    node->lvalue->renderer(this);
    node->rvalue->renderer(this);
    unindent();
}

void ast::AstRenderer::render(ExprStmt *node) {
    render_indent();
    stream << "Expression Stmt";
    stream << std::endl;
    indent();
    node->expr->renderer(this);
    unindent();
}

void ast::AstRenderer::render(DeclStmt *node) {
    render_indent();
    stream << "Declaration Stmt";
    stream << std::endl;
    indent();
    node->decl->renderer(this);
    unindent();
}

void ast::AstRenderer::render(EmptyStmt *node) {
    render_indent();
    stream << "Empty Stmt";
    stream << std::endl;
}

void ast::AstRenderer::render(ExprSpec *node) {
    render_indent();
    stream << "Expression Type";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(TupleSpec *node) {
    render_indent();
    stream << "Tuple Type";
    stream << std::endl;
    indent();
    for(auto& e : node->elements)
        e->renderer(this);
    unindent();
}

void ast::AstRenderer::render(ListSpec *node) {
    render_indent();
    stream << "List Type";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    node->size->renderer(this);
    unindent();
}

void ast::AstRenderer::render(DynListSpec *node) {
    render_indent();
    stream << "Dynamic List Type";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(ProcedureSpec *node) {
    render_indent();
    stream << "Function Type";
    stream << std::endl;

    indent();
    for(auto& p : node->params)
        p->renderer(this);
    node->ret->renderer(this);
    unindent();
}

void ast::AstRenderer::render(PtrSpec *node) {
    render_indent();
    stream << "Pointer Type";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(RefSpec *node) {
    render_indent();
    stream << "Reference Type";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(MutSpec *node) {
    render_indent();
    stream << "Mutable Type";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(SelfSpec *node) {
    render_indent();
    stream << "Self Type";
    stream << std::endl;
}

void ast::AstRenderer::render(InferSpec *node) {
    render_indent();
    stream << "Infer Type";
    stream << std::endl;
}

void ast::AstRenderer::render(TypeLitSpec *node) {
    render_indent();
    stream << "Type Literal Type";
    stream << std::endl;
}

void ast::AstRenderer::render(UnitSpec *node) {
    render_indent();
    stream << "Unit Type";
    stream << std::endl;
}

void ast::AstRenderer::render(Local *node) {
    render_indent();
    stream << "Local";
    stream << std::endl;
    indent();
    node->names->renderer(this);
    node->type->renderer(this);
    if(node->init) node->init->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Mutable *node) {
    render_indent();
    stream << "Mutable Local";
    stream << std::endl;
    indent();
    node->names->renderer(this);
    node->type->renderer(this);
    if(node->init) node->init->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Global *node) {
    render_indent();
    stream << "Global";
    stream << std::endl;
    indent();
    render(node->name);
    node->type->renderer(this);
    node->init->renderer(this);
    unindent();
}

void ast::AstRenderer::render(GlobalMut *node) {
    render_indent();
    stream << "Global Mutable";
    stream << std::endl;
    indent();
    render(node->name);
    node->type->renderer(this);
    node->init->renderer(this);
    unindent();
}

void ast::AstRenderer::render(AttributeList *node) {
    render_indent();
    stream << "Attribute List";
    stream << std::endl;
    if(all) {
        indent();
        for(auto attr : node->attributes) {
            render(attr.attr);
            render_indent();
            stream << attr.value;
        }
        unindent();
    }
}

void ast::AstRenderer::render(ProcedureSigniture *node) {
    render_indent();
    stream << "Procedure Signiture";
    stream << std::endl;
    indent();
    if(node->generics) node->generics->renderer(this);
    for(auto p : node->parameters) {
        p->renderer(this);
    }
    if(node->ret) node->ret->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Procedure *node) {
    render_indent();
    stream << "Procedure Declaration";
    stream << std::endl;
    indent();
    if(all) {
        render(&node->attributeList);
        render(node->name);
    }
    render(node->signiture.get());
    node->body->renderer(this);
    unindent();

}

void ast::AstRenderer::render(ProcedureParameter *node) {
    render_indent();
    stream << "Parameter";
    stream << std::endl;
    indent();
    node->pattern->renderer(this);
    node->type->renderer(this);
    if(node->init) node->init->renderer(this);
    unindent();
}

void ast::AstRenderer::render(SelfParameter *node) {
    render_indent();
    stream << "Self Parameter";
    if(all) {
        stream << "("  << (node->mut ? "mutable" : "constant") << ")";
    }
    stream << std::endl;
}

void ast::AstRenderer::render(CVariadicParameter *node) {
    render_indent();
    stream << "C Variadic Parameter";
    stream << std::endl;
    indent();
    node->pattern->renderer(this);
    unindent();
}

void ast::AstRenderer::render(VariadicParameter *node) {
    render_indent();
    stream << "Variadic Parameter";
    stream << std::endl;
    node->pattern->renderer(this);
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Structure *node) {
    render_indent();
    stream << "Structure";
    stream << std::endl;
    indent();
    render(node->name);
    if(node->generics) node->generics->renderer(this);
    for(auto b : node->bounds)
        b->renderer(this);
    for(auto m : node->members)
        m->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Type *node) {
    render_indent();
    stream << "Sum Type";
    stream << std::endl;
    indent();
    render(node->name);
    if(node->generics) node->generics->renderer(this);
    for(auto b : node->bounds)
        b->renderer(this);
    for(auto m : node->members)
        m->renderer(this);
    unindent();
}

void ast::AstRenderer::render(TypeClass *node) {
    render_indent();
    stream << "Trait";
    stream << std::endl;
    indent();
    render(node->name);
    if(node->generics) node->generics->renderer(this);
    for(auto m : node->members)
        m->renderer(this);
    unindent();
}

void ast::AstRenderer::render(UsePath *node) {
    render_indent();
    stream << "Use Path";
    stream << std::endl;
    indent();
    for(auto n : node->path)
        render(n);
    unindent();
}

void ast::AstRenderer::render(UsePathList *node) {
    render_indent();
    stream << "Use Path List";
    stream << std::endl;
    indent();
    for(auto n : node->base)
        render(n);
    for(auto& n : node->subpaths)
        n->renderer(this);
    unindent();
}

void ast::AstRenderer::render(UsePathAlias *node) {
    render_indent();
    stream << "Use Path Alias";
    stream << std::endl;
    indent();
    for(auto n : node->path)
        render(n);
    render(node->alias);
    unindent();
}

void ast::AstRenderer::render(Use *node) {
    render_indent();
    stream << "Use";
    stream << std::endl;
    indent();
    node->use_path->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Alias *node) {
    render_indent();
    stream << "Alias";
    stream << std::endl;
    indent();
    render(node->name);
    node->type->renderer(this);
    unindent();
}

void ast::AstRenderer::render(BoundedGeneric *node) {
    render_indent();
    stream << "Bounded Generic";
    stream << std::endl;
    indent();
    render(node->name);
    for(auto t : node->bounds.type_bounds)
        t->renderer(this);
    unindent();
}

void ast::AstRenderer::render(GenericGroup *node) {
    render_indent();
    stream << "Generic Group";
    stream << std::endl;
    indent();
    for(auto g : node->generics)
        g->renderer(this);
    unindent();
}

void ast::AstRenderer::render(MemberVariable *node) {
    render_indent();
    stream << "Struct Member";
    stream << std::endl;
    indent();
    for(auto n : node->names)
        render(n);
    node->type->renderer(this);
    for(auto i : node->init)
        i->renderer(this);
    unindent();
}

void ast::AstRenderer::render(Impl *node) {
    render_indent();
    stream << "Impl";
    stream << std::endl;
    indent();
    if(node->generics)
        node->generics->renderer(this);
    for(auto n : node->methods)
        n->renderer(this);
    unindent();

}

void ast::AstRenderer::render(TypeMember *node) {
    render_indent();
    stream << "Sum Type Member";
    stream << std::endl;
    indent();
    render(node->name);
    for(auto n : node->types)
        n->renderer(this);
    unindent();
}

void ast::AstRenderer::render(TraitElementType *node) {
    render_indent();
    stream << "Trait Element";
    stream << std::endl;
    indent();
    render(node->name);
    if(node->init) node->init->renderer(this);
    unindent();
}

void ast::AstRenderer::render(IdentPattern *node) {
    render_indent();
    stream << "Ident Pattern";
    stream << std::endl;
    indent();
    render(node->name);
    unindent();
}

void ast::AstRenderer::render(MultiPattern *node) {
    render_indent();
    stream << "Multi Pattern";
    stream << std::endl;
    indent();
    for(auto p : node->patterns)
        p->renderer(this);
    unindent();
}

void ast::AstRenderer::render(TuplePattern *node) {
    render_indent();
    stream << "Tuple Pattern";
    stream << std::endl;
    indent();
    for(auto p : node->patterns)
        p->renderer(this);
    unindent();
}

void ast::AstRenderer::render(StructPattern *node) {
    render_indent();
    stream << "Structure Pattern";
    stream << std::endl;
    indent();
    node->type->renderer(this);
    for(auto m : node->elements)
        m->renderer(this);
    unindent();
}

void ast::AstRenderer::render(ListPattern *node) {
    render_indent();
    stream << "List Pattern";
    stream << std::endl;
    indent();
    for(auto n : node->elements)
        n->renderer(this);
    unindent();
}

void ast::AstRenderer::render(TypePattern *node) {
    render_indent();
    stream << "Sum Type Pattern";
    stream << std::endl;
    indent();
    for(auto n : node->elements)
        n->renderer(this);
    unindent();
}

void ast::AstRenderer::render(IgnorePattern *node) {
    render_indent();
    stream << "Ignore Pattern";
    stream << std::endl;
}

void ast::AstRenderer::render(BindPattern *node) {
    render_indent();
    stream << "Bind Pattern";
    stream << std::endl;
    indent();
    render(node->name);
    node->patterns->renderer(this);
    unindent();
}

void ast::AstRenderer::render(IntPattern *node) {
    render_indent();
    stream << "Integer Pattern";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;
}

void ast::AstRenderer::render(FloatPattern *node) {
    render_indent();
    stream << "Float Pattern";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;

}

void ast::AstRenderer::render(CharPattern *node) {
    render_indent();
    stream << "Character Pattern";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;
}

void ast::AstRenderer::render(StringPattern *node) {
    render_indent();
    stream << "String Pattern";
    if(all) {
        stream << "(" << node->value << ")";
    }
    stream << std::endl;

}

void ast::AstRenderer::render(BoolPattern *node) {
    render_indent();
    stream << "Bool Pattern";
    if(all) {
        stream << "(" << (node->value ? "true" : "false") << ")";
    }
    stream << std::endl;
}

void ast::AstRenderer::render(RangePattern *node) {
    render_indent();
    stream << "Range Pattern";
    stream << std::endl;
    indent();
    node->start->renderer(this);
    node->end->renderer(this);
    unindent();
}

void ast::AstRenderer::render(ModuleFile *node) {
    render_indent();
    stream << "Module File";
    stream << std::endl;
    indent();
    render(node->get_name());
    for(auto e : *node) {
        e->renderer(this);
    }
    unindent();
}

void ast::AstRenderer::render(ModuleDirectory *node) {
    render_indent();
    stream << "Module Directory";
    stream << std::endl;
    indent();
    render(node->get_name());
    for(auto e : *node) {
        e->renderer(this);
    }
    unindent();
}
