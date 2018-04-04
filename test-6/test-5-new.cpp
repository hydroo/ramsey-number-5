#include "test-5-prereqs.hpp"

#include <chrono>
#include <functional>

#include "test-5-check.hpp"
#include "test-5-config.hpp"

#include "adjacencymatrix.hpp"

using std::cerr;
using std::cout;
using std::endl;

using r5::AdjacencyMatrix;
using r5::AdjacencyMatrixIndexer;

/* Returns an array of adjacency matrices.
 * Returns the set of all possible graphs that have exactly one complete subgraph of size `subGraphSize`.
 */
template <s64 edges, s64 nodes, s64 subGraphSize>
// constexpr // constexpr'ing this makes compiling considerably slower
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
template <s64 edges, s64 nodes, s64 subGraphSize, bool digit>
std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1> subGraphEdgeMasksByLastDigit(const std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)>& edgeMasks) {
    std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1> ret;

    for (const auto& mask : edgeMasks) {
        s64 last = -1;  // offset everything by +1 so that we can use -1, and don't have to worry about array[nextEdge-1]
        for (s64 i = ((s64)mask.edges()) - 1; i >= 0; i -= 1) {
            if (mask.edge(i) == digit) {
                last = i;
                break;
            }
        }
        ret[last + 1].push_back(mask);
    }

    return ret;
}

// Enumerates all graphs iteratively (iterative DFS).
// I.e. for each edge, set it to 0, iterate, set it to 1, iterate.
//
// Note: Assumes config::e >= 1, and therefore config::n >= 2
bool allColoringsHaveCompleteOrEmptySubgraph(const std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>& edgeMasksCompleteByLastOne, const std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>& edgeMasksEmptyByLastZero, AdjacencyMatrix<config::n>* counterExample, s64* recursionSteps, s64* coloringsChecked, s64* edgeMaskChecks) {
    *recursionSteps   = 0;
    *coloringsChecked = 0;
    *edgeMaskChecks   = 0;

    AdjacencyMatrix<config::n> coloring;
    coloring.unsetAllEdges();

    bool ret = true;

    std::array<s64, config::e + 1> stack;
    s64 stackTop = 1;
    stack[0] = 0;
    stack[1] = 0;

    while (stackTop >= 0) {

        R5_DEBUG_ASSERT(stackTop < stack.size());

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
                R5_BENCH(*edgeMaskChecks += currentEdgeMasksComplete.size());
                for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                    if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) {
                        // cerr << "    " << currentEdgeMasksComplete[i] << " is complete subgraph" << endl;
                        R5_BENCH(*coloringsChecked += 1);
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
                R5_BENCH(*edgeMaskChecks += currentEdgeMasksEmpty.size());
                for (std::size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
                    if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                        // cerr << "    " << currentEdgeMasksEmpty << " is empty subgraph" << endl;
                        R5_BENCH(*coloringsChecked += 1);
                        foundEmptySubgraph = true;
                        break;
                    }
                }
                if (foundEmptySubgraph) { continue; }
            }

        }

        if (edge < config::e - 1) {
            stack[stackTop+1] = edge + 1;
            stack[stackTop+2] = edge + 1;
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

int main(int argc, char** args) {
    (void) argc;
    (void) args;

    cerr << config::print() << endl;

    auto t1 = std::chrono::steady_clock::now();

    // generate all complete and empty subgraph configurations
    // constexpr // constexpr'ing this makes compiling considerably slower
    std::array<AdjacencyMatrix<config::n>, nChooseK(config::n, config::r)> edgeMasksComplete
            = subGraphEdgeMasks<config::e, config::n, config::r>();
    // constexpr // constexpr'ing this makes compiling considerably slower
    std::array<AdjacencyMatrix<config::n>, nChooseK(config::n, config::s)> edgeMasksEmpty
            = invertSubgraphEdgeMasks<config::e, config::n, config::s>(subGraphEdgeMasks<config::e, config::n, config::s>());

    // index the generated graphs by lastly set edge
    std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1> edgeMasksCompleteByLastOne
        = subGraphEdgeMasksByLastDigit<config::e, config::n, config::r, true>(edgeMasksComplete);
    std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1> edgeMasksEmptyByLastZero
        = subGraphEdgeMasksByLastDigit<config::e, config::n, config::s, false>(edgeMasksEmpty);

    auto t2 = std::chrono::steady_clock::now();

    auto defaultPrecision = cerr.precision();
    cerr << "Timing: Create subgraph edge masks:  " << std::setw(15 + 4) << std::fixed << std::setprecision(3) << std::chrono::duration<double>(t2 - t1).count() << " seconds" << endl;

    // cerr << "Complete edge masks:                             " << edgeMasksComplete << endl;
    // cerr << "Complete edge masks by last 1(last+1):           " << edgeMasksCompleteByLastOne << endl;
    // cerr << "Empty edge masks:                                " << edgeMasksEmpty << endl;
    // cerr << "Empty edge masks by last 0   (last+1):           " << edgeMasksEmptyByLastZero << endl;

    // cerr << "Complete edge masks by last 1 (last:vectorsize): " << printMatrixCountPerLastDigit(edgeMasksCompleteByLastOne) << endl;
    // cerr << "Empty edge masks by last 0    (last:vectorsize): " << printMatrixCountPerLastDigit(edgeMasksEmptyByLastZero) << endl;

    AdjacencyMatrix<config::n> counterExample;
    s64 recursionSteps   = 0;
    s64 coloringsChecked = 0;
    s64 edgeMaskChecks   = 0;

    auto t3 = std::chrono::steady_clock::now();

    bool allColoringsHaveCompleteOrEmptySubgraph_;
    if (config::r == 1 || config::s == 1) {
        allColoringsHaveCompleteOrEmptySubgraph_ = true;
    } else if (config::n == 1) {
        allColoringsHaveCompleteOrEmptySubgraph_ = false;
    } else {
        // Now we can assume n >= 2 and therefore e >= 1

        // enumerate all graphs of size `n` and find out whether all have a complete subgraph of size `r` or an empty subgraph of size `s`
        allColoringsHaveCompleteOrEmptySubgraph_ = allColoringsHaveCompleteOrEmptySubgraph(edgeMasksCompleteByLastOne, edgeMasksEmptyByLastZero, &counterExample, &recursionSteps, &coloringsChecked, &edgeMaskChecks);
    }

    auto t4 = std::chrono::steady_clock::now();
    cerr << "Timing: Check all colorings:         " << std::setw(15 + 4) << std::fixed << std::chrono::duration<double>(t4 - t3).count() << " seconds" << endl;
    cerr.precision(defaultPrecision);
    cerr.imbue(std::locale("en_US.UTF-8"));
    cerr << "Timing: Number of recursion steps:   " << std::setw(15) << recursionSteps << endl;
    cerr << "Timing: Number of colorings checked: " << std::setw(15) << coloringsChecked << endl;
    cerr << "Timing: Number of edge mask checks:  " << std::setw(15) << edgeMaskChecks << endl;

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
