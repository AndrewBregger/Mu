//
// Created by Andrew Bregger on 2019-02-03.
//

#include <fstream>
#include "value.hpp"
#include "types/type.hpp"

#define Constructor(ctype, vtype, mtype) Val::Val(ctype val) : type(mtype), is_constant(true) { _##vtype = val; }


extern mu::types::Type* type_u8;
extern mu::types::Type* type_u16;
extern mu::types::Type* type_u32;
extern mu::types::Type* type_u64;
extern mu::types::Type* type_i8;
extern mu::types::Type* type_i16;
extern mu::types::Type* type_i32;
extern mu::types::Type* type_i64;
extern mu::types::Type* type_f32;
extern mu::types::Type* type_f64;
extern mu::types::Type* type_char;
extern mu::types::Type* type_bool;

namespace mu {
    Val::Val() {
        _U64 = 0;
        invalid = true;
    }

    Val::Val(mu::types::Type* type) : type(type), is_constant(false) {}

    Constructor(i8 , I8,  type_i8)
    Constructor(i16, I16, type_i16)
    Constructor(i32, I32, type_i32)
    Constructor(i64, I64, type_i64)
    Constructor(u8 , U8,  type_u8)
    Constructor(u16, U16, type_u16)
    Constructor(u32, U32, type_u32)
    Constructor(u64, U64, type_u64)
    Constructor(f32, F32, type_f32)
    Constructor(f64, F64, type_f64)
    Constructor(char, Char, type_char)
    Constructor(bool, Bool, type_bool)

#if defined(VAL_CAST)
#undef VAL_CAST
#endif

#define VAL_CAST(val, ty) \
    switch(ty->kind()) { \
        case types::Primitive_I8: { \
            this->_I8 = (i8) this->val; \
        } break; \
        case types::Primitive_I16: { \
            this->_I16 = (i16) this->val; \
        } break; \
        case types::Primitive_I32: { \
            this->_I32 = (i32) this->val; \
        } break;  \
        case types::Primitive_I64: { \
            this->_I64 = (i64) this->val; \
        } break;  \
        case types::Primitive_U8: { \
            this->_U8 = (u8) this->val; \
        } break;  \
        case types::Primitive_U16: { \
            this->_U16 = (u16) this->val; \
        } break;  \
        case types::Primitive_U32: { \
            this->_U32 = (u32) this->val; \
        } break;  \
        case types::Primitive_U64: { \
            this->_U64 = (u64) this->val; \
        } break;  \
        case types::Primitive_Float32: { \
            this->_F32 = (f32) this->val; \
        } break;  \
        case types::Primitive_Float64: { \
            this->_F64 = (f64) this->val; \
        } break;  \
        case types::Primitive_Char: { \
            this->_Char = (char) this->val; \
        } break;  \
        case types::Primitive_Bool: { \
           this->_Bool = (bool) this->val; \
        } break;  \
        default: \
            break; \
    }

    void Val::cast_to(types::Type *ty) {
        // if constant value, cast the value to appropriate type and set the new type
        // otherwise just set the new type.
        if(is_constant) {
            if (ty->is_primative()) {
                switch (type->kind()) {
                    case types::Primitive_I8: {
                        VAL_CAST(_I8, ty)
                    }
                        break;
                    case types::Primitive_I16: {
                        VAL_CAST(_I16, ty)
                    }
                        break;
                    case types::Primitive_I32: {
                        VAL_CAST(_I32, ty)
                    }
                        break;
                    case types::Primitive_I64: {
                        VAL_CAST(_I64, ty)
                    }
                        break;
                    case types::Primitive_U8: {
                        VAL_CAST(_U8, ty)
                    }
                        break;
                    case types::Primitive_U16: {
                        VAL_CAST(_U16, ty)
                    }
                        break;
                    case types::Primitive_U32: {
                        VAL_CAST(_U32, ty)
                    }
                        break;
                    case types::Primitive_U64: {
                        VAL_CAST(_U64, ty)
                    }
                        break;
                    case types::Primitive_Float32: {
                        VAL_CAST(_F32, ty)
                    }
                        break;
                    case types::Primitive_Float64: {
                        VAL_CAST(_F64, ty)
                    }
                        break;
                    case types::Primitive_Bool: {
                        VAL_CAST(_Bool, ty)
                    }
                        break;
                    case types::Primitive_Char: {
                        VAL_CAST(_Char, ty)
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        type = ty;
    }

#undef VAL_CAST

    std::ostream &operator<<(std::ostream &out, const Val &val) {
        if(val.is_constant) {
            switch (val.type->kind()) {
                case types::Primitive_I8: {
                    out << val._I8;
                }
                break;
                case types::Primitive_I16: {
                    out << val._I16;
                }
                break;
                case types::Primitive_I32: {
                    out << val._I32;
                }
                break;
                case types::Primitive_I64: {
                    out << val._I64;
                }
                break;
                case types::Primitive_U8: {
                    out << val._U8;
                }
                break;
                case types::Primitive_U16: {
                    out << val._U16;
                }
                break;
                case types::Primitive_U32: {
                    out << val._U32;
                }
                break;
                case types::Primitive_U64: {
                    out << val._U64;
                }
                break;
                case types::Primitive_Float32: {
                    out << val._F32;
                }
                break;
                case types::Primitive_Float64: {
                    out << val._F64;
                }
                break;
                case types::Primitive_Bool: {
                    out << (val._Bool ? "true" : "false");
                }
                break;
                case types::Primitive_Char: {
                    out << val._Char;
                }
                break;
                default:
                    break;
            }
        }
        else {
            out << "no value";
        }
        return out;
    }
}
