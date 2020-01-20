#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "prereqs.hpp"

#include <cmath>

#include "config.inc"

static_assert(config_inc::r >= 1);
static_assert(config_inc::s >= 1);
//static_assert(config_inc::n >= std::min(config_inc::r, config_inc::s));

namespace config {

constexpr s64 r = std::min(config_inc::r, config_inc::s);
constexpr s64 s = std::max(config_inc::r, config_inc::s);
constexpr s64 n = config_inc::n;

constexpr s64 e  = n * (n - 1) / 2;

std::string print(const std::string indent = "") {
    std::ostringstream o;

    o << indent << "Nodes                   : " << std::setw(6) << n << "   # n" << std::endl;
    o << indent << "Nodes Complete Subgraphs: " << std::setw(6) << r << "   # r" << std::endl;
    o << indent << "Nodes Empty Subgraphs   : " << std::setw(6) << s << "   # s" << std::endl;
    o << indent << "Edges                   : " << std::setw(6) << e << "   # n*(n-1)/2" << std::endl;

    return o.str();
}

} // namespace config

#endif // CONFIG_HPP
