#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
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

template <std::size_t N>
void printAdjacencyMatrix(std::ostream& o, const std::bitset<N>& m, int nodes, const std::string& indent = "") {
    ASSERT(nodes*(nodes-1)/2 == m.size());

    for (auto n = nodes-1; n > 0; n -= 1) {

        if (n == nodes-1) {
            o << indent << '[';
        } else {
            o << indent << ' ';
        }

        for (int i = n; i < nodes-1; i += 1) {
            o << ' ';
        }

        for (int i = 0; i < n; i += 1) {
            o << m[i];
        }

        if (n > 1) {
            o << std::endl;
        }
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

constexpr int pow(auto base, auto exponent) {
    return (exponent != 0 )? base * pow(base, exponent - 1) : 1;
}


#endif /* PREREQS_HPP */
