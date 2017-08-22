#ifndef PREREQS_HPP
#define PREREQS_HPP

#include <algorithm>
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

#endif /* PREREQS_HPP */
