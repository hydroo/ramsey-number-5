#ifndef TEST_5_PREREQS_HPP
#define TEST_5_PREREQS_HPP

#include <cmath>
#include <iomanip>
#include <sstream>

template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T nChooseK(T n, T k) {
    if (n < k) {
        return -1;
    } else if (n < 0) {
        return -1;
    } else if (k < 0) {
        return -1;
    }

    if (k == 0 || k == n) {
        return 1;
    } else {
        return nChooseK(n - 1, k - 1) + nChooseK(n - 1, k);
    }
}

#endif // TEST_5_PREREQS_HPP
