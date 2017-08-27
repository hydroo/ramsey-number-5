#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef DEBUG
#define ASSERT(expression)                                                                                \
    if ((expression) == false) {                                                                          \
        std::cerr << "\e[0;31m\033[1m"                                                                    \
                  << "ASSERT"                                                                             \
                  << "\033[0m\e[0;30m"                                                                    \
                  << " in " << __FILE__ << ":" << __LINE__ << ": \"" << #expression << "\"" << std::endl; \
        std::abort();                                                                                     \
    }
#else
#define ASSERT(expression)
#endif

template <std::size_t nodes>
void printAdjacencyMatrix(std::ostream& o, const std::bitset<nodes*(nodes-1)/2>& m, const std::string& indent = "") {

    int offset=0;
    for (auto n = nodes-1; n > 0; n -= 1) {

        if (n == nodes-1) {
            o << indent << '[';
        } else {
            o << indent << ' ';
        }

        for (auto i = n; i < nodes-1; i += 1) {
            o << ' ';
        }

        for (std::size_t i = 0; i < n; i += 1) {
            o << m[offset + i];
        }

        if (n > 1) {
            o << std::endl;
        }
        offset += n;
    }
    o << ']' << std::endl;
}

template <std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::bitset<N>& v) {
    o << '[' << v.to_string() << ']';
    return o;
}

template <typename T, long unsigned int N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& v) {
    o << '[';
    for (unsigned long int i = 0; i < N-1; i += 1) {
        o << v[i] << ", ";
    }
    if (v.size() > 0) {
        o << v.back();
    }
    o << ']';
    return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v) {
    o << '[';
    for (auto i = begin(v); i != --end(v); ++i) {
        o << *i << ", ";
    }
    if (v.size() > 0) {
        o << v.back();
    }
    o << ']';
    return o;
}

template <typename T>
std::string arrayToString(T * a, std::size_t length) {
    std::ostringstream o;

    o << '[';
    for (std::size_t i = 0; i < length-1; i += 1) {
        o << a[i] << ", ";
    }
    if (length > 0) {
        o << a[length-1];
    }
    o << ']';

    return o.str();
}

template<typename T>
constexpr T constPow(T base, T exponent) {
    return (exponent != 0 ) ? base * pow(base, exponent - 1) : 1;
}

constexpr int nChooseK(int n, int k) {

    if      (n < k) { return -1; }
    else if (n < 0) { return -1; }
    else if (k < 0) { return -1; }

    if (k == 0 || k == n) {
        return 1;
    } else {
        return nChooseK(n-1, k-1) + nChooseK(n-1, k);
    }
}

template<typename T>
constexpr void constSwap(T* a, T* b) {
    T tmp = *a;
    *a    = *b;
    *b    = tmp;
}

template<typename T>
constexpr void constReverse(T* p, std::size_t from, std::size_t to) {

    if (from >= to) { return; }

    for (std::size_t i = 0; i < (to - from+1)/2; i += 1) {
        constSwap(&p[from+i], &p[to-i]);
    }

}

template<typename T>
constexpr bool previousPermutation(T* p, std::size_t length)
{

    //std::cerr << "before" << arrayToString(p, length) << " length " << length << std::endl;

    if (length < 2) { return false; }

    int i = length-1;

    while (true) {
        int j = i;
        i -= 1;

        //std::cerr << "  i " << i << " j " << j << " p[i] " << p[i] << " p[j] " << p [j] << " p[i] < p[j] " << (p[i] < p[j]) << std::endl;

        if (p[i] > p[j]) {
            int k = length-1;

            while (!(p[k] < p[i]))
                k -= 1;

            //std::cerr << "  swap p[i] p[k] (p[" << i << "] " << "p[" << k << "]" << std::endl;
            constSwap(&p[i], &p[k]);
            //std::cerr << "  reverse j to length-1; j " << j << " length-1 " << length-1 << std::endl;
            constReverse(p, j, length-1);
            //std::cerr << "after" << arrayToString(p, length) << " return true" << std::endl;
            return true;
        }

        if (i == 0)
        {
            constReverse(p, 0, length-1);
            //std::cerr << "after" << arrayToString(p, length) << " return false" << std::endl;
            return false;
        }
    }
}

#endif /* PREREQS_HPP */
