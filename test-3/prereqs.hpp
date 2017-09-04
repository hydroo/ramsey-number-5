#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#define ASSERT(expression)                                                                                \
    if ((expression) == false) {                                                                          \
        std::cerr << "\e[0;31m\033[1m"                                                                    \
                  << "ASSERT"                                                                             \
                  << "\033[0m\e[0;30m"                                                                    \
                  << " in " << __FILE__ << ":" << __LINE__ << ": \"" << #expression << "\"" << std::endl; \
        std::abort();                                                                                     \
    }

#ifdef DEBUG
#   define DEBUG_ASSERT(expression) ASSERT(expression)
#else
#   define DEBUG_ASSERT(expression)
#endif

std::ostringstream vout;

void init_prereqs() {
#ifdef VERBOSE
#   define vout std::cerr
#else
    vout.setstate(std::ios_base::badbit); // makes it a sink
#endif

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

std::ostream& operator<<(std::ostream& o, const std::vector<bool>& v) {
    for (auto i = std::begin(v); i != std::end(v); ++i) {
        o << *i;
    }
    return o;
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const boost::dynamic_bitset<T>& v) {
    for (std::size_t i = 0; i < v.size(); i += 1) {
        o << v[i];
    }
    return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v) {
    o << '[';
    for (auto i = std::begin(v); i != --std::end(v); ++i) {
        o << *i << ", ";
    }
    if (v.size() > 0) {
        o << v.back();
    }
    o << ']';
    return o;
}

void printAdjacencyMatrix(std::ostream& o, const boost::dynamic_bitset<uint64_t>& m, int nodes, const std::string& indent = "") {

    o << indent << '[';

    int offset=0;
    for (auto n = nodes-1; n > 0; n -= 1) {

        if (n < nodes-1) {
            o << indent << ' ';
        }

        for (auto i = n; i < nodes-1; i += 1) {
            o << ' ';
        }

        for (int i = 0; i < n; i += 1) {
            o << m[offset + i];
        }

        if (n > 1) {
            o << std::endl;
        }
        offset += n;
    }
    o << ']' << std::endl;
}

#endif /* PREREQS_HPP */
