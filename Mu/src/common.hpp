#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cstdint>
#include <vector>
#include <map>
#include <string>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


typedef float f32;
typedef double f64;

// for unicode support
typedef u32 rune;

#ifndef CAST_PTR
    #define CAST_PTR(Type, expr) static_cast<Type*>(expr)
#endif

#ifndef CAST
    #define CAST(Type, expr) static_cast<Type>(expr)
#endif

// Windows is annoying
#if defined(__APPLE__)
    #define MU_APPLE
#elif defined(__WIN32__)
    #define MU_WIN
#elif defined(__WIN32)
    #define MU_WIN
#elif defined(_MSC_VER)
    #define MU_WIN
#elif defined(__CYGWIN32__)
    #define MU_WIN
#elif defined(__LINUX__)
    #define MU_LINUX
#endif

#if defined(MU_WIN)
#if defined(and)
#undef and
#endif
#define and &&
#if defined(or)
#undef or
#endif
#define or ||
#endif

#endif