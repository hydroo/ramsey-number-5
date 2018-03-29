#include "test-5-check.hpp"

#include <iostream>
#include <vector>

int expectedResult(int r, int s) {
    R5_ASSERT(r >= 1);
    R5_ASSERT(s >= 1);

    int r_ = r;
    int s_ = s;
    r = std::min(r_, s_);
    s = std::max(r_, s_);

    if (r == 1) {
        return 1;
    } else if (r == 2) {
        return s;
    }

    if (r == 3) {
        const std::vector<int> results{-1, -1, -1, 6, 9, 14, 18, 23, 28, 36};
        if (s >= int(results.size()) - 1) {
            std::cerr << "I don't know R(" << r << "," << s << ")" << std::endl;
            return -1;
        } else {
            return results[s];
        }
    }

    if (r == 4) {
        const std::vector<int> results{-1, -1, -1, -1, 18, 25};
        if (s >= int(results.size()) - 1) {
            std::cerr << "I don't know R(" << r << "," << s << ")" << std::endl;
            return -1;
        } else {
            return results[s];
        }
    }

    std::cerr << "I don't know whether R(" << r << "," << s << ")" << std::endl;
    return -1;
}
