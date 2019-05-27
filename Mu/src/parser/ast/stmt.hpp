//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_STMT_HPP
#define MU_STMT_HPP

#include "ast_common.hpp"


namespace ast {
    struct Stmt : public AstNode {
        Stmt(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}

        virtual std::ostream& operator<< (std::ostream& out) = 0;
    };

    struct ExprStmt : public Stmt {
        ExprPtr expr;
        ExprStmt(ExprPtr& expr, const mu::Pos& pos) : Stmt(ast_expr, pos), expr(std::move(expr)) {}


        virtual std::ostream&operator<< (std::ostream& out) {
            AstNode::operator<<(out) << std::endl;
            expr->operator<<(out);
            return out;
        }
    };

    struct DeclStmt : public Stmt {
        DeclPtr decl;
        DeclStmt(DeclPtr& decl, const mu::Pos& pos) : Stmt(ast_decl, pos), decl(std::move(decl)) {}

        virtual std::ostream&operator<< (std::ostream& out) {
            AstNode::operator<<(out) << std::endl;
            // decl->operator<<(out);
            return out;
        }
    };

    struct EmptyStmt : public Stmt {
        EmptyStmt(const mu::Pos pos) : Stmt(ast_empty, pos) {}

        virtual std::ostream&operator<< (std::ostream& out) {
            AstNode::operator<<(out) << std::endl;
            return out;
        }
    };
}


#endif //MU_STMT_HPP
