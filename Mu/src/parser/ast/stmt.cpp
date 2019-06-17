#include "stmt.hpp"

namespace ast {
    ExprStmt::ExprStmt(ExprPtr& expr, const mu::Pos& pos) : Stmt(ast_expr, pos), expr(std::move(expr)) {}
    DeclStmt::DeclStmt(DeclPtr& decl, const mu::Pos& pos) : Stmt(ast_decl, pos), decl(std::move(decl)) {}
    EmptyStmt::EmptyStmt(const mu::Pos pos) : Stmt(ast_empty, pos) {}
}