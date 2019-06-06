//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_EXPR_HPP
#define MU_EXPR_HPP

#include "ast_common.hpp"
#include "parser/scanner/token.hpp"

#include <cassert>
#include <memory>

namespace ast {
    struct Expr : public AstNode {
        Expr(AstKind k, const mu::Pos& pos) : AstNode(k, pos) {}

        virtual ~Expr() = default;

        virtual std::ostream& operator<< (std::ostream& out) override {
            AstNode::operator<<(out);
            return out;
        }
    };


    struct Name : public Expr {
        Ident* name;

        Name(Ident* name, const mu::Pos& pos) : Expr(ast_name, pos), name(name) {
            assert(name);
        }

        ~Name() override = default;

        const std::string& str() { return name->value; }

         std::ostream&operator<< (std::ostream& out) override {
           Expr::operator<<(out);
           out << "name: " << *name << std::endl;
           return out;
        }
    };
    
    struct NameGeneric : Expr {
        Ident* name;
        std::vector<ast::SpecPtr> type_params;

        NameGeneric(Ident* name, std::vector<ast::SpecPtr>& type_params, const mu::Pos& pos) : Expr(ast_name_generic, pos), name(name), type_params(std::move(type_params)) {}
    };

    struct Integer : public Expr {
        u64 value;

        Integer(u64 value, const mu::Pos& pos) : Expr(ast_integer, pos), value(value) {}
        ~Integer() override = default;


        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "value: " << value << std::endl;
            return out;
        }
    };

    struct Float : public Expr {
        f64 value;

        Float(f64 value, const mu::Pos& pos) : Expr(ast_fl, pos), value(value) {}
        ~Float() override = default;

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "value: " << value << std::endl;
            return out;
        }
    };

    struct Char : public Expr {
        char value;

        Char(char value, const mu::Pos& pos) : Expr(ast_ch, pos), value(value) {}
        ~Char() override = default;

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "value: " << value << std::endl;
            return out;
        }
    };

    struct Str : public Expr {
        std::string value;

        Str(const std::string& value, const mu::Pos& pos) : Expr(ast_str, pos), value(value) {}
        ~Str() override = default;

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "value: " << value << std::endl;
            return out;
        }
    };

    struct Bool : public Expr {
        bool value;

        Bool(bool value, const mu::Pos& pos) : Expr(ast_bool, pos), value(value) {}
    };

    struct Nil : public Expr {
        explicit Nil(const mu::Pos& pos) : Expr(ast_nil, pos) {}
    };

    struct Lambda : public Expr {
        std::vector<DeclPtr> parameters;
        SpecPtr ret;
        ExprPtr body;

        Lambda(std::vector<DeclPtr>& parameters, SpecPtr& ret, ExprPtr& body, const mu::Pos& pos) :
            Expr(ast_lambda, pos), parameters(std::move(parameters)), ret(std::move(ret)),
            body(std::move(body)) {}
    };

    struct TupleExpr : public Expr {
        std::vector<ExprPtr> elements;

        TupleExpr(std::vector<ExprPtr>& elements, const mu::Pos& pos) : Expr(ast_tuple_expr, pos),
            elements(std::move(elements)){}
    };

    struct List : public Expr {
        std::vector<ExprPtr> elements;

        List(std::vector<ExprPtr>& elements, const mu::Pos& pos) : Expr(ast_list, pos),
            elements(std::move(elements)) {}

        ~List() override = default;

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "value: " << std::endl;
            int i = 0;
            for(auto& element : elements) {
                out << "\t" << i++ << " ";
                element->operator<<(out);
                out << std::endl;
            }
            return out;
        }
    };

    struct Map : public Expr {
        std::vector<std::tuple<ExprPtr, ExprPtr>> elements;

        Map(std::vector<std::tuple<ExprPtr, ExprPtr>>& elements, const mu::Pos& pos) : Expr(ast_map, pos),
            elements(std::move(elements)) {}

        ~Map() override = default;

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "value: " << std::endl;
            int i = 0;

            for(auto& [key, value] : elements) {
                out << "\t" << i++ << " ";
                key->operator<<(out);
                out << "|";
                value->operator<<(out);
                out << std::endl;
            }
            return out;
        }
    };

    struct Unary : public Expr {
        mu::TokenKind op;
        ExprPtr expr;

        Unary(mu::TokenKind op, ExprPtr& expr, const mu::Pos& pos) : Expr(ast_unary, pos), op(op), expr(std::move(expr)) {}

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "op: " << mu::Token::get_string(op) << std::endl;
            expr->operator<<(out) << std::endl;
            return out;
        }
    };

    struct Binary : public Expr {
        mu::TokenKind op;
        ExprPtr lhs, rhs;

        Binary(mu::TokenKind op, ExprPtr& lhs, ExprPtr& rhs, const mu::Pos& pos) : Expr(ast_binary, pos),
            op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        virtual std::ostream&operator<< (std::ostream& out) {
            Expr::operator<<(out);
            out << "op: " << mu::Token::get_string(op) << std::endl;
            lhs->operator<<(out) << std::endl;
            rhs->operator<<(out) << std::endl;
            return out;
        }
    };

    struct Accessor : public Expr {
        ExprPtr operand;
        Ident* name;

        Accessor(ExprPtr& operand, Ident* name, const mu::Pos& pos) : Expr(ast_accessor, pos),
            operand(std::move(operand)), name(name) {}
    };

    struct TupleAcessor : public Expr {
        ExprPtr operand;
        u64 value;

        TupleAcessor(ExprPtr& operand, u64 value, const mu::Pos& pos) : Expr(ast_tuple_accessor, pos),
            operand(std::move(operand)), value(value) {}
    };

    struct Method : public Expr {
        ExprPtr operand;
        ExprPtr name;
        // Ident* name;
        // std::vector<ast::SpecPtr> type_parameters;
        std::vector<ExprPtr> actuals;

        Method(ExprPtr& operand, ExprPtr& name, const std::vector<ExprPtr>& actuals, const mu::Pos& pos): Expr(ast_method, pos),
            operand(std::move(operand)),
            name(std::move(name)), actuals(std::move(actuals)) {}
    };

    struct Block : public Expr {
        std::vector<StmtPtr> elements;

        Block(std::vector<StmtPtr>& elements, const mu::Pos& pos) : Expr(ast_block, pos),
            elements(std::move(elements)) {}

        std::ostream&operator<< (std::ostream& out) override {
            Expr::operator<<(out);

            i32 i = 0;
            for(auto& element : elements) {
                out << "\t" << i++ << " ";
                ((AstNode*) element.get())->operator<<(out);
                out << std::endl;
            }

            return out;
        }
    };

    struct Call : public Expr {
        ExprPtr name;
        // std::vector<ast::SpecPtr> type_parameters;
        std::vector<ExprPtr> actuals;

        Call(ExprPtr &operand, const std::vector<ExprPtr> &actuals,
             const mu::Pos &pos) : Expr(ast_call, pos), name(std::move(operand)), actuals(std::move(actuals)) {}
    };

    struct If : public Expr {
        ExprPtr cond;
        ExprPtr body;
        ExprPtr else_if;

        If(ExprPtr& cond, ExprPtr& body, ExprPtr& else_if, const mu::Pos& pos) : Expr(ast_if_expr, pos),
            cond(std::move(cond)), body(std::move(body)), else_if(std::move(else_if)) {}

    };

    struct While : public Expr {
        ExprPtr cond;
        ExprPtr body;

        While(ExprPtr& cond, ExprPtr& body, const mu::Pos& pos) : Expr(ast_while_expr, pos),
            cond(std::move(cond)), body(std::move(body)) {}

    };

    struct MatchArm : public Expr {
       std::vector<PatternPtr> patterns;
       ExprPtr body;

       MatchArm(std::vector<PatternPtr>& patterns, ExprPtr& body, const mu::Pos& pos) : Expr(ast_match_arm, pos),
        patterns(std::move(patterns)), body(std::move(body)) {}
    };

    struct Match : public Expr {
        ExprPtr cond;
        std::vector<ExprPtr> members;

        Match(ExprPtr& cond, std::vector<ExprPtr>& members, const mu::Pos& pos) : Expr(ast_match_expr, pos),
            cond(std::move(cond)), members(std::move(members)) {}
    };

    struct For : public Expr {
    };

    struct Defer : public Expr {
        ExprPtr body;

        Defer(ExprPtr& body, const mu::Pos& pos) : Expr(ast_defer_expr, pos),
            body(std::move(body)) {}
    };

    struct Return : public Expr {
        ExprPtr body;

        Return(ExprPtr& body, const mu::Pos& pos) : Expr(ast_return, pos), body(std::move(body)) {}
    };

    struct BindingExpr : public Expr {
        Ident* name;
        ExprPtr expr;

        BindingExpr(Ident* name, ExprPtr& expr, const mu::Pos& pos) : Expr(ast_expr_binding, pos),
            name(name), expr(std::move(expr)) {}
    };

    struct StructExpr : public Expr {
        SpecPtr spec;
        std::vector<ExprPtr> members;

        StructExpr(SpecPtr& spec, std::vector<ExprPtr>& members, const mu::Pos& pos) : Expr(ast_struct_expr, pos),
            spec(std::move(spec)), members(std::move(members)) {}
    };
}

#endif //MU_EXPR_HPP
