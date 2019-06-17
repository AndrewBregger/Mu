//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_STMT_HPP
#define MU_STMT_HPP

#include "ast_common.hpp"


namespace ast {

    struct ExprStmt : public Stmt {
        ExprPtr expr;
        ExprStmt(ExprPtr& expr, const mu::Pos& pos);


//        virtual std::ostream&operator<< (std::ostream& out) {
//            AstNode::operator<<(out) << std::endl;
//            expr->operator<<(out);
//            return out;
//        }
    };

    struct DeclStmt : public Stmt {
        DeclPtr decl;
        DeclStmt(DeclPtr& decl, const mu::Pos& pos);

//        virtual std::ostream&operator<< (std::ostream& out) {
//            AstNode::operator<<(out) << std::endl;
//            // decl->operator<<(out);
//            return out;
//        }
    };

    struct EmptyStmt : public Stmt {
        EmptyStmt(const mu::Pos pos);

//        virtual std::ostream&operator<< (std::ostream& out) {
//            AstNode::operator<<(out) << std::endl;
//            return out;
//        }
    };
}


#endif //MU_STMT_HPP
