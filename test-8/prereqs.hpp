#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdint.h>
#include <unordered_map>
#include <vector>

//#include <boost/container/flat_map.hpp>
//#include <boost/container/flat_set.hpp>
//#include <boost/container/small_vector.hpp>
//#include <boost/container/static_vector.hpp>
#include <boost/functional/hash.hpp>

#ifdef R5_GTEST
#   include <gtest/gtest.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#   define R5_OS_MAC
#elif defined(__linux__)
#   define R5_OS_LINUX
#elif defined(_WIN32)
#   define R5_OS_WINDOWS
#endif

#if defined(R5_OS_MAC)
#   include<mach/mach.h>
#elif defined(R5_OS_LINUX)
#   include <sys/resource.h>
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

#define R5_NOINLINE __attribute__((noinline))

#if defined(RELWITHDEBINFO) || defined(DEBUG)
    #define R5_DEBUG_NOINLINE R5_NOINLINE
#else // RELEASE
    #define R5_DEBUG_NOINLINE
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

// // for uniqueGraphs with unordered_map
// template<class s64, std::size_t N>
// struct std::hash<std::array<s64, N>> {
//     auto operator() (const std::array<s64, N>& key) const {
//         std::size_t result = 0;
//         for(std::size_t i = 0; i < N; ++i) {
//             result = result * 31 + key[i];
//         }
//         return result;
//     }
// };

template <typename T, std::size_t length>
std::ostream& operator<<(std::ostream& o, const std::array<T, length>& a);
template <typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::pair<T, U>& p);
//template <typename K, typename V>
//std::ostream& operator<<(std::ostream& o, const boost::container::flat_map<K, V>& m);
template <typename K, typename V>
std::ostream& operator<<(std::ostream& o, const std::map<K, V>& m);
template <typename T>
std::ostream& operator<<(std::ostream& o, const std::set<T>& s);
template<class... Args>
std::ostream& operator<<(std::ostream& o, const std::tuple<Args...>& t);
//template <typename T, std::size_t Capacity>
//std::ostream& operator<<(std::ostream& o, const boost::container::small_vector<T, Capacity>& v);
//template <typename T, std::size_t Capacity>
//std::ostream& operator<<(std::ostream& o, const boost::container::static_vector<T, Capacity>& v);
template <typename K, typename V>
std::ostream& operator<<(std::ostream& o, const std::unordered_map<K, V>& m);
template <typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v);

namespace r5 {
    using ::operator<<;
}

template <typename T, std::size_t Capacity>
std::ostream& operator<<(std::ostream& o, const std::array<T, Capacity>& a) {
    o << '[';
    for (s64 i = 0; i < ((s64)Capacity) - 1; i += 1) {
        o << a[i] << ", ";
    }
    if (a.size() > 0) {
        o << a.back();
    }
    o << ']';
    return o;
}

//template <typename K, typename V>
//std::ostream& operator<<(std::ostream& o, const boost::container::flat_map<K, V>& m) {
//    o << '{';
//
//    for (auto i = m.cbegin(); i != m.cend(); ++i) {
//        o << i->first << " : " << i->second;
//        auto j = i;
//        if (++j != m.cend()) {
//            o << ", ";
//        }
//    }
//    o << '}';
//    return o;
//}

//template <typename T>
//std::ostream& operator<<(std::ostream& o, const boost::container::flat_set<T>& s) {
//    o << '{';
//    std::size_t i = 0;
//    for (const auto& e : s) {
//        o << e;
//        if (i < s.size()-1) {
//            o << ", ";
//        }
//        i += 1;
//    }
//    o << '}';
//    return o;
//}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& o, const std::map<K, V>& m) {
    o << '{';

    for (auto i = m.cbegin(); i != m.cend(); ++i) {
        o << i->first << " : " << i->second;
        auto j = i;
        if (++j != m.cend()) {
            o << ", ";
        }
    }
    o << '}';
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

//template <typename T, std::size_t Capacity>
//std::ostream& operator<<(std::ostream& o, const boost::container::small_vector<T, Capacity>& v) {
//    o << '[';
//    for (s64 i = 0; i < ((s64)v.size() - 1); i += 1) {
//        o << v[i] << ", ";
//    }
//    if (v.size() > 0) {
//        o << v.back();
//    }
//    o << ']';
//    return o;
//}

//template <typename T, std::size_t Capacity>
//std::ostream& operator<<(std::ostream& o, const boost::container::static_vector<T, Capacity>& v) {
//    o << '[';
//    for (s64 i = 0; i < ((s64)v.size() - 1); i += 1) {
//        o << v[i] << ", ";
//    }
//    if (v.size() > 0) {
//        o << v.back();
//    }
//    o << ']';
//    return o;
//}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& o, const std::unordered_map<K, V>& m) {
    o << '{';

    for (auto i = m.cbegin(); i != m.cend(); ++i) {
        o << i->first << " : " << i->second;
        auto j = i;
        if (++j != m.cend()) {
            o << ", ";
        }
    }
    o << '}';
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

namespace r5 {
// Needed to print u8 as int rather than character
template<std::size_t Capacity>
static std::array<u32, Capacity> u8ArrayToU32(const std::array<u8, Capacity>& a) {
    std::array<u32, Capacity> ret;
    for (std::size_t i = 0; i < a.size(); ++i) {
        ret[i] = u32(a[i]);
    }
    return ret;
}
}

namespace std {

    template <typename T, std::size_t Capacity>
    struct hash<std::array<T, Capacity>> {
        std::size_t operator()(const std::array<T, Capacity>& a) const {
            std::size_t seed = 0;
            for (const auto& v : a) {
                boost::hash_combine(seed, v);
            }
            return seed;
        }
    };

    namespace {
        template <class Tuple, std::size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl {
          static void apply(std::size_t& seed, const Tuple& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            boost::hash_combine(seed, std::get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0> {
          static void apply(std::size_t& seed, const Tuple& tuple) {
            boost::hash_combine(seed, std::get<0>(tuple));
          }
        };
    }

    template <typename ... TT>
    struct hash<std::tuple<TT...>> {
        std::size_t operator()(const std::tuple<TT...>& tt) const {
            std::size_t seed = 0;
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
            return seed;
        }
    };
}

namespace r5 {
std::size_t memoryUsage() {
    std::size_t usedBytes = 0;
#if defined(R5_OS_MAC)
    struct task_basic_info info;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;
    kern_return_t ret =  task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &infoCount);
    if (ret == KERN_SUCCESS) {
        usedBytes = info.resident_size;
    }
#elif defined(R5_OS_LINUX)
    struct rusage ru;
    int ret = getrusage(RUSAGE_SELF, &ru);
    if (ret == 0) {
        usedBytes = ru.ru_maxrss * 1024; // ru_maxrss is in KiB;
    }

#else
#   pragma message("not implemented")
#endif
    return usedBytes;
}
} // namespace r5

#endif // PREREQS_HPP
