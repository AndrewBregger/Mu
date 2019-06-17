//
// Created by Andrew Bregger on 2019-02-03.
//

#pragma once

#include "common.hpp"
#include <limits>


// for now this will only evaluate primitive values.
namespace mu {

    namespace types {
        class Type;
    }

    // defines the limits of the values.
    const u8  MAX_U8 = std::numeric_limits<u8>::max();
    const u16 MAX_U16 = std::numeric_limits<u16>::max();
    const u32 MAX_U32 = std::numeric_limits<u32>::max();
    const u64 MAX_U64 = std::numeric_limits<u64>::max();

    const i8  MAX_I8 =  std::numeric_limits<i8>::max();
    const i16 MAX_I16 = std::numeric_limits<i16>::max();
    const i32 MAX_I32 = std::numeric_limits<i32>::max();
    const i64 MAX_I64 = std::numeric_limits<i64>::max();

    const f32 MAX_F32 = std::numeric_limits<f32>::max();
    const f64 MAX_F64 = std::numeric_limits<f64>::max();

    const char MAX_char = std::numeric_limits<char>::max();
    const bool MAX_BOOL = std::numeric_limits<bool>::max();

    struct Val {
        Val();
        explicit Val(types::Type* type);
        explicit Val(i8 val);
        explicit Val(i16 val);
        explicit Val(i32 val);
        explicit Val(i64 val);
        explicit Val(u8 val);
        explicit Val(u16 val);
        explicit Val(u32 val);
        explicit Val(u64 val);
        explicit Val(f32 val);
        explicit Val(f64 val);
        explicit Val(char val);
        explicit Val(bool _bool);

        // changes the type of this value to the new type
        // if it is constant then the value is constasted
        void cast_to(types::Type* ty);

        types::Type* type{nullptr};
//        ast::ExprPtr expr;
        bool is_constant{false};
        bool invalid{false};
        union {
            i8  _I8;
            i16 _I16;
            i32 _I32;
            i64 _I64;

            u8  _U8;
            u16 _U16;
            u32 _U32;
            u64 _U64;

            f32 _F32;
            f64 _F64;
            char _Char;
            bool _Bool;
        };

        friend std::ostream& operator<< (std::ostream& out, const Val& val);
    };

}
