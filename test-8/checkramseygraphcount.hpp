#pragma once

#include "prereqs.hpp"

using r5::Size;

bool checkRamseyGraphCount(Size r_, Size s_, Size n, s64 ramseyGraphCount) {
    auto r = std::min(r_, s_);
    auto s = std::max(r_, s_);

    const std::map<std::tuple<Size, Size, Size>, s64> knownResults{
        {{ 1,  2,  2},         0},
        {{ 2,  2,  2},         0},
        {{ 2,  3,  3},         0},
        {{ 2,  4,  4},         0},
        {{ 2,  5,  5},         0},
        {{ 2,  6,  6},         0},
        {{ 2,  7,  7},         0},
        {{ 2,  8,  8},         0},
        {{ 2,  9,  9},         0},
        {{ 2, 10, 10},         0},
        {{ 3,  3,  6},         0},
        {{ 3,  4,  1},         1},
        {{ 3,  4,  2},         2},
        {{ 3,  4,  3},         3},
        {{ 3,  4,  4},         6},
        {{ 3,  4,  5},         9},
        {{ 3,  4,  6},        15},
        {{ 3,  4,  7},         9},
        {{ 3,  4,  8},         3},
        {{ 3,  4,  9},         0},
        {{ 3,  5,  1},         1},
        {{ 3,  5,  2},         2},
        {{ 3,  5,  3},         3},
        {{ 3,  5,  4},         7},
        {{ 3,  5,  5},        13},
        {{ 3,  5,  6},        32},
        {{ 3,  5,  7},        71},
        {{ 3,  5,  8},       179},
        {{ 3,  5,  9},       290},
        {{ 3,  5, 10},       313},
        {{ 3,  5, 11},       105},
        {{ 3,  5, 12},        12},
        {{ 3,  5, 13},         1},
        {{ 3,  5, 14},         0},
        {{ 3,  6,  1},         1},
        {{ 3,  6,  2},         2},
        {{ 3,  6,  3},         3},
        {{ 3,  6,  4},         7},
        {{ 3,  6,  5},        14},
        {{ 3,  6,  6},        37},
        {{ 3,  6,  7},       100},
        {{ 3,  6,  8},       356},
        {{ 3,  6,  9},      1407},
        {{ 3,  6, 10},      6657},
        {{ 3,  6, 11},     30395},
        {{ 3,  6, 12},    116792},
        {{ 3,  6, 13},    275086},
        {{ 3,  6, 14},    263520},
        {{ 3,  6, 15},     64732},
        {{ 3,  6, 16},      2576},
        {{ 3,  6, 17},         7},
        {{ 3,  6, 18},         0},
        {{ 3,  7, 21},   1118436},
        {{ 3,  7, 22},       191},
        {{ 3,  7, 23},         0},
        {{ 3,  8, 27},    477142},
        {{ 3,  8, 28},         0},
        {{ 3,  9, 35},         1},
        {{ 3,  9, 36},         0},
        {{ 4,  4,  1},         1},
        {{ 4,  4,  2},         2},
        {{ 4,  4,  3},         4},
        {{ 4,  4,  4},         9},
        {{ 4,  4,  5},        24},
        {{ 4,  4,  6},        84},
        {{ 4,  4,  7},       362},
        {{ 4,  4,  8},      2079},
        {{ 4,  4,  9},     14701},
        {{ 4,  4, 10},    103706},
        {{ 4,  4, 11},    546356},
        {{ 4,  4, 12},   1449166},
        {{ 4,  4, 13},   1184231},
        {{ 4,  4, 14},    130816},
        {{ 4,  4, 15},       640},
        {{ 4,  4, 16},         2},
        {{ 4,  4, 17},         1},
        {{ 4,  4, 18},         0},
        {{ 4,  5, 24},    352366}, // https://arxiv.org/abs/1703.08768
        {{ 4,  5, 25},         0},
    };

    auto i = knownResults.find(std::make_tuple(r, s, n));
    if (i != knownResults.end() && i->second != ramseyGraphCount) {
        std::cerr << "Error: R(" << r << ", " << s <<", " << n << ") = " << ramseyGraphCount << ", but it should be "<< i->second << "!" << std::endl;
        std::abort();
    }
    return true;
}