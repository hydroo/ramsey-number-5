#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <stdint.h>

#include <iostream>

#ifdef R5_GTEST
#   include <gtest/gtest.h>
#endif

using s64 =  int64_t;
using u64 = uint64_t;

namespace r5 {

// std::copy is not constexpr until c++20, remove this when it's available
template<typename T>
constexpr void copy(const T* from, s64 count, T* to) {
    for (s64 i = 0; i < count; i += 1) {
        to[i] = from[i];
    }
}

// std::fill_n is not constexpr until c++20, remove this when it's available
template<typename T>
constexpr void fill_n(T* to, s64 count, const T& value) {
    for (s64 i = 0; i < count; i += 1) {
        to[i] = value;
    }
}

}

#define R5_STATIC_ASSERT(expr) static_assert(expr)

#ifndef R5_GTEST
#   define R5_ASSERT(expr)                                                                             \
       if ((expr) == false) {                                                                          \
           std::cerr << "\e[0;31m\033[1m"                                                              \
                     << "ASSERT"                                                                       \
                     << "\033[0m\e[0;30m"                                                              \
                     << " in " << __FILE__ << ":" << __LINE__ << ": \"" << #expr << "\"" << std::endl; \
           std::abort();                                                                               \
       }
# else
#   define R5_ASSERT(expr)                                                                              \
        if ((expr) == false) {                                                                          \
            throw std::runtime_error("ASSERT in " __FILE__ ":" R5_VALUE(__LINE__) ": " R5_VALUE(expr)); \
        }
#   define ASSERT_THROW2(expr) ASSERT_THROW(expr, std::runtime_error)
#endif

#ifdef DEBUG
    #define R5_STATIC_DEBUG_ASSERT(expr) R5_STATIC_ASSERT(expr)
    #define R5_DEBUG_ASSERT(expr)        R5_ASSERT(expr)
#else
    #define R5_STATIC_DEBUG_ASSERT(expr)
    #define R5_DEBUG_ASSERT(expr)
#endif


namespace r5 {
template<typename T, T t> struct CompileTimePrint;
}

#define R5_VALUE2(x) #x
#define R5_VALUE(x) R5_VALUE2(x)

#define R5_CAT(a, b)      R5_CAT_I(a, b)
#define R5_CAT_I(a, b)    R5_CAT_II(~, a ## b)
#define R5_CAT_II(p, res) res

#define R5_UNIQUE_NAME(base) R5_CAT(base, __COUNTER__)

#define R5_COMPILETIME_PRINT(x) r5::CompileTimePrint<decltype(x), x> R5_UNIQUE_NAME(compileTimePrint)

template <typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::pair<T, U>& p) {
    o << '(' << p.first << "," << p.second << ")";
    return o;
}

#endif
