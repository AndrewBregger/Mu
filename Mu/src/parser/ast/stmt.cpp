#include "stmt.hpp"
#include "renderer.hpp"

namespace ast {
    ExprStmt::ExprStmt(ExprPtr& expr, const mu::Pos& pos) : Stmt(ast_expr, pos), expr(std::move(expr)) {}
    DeclStmt::DeclStmt(DeclPtr& decl, const mu::Pos& pos) : Stmt(ast_decl, pos), decl(std::move(decl)) {}
    EmptyStmt::EmptyStmt(const mu::Pos pos) : Stmt(ast_empty, pos) {}
    void ExprStmt::renderer(AstRenderer* renderer) { renderer->render(this); }
    void DeclStmt::renderer(AstRenderer* renderer) { renderer->render(this); }
    void EmptyStmt::renderer(AstRenderer* renderer) { renderer->render(this); }
}