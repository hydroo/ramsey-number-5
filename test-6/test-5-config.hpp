#ifndef TEST_5_CONFIG_HPP
#define TEST_5_CONFIG_HPP

#include "test-5-prereqs.hpp"

#include <cmath>

#include "config.inc"

static_assert(config_inc::r >= 1);
static_assert(config_inc::s >= 1);
static_assert(config_inc::n >= std::min(config_inc::r, config_inc::s));

namespace config {

constexpr s64 r = std::min(config_inc::r, config_inc::s);
constexpr s64 s = std::max(config_inc::r, config_inc::s);
constexpr s64 n = config_inc::n;

constexpr s64 e = n * (n - 1) / 2;

std::string print(const std::string indent = "") {
    std::ostringstream o;

    o << indent << "Problem: R(" << r << "," << s << ") <= " << n << " ? " << std::endl;
    o.imbue(std::locale("en_US.UTF-8"));
    o << indent << "Number of complete subgraphs: " << std::setw(42) << nChooseK(n, r) << "   # n choose r"
      << std::endl;
    o << indent << "Number of empty subgraphs:    " << std::setw(42) << nChooseK(n, s) << "   # n choose s"
      << std::endl;
    o << indent << "Edges:                        " << std::setw(42) << e << "   # n*(n-1)/2" << std::endl;
    o << indent << "Edge colorings:               " << std::setw(42) << std::fixed << std::setprecision(0)
      << std::pow(2, e) << "   # 2^e" << std::endl;

    return o.str();
}

} // namespace config

#endif // TEST_5_CONFIG_HPP
