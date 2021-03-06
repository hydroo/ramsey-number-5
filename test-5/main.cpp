#include "prereqs.hpp"

#include "check.hpp"
#include "config.hpp"

/* Returns an array of bitsets that represent adjacency matrices.
 * `edgeMasks` is the set of all possible graphs that have exactly one complete subgraph of size `subGraphSize`.
 *
 * Indexing into an adjacency matrix is column-major:
 * Index 0 = edge between 0 and 1; 1 = 0,2; 2 = 1,2; 3 = 0, 3; 4 = 1,3; 5 = 2,3;
 * (The matrix is symmetric and irreflexive / a triangle without the diagonal.)
 */
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
            int x = 1;
            int y = e;
            while (y >= x) {
                y -= x;
                x += 1;
            }

            // std::cerr << "e " << e << ", x " << x << ", y " << y << std::endl;

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

// Converts a set of complete subgraphs into a set of empty subgraphs
template <int edges, int nodes, int subGraphSize>
void invertSubgraphEdgeMasks(std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)>* masks) {
    for (auto&& m : *masks) {
        m = ~m;
    }
}

/* Split the set of graphs into multiple sets,
 * where each set with index `i` contains all graphs where the last edge of the complete subgraph is `i`.
 * This will be used to only compare a complete subgraph if the enumerated graph has exactly
 * reached this number of edges, not more, not less.
 *
 * It is useless to compare a smaller enumerated graph (I.e. not enough edges have been enumerated)
 * to a complete subgraph that needs an edge at a position that is not yet enumerated.
 * On the other hand it is useless to compare an enumerated graph that is larger than the complete subgraph,
 * because the result is the same as comparing it to one which has fewer edges.
 */
template <int edges, int nodes, int subGraphSize, bool digit>
void subGraphEdgeMasksByLastDigit(std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)> edgeMasks,
        std::array<std::vector<std::bitset<edges>>, edges + 1>* edgeMasksByLastDigit) {
    for (const auto& mask : edgeMasks) {
        int last = -1;  // offset everything by +1 so that we can use -1, and don't have to worry about array[nextEdge-1]
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

    // static sizing for speed
    std::array<std::bitset<config::e>, nChooseK(config::n, config::r)> edgeMasksComplete;
    std::array<std::bitset<config::e>, nChooseK(config::n, config::s)> edgeMasksEmpty;

    // generate all complete and empty subgraph configurations
    subGraphEdgeMasks<config::e, config::n, config::r>(&edgeMasksComplete);
    subGraphEdgeMasks<config::e, config::n, config::s>(&edgeMasksEmpty);

    invertSubgraphEdgeMasks<config::e, config::n, config::s>(&edgeMasksEmpty);

    std::array<std::vector<std::bitset<config::e>>, config::e + 1> edgeMasksCompleteByLastOne;
    std::array<std::vector<std::bitset<config::e>>, config::e + 1> edgeMasksEmptyByLastZero;

    // index the generated graphs by lastly set edge
    subGraphEdgeMasksByLastDigit<config::e, config::n, config::r, true>(edgeMasksComplete, &edgeMasksCompleteByLastOne);
    subGraphEdgeMasksByLastDigit<config::e, config::n, config::s, false>(edgeMasksEmpty, &edgeMasksEmptyByLastZero);

    auto t2 = std::chrono::steady_clock::now();

    std::cerr << "Timing: Create subgraph edge masks:  " << std::fixed
              << std::chrono::duration<double>(t2 - t1).count() << " seconds" << std::endl;

    // std::cerr << "Complete edge masks:                             " << edgeMasksComplete << std::endl;
    // std::cerr << "Complete edge masks by last 1(last+1):           " << edgeMasksCompleteByLastOne << std::endl;
    // std::cerr << "Empty edge masks:                                " << edgeMasksEmpty << std::endl;
    // std::cerr << "Empty edge masks by last 0   (last+1):           " << edgeMasksEmptyByLastZero << std::endl;

    // std::cerr << "Complete edge masks by last 1 (last:vectorsize): "
    //           << printMasksByLastDigit(edgeMasksCompleteByLastOne) << std::endl;
    // std::cerr << "Empty edge masks by last 0    (last:vectorsize): " << printMasksByLastDigit(edgeMasksEmptyByLastZero)
    //           << std::endl;

    std::bitset<config::e> coloring;
    std::bitset<config::e> counterExample;
    int64_t recursionSteps = 0;
    int64_t coloringsChecked = 0;
    int64_t edgeMaskChecks = 0;

    // // unused progress towards making this nonrecursive
    //
    // coloring.set();
    //
    // int oldLastChanged = config::e-1;
    // int nextLastChanged = 0;
    // do {

    //     //std::cerr << "x   " << coloring << " " << oldLastChanged << " " << nextLastChanged << std::endl;

    //     if (nextLastChanged <= oldLastChanged) {
    //         for (int i = nextLastChanged; i <= oldLastChanged; i += 1) {
    //             std::cerr << "a   " << coloring << ", next edge " << i+1 << std::endl;
    //             // check
    //         }
    //     } else {
    //         std::cerr << "b   " << coloring << ", next edge " << nextLastChanged+1 << std::endl;
    //         // check
    //     }
    //     // todo, how do you prune the tree

    //     oldLastChanged = nextLastChanged;

    //     for (int i = config::e-1; i >= 0; i -= 1) {
    //         if (coloring.test(i) == true) {
    //             nextLastChanged = i;
    //             break;
    //         }
    //     }

    //     bool continueEnumerating = false;
    //     for (int i = config::e-1; i >= 0; i -= 1) {
    //         coloring.flip(i);
    //         if (coloring.test(i) == false) {
    //             continueEnumerating = true;
    //             break;
    //         }
    //     }

    //     if (continueEnumerating) { continue; }
    //     else                     { break   ; }
    // } while (true);

    // std::abort();

    // Enumerates all graphs recursively.
    // I.e. for each edge, set it to 0, recurse, set it to 1, recurse.
    std::function<bool(int)> foreachColoringHasCompleteOrEmptySubgraph = [&coloring, &edgeMasksCompleteByLastOne,
            &edgeMasksEmptyByLastZero, &counterExample, &recursionSteps, &coloringsChecked, &edgeMaskChecks,
            &foreachColoringHasCompleteOrEmptySubgraph](int nextEdge) -> bool {

        BENCH(recursionSteps += 1);

        // std::cerr << "  " << coloring << " nextEdge " << nextEdge << std::endl;

        // Compare this graph against all appropriate complete subgraphs.
        // Appropriate means every graph whos complete subgraph's laste edge is exactly the lastly enumerated edge
        const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[nextEdge - 1 + 1];
        BENCH(edgeMaskChecks += currentEdgeMasksComplete.size());
        for (size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
            if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) {
                if (config::n >= config::r) {  // avoids matching subgraphs larger than the to-be-checked graph
                    // std::cerr << "      Mask " << currentEdgeMasksComplete[i] << " is a subgraph" << std::endl;
                    BENCH(coloringsChecked += 1);
                    return true;
                }
            }
        }

        // Do the same for empty subgraphs
        const auto& currentEdgeMasksEmpty = edgeMasksEmptyByLastZero[nextEdge - 1 + 1];
        BENCH(edgeMaskChecks += currentEdgeMasksEmpty.size());
        for (size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
            if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                if (config::n >= config::s) {  // avoids matching subgraphs larger than the to-be-checked graph
                    // std::cerr << "      Mask " << mask << " is a subgraph" << std::endl;
                    BENCH(coloringsChecked += 1);
                    return true;
                }
            }
        }

        // If this graph is completely enumerated and no complete or empty subgraph has been found,
        // return false and provide this graph as a counter example.
        if (nextEdge == config::e) {
            BENCH(coloringsChecked += 1);
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

        return ret;
    };

    auto t3 = std::chrono::steady_clock::now();
    // enumerate all graphs of size `n` and find out whether all have a complete subgraph of size `r`
    // or an empty subgraph of size `s`
    bool allColoringsHaveCompleteOrEmptySubgraph = foreachColoringHasCompleteOrEmptySubgraph(0);
    auto t4 = std::chrono::steady_clock::now();
    std::cerr << "Timing: Check all colorings:         " << std::fixed
              << std::chrono::duration<double>(t4 - t3).count() << " seconds" << std::endl;
    std::cerr.imbue(std::locale("en_US.UTF-8"));
    std::cerr << "Timing: Number of recursion steps:   " << std::setw(15) << recursionSteps << std::endl;
    std::cerr << "Timing: Number of colorings checked: " << std::setw(15) << coloringsChecked << std::endl;
    std::cerr << "Timing: Number of edge mask checks:  " << std::setw(15) << edgeMaskChecks << std::endl;

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
