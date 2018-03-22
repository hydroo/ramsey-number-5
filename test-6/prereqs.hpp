#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <stdint.h>

#include <iostream>
#include <type_traits>

using s64 =  int64_t;
using u64 = uint64_t;

#define STATIC_ASSERT(expr) static_assert(expr)

#define ASSERT(expr)                                                                                \
    if ((expr) == false) {                                                                          \
        std::cerr << "\e[0;31m\033[1m"                                                              \
                  << "ASSERT"                                                                       \
                  << "\033[0m\e[0;30m"                                                              \
                  << " in " << __FILE__ << ":" << __LINE__ << ": \"" << #expr << "\"" << std::endl; \
        std::abort();                                                                               \
    }

#ifdef DEBUG
    #define STATIC_DEBUG_ASSERT(expr) STATIC_ASSERT(expr)
    #define DEBUG_ASSERT(expr)        ASSERT(expr)
#else
    #define STATIC_DEBUG_ASSERT(expr)
    #define DEBUG_ASSERT(expr)
#endif


template<typename T, T t> struct CompileTimePrintTemplate;

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define UNIQUE_NAME(base) PP_CAT(base, __COUNTER__)

#define CompileTimePrint(x) CompileTimePrintTemplate<decltype(x), x> UNIQUE_NAME(compileTimePrint)

#endif
