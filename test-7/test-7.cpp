#include "prereqs.hpp"

#include <chrono>
#include <functional>

#include "check.hpp"
#include "config.hpp"
#include "unique.hpp"

#include "adjacencymatrix.hpp"

using std::cerr;
using std::cout;
using std::endl;

using r5::AdjacencyMatrix;
using r5::AdjacencyMatrixIndexer;

// Returns an array of adjacency matrices.
// Returns the set of all possible graphs that have exactly one complete subgraph of size `subGraphSize`.
template <s64 edges, s64 nodes, s64 subGraphSize>
std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)> subGraphEdgeMasks() {
    std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)> masks;

    std::array<bool, nodes> nodeMask{};

    for (s64 i = 0; i < subGraphSize; i += 1) {
        nodeMask[i] = true;
    }
    for (s64 i = subGraphSize; i < nodes; i += 1) {
        nodeMask[i] = false;
    }

    // cerr << "nodemask " << nodeMask << endl;

    s64 p = 0;

    do {
        for (s64 e = 0; e < edges; e += 1) {
            auto pair = AdjacencyMatrixIndexer<nodes>::reverse(e);
            s64 c = pair.first;
            s64 r = pair.second;

            // cerr << "e " << e << ", c " << c << ", r " << r << endl;

            if (nodeMask[c] == true && nodeMask[r] == true) {
                masks[p].setEdge(c, r);
            } else {
                masks[p].unsetEdge(c, r);
            }
        }

        // cerr << "node mask " << nodeMask << ", edge mask " << (*masks)[p] << endl;

        p += 1;

    } while (r5::prev_permutation(std::begin(nodeMask), std::end(nodeMask)));

    return masks;
}

// flips all edges
template <s64 edges, s64 nodes, s64 subGraphSize>
constexpr std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)> invertSubgraphEdgeMasks(const std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)>& edgeMasks) {
    std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)> ret{};
    for (std::size_t i = 0; i < edgeMasks.size(); i += 1) {
        ret[i] = ~edgeMasks[i];
    }
    return ret;
}

// Returns a vector of edge masks for each unique base graph
//
// Filter out edge masks that have bits in the lower edges, so that
// comparison to a filtered out edge mask will never result in a positive hit.
template <s64 edges, s64 nodes, s64 subGraphSize, bool complete, s64 uniqueNodeCount>
std::vector<std::vector<AdjacencyMatrix<nodes>>> filterSubGraphEdgeMasks(const std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)>& masks, const std::vector<r5::AdjacencyMatrix<uniqueNodeCount>>& uniqueBases) {

    std::vector<std::vector<AdjacencyMatrix<nodes>>> ret(uniqueBases.size());

    R5_STATIC_ASSERT(nodes >= uniqueNodeCount);

    constexpr AdjacencyMatrix<uniqueNodeCount> x;
    constexpr s64 uniqueBaseEdges = x.edges();

    // shorten the mask to the unique bases' sizes
    std::array<AdjacencyMatrix<uniqueNodeCount>, nChooseK(nodes, subGraphSize)> shortMasks;
    for (std::size_t i = 0; i < masks.size(); i += 1) {
        for (s64 e = 0; e < uniqueBaseEdges; e += 1) {
            if (masks[i].edge(e) == true) {
                shortMasks[i].setEdge(e);
            } else {
                shortMasks[i].unsetEdge(e);
            }
        }
    }

    // add only those edge masks that result in hits in these lower bits
    for (std::size_t i = 0; i < uniqueBases.size(); i += 1) {
        for (std::size_t j = 0; j < masks.size(); j += 1) {
            if(complete == true) {
                if ((uniqueBases[i] & shortMasks[j]) == shortMasks[j]) {
                    ret[i].push_back(masks[j]);
                }
            } else {
                if ((uniqueBases[i] | shortMasks[j]) == shortMasks[j]) {
                    ret[i].push_back(masks[j]);
                }
            }
        }
    }

    return ret;
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
template <s64 edges, s64 nodes, bool digit>
std::vector<std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1>> subGraphEdgeMasksByLastDigit(const std::vector<std::vector<AdjacencyMatrix<nodes>>>& edgeMasks) {
    std::vector<std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1>> ret(edgeMasks.size());

    for (std::size_t i = 0; i < edgeMasks.size(); i += 1) { // foreach unique base graph

        for (const auto& mask : edgeMasks[i]) {
            s64 last = -1;  // offset everything by +1 so that we can use -1, and don't have to worry about array[nextEdge-1]
            for (s64 i = ((s64)mask.edges()) - 1; i >= 0; i -= 1) {
                if (mask.edge(i) == digit) {
                    last = i;
                    break;
                }
            }
            ret[i][last + 1].push_back(mask);
        }

    }

    return ret;
}

bool allColoringsHaveCompleteOrEmptySubgraph2(const r5::AdjacencyMatrix<config::u>& baseMask, const std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>& edgeMasksCompleteByLastOne, const std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>& edgeMasksEmptyByLastZero, AdjacencyMatrix<config::n>* counterExample, s64* recursionSteps, s64* coloringsChecked, s64* edgeMaskChecks) {
    *recursionSteps   = 0;
    *coloringsChecked = 0;
    *edgeMaskChecks   = 0;

    AdjacencyMatrix<config::n> coloring;
    coloring.unsetAllEdges();

    for (s64 e = 0; e < baseMask.edges(); e += 1) {
        if (baseMask.edge(e) == true) {
            coloring.setEdge(e);
        } else {
            coloring.unsetEdge(e);
        }
    }

    R5_BENCH(*coloringsChecked += 1;);

    // before starting the DFS we need to check the edge masks in <= baseMask.edges(), because we wouldn't check them in the DFS
    // because there we only look at the edge masks that have the last bit (0/1) where the last toggled edge is
    for (std::size_t j = 0; j < baseMask.edges() + 1 /*+1 because of the +1 for byLastOne arrays*/; j += 1) {

        if (config::n >= config::r) {
            const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[j];
            bool foundCompleteSubgraph = false;
            for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                R5_BENCH(*edgeMaskChecks += 1);
                if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) {
                    foundCompleteSubgraph = true;
                    break;
                }
            }
            if (foundCompleteSubgraph) { return true; }
        }

        if (config::n >= config::s) {
            const auto& currentEdgeMasksEmpty = edgeMasksEmptyByLastZero[j];
            bool foundEmptySubgraph = false;
            for (std::size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
                R5_BENCH(*edgeMaskChecks += 1);
                if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                    foundEmptySubgraph = true;
                    break;
                }
            }
            if (foundEmptySubgraph) { return true; }
        }
    }

    // Start DFS

    bool ret = true;

    std::array<s64, config::e + 1> stack;
    s64 stackTop = 1;
    stack[0] = baseMask.edges(); // set   edge x
    stack[1] = baseMask.edges(); // unset edge x

    while (stackTop >= 0) {

        R5_DEBUG_ASSERT(stackTop < (s64) stack.size());

        // cerr << stackTop << stack << endl;

        R5_BENCH(*recursionSteps += 1);

        s64 edge = stack[stackTop];
        stackTop -= 1;

        coloring.toggleEdge(edge);

        // cerr << "  " << coloring << " edge " << edge << endl;

        if (coloring.edge(edge) == true) {

            if (config::n >= config::r) { // avoids matching subgraphs larger than the to-be-checked graph
                // Compare this graph against all appropriate complete subgraphs.
                // Appropriate means every graph whos complete subgraph's laste edge is exactly the lastly enumerated edge
                bool foundCompleteSubgraph = false;
                const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[edge + 1];
                R5_BENCH(if (edgeMasksCompleteByLastOne.size() > 0) { *coloringsChecked += 1; });
                for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                    R5_BENCH(*edgeMaskChecks += 1);
                    if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) {
                        foundCompleteSubgraph = true;
                        break;
                    }
                }
                if (foundCompleteSubgraph) { continue; }
            }

        } else {

            if (config::n >= config::s) { // avoids matching subgraphs larger than the to-be-checked graph
                // Do the same for empty subgraphs
                bool foundEmptySubgraph = false;
                const auto& currentEdgeMasksEmpty = edgeMasksEmptyByLastZero[edge + 1];
                R5_BENCH(if (edgeMasksEmptyByLastZero.size() > 0) { *coloringsChecked += 1; });
                for (std::size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
                    R5_BENCH(*edgeMaskChecks += 1);
                    if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                        foundEmptySubgraph = true;
                        break;
                    }
                }
                if (foundEmptySubgraph) { continue; }
            }

        }

        if (edge < config::e - 1) {
            stack[stackTop+1] = edge + 1; // set   edge + 1
            stack[stackTop+2] = edge + 1; // unset edge + 1
            stackTop += 2;
        } else {
            // If this graph is completely enumerated and no complete or empty subgraph has been found,
            // return false and provide this graph as a counter example.
            R5_BENCH(*coloringsChecked += 1);
            *counterExample = coloring;

            // cerr << "    " << coloring << " has no complete or empty subgraphs" << endl;
            ret = false;
            break;
        }
    }

    return ret;
}

bool allColoringsHaveCompleteOrEmptySubgraph(const std::vector<r5::AdjacencyMatrix<config::u>>& uniqueBaseMasks, const std::vector<std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>>& filteredEdgeMasksCompleteByLastOne, const std::vector<std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>>& filteredEdgeMasksEmptyByLastZero, AdjacencyMatrix<config::n>* counterExample, s64* recursionSteps, s64* coloringsChecked, s64* edgeMaskChecks) {
    *recursionSteps   = 0;
    *coloringsChecked = 0;
    *edgeMaskChecks   = 0;

    for (std::size_t i = 0; i < uniqueBaseMasks.size(); i += 1) {

        s64 recursionSteps2   = 0;
        s64 coloringsChecked2 = 0;
        s64 edgeMaskChecks2   = 0;
        AdjacencyMatrix<config::n> counterExample2;

        bool all = allColoringsHaveCompleteOrEmptySubgraph2(uniqueBaseMasks[i], filteredEdgeMasksCompleteByLastOne[i], filteredEdgeMasksEmptyByLastZero[i], &counterExample2, &recursionSteps2, &coloringsChecked2, &edgeMaskChecks2);

        *recursionSteps   += recursionSteps2;
        *coloringsChecked += coloringsChecked2;
        *edgeMaskChecks   += edgeMaskChecks2;

        if (all == false) {
            *counterExample = counterExample2;
            return false;
        }
    }

    return true;
}

int main(int argc, char** args) {
    (void) argc;
    (void) args;

    cerr << config::print() << endl;

    auto t1 = std::chrono::steady_clock::now();

    std::vector<r5::AdjacencyMatrix<config::u>> uniqueBaseMasks = uniqueAdjacencyMatrices<config::u>();

    auto t2 = std::chrono::steady_clock::now();

    auto defaultPrecision = cerr.precision();
    cerr << "Timing: Create unique base graphs:                " << std::setw(15 + 4) << std::fixed << std::setprecision(3) << std::chrono::duration<double>(t2 - t1).count() << " seconds" << endl;
    cerr << "Timing: Number of unique base graphs:             " << std::setw(15) << uniqueBaseMasks.size() << endl;

    std::array<AdjacencyMatrix<config::n>, nChooseK(config::n, config::r)> edgeMasksComplete
            = subGraphEdgeMasks<config::e, config::n, config::r>();
    std::array<AdjacencyMatrix<config::n>, nChooseK(config::n, config::s)> edgeMasksEmpty
            = invertSubgraphEdgeMasks<config::e, config::n, config::s>(subGraphEdgeMasks<config::e, config::n, config::s>());

    std::vector<std::vector<AdjacencyMatrix<config::n>>> filteredEdgeMasksComplete
            = filterSubGraphEdgeMasks<config::e, config::n, config::r, true , config::u>(edgeMasksComplete, uniqueBaseMasks);
    std::vector<std::vector<AdjacencyMatrix<config::n>>> filteredEdgeMasksEmpty
            = filterSubGraphEdgeMasks<config::e, config::n, config::s, false, config::u>(edgeMasksEmpty, uniqueBaseMasks);

    std::vector<std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>> filteredEdgeMasksCompleteByLastOne
        = subGraphEdgeMasksByLastDigit<config::e, config::n, true>(filteredEdgeMasksComplete);
    std::vector<std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>> filteredEdgeMasksEmptyByLastZero
        = subGraphEdgeMasksByLastDigit<config::e, config::n, false>(filteredEdgeMasksEmpty);

    auto t3 = std::chrono::steady_clock::now();

    cerr << "Timing: Create, filter, sort subgraph edge masks: " << std::setw(15 + 4) << std::fixed << std::setprecision(3) << std::chrono::duration<double>(t3 - t2).count() << " seconds" << endl;

    // cerr << endl;
    // cerr << "Unique base graphs                                 " << uniqueBaseMasks << endl;
    // cerr << "Complete edge masks:                               " << edgeMasksComplete << endl;
    // cerr << "Filtered complete edge masks:                      [";

    // for (std::size_t i = 0; i < filteredEdgeMasksComplete.size(); i += 1) {
    //     cerr << uniqueBaseMasks[i] << ": " << filteredEdgeMasksComplete[i];
    //     if (i < filteredEdgeMasksComplete.size()-1) {
    //         cerr << ", ";
    //     }
    // }
    // cerr << "]" << endl;

    // cerr << "Complete filtered edge masks by last 1(last+1):    [";
    // for (std::size_t i = 0; i < filteredEdgeMasksCompleteByLastOne.size(); i += 1) {
    //     cerr << uniqueBaseMasks[i] << ": " << filteredEdgeMasksCompleteByLastOne[i];
    //     if (i < filteredEdgeMasksCompleteByLastOne.size()-1) {
    //         cerr << ", ";
    //     }
    // }
    // cerr << "]" << endl;

    // cerr << "Empty edge masks:                                  " << edgeMasksEmpty << endl;
    // cerr << "Filtered empty edge masks:                         [";
    // for (std::size_t i = 0; i < filteredEdgeMasksEmpty.size(); i += 1) {
    //     cerr << uniqueBaseMasks[i] << ": " << filteredEdgeMasksEmpty[i];
    //     if (i < filteredEdgeMasksEmpty.size()-1) {
    //         cerr << ", ";
    //     }
    // }
    // cerr << "]" << endl;

    // cerr << "Filtered empty edge masks by last 0   (last+1):    [";
    // for (std::size_t i = 0; i < filteredEdgeMasksEmptyByLastZero.size(); i += 1) {
    //     cerr << uniqueBaseMasks[i] << ": " << filteredEdgeMasksEmptyByLastZero[i];
    //     if (i < filteredEdgeMasksEmptyByLastZero.size()-1) {
    //         cerr << ", ";
    //     }
    // }
    // cerr << "]" << endl;

    // cerr << "Complete edge masks by last 1 (last:vectorsize):   " << printMatrixCountPerLastDigit(filteredEdgeMasksCompleteByLastOne) << endl;
    // cerr << "Empty edge masks by last 0    (last:vectorsize):   " << printMatrixCountPerLastDigit(filteredEdgeMasksEmptyByLastZero) << endl;
    // cerr << endl;

    AdjacencyMatrix<config::n> counterExample;
    s64 recursionSteps   = 0;
    s64 coloringsChecked = 0;
    s64 edgeMaskChecks   = 0;


    bool allColoringsHaveCompleteOrEmptySubgraph_;
    if (config::r == 1 || config::s == 1) {
        allColoringsHaveCompleteOrEmptySubgraph_ = true;
    } else if (config::n == 1) {
        allColoringsHaveCompleteOrEmptySubgraph_ = false;
    } else {
        // Now we can assume n >= 2 and therefore e >= 1

        // enumerate all graphs of size `n` and find out whether all have a complete subgraph of size `r` or an empty subgraph of size `s`
        allColoringsHaveCompleteOrEmptySubgraph_ = allColoringsHaveCompleteOrEmptySubgraph(uniqueBaseMasks, filteredEdgeMasksCompleteByLastOne, filteredEdgeMasksEmptyByLastZero, &counterExample, &recursionSteps, &coloringsChecked, &edgeMaskChecks);
    }

    auto t4 = std::chrono::steady_clock::now();

    cerr << "Timing: Check all colorings:                      " << std::setw(15 + 4) << std::fixed << std::chrono::duration<double>(t4 - t3).count() << " seconds" << endl;
    cerr.precision(defaultPrecision);
    cerr.imbue(std::locale("en_US.UTF-8"));
    cerr << "Timing: Number of recursion steps:                " << std::setw(15) << recursionSteps << endl;
    cerr << "Timing: Number of colorings checked:              " << std::setw(15) << coloringsChecked << endl;
    cerr << "Timing: Number of edge mask checks:               " << std::setw(15) << edgeMaskChecks << endl;

    // Check against expected result
    if (allColoringsHaveCompleteOrEmptySubgraph_ == true) {
        if (config::n < expectedResult(config::r, config::s)) {
            cerr << "Error: Check Result for R(" << config::r << "," << config::s << ") <= " << config::n
                      << " disagrees with our result (" << allColoringsHaveCompleteOrEmptySubgraph_ << ")" << endl;
            std::abort();
        }
    } else {
        if (config::n >= expectedResult(config::r, config::s)) {
            cerr << "Error: Check Result for R(" << config::r << "," << config::s << ") <= " << config::n
                      << " disagrees with our result (" << allColoringsHaveCompleteOrEmptySubgraph_ << ")" << endl;
            std::abort();
        }
    }

    // Final output
    if (allColoringsHaveCompleteOrEmptySubgraph_ == true) {
        cout << "R(" << config::r << "," << config::s << ") <= " << config::n << " (TRUE)" << endl;
    } else {
        cout << "R(" << config::r << "," << config::s << ")  > " << config::n << " (FALSE)" << endl;
        cerr << "Counter example: " << endl;
        cerr << counterExample.print(true, "    ") << endl;

        // verify the counter example again
        R5_ASSERT(hasCompleteOrEmptySubgraph(counterExample, config::r, config::s) == false);
    }

    return 0;
}
