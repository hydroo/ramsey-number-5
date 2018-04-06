#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "prereqs.hpp"

#include <cmath>

#include "config.inc"

static_assert(config_inc::r >= 1);
static_assert(config_inc::s >= 1);
static_assert(config_inc::n >= std::min(config_inc::r, config_inc::s));
static_assert(config_inc::u >= 1 || config_inc::u == -1);

namespace config {

constexpr s64 r = std::min(config_inc::r, config_inc::s);
constexpr s64 s = std::max(config_inc::r, config_inc::s);
constexpr s64 n = config_inc::n;
constexpr s64 u = config_inc::u == -1 ? std::min(n, (s64) 5) : std::min(n, config_inc::u);

constexpr s64 e  = n * (n - 1) / 2;
constexpr s64 ue = u * (u - 1) / 2;

std::string print(const std::string indent = "") {
    std::ostringstream o;

    o << indent << "Problem: R(" << r << "," << s << ") <= " << n << " ? " << std::endl;
    o.imbue(std::locale("en_US.UTF-8"));
    o << indent << "Number of complete subgraphs:  " << std::setw(9) << nChooseK(n, r) << "   # n choose r"
      << std::endl;
    o << indent << "Number of empty subgraphs:     " << std::setw(9) << nChooseK(n, s) << "   # n choose s"
      << std::endl;
    o << indent << "Edges:                         " << std::setw(9) << e << "   # n*(n-1)/2" << std::endl;

    std::ostringstream p;
    p << "2^" << e;

    o << indent << "Edge colorings:                " << std::setw(9) << p.str() << "   # 2^e" << std::endl;
    o << indent << "Unique base graphs node count: " << std::setw(9) << u << std::endl;

    return o.str();
}

} // namespace config

#endif // CONFIG_HPP
