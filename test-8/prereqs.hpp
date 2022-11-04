#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdint.h>
#include <vector>

#ifdef R5_GTEST
#   include <gtest/gtest.h>
#endif

using s8  =  int8_t;
using u8  = uint8_t;
using s16 =  int16_t;
using u16 = uint16_t;
using s32 =  int32_t;
using u32 = uint32_t;
using s64 =  int64_t;
using u64 = uint64_t;

namespace r5 {
    using Size = s64;
}

#ifndef R5_VERBOSE
#   define R5_VERBOSE 0
#endif

#if R5_VERBOSE >= 1
#   define R5_VERBOSE_1(x) x
#else
#   define R5_VERBOSE_1(x)
#endif

#define R5_STATIC_ASSERT(expr) static_assert(expr)

#ifndef R5_GTEST
#   define R5_ASSERT(expr)                                                                             \
       if ((expr) == false) {                                                                          \
           std::cerr << "\033[0;31m\033[1m"                                                              \
                     << "ASSERT"                                                                       \
                     << "\033[0m\033[0;30m"                                                              \
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

constexpr s64 factorial(s64 n) {
    if (n == 0) { return 1; }
    else        { return factorial(n-1) * n; }
}

template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T nChooseK(T n, T k) {
    if (n < k) {
        return 0; // error
    } else if (n < 0) {
        return 0; // error
    } else if (k < 0) {
        return 0; // error
    }

    if (k == 0 || k == n) {
        return 1;
    } else {
        return nChooseK(n - 1, k - 1) + nChooseK(n - 1, k);
    }
}

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

// // std::swap is not constexpr
// template<typename T>
// constexpr void swap(T& t1, T& t2) {
//     T temp = std::move(t1);
//     t1 = std::move(t2);
//     t2 = std::move(temp);
// }

// // std::iter_swap is not constexpr
// template<class ForwardIt1, class ForwardIt2>
// constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b)
// {
//     r5::swap(*a, *b);
// }

// template<class BidirIt>
// constexpr void reverse(BidirIt first, BidirIt last)
// {
//     while ((first != last) && (first != --last)) {
//         r5::iter_swap(first++, last);
//     }
// }

// // std::prev_permutation is not constexpr
// template<class BidirIt>
// constexpr bool prev_permutation(BidirIt first, BidirIt last)
// {
//     if (first == last) return false;
//     BidirIt i = last;
//     if (first == --i) return false;
// 
//     while (1) {
//         BidirIt i1 = i, i2 = i;
// 
//         i1 = i;
//         if (*i1 < *--i) {
//             i2 = last;
//             while (!(*--i2 < *i))
//                 ;
//             r5::iter_swap(i, i2);
//             r5::reverse(i1, last);
//             return true;
//         }
//         if (i == first) {
//             r5::reverse(first, last);
//             return false;
//         }
//     }
// }

} // namespace r5

// // for uniqueGraphs with unordered_map
// template<class s64, size_t N> 
// struct std::hash<std::array<s64, N>> {
//     auto operator() (const std::array<s64, N>& key) const {
//         size_t result = 0;
//         for(size_t i = 0; i < N; ++i) {
//             result = result * 31 + key[i];
//         }
//         return result;
//     }
// };

template <typename T, std::size_t length>
std::ostream& operator<<(std::ostream& o, const std::array<T, length>& a) {
    o << '[';
    for (s64 i = 0; i < ((s64)length) - 1; i += 1) {
        o << a[i] << ", ";
    }
    if (a.size() > 0) {
        o << a.back();
    }
    o << ']';
    return o;
}

template <typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::pair<T, U>& p) {
    o << '(' << p.first << "," << p.second << ")";
    return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const std::set<T>& s) {
    o << '{';
    std::size_t i = 0;
    for (const auto& e : s) {
        o << e;
        if (i < s.size()-1) {
            o << ", ";
        }
        i += 1;
    }
    o << '}';
    return o;
}

template<class Tuple, std::size_t N>
struct TuplePrinter {
    static std::string print(const Tuple& t)
    {
        std::ostringstream o;
        o << TuplePrinter<Tuple, N-1>::print(t) << ", " << std::get<N-1>(t);
        return o.str();
    }
};

template<class Tuple>
struct TuplePrinter<Tuple, 1> {
    static std::string print(const Tuple& t)
    {
        std::ostringstream o;
        o << std::get<0>(t);
        return o.str();
    }
};

template<class... Args>
std::ostream& operator<<(std::ostream& o, const std::tuple<Args...>& t)
{
    o << "(" << TuplePrinter<decltype(t), sizeof...(Args)>::print(t) << ")";
    return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v) {
    o << '[';
    for (s64 i = 0; i < ((s64)v.size() - 1); i += 1) {
        o << v[i] << ", ";
    }
    if (v.size() > 0) {
        o << v.back();
    }
    o << ']';
    return o;
}

#endif // PREREQS_HPP
