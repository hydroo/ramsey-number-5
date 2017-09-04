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

    std::cerr << "Problem: R(" << *r << "," << *s << ") = ? , where " << *minN << " <= ? <= " << *maxN << std::endl;
    std::cerr << std::endl;
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
        return n == s;
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

void subGraphEdgeMasks(int subGraphSize, int nodes, std::vector<boost::dynamic_bitset<uint64_t>>* edgeMasks) {

    int edges = nodes*(nodes-1)/2;

    edgeMasks->resize(nChooseK(nodes, std::min(subGraphSize, nodes)));

    std::vector<bool> nodeMask(nodes, false);

    for (int i = 0; i < subGraphSize; i += 1) {
        nodeMask[i] = true;
    }

    int p = 0;

    do {

        (*edgeMasks)[p].resize(edges);

        for (int e = 0; e < edges; e += 1) {
            int j = e;
            int y = 0;
            int w = nodes - 1;
            while (j >= w && w > 0) {
                j -= w;
                w -= 1;
                y += 1;
            }
            int x = j + 1 + y;

            if (nodeMask[x] == true && nodeMask[y] == true) {
                (*edgeMasks)[p][e] = true;
            }

        }

        p += 1;

    } while (std::prev_permutation(std::begin(nodeMask), std::end(nodeMask)));
}

int main(int argc, char** args) {

    int r, s, minN, maxN;

    parseArguments(argc, args, &r, &s, &minN, &maxN);

    for (int nodes = minN; nodes <= maxN; nodes += 1) {

        int edges = nodes*(nodes-1)/2;

        std::cerr << "Number of complete subgraphs: " << nChooseK(nodes, r) << "   # n choose r" << std::endl;
        std::cerr << "Number of empty subgraphs:    " << nChooseK(nodes, s) << "   # n choose s" << std::endl;
        std::cerr << "Edges:                        " << edges << "   # n(n-1)/2" << std::endl;
        std::cerr << "Edge colorings:               " << (int64_t) std::pow(2, edges) << "   # 2^e" << std::endl;

        std::vector<boost::dynamic_bitset<uint64_t>> edgeMasksComplete;
        std::vector<boost::dynamic_bitset<uint64_t>> edgeMasksEmpty;

        std::vector<int> edgeMasksCompleteLastOne;
        std::vector<int> edgeMasksEmptyLastZero;

        auto t1 = std::chrono::steady_clock::now();
        subGraphEdgeMasks(r, nodes, &edgeMasksComplete);
        subGraphEdgeMasks(s, nodes, &edgeMasksEmpty);

        for (auto&& edgeMask : edgeMasksEmpty) {
            edgeMask = ~edgeMask;
        }

        for (const auto& mask : edgeMasksComplete) {
            int last = -1;
            for (int i = ((int)mask.size())-1; i >= 0; i -= 1) {
                if (mask[i] == true) {
                    last = i;
                    break;
                }
            }
            edgeMasksCompleteLastOne.push_back(last);
        }

        for (const auto& mask : edgeMasksEmpty) {
            int last = -1;
            for (int i = ((int)mask.size())-1; i >= 0; i -= 1) {
                if (mask[i] == false) {
                    last = i;
                    break;
                }
            }
            edgeMasksEmptyLastZero.push_back(last);
        }

        ASSERT(edgeMasksComplete.size() == edgeMasksCompleteLastOne.size());
        ASSERT(edgeMasksEmpty.size()    == edgeMasksEmptyLastZero.size()  );

        auto t2 = std::chrono::steady_clock::now();

        std::cerr << "Timing: Create subgraph edge masks: " << std::chrono::duration<double>(t2 - t1).count() << " seconds" << std::endl;


        vout << "Complete edge masks:        " << edgeMasksComplete        << std::endl;
        vout << "Complete edge masks last 1: " << edgeMasksCompleteLastOne << std::endl;
        vout << "Empty edge masks:           " << edgeMasksEmpty           << std::endl;
        vout << "Empty edge masks last 0:    " << edgeMasksEmptyLastZero   << std::endl;

        boost::dynamic_bitset<uint64_t> coloring(edges, 0);
        boost::dynamic_bitset<uint64_t> counterExample;
        int64_t recursionSteps     = 0;
        int64_t coloringsChecked   = 0;
        int64_t edgeMaskSizeChecks = 0;

        std::function<bool(int)> foreachColoringHasCompleteOrEmptySubgraph = [nodes, edges, r, s, &coloring, &edgeMasksComplete, &edgeMasksCompleteLastOne, &edgeMasksEmpty, &edgeMasksEmptyLastZero, &counterExample, &recursionSteps, &coloringsChecked, &edgeMaskSizeChecks, &foreachColoringHasCompleteOrEmptySubgraph](int nextEdge) -> bool {

            recursionSteps += 1;

            for (std::size_t i = 0; i < edgeMasksComplete.size(); i += 1) {
                edgeMaskSizeChecks += 1;
                if (edgeMasksCompleteLastOne[i] == nextEdge-1) {
                    if ((coloring &  edgeMasksComplete[i]) == edgeMasksComplete[i]) {
                        if (nodes >= r) { // avoids matching subgraphs larger than the to-be-checked graph
                            coloringsChecked += 1;
                            return true;
                        }
                    }
                }
            }

            for (std::size_t i = 0; i < edgeMasksEmpty.size(); i += 1) {
                edgeMaskSizeChecks += 1;
                if (edgeMasksEmptyLastZero[i] == nextEdge-1) {
                    if ((coloring |  edgeMasksEmpty[i]) == edgeMasksEmpty[i]) {
                        if (nodes >= s) { // avoids matching subgraphs larger than the to-be-checked graph
                            coloringsChecked += 1;
                            return true;
                        }
                    }
                }
            }

            if (nextEdge == edges) {
                coloringsChecked += 1;
                counterExample = coloring;
                return false;
            }

            bool ret;

            coloring[nextEdge] = true;
            ret = foreachColoringHasCompleteOrEmptySubgraph(nextEdge+1);

            if (ret == false) { return false; }

            coloring[nextEdge] = false;
            ret = foreachColoringHasCompleteOrEmptySubgraph(nextEdge+1);

            if (ret == false) {
                return false;
            } else {
                return true;
            }
        };

        auto t3 = std::chrono::steady_clock::now();
        bool allColoringsHaveCompleteOrEmptySubgraph = foreachColoringHasCompleteOrEmptySubgraph(0);
        auto t4 = std::chrono::steady_clock::now();
        std::cerr << "Timing: Check all colorings:          " << std::chrono::duration<double>(t4 - t3).count() << " seconds" << std::endl;
        std::cerr << "Timing: Number of recursion steps:    " << recursionSteps     << std::endl;
        std::cerr << "Timing: Number of colorings checked:  " << coloringsChecked   << std::endl;
        std::cerr << "Timing: Number edge mask size checks: " << edgeMaskSizeChecks << std::endl;

        if (allColoringsHaveCompleteOrEmptySubgraph == true) {
            if (nodes > minN) {
                std::cerr << "R(" << r << "," << s << ")  = " << nodes << std::endl;
            } else {
                std::cerr << "R(" << r << "," << s << ") <= " << nodes << std::endl;
            }
        } else {
            std::cerr << "R(" << r << "," << s << ")  > " << nodes << std::endl;
            std::cerr << "Counter example: " << std::endl;
            printAdjacencyMatrix(std::cerr, counterExample, nodes, "    ");
        }

        if (checkResult(r, s, nodes) != allColoringsHaveCompleteOrEmptySubgraph) {
            std::cerr << "Error: Check Result for R(" << r << "," << s << ") <= " << nodes << " disagrees with our result (" << allColoringsHaveCompleteOrEmptySubgraph << ")" << std::endl;
            std::abort();
        }

        if (allColoringsHaveCompleteOrEmptySubgraph == true) { break; }

    }

    return 0;
}
