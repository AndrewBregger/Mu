//
// Created by Andrew Bregger on 2019-06-13.
//

#include "typer_op_eval.hpp"

#define UOPERATOR(op, v, t) { \
    switch(t->kind()) { \
        case types::Primitive_I8: \
            val = Val(op v._I8); \
            break; \
        case types::Primitive_I16: \
            val = Val(op v._I16); \
            break; \
        case types::Primitive_I32: \
            val = Val(op v._I32); \
            break; \
        case types::Primitive_I64: \
            val = Val(op v._I64); \
            break; \
        case types::Primitive_U8: \
            val = Val(op v._U8); \
            break; \
        case types::Primitive_U16: \
            val = Val(op v._U16); \
            break; \
        case types::Primitive_U32: \
            val = Val(op v._U32); \
            break; \
        case types::Primitive_U64: \
            val = Val(op v._U64); \
            break; \
        case types::Primitive_Float32: \
            val = Val(op v._F32); \
            break; \
        case types::Primitive_Float64: \
            val = Val(op v._F64); \
            break; \
        default: \
            break; \
    } \
}

#define UOPERATOR_WITHOUTFLOAT(op, v, t) { \
    switch(t->kind()) { \
        case types::Primitive_I8: \
            val = Val(op v._I8); \
            break; \
        case types::Primitive_I16: \
            val = Val(op v._I16); \
            break; \
        case types::Primitive_I32: \
            val = Val(op v._I32); \
            break; \
        case types::Primitive_I64: \
            val = Val(op v._I64); \
            break; \
        case types::Primitive_U8: \
            val = Val(op v._U8); \
            break; \
        case types::Primitive_U16: \
            val = Val(op v._U16); \
            break; \
        case types::Primitive_U32: \
            val = Val(op v._U32); \
            break; \
        case types::Primitive_U64: \
            val = Val(op v._U64); \
            break; \
        default: \
            break; \
    } \
}

#define BOPERATOR(op, lhs, rhs, t) { \
    switch(t->kind()) { \
        case types::Primitive_I8: \
            val = Val(lhs._I8 op rhs._I8); \
            break; \
        case types::Primitive_I16: \
            val = Val(lhs._I16 op rhs._I16); \
            break; \
        case types::Primitive_I32: \
            val = Val(lhs._I32 op rhs._I32); \
            break; \
        case types::Primitive_I64: \
            val = Val(lhs._I64 op rhs._I64); \
            break; \
        case types::Primitive_U8: \
            val = Val(lhs._U8 op rhs._U8); \
            break; \
        case types::Primitive_U16: \
            val = Val(lhs._U16 op rhs._U16); \
            break; \
        case types::Primitive_U32: \
            val = Val(lhs._U32 op rhs._U32); \
            break; \
        case types::Primitive_U64: \
            val = Val(lhs._U64 op rhs._U64); \
            break; \
        case types::Primitive_Float32: \
            val = Val(lhs._F32 op rhs._F32); \
            break; \
        case types::Primitive_Float64: \
            val = Val(lhs._F64 op rhs._F64); \
            break; \
        default: \
            break; \
    } \
}

#define BOPERATOR_WITHOUTFLOAT(op, lhs, rhs, t) { \
    switch(t->kind()) { \
        case types::Primitive_I8: \
            val = Val(lhs._I8 op rhs._I8); \
            break; \
        case types::Primitive_I16: \
            val = Val(lhs._I16 op rhs._I16); \
            break; \
        case types::Primitive_I32: \
            val = Val(lhs._I32 op rhs._I32); \
            break; \
        case types::Primitive_I64: \
            val = Val(lhs._I64 op rhs._I64); \
            break; \
        case types::Primitive_U8: \
            val = Val(lhs._U8 op rhs._U8); \
            break; \
        case types::Primitive_U16: \
            val = Val(lhs._U16 op rhs._U16); \
            break; \
        case types::Primitive_U32: \
            val = Val(lhs._U32 op rhs._U32); \
            break; \
        case types::Primitive_U64: \
            val = Val(lhs._U64 op rhs._U64); \
            break; \
        default: \
            break; \
    } \
}

namespace mu {
    Operand eval_binary_op(Typer *typer, mu::TokenKind op, Operand lhs, Operand rhs, ast::Expr *expr,
                           types::Type *expected_type) {
        // the expected type is not always the type being used.
        if(lhs.val.is_constant and rhs.val.is_constant) {
            Val val(expr);
            switch(op) {

                case mu::Tkn_Plus:
                    BOPERATOR(+, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_Minus:
                    BOPERATOR(-, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_Astrick:
                    BOPERATOR(*, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_Slash:
                    BOPERATOR(/, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_AstrickAstrick:
                    // translate this to a call to powf
                case mu::Tkn_Percent:
                    // translate this to a call to modf
                    // equality operations
                    break;
                case mu::Tkn_EqualEqual:
                    BOPERATOR(==, lhs.val, rhs.val, lhs.type)
                    break;
                case mu::Tkn_BangEqual:
                    BOPERATOR(!=, lhs.val, rhs.val, lhs.type);
                    break;
                case mu::Tkn_LessEqual:
                    BOPERATOR(<=, lhs.val, rhs.val, lhs.type);
                    break;
                case mu::Tkn_GreaterEqual:
                    BOPERATOR(>=, lhs.val, rhs.val, lhs.type);
                    break;
                case mu::Tkn_And:
                    val = Val(lhs.val._Bool and rhs.val._Bool);
                    break;
                case mu::Tkn_Or:
                    val = Val(lhs.val._Bool or rhs.val._Bool);
                    break;
                case mu::Tkn_LessLess:
                    BOPERATOR_WITHOUTFLOAT(<<, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_GreaterGreater:
                    BOPERATOR_WITHOUTFLOAT(>>, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_Ampersand:
                    BOPERATOR_WITHOUTFLOAT(&, lhs.val, rhs.val, expected_type)
                    break;
                case mu::Tkn_Pipe:
                    BOPERATOR_WITHOUTFLOAT(|, lhs.val, rhs.val, expected_type)
                    break;
                default:
                    break;
            }
            return Operand(expected_type, expr, val);
        }
        else {
            return Operand(expected_type, expr, RValue);
        }
    }

    Operand eval_unary_op(Typer *typer, mu::TokenKind op, Operand operand, ast::Expr *expr, types::Type *expected_type) {
        if(operand.val.is_constant) {
            Val val(expr);
            switch(op) {
                case mu::Tkn_Minus:
                    UOPERATOR(-, operand.val, expected_type)
                    break;
                case mu::Tkn_Tilde:
                    UOPERATOR_WITHOUTFLOAT(-, operand.val, expected_type)
                    break;
                case mu::Tkn_Bang:
                    UOPERATOR(-, operand.val, operand.type);
                    break;
                case mu::Tkn_Ampersand:
                case mu::Tkn_Astrick:
                    return Operand(expr);
                    break;
                default:
                    break;
            }
            return Operand(expected_type, expr, val);
        }
        else {
            return Operand(operand.type, expr, RValue);
        }
    }
}