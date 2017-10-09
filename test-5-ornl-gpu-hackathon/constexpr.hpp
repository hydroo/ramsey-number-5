#ifndef CONSTEXPR_HPP
#define CONSTEXPR_HPP

constexpr int nChooseK(int n, int k) {
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

#endif  // CONSTEXPR_HPP
