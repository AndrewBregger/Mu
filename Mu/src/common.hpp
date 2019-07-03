#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <utility>

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

#define MU_DEBUG

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
#elif defined(__LINUX__) || defined(__linux__) || defined(__unix) || defined(__UNIX__) || defined(__linux) || defined(__LINUX)
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

// namespace mem {
//     template <typename T>
//     class Pr : public std::shared_ptr<T> {
//     public:
//         using std::shared_ptr<T>::shared_ptr;

//         template <typename Ty>
//         Ty* as() {
//             return const_cast<const Pr<T>&>(*this).as<Ty>();
//         }

//         template <typename Ty>
//         Ty* as() const {
//             return dynamic_cast<Ty*>(this->get());
//         }
//     };
// }

// namespace std {
//     template<typename T>
//     struct hash<mem::Pr<T>> {
//         u64 operator() (const mem::Pr<T>& ptr) const {
//             std::hash<T*> hasher;
//             return hasher(ptr.get());
//         }
//     };
// }

struct Atom {
    std::string value;

    Atom(const std::string& value) : value(value) {}
};

#endif
