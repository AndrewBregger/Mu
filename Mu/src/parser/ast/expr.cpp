#include "expr.hpp"
#include "renderer.hpp"

namespace ast {

    Name::Name(Ident* name, const mu::Pos& pos) : Expr(ast_name, pos), name(name) {
        assert(name);
    }

//    Name::~Name()  = default;

//        inline std::string str() { return name->val->value; }

//        std::ostream&operator<< (std::ostream& out)  {
//           Expr::operator<<(out);
//           out << "name: " << str() << std::endl;
//           return out;
//        }

    NameGeneric::NameGeneric(Ident* name, std::vector<ast::SpecPtr>& type_params, const mu::Pos& pos) : Expr(ast_name_generic, pos), name(name), type_params(std::move(type_params)) {}

    Integer::Integer(u64 value, const mu::Pos& pos) : Expr(ast_integer, pos), value(value) {}
    Integer::~Integer()  = default;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }
//    };

    Float::Float(f64 value, const mu::Pos& pos) : Expr(ast_fl, pos), value(value) {}
    Float::~Float()  = default;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;

    Char::Char(char value, const mu::Pos& pos) : Expr(ast_ch, pos), value(value) {}
    Char::~Char()  = default;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }

    Str::Str(const std::string& value, const mu::Pos& pos) : Expr(ast_str, pos), value(value) {}
    Str::~Str()  = default;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }

    Bool::Bool(bool value, const mu::Pos& pos) : Expr(ast_bool, pos), value(value) {}

    Nil::Nil(const mu::Pos& pos) : Expr(ast_nil, pos) {}

    Unit::Unit(const mu::Pos& pos) : Expr(ast_unit_expr, pos) {}

    Self::Self(const mu::Pos& pos): Expr(ast_self_expr, pos) {}


    Lambda::Lambda(std::vector<DeclPtr>& parameters, SpecPtr& ret, ExprPtr& body, const mu::Pos& pos) :
                Expr(ast_lambda, pos), parameters(std::move(parameters)), ret(std::move(ret)),
                body(std::move(body)) {}

    Lambda::~Lambda() = default;

    TupleExpr::TupleExpr(std::vector<ExprPtr>& elements, const mu::Pos& pos) : Expr(ast_tuple_expr, pos),
                                                                        elements(std::move(elements)){}

    TupleExpr::~TupleExpr() = default;

    List::List(std::vector<ExprPtr>& elements, const mu::Pos& pos) : Expr(ast_list, pos),
                                                                   elements(std::move(elements)) {}

//     List::~List()  = default;


//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << std::endl;
//            int i = 0;
//            for(auto& element : elements) {
//                out << "\t" << i++ << " ";
//                element->operator<<(out);
//                out << std::endl;
//            }
//            return out;
//        }


    Map::Map(std::vector<std::tuple<ExprPtr, ExprPtr>>& elements, const mu::Pos& pos) : Expr(ast_map, pos),
                                                                                   elements(std::move(elements)) {}

//    Map::~Map()  = default;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << std::endl;
//            int i = 0;
//
//            for(auto& [key, value] : elements) {
//                out << "\t" << i++ << " ";
//                key->operator<<(out);
//                out << "|";
//                value->operator<<(out);
//                out << std::endl;
//            }
//            return out;
//        }

        Unary::Unary(mu::TokenKind op, ExprPtr& expr, const mu::Pos& pos) : Expr(ast_unary, pos), op(op), expr(std::move(expr)) {}

        Unary::~Unary() = default;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "op: " << mu::Token::get_string(op) << std::endl;
//            expr->operator<<(out) << std::endl;
//            return out;
//        }

        Binary::Binary(mu::TokenKind op, ExprPtr& lhs, ExprPtr& rhs, const mu::Pos& pos) : Expr(ast_binary, pos),
                                                                                   op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
        Binary::~Binary() = default;
//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "op: " << mu::Token::get_string(op) << std::endl;
//            lhs->operator<<(out) << std::endl;
//            rhs->operator<<(out) << std::endl;
//            return out;
//        }


        Accessor::Accessor(ExprPtr& operand, Ident* name, const mu::Pos& pos) : Expr(ast_accessor, pos),
                                                                      operand(std::move(operand)), name(name) {}
        Accessor::~Accessor() = default;


        TupleAcessor::TupleAcessor(ExprPtr& operand, u64 value, const mu::Pos& pos) : Expr(ast_tuple_accessor, pos),
            operand(std::move(operand)), value(value) {}

        TupleAcessor::~TupleAcessor() = default;


        Method::Method(ExprPtr name, const std::vector<ExprPtr>& actuals, const mu::Pos& pos):
			Expr(ast_method, pos), name(std::move(name)), actuals(actuals) {}

        Method::~Method() = default;

        Cast::Cast(ExprPtr& operand, SpecPtr& type, const mu::Pos& pos) :
                Expr(ast_cast_expr, pos), operand(std::move(operand)), type(std::move(type)) {}
        Cast::~Cast() = default;


        Block::Block(std::vector<StmtPtr>& elements, const mu::Pos& pos) : Expr(ast_block, pos),
                                                                    elements(std::move(elements)) {}
        Block::~Block() = default;
//        std::ostream&operator<< (std::ostream& out)  {
//            Expr::operator<<(out);
//
//            i32 i = 0;
//            for(auto& element : elements) {
//                out << "\t" << i++ << " ";
//                ((AstNode*) element.get())->operator<<(out);
//                out << std::endl;
//            }
//
//            return out;
//        }


        Call::Call(ExprPtr &operand, const std::vector<ExprPtr> &actuals,
             const mu::Pos &pos) : Expr(ast_call, pos), name(std::move(operand)), actuals(std::move(actuals)) {}
        Call::~Call() = default;


        If::If(ExprPtr& cond, ExprPtr& body, ExprPtr& else_if, const mu::Pos& pos) : Expr(ast_if_expr, pos),
                                                                                 cond(std::move(cond)), body(std::move(body)), else_if(std::move(else_if)) {}
        If::~If() = default;

        While::While(ExprPtr& cond, ExprPtr& body, const mu::Pos& pos) : Expr(ast_while_expr, pos),
                                                                  cond(std::move(cond)), body(std::move(body)) {}
        While::~While() = default;


        MatchArm::MatchArm(std::vector<PatternPtr>& patterns, ExprPtr& body, const mu::Pos& pos) : Expr(ast_match_arm, pos),
                                                                                         patterns(std::move(patterns)), body(std::move(body)) {}


        Match::Match(ExprPtr& cond, std::vector<ExprPtr>& members, const mu::Pos& pos) : Expr(ast_match_expr, pos),
                                                                                  cond(std::move(cond)), members(std::move(members)) {}

        For::For(ast::PatternPtr& pattern, ast::ExprPtr& expr, ast::ExprPtr& body, const mu::Pos& pos) :
                Expr(ast_expr, pos), pattern(std::move(pattern)), expr(std::move(expr)),
                body(std::move(body)) {}

        Defer::Defer(ExprPtr& body, const mu::Pos& pos) : Expr(ast_defer_expr, pos),
                                                   body(std::move(body)) {}

        Return::Return(ExprPtr& body, const mu::Pos& pos) : Expr(ast_return, pos), body(std::move(body)) {}


        BindingExpr::BindingExpr(Ident* name, ExprPtr& expr, const mu::Pos& pos) : Expr(ast_expr_binding, pos),
                                                                      name(name), expr(std::move(expr)) {}


        StructExpr::StructExpr(SpecPtr& spec, std::vector<ExprPtr>& members, const mu::Pos& pos) : Expr(ast_struct_expr, pos),
                                                                                       spec(std::move(spec)), members(std::move(members)) {}


        Range::Range(ExprPtr& start, ExprPtr& end, ExprPtr& step, const mu::Pos& pos) : Expr(ast_range, pos),
                                                                                 start(std::move(start)), end(std::move(end)), step(std::move(step)) {}


        Assign::Assign(mu::TokenKind op, ExprPtr& lvalue, ExprPtr& rvalue, const mu::Pos& pos) :
                Expr(ast_assign, pos), op(op), lvalue(std::move(lvalue)), rvalue(std::move(rvalue)) {}

        void Name::renderer(AstRenderer* renderer) { renderer->render(this); }
        void NameGeneric::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Integer::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Float::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Char::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Str::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Bool::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Nil::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Unit::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Self::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Lambda::renderer(AstRenderer* renderer) { renderer->render(this); }
        void TupleExpr::renderer(AstRenderer* renderer) { renderer->render(this); }
        void List::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Map::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Unary::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Binary::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Accessor::renderer(AstRenderer* renderer) { renderer->render(this); }
        void TupleAcessor::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Method::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Cast::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Block::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Call::renderer(AstRenderer* renderer) { renderer->render(this); }
        void If::renderer(AstRenderer* renderer) { renderer->render(this); }
        void While::renderer(AstRenderer* renderer) { renderer->render(this); }
        void MatchArm::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Match::renderer(AstRenderer* renderer) { renderer->render(this); }
        void For::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Defer::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Return::renderer(AstRenderer* renderer) { renderer->render(this); }
        // void Break::renderer(AstRenderer* renderer) { renderer->render(this); }
        // void Continue::renderer(AstRenderer* renderer) { renderer->render(this); }
        void BindingExpr::renderer(AstRenderer* renderer) { renderer->render(this); }
        void StructExpr::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Range::renderer(AstRenderer* renderer) { renderer->render(this); }
        void Assign::renderer(AstRenderer* renderer) { renderer->render(this); }
}
