//
// Created by andy on 6/22/19.
//

#ifndef MU_OPERAND_HPP
#define MU_OPERAND_HPP

#include "common.hpp"
#include "value.hpp"

namespace ast {
    struct Expr;
}

namespace mu {
    const Val NO_VALUE;

    enum AccessType {
        LValue, // assignable value
        RValue, // result value
        TypeAccess, // the result is a type.
    };

    // the result of an expression
    // when an expression is resolved, either a type or a type with a value is
    // the result. This struct encapsulates both results.
    // if val is invalid, then the value is computed at run time
    // if val is constant, then we known the value at
    // compile time.
    // AccessType determines how this expression can be used.
    //  If it it is an LValue can be on the left side of an assign.
    //  If is it is an RValue then it must be on the right side.
    struct Operand {
        types::Type *type;
        ast::Expr *expr;
        AccessType access{RValue};
        Val val{NO_VALUE};
        bool error{false};

        // Valid run time expression
        Operand(mu::types::Type *type, ast::Expr *expr, AccessType access_type);

        // Valid compile time expression
        Operand(mu::types::Type *type, ast::Expr *expr, const Val &val);

        // error on expression
        // will have been reported by this point.
        // this is so the error can be propagated
        explicit Operand(ast::Expr *expr);
    };
}
#endif //MU_OPERAND_HPP
