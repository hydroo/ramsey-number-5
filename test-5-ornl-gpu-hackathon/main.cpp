#include "prereqs.hpp"

#include "check.hpp"
#include "config.hpp"

template <int edges, int nodes, int subGraphSize>
void subGraphEdgeMasks(std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)>* edgeMasks) {
    std::array<bool, nodes> nodeMask;

    for (int i = 0; i < subGraphSize; i += 1) {
        nodeMask[i] = true;
    }
    for (int i = subGraphSize; i < nodes; i += 1) {
        nodeMask[i] = false;
    }

    // std::cerr << "nodemask " << nodeMask << std::endl;

    int p = 0;

    do {
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
            } else {
                (*edgeMasks)[p][e] = false;
            }
        }

        // std::cerr << "node mask " << nodeMask << ", edge mask " << (*edgeMasks)[p] << std::endl;

        p += 1;

    } while (std::prev_permutation(std::begin(nodeMask), std::end(nodeMask)));
}

template <int edges, int nodes, int subGraphSize>
void invertSubgraphEdgeMasks(std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)>* masks) {
    for (auto&& m : *masks) {
        m = ~m;
    }
}

template <int edges, int nodes, int subGraphSize, bool digit>
void subGraphEdgeMasksByLastDigit(std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)> edgeMasks,
        std::array<std::vector<std::bitset<edges>>, edges + 1>* edgeMasksByLastDigit) {
    for (const auto& mask : edgeMasks) {
        int last =
                -1;  // offset everything by +1 so that we can use -1, and don't have to worry about array[nextEdge-1]
        for (int i = ((int)mask.size()) - 1; i >= 0; i -= 1) {
            if (mask[i] == digit) {
                last = i;
                break;
            }
        }
        (*edgeMasksByLastDigit)[last + 1].push_back(mask);
    }
}

int main(int argc, char** args) {
    (void)argc;
    (void)args;

    std::cerr << config::print() << std::endl;

    auto t1 = std::chrono::steady_clock::now();

    std::array<std::bitset<config::e>, nChooseK(config::n, config::r)> edgeMasksComplete;
    std::array<std::bitset<config::e>, nChooseK(config::n, config::s)> edgeMasksEmpty;

    subGraphEdgeMasks<config::e, config::n, config::r>(&edgeMasksComplete);
    subGraphEdgeMasks<config::e, config::n, config::s>(&edgeMasksEmpty);

    invertSubgraphEdgeMasks<config::e, config::n, config::s>(&edgeMasksEmpty);

    std::array<std::vector<std::bitset<config::e>>, config::e + 1> edgeMasksCompleteByLastOne;
    std::array<std::vector<std::bitset<config::e>>, config::e + 1> edgeMasksEmptyByLastZero;

    subGraphEdgeMasksByLastDigit<config::e, config::n, config::r, true>(edgeMasksComplete, &edgeMasksCompleteByLastOne);
    subGraphEdgeMasksByLastDigit<config::e, config::n, config::s, false>(edgeMasksEmpty, &edgeMasksEmptyByLastZero);

    auto t2 = std::chrono::steady_clock::now();

    std::cerr << "Timing: Create subgraph edge masks:    " << std::fixed
              << std::chrono::duration<double>(t2 - t1).count() << " seconds" << std::endl;

    // std::cerr << "Complete edge masks:                   " << edgeMasksComplete << std::endl;
    // std::cerr << "Complete edge masks by last 1(last+1): " << edgeMasksCompleteByLastOne << std::endl;
    // std::cerr << "Empty edge masks:                      " << edgeMasksEmpty << std::endl;
    // std::cerr << "Empty edge masks by last 0   (last+1): " << edgeMasksEmptyByLastZero << std::endl;

    // std::cerr << "Complete edge masks by last 1 (last:vectorsize): "
    //           << printMasksByLastDigit(edgeMasksCompleteByLastOne) << std::endl;
    // std::cerr << "Empty edge masks by last 0    (last:vectorsize): " << printMasksByLastDigit(edgeMasksEmptyByLastZero)
    //           << std::endl;

    std::bitset<config::e> coloring;
    std::bitset<config::e> counterExample;
    int64_t recursionSteps = 0;
    int64_t coloringsChecked = 0;
    int64_t edgeMaskChecks = 0;

    std::function<bool(int)> foreachColoringHasCompleteOrEmptySubgraph = [&coloring, &edgeMasksCompleteByLastOne,
            &edgeMasksEmptyByLastZero, &counterExample, &recursionSteps, &coloringsChecked, &edgeMaskChecks,
            &foreachColoringHasCompleteOrEmptySubgraph](int nextEdge) -> bool {

        recursionSteps += 1;

        // std::cerr << "  " << coloring << " nextEdge " << nextEdge << std::endl;

        const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[nextEdge - 1 + 1];
        for (int i = 0; i < (int)currentEdgeMasksComplete.size(); i += 1) {
            edgeMaskChecks += 1;
            if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) {
                if (config::n >= config::r) {  // avoids matching subgraphs larger than the to-be-checked graph
                    // std::cerr << "      Mask " << currentEdgeMasksComplete[i] << " is a subgraph" << std::endl;
                    coloringsChecked += 1;
                    return true;
                }
            }
        }

        const auto& currentEdgeMasksEmpty = edgeMasksEmptyByLastZero[nextEdge - 1 + 1];
        for (int i = 0; i < (int)currentEdgeMasksEmpty.size(); i += 1) {
            edgeMaskChecks += 1;
            if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                if (config::n >= config::s) {  // avoids matching subgraphs larger than the to-be-checked graph
                    // std::cerr << "      Mask " << mask << " is a subgraph" << std::endl;
                    coloringsChecked += 1;
                    return true;
                }
            }
        }

        if (nextEdge == config::e) {
            coloringsChecked += 1;
            counterExample = coloring;
            return false;
        }

        bool ret;

        coloring[nextEdge] = true;
        ret = foreachColoringHasCompleteOrEmptySubgraph(nextEdge + 1);

        if (ret == false) {
            return false;
        }

        coloring[nextEdge] = false;
        ret = foreachColoringHasCompleteOrEmptySubgraph(nextEdge + 1);

        if (ret == false) {
            return false;
        } else {
            return true;
        }
    };

    auto t3 = std::chrono::steady_clock::now();
    bool allColoringsHaveCompleteOrEmptySubgraph = foreachColoringHasCompleteOrEmptySubgraph(0);
    auto t4 = std::chrono::steady_clock::now();
    std::cerr << "Timing: Check all colorings:          " << std::fixed
              << std::chrono::duration<double>(t4 - t3).count() << " seconds" << std::endl;
    std::cerr.imbue(std::locale("en_US.UTF-8"));
    std::cerr << "Timing: Number of recursion steps:    " << std::setw(15) << recursionSteps << std::endl;
    std::cerr << "Timing: Number of colorings checked:  " << std::setw(15) << coloringsChecked << std::endl;
    std::cerr << "Timing: Number edge mask size checks: " << std::setw(15) << edgeMaskChecks << std::endl;

    // Check against expected result
    if (allColoringsHaveCompleteOrEmptySubgraph == true) {
        if (config::n < expectedResult(config::r, config::s)) {
            std::cerr << "Error: Check Result for R(" << config::r << "," << config::s << ") <= " << config::n
                      << " disagrees with our result (" << allColoringsHaveCompleteOrEmptySubgraph << ")" << std::endl;
            std::abort();
        }
    } else {
        if (config::n >= expectedResult(config::r, config::s)) {
            std::cerr << "Error: Check Result for R(" << config::r << "," << config::s << ") <= " << config::n
                      << " disagrees with our result (" << allColoringsHaveCompleteOrEmptySubgraph << ")" << std::endl;
            std::abort();
        }
    }

    // Final output
    if (allColoringsHaveCompleteOrEmptySubgraph == true) {
        std::cout << "R(" << config::r << "," << config::s << ") <= " << config::n << " (TRUE)" << std::endl;
    } else {
        std::cout << "R(" << config::r << "," << config::s << ")  > " << config::n << " (FALSE)" << std::endl;
        std::cerr << "Counter example: " << std::endl;
        std::cerr << printAdjacencyMatrix<config::n>(counterExample, "    ") << std::endl;
    }

    return 0;
}
