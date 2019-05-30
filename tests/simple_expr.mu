Expr : type {
    Unary(Op, *Expr),
    Binary(Op, *Expr, *Expr)
}