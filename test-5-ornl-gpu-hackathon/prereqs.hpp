#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define ASSERT(expression)                                                                                \
    if ((expression) == false) {                                                                          \
        std::cerr << "\e[0;31m\033[1m"                                                                    \
                  << "ASSERT"                                                                             \
                  << "\033[0m\e[0;30m"                                                                    \
                  << " in " << __FILE__ << ":" << __LINE__ << ": \"" << #expression << "\"" << std::endl; \
        std::abort();                                                                                     \
    }

template <std::size_t N>
std::string bitsetToString(const std::bitset<N>& v, bool reverseString = true) {
    std::ostringstream o;
    auto s = v.to_string();
    if (reverseString) {
        std::reverse(begin(s), end(s));
    }
    o << '[' << s << ']';
    return o.str();
}

template <typename T, std::size_t length>
std::ostream& operator<<(std::ostream& o, const std::array<T, length>& a) {
    o << '[';
    for (int i = 0; i < ((int)length) - 1; i += 1) {
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
    for (auto i = begin(v); i != --end(v); ++i) {
        o << *i << ", ";
    }
    if (v.size() > 0) {
        o << v.back();
    }
    o << ']';
    return o;
}

template <std::size_t length>
std::ostream& operator<<(std::ostream& o, const std::bitset<length>& v) {
    o << bitsetToString(v, true);
    return o;
}

template <typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::map<T, U>& m) {
    o << '{';
    for (auto i = std::begin(m); i != --std::end(m); ++i) {
        o << i->first << ":" << i->second << ", ";
    }
    if (m.size() > 0) {
        auto i = --std::end(m);
        o << i->first << ":" << i->second;
    }
    o << '}';
    return o;
}

template <typename T, typename U>
std::ostream& operator<<(std::ostream& o, const std::unordered_map<T, U>& u) {
    std::map<T, U> m;
    for (auto i : u) {
        m[i.first] = i.second;
    }
    o << m;
    return o;
}

template <std::size_t nodes>
std::string printAdjacencyMatrix(const std::bitset<nodes*(nodes - 1) / 2>& m, const std::string& indent = "") {
    std::ostringstream o;

    int offset = 0;
    for (auto n = nodes - 1; n > 0; n -= 1) {
        if (n == nodes - 1) {
            o << indent << '[';
        } else {
            o << indent << ' ';
        }

        for (auto i = n; i < nodes - 1; i += 1) {
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

    return o.str();
}

template <std::size_t length, std::size_t length2>
std::string printMasksByLastDigit(const std::array<std::vector<std::bitset<length>>, length2>& a) {
    std::ostringstream o;

    o << '{';
    for (int i = 0; i < ((int)a.size() - 1); i += 1) {
        o << i - 1 << ":" << a[i].size() << ", ";
    }
    if (a.size() > 0) {
        o << a.size() - 1 << ":" << a.back().size();
    }
    o << '}';

    return o.str();
}

#endif  // PREREQS_HPP
