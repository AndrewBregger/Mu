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
    struct Name : public Expr {
        Ident* name;

        Name(Ident* name, const mu::Pos& pos);

        ~Name() override = default;

//        inline std::string str() { return name->val->value; }

//        std::ostream&operator<< (std::ostream& out) override {
//           Expr::operator<<(out);
//           out << "name: " << str() << std::endl;
//           return out;
//        }
    };
    
    struct NameGeneric : Expr {
        Ident* name;
        std::vector<ast::SpecPtr> type_params;

        NameGeneric(Ident* name, std::vector<ast::SpecPtr>& type_params, const mu::Pos& pos);
    };

    struct Integer : public Expr {
        u64 value;

        Integer(u64 value, const mu::Pos& pos);
        ~Integer() override;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }
    };

    struct Float : public Expr {
        f64 value;

        Float(f64 value, const mu::Pos& pos);
        ~Float() override;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }
    };

    struct Char : public Expr {
        char value;

        Char(char value, const mu::Pos& pos);
        ~Char() override;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }
    };

    struct Str : public Expr {
        std::string value;

        Str(const std::string& value, const mu::Pos& pos);
        ~Str() override;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "value: " << value << std::endl;
//            return out;
//        }
    };

    struct Bool : public Expr {
        bool value;

        Bool(bool value, const mu::Pos& pos);
    };

    struct Nil : public Expr {
        explicit Nil(const mu::Pos& pos);
    };

    struct Unit : public Expr {
        explicit Unit(const mu::Pos& pos);
    };

    struct Self : public Expr {
        explicit Self(const mu::Pos& pos);
    };

    struct Lambda : public Expr {
        std::vector<DeclPtr> parameters;
        SpecPtr ret;
        ExprPtr body;

        Lambda(std::vector<DeclPtr>& parameters, SpecPtr& ret, ExprPtr& body, const mu::Pos& pos);

        ~Lambda() override;
    };

    struct TupleExpr : public Expr {
        std::vector<ExprPtr> elements;

        TupleExpr(std::vector<ExprPtr>& elements, const mu::Pos& pos);

        ~TupleExpr() override;
    };

    struct List : public Expr {
        std::vector<ExprPtr> elements;

        List(std::vector<ExprPtr>& elements, const mu::Pos& pos);

        ~List() override = default;

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
    };

    struct Map : public Expr {
        std::vector<std::tuple<ExprPtr, ExprPtr>> elements;

        Map(std::vector<std::tuple<ExprPtr, ExprPtr>>& elements, const mu::Pos& pos);

        ~Map() override = default;

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
    };

    struct Unary : public Expr {
        mu::TokenKind op;
        ExprPtr expr;

        Unary(mu::TokenKind op, ExprPtr& expr, const mu::Pos& pos);
        ~Unary() override;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "op: " << mu::Token::get_string(op) << std::endl;
//            expr->operator<<(out) << std::endl;
//            return out;
//        }
    };

    struct Binary : public Expr {
        mu::TokenKind op;
        ExprPtr lhs, rhs;

        Binary(mu::TokenKind op, ExprPtr& lhs, ExprPtr& rhs, const mu::Pos& pos);

        ~Binary() override;

//        virtual std::ostream&operator<< (std::ostream& out) {
//            Expr::operator<<(out);
//            out << "op: " << mu::Token::get_string(op) << std::endl;
//            lhs->operator<<(out) << std::endl;
//            rhs->operator<<(out) << std::endl;
//            return out;
//        }
    };

    struct Accessor : public Expr {
        ExprPtr operand;
        Ident* name;

        Accessor(ExprPtr& operand, Ident* name, const mu::Pos& pos);

        ~Accessor() override;
    };

    struct TupleAcessor : public Expr {
        ExprPtr operand;
        u64 value;

        TupleAcessor(ExprPtr& operand, u64 value, const mu::Pos& pos);

        ~TupleAcessor() override;
    };

    struct Method : public Expr {
        ExprPtr operand;
        ExprPtr name;
        // Ident* name;
        // std::vector<ast::SpecPtr> type_parameters;
        std::vector<ExprPtr> actuals;

        Method(ExprPtr& operand, ExprPtr& name, const std::vector<ExprPtr>& actuals, const mu::Pos& pos);

        ~Method() override;
    };

    struct Cast : public Expr {
        ExprPtr operand;
        SpecPtr type;

        Cast(ExprPtr& operand, SpecPtr& type, const mu::Pos& pos);

        ~Cast() override;
    };

    struct Block : public Expr {
        std::vector<StmtPtr> elements;

        Block(std::vector<StmtPtr>& elements, const mu::Pos& pos);

        ~Block() override;

//        std::ostream&operator<< (std::ostream& out) override {
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
    };

    struct Call : public Expr {
        ExprPtr name;
        // std::vector<ast::SpecPtr> type_parameters;
        std::vector<ExprPtr> actuals;

        Call(ExprPtr &operand, const std::vector<ExprPtr> &actuals,
             const mu::Pos &pos);

        ~Call() override;
    };

    struct If : public Expr {
        ExprPtr cond;
        ExprPtr body;
        ExprPtr else_if;

        If(ExprPtr& cond, ExprPtr& body, ExprPtr& else_if, const mu::Pos& pos);

        ~If() override;
    };

    struct While : public Expr {
        ExprPtr cond;
        ExprPtr body;

        While(ExprPtr& cond, ExprPtr& body, const mu::Pos& pos);

        ~While() override;
    };

    struct MatchArm : public Expr {
       std::vector<PatternPtr> patterns;
       ExprPtr body;

       MatchArm(std::vector<PatternPtr>& patterns, ExprPtr& body, const mu::Pos& pos);
    };

    struct Match : public Expr {
        ExprPtr cond;
        std::vector<ExprPtr> members;

        Match(ExprPtr& cond, std::vector<ExprPtr>& members, const mu::Pos& pos);
    };

    struct For : public Expr {
        ast::PatternPtr pattern;
        ast::ExprPtr expr;
        ast::ExprPtr body;

        For(ast::PatternPtr& pattern, ast::ExprPtr& expr, ast::ExprPtr& body, const mu::Pos& pos);
    };

    struct Defer : public Expr {
        ExprPtr body;

        Defer(ExprPtr& body, const mu::Pos& pos);
    };

    struct Return : public Expr {
        ExprPtr body;

        Return(ExprPtr& body, const mu::Pos& pos);
    };

    struct BindingExpr : public Expr {
        Ident* name;
        ExprPtr expr;

        BindingExpr(Ident* name, ExprPtr& expr, const mu::Pos& pos);
    };

    struct StructExpr : public Expr {
        SpecPtr spec;
        std::vector<ExprPtr> members;

        StructExpr(SpecPtr& spec, std::vector<ExprPtr>& members, const mu::Pos& pos);
    };

    struct Range : public Expr {
        ExprPtr start;
        ExprPtr end;
        ExprPtr step;

        Range(ExprPtr& start, ExprPtr& end, ExprPtr& step, const mu::Pos& pos);
    };

    struct Assign : public Expr {
        mu::TokenKind op;
        ExprPtr lvalue;
        ExprPtr rvalue;

        Assign(mu::TokenKind op, ExprPtr& lvalue, ExprPtr& rvalue, const mu::Pos& pos);
    };
}

#endif //MU_EXPR_HPP
