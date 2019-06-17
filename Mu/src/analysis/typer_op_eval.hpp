//
// Created by Andrew Bregger on 2019-06-13.
//

#ifndef MU_TYPER_OP_EVAL_HPP
#define MU_TYPER_OP_EVAL_HPP

#include "typer.hpp"

namespace mu {

    Operand eval_binary_op(Typer *typer, mu::TokenKind op, Operand lhs, Operand rhs, ast::Expr *expr,
                           types::Type *expected_type);
    Operand eval_unary_op(Typer *typer, mu::TokenKind op, Operand operand, ast::Expr *expr, types::Type *expected_type);

}

#endif //MU_TYPER_OP_EVAL_HPP

