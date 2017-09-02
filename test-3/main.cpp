#include "prereqs.hpp"

void parseArguments(int argc, char **args, int* r, int* s, int* minN, int* maxN) {
    if (argc < 2) {
        std::cout << "Usage:" << std::endl
                  << "  " << args[0] << " <r> [s] [minN] [maxN]" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    *r = std::stoi(args[1]); // throws std::invalid_argument, std::out_of_range
    if (*r < 1) {
        std::cerr << "r (" << *r << ") < 1. Aborting." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    *s = *r;
    if (argc >= 3) {
        *s = std::stoi(args[2]); // throws std::invalid_argument, std::out_of_range
    }
    if (*s < 1) {
        std::cerr << "s (" << *s << ") < 1. Aborting." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    *minN = std::min(*r, *s);
    if (argc >= 4) {
        *minN = std::max(*minN, std::stoi(args[3])); // throws std::invalid_argument, std::out_of_range
    }
    if (*minN < 1) {
        std::cerr << "minN (" << *minN << ") < 1. Aborting." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    *maxN = std::numeric_limits<int>::max();
    if (argc >= 5) {
        *maxN = std::max(*minN, std::stoi(args[4])); // throws std::invalid_argument, std::out_of_range
    }
    if (*maxN < 1) {
        std::cerr << "maxN (" << *maxN << ") < 1. Aborting." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cerr << "Problem: R(" << *r << "," << *s << ") = ?   , where " << *minN << " <= ? <= " << *maxN << std::endl;
}

bool checkResult(int r, int s, int n) {

    ASSERT(r >= 1);
    ASSERT(s >= 1);
    ASSERT(n >= std::min(r, s));

    int r_ = r;
    int s_ = s;
    r = std::min(r_, s_);
    s = std::max(r_, s_);

    if (r == 1) {
        return n == 1;
    } else if (r == 2) {
        return n == r;
    }

    if (r == 3) {
        const std::vector<int> results{-1, -1, -1, 6, 9, 14, 18, 23, 28, 36};
        if (s >= int(results.size())-1) {
            std::cerr << "I don't know whether R(" << r << "," << s << ") = "<< n<< std::endl;
            return false;
        } else {
            return results[s] == n;
        }
    }

    if (r == 4) {
        const std::vector<int> results{-1, -1, -1, -1, 18, 25};
        if (s >= int(results.size())-1) {
            std::cerr << "I don't know whether R(" << r << "," << s << ") = " << n<< std::endl;
            return false;
        } else {
            return results[s] == n;
        }
    }

    std::cerr << "I don't know whether R(" << r << "," << s << ") = "<< n<< std::endl;
    return false;
}

int main(int argc, char** args) {

    int r, s, minN, maxN;

    parseArguments(argc, args, &r, &s, &minN, &maxN);



    return 0;
}
