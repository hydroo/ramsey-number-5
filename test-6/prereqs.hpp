#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <stdint.h>

#include <iostream>

using s64 =  int64_t;
using u64 = uint64_t;

#define R5_STATIC_ASSERT(expr) static_assert(expr)

#define R5_ASSERT(expr)                                                                             \
    if ((expr) == false) {                                                                          \
        std::cerr << "\e[0;31m\033[1m"                                                              \
                  << "ASSERT"                                                                       \
                  << "\033[0m\e[0;30m"                                                              \
                  << " in " << __FILE__ << ":" << __LINE__ << ": \"" << #expr << "\"" << std::endl; \
        std::abort();                                                                               \
    }

#ifdef DEBUG
    #define R5_STATIC_DEBUG_ASSERT(expr) R5_STATIC_ASSERT(expr)
    #define R5_DEBUG_ASSERT(expr)        R5_ASSERT(expr)
#else
    #define R5_STATIC_DEBUG_ASSERT(expr)
    #define R5_DEBUG_ASSERT(expr)
#endif


template<typename T, T t> struct R5CompileTimePrint;

#define R5_VALUE2(x) #x
#define R5_VALUE(x) R5_VALUE2(x)

#define R5_CAT(a, b)      R5_CAT_I(a, b)
#define R5_CAT_I(a, b)    R5_CAT_II(~, a ## b)
#define R5_CAT_II(p, res) res

#define R5_UNIQUE_NAME(base) R5_CAT(base, __COUNTER__)

#define R5_COMPILETIME_PRINT(x) CompileTimePrint<decltype(x), x> R5_UNIQUE_NAME(compileTimePrint)

template <typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::pair<T, U>& p) {
    o << '(' << p.first << "," << p.second << ")";
    return o;
}

#endif
