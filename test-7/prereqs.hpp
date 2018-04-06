#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>
#include <vector>

#ifndef R5_VERBOSE
#   define R5_VERBOSE 0
#endif

#if R5_VERBOSE >= 1
#   define R5_VERBOSE_1(x) x
#else
#   define R5_VERBOSE_1(x)
#endif

#include "adjacencymatrix.hpp"

template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T nChooseK(T n, T k) {
    if (n < k) {
        return -1;
    } else if (n < 0) {
        return -1;
    } else if (k < 0) {
        return -1;
    }

    if (k == 0 || k == n) {
        return 1;
    } else {
        return nChooseK(n - 1, k - 1) + nChooseK(n - 1, k);
    }
}

namespace r5 {

// std::swap is not constexpr
template<typename T>
constexpr void swap(T& t1, T& t2) {
    T temp = std::move(t1);
    t1 = std::move(t2);
    t2 = std::move(temp);
}

// std::iter_swap is not constexpr
template<class ForwardIt1, class ForwardIt2>
constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b)
{
    r5::swap(*a, *b);
}

template<class BidirIt>
constexpr void reverse(BidirIt first, BidirIt last)
{
    while ((first != last) && (first != --last)) {
        r5::iter_swap(first++, last);
    }
}

// std::prev_permutation is not constexpr
template<class BidirIt>
constexpr bool prev_permutation(BidirIt first, BidirIt last)
{
    if (first == last) return false;
    BidirIt i = last;
    if (first == --i) return false;

    while (1) {
        BidirIt i1 = i, i2 = i;

        i1 = i;
        if (*i1 < *--i) {
            i2 = last;
            while (!(*--i2 < *i))
                ;
            r5::iter_swap(i, i2);
            r5::reverse(i1, last);
            return true;
        }
        if (i == first) {
            r5::reverse(first, last);
            return false;
        }
    }
}

} // namespace r5

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
