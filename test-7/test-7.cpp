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

// Filter out edge masks that have bits in the lower bits/edges that would cause a comparison to always fail
template <s64 edges, s64 nodes, s64 subGraphSize, bool complete, s64 uniqueNodeCount>
std::vector<AdjacencyMatrix<nodes>> filterSubGraphEdgeMasks(const std::array<AdjacencyMatrix<nodes>, nChooseK(nodes, subGraphSize)>& masks, const AdjacencyMatrix<uniqueNodeCount>& uniqueBase) {

    R5_STATIC_ASSERT(nodes >= uniqueNodeCount);

    std::vector<AdjacencyMatrix<nodes>> ret;

    // shorten the mask to the unique bases' sizes
    std::array<AdjacencyMatrix<uniqueNodeCount>, nChooseK(nodes, subGraphSize)> shortMasks;
    for (std::size_t i = 0; i < masks.size(); i += 1) {
        for (s64 e = 0; e < config::ue; e += 1) {
            if (masks[i].edge(e) == true) {
                shortMasks[i].setEdge(e);
            } else {
                shortMasks[i].unsetEdge(e);
            }
        }
    }

    // add only those edge masks that result in hits in these lower bits
    for (std::size_t j = 0; j < masks.size(); j += 1) {
        if(complete == true) {
            if ((uniqueBase & shortMasks[j]) == shortMasks[j]) {
                ret.push_back(masks[j]);
            }
        } else {
            if ((uniqueBase | shortMasks[j]) == shortMasks[j]) {
                ret.push_back(masks[j]);
            }
        }
    }

    return ret;
}

/* Split the set of graphs into multiple sets,
 * where each set with index `i + 1` contains all graphs where the last edge of the complete subgraph is `i`.
 * This will be used to only compare a complete subgraph if the enumerated graph has exactly
 * reached this number of edges, not more, not less.
 *
 * It is useless to compare a smaller enumerated graph (I.e. not enough edges have been enumerated)
 * to a complete subgraph that needs an edge at a position that is not yet enumerated.
 * On the other hand it is useless to compare an enumerated graph that is larger than the complete subgraph,
 * because the result is the same as comparing it to one which has fewer edges.
 */
template <s64 edges, s64 nodes, bool digit>
std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1> subGraphEdgeMasksByLastDigit(const std::vector<AdjacencyMatrix<nodes>>& edgeMasks) {
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

bool allColoringsHaveCompleteOrEmptySubgraph(
        const AdjacencyMatrix<config::u>& uniqueBase,
        const std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>& edgeMasksCompleteByLastOne,
        const std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1>& edgeMasksEmptyByLastZero,
        AdjacencyMatrix<config::n>* counterExample,
        s64* recursionSteps,
        s64* coloringsChecked,
        s64* edgeMaskChecks) {

    R5_ASSERT(*recursionSteps   == 0);
    R5_ASSERT(*coloringsChecked == 0);
    R5_ASSERT(*edgeMaskChecks   == 0);

    // assign base graph to coloring
    AdjacencyMatrix<config::n> coloring;
    coloring.unsetAllEdges();

    for (s64 e = 0; e < uniqueBase.edges(); e += 1) {
        if (uniqueBase.edge(e) == true) {
            coloring.setEdge(e);
        } else {
            coloring.unsetEdge(e);
        }
    }

    R5_VERBOSE_1(*coloringsChecked += 1;);

    // *** Check the base graph first ***

    // before starting the DFS we need to check the edge masks in <= uniqueBase.edges(), because we wouldn't check them in the DFS
    // because there we only look at the edge masks that have the last bit (0/1) where the last toggled edge is
    for (std::size_t j = 0; j < uniqueBase.edges() + 1 /*+1 because of the +1 for byLastOne arrays*/; j += 1) {

        if (config::n >= config::r) {
            const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[j];
            bool foundCompleteSubgraph = false;
            for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                R5_VERBOSE_1(*edgeMaskChecks += 1);
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
                R5_VERBOSE_1(*edgeMaskChecks += 1);
                if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                    foundEmptySubgraph = true;
                    break;
                }
            }
            if (foundEmptySubgraph) { return true; }
        }
    }

    if (uniqueBase.edges() >= config::e) {
        R5_ASSERT(uniqueBase.edges() == config::e);

        // If this graph is completely enumerated and no complete or empty subgraph has been found,
        // return false and provide this graph as a counter example.
        R5_VERBOSE_1(*coloringsChecked += 1);
        *counterExample = coloring;

        // cerr << "    " << coloring << " has no complete or empty subgraphs" << endl;
        return false;
    }

    // *** Start DFS ***

    bool ret = true;

    std::array<s64, config::e + 1> stack;
    s64 stackTop = 1;
    stack[0] = uniqueBase.edges(); // set   edge x
    stack[1] = uniqueBase.edges(); // unset edge x

    while (stackTop >= 0) {

        R5_DEBUG_ASSERT(stackTop < (s64) stack.size());

        // cerr << stackTop << stack << endl;

        R5_VERBOSE_1(*recursionSteps += 1);

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
                R5_VERBOSE_1(if (edgeMasksCompleteByLastOne.size() > 0) { *coloringsChecked += 1; });
                for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                    R5_VERBOSE_1(*edgeMaskChecks += 1);
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
                R5_VERBOSE_1(if (edgeMasksEmptyByLastZero.size() > 0) { *coloringsChecked += 1; });
                for (std::size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
                    R5_VERBOSE_1(*edgeMaskChecks += 1);
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
            R5_VERBOSE_1(*coloringsChecked += 1);
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

    cerr.precision(3);
    cerr.imbue(std::locale("en_US.UTF-8"));

    cerr << config::print() << endl;

    auto t1 = std::chrono::steady_clock::now();

    std::vector<AdjacencyMatrix<config::u>> uniqueBaseMasks = uniqueAdjacencyMatrices<config::u>();

    auto t2 = std::chrono::steady_clock::now();

    cerr << "Create unique base graphs:                    " << std::setw(15 + 4) << std::fixed << std::chrono::duration<double>(t2 - t1).count() << " seconds" << endl;
    cerr << "Number of unique base graphs:                 " << std::setw(15) << uniqueBaseMasks.size() << endl;
    cerr << "Max recursion depth                           " << std::setw(15) << config::e - config::ue << endl;

    std::ostringstream o1;
    o1 << "2^" << config::e - config::ue;

    cerr << "Max recursion steps per unique base graph     " << std::setw(15) << o1.str() << endl;

    std::ostringstream o2;
    o2 << uniqueBaseMasks.size() << " * 2^" << config::e - config::ue;

    cerr << "Max recursion steps                           " << std::setw(15) << o2.str() << endl;

#if R5_VERBOSE >= 2
    cerr << "Unique base graphs                            " << std::setw(15) << uniqueBaseMasks << endl;
#endif

    std::array<AdjacencyMatrix<config::n>, nChooseK(config::n, config::r)> edgeMasksComplete
            = subGraphEdgeMasks<config::e, config::n, config::r>();
    std::array<AdjacencyMatrix<config::n>, nChooseK(config::n, config::s)> edgeMasksEmpty
            = invertSubgraphEdgeMasks<config::e, config::n, config::s>(subGraphEdgeMasks<config::e, config::n, config::s>());

    auto t3 = std::chrono::steady_clock::now();

    cerr << "Create subgraph edge masks:                   " << std::setw(15 + 4) << std::fixed << std::chrono::duration<double>(t3 - t2).count() << " seconds" << endl;

#if R5_VERBOSE >= 3
    cerr << "Complete edge masks:                          " << std::setw(15) << edgeMasksComplete        << endl;
    cerr << "Empty edge masks:                             " << std::setw(15) << edgeMasksEmpty           << endl;
#endif

    AdjacencyMatrix<config::n> counterExample;
    s64 recursionSteps   = 0;
    s64 coloringsChecked = 0;
    s64 edgeMaskChecks   = 0;

    bool allColoringsHaveCompleteOrEmptySubgraph_ = true;

    double t45Min = -1;
    double t45Max = -1;
    double t45Sum = 0.0;
    double t56Min = -1;
    double t56Max = -1;
    double t56Sum = 0.0;

    auto t456RealStart = std::chrono::steady_clock::now();

#if R5_VERBOSE >= 2
        cerr << endl;
#endif
    for (std::size_t u = 0; u < uniqueBaseMasks.size(); u += 1) {

        const auto& uniqueBase = uniqueBaseMasks[u];

        auto t4 = std::chrono::steady_clock::now();

#if R5_VERBOSE >= 2
        cerr << "Unique base graph " << uniqueBase << " [" << endl;
#endif
        std::vector<AdjacencyMatrix<config::n>> filteredEdgeMasksComplete
                = filterSubGraphEdgeMasks<config::e, config::n, config::r, true , config::u>(edgeMasksComplete, uniqueBase);
        std::vector<AdjacencyMatrix<config::n>> filteredEdgeMasksEmpty
                = filterSubGraphEdgeMasks<config::e, config::n, config::s, false, config::u>(edgeMasksEmpty, uniqueBase);

        std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1> filteredEdgeMasksCompleteByLastOne
            = subGraphEdgeMasksByLastDigit<config::e, config::n, true>(filteredEdgeMasksComplete);
        std::array<std::vector<AdjacencyMatrix<config::n>>, config::e + 1> filteredEdgeMasksEmptyByLastZero
            = subGraphEdgeMasksByLastDigit<config::e, config::n, false>(filteredEdgeMasksEmpty);

        auto t5  = std::chrono::steady_clock::now();
        auto t45 = std::chrono::duration<double>(t5 - t4).count();

        t45Min  = t45Min != -1 ? std::min(t45Min, t45) : t45;
        t45Max  = t45Max != -1 ? std::max(t45Max, t45) : t45;
        t45Sum += t45;

#if R5_VERBOSE >= 2
        cerr << "  Filter, sort subgraph edge masks:                       " << std::setw(15 + 4) << std::fixed << t45 << " seconds" << endl;
        cerr << "  Number of remaining complete edge masks:                " << std::setw(15) << filteredEdgeMasksComplete.size() << "/" << edgeMasksComplete.size() << endl;
        cerr << "  Number of remaining empty    edge masks:                " << std::setw(15) << filteredEdgeMasksEmpty.size()    << "/" << edgeMasksEmpty.size() << endl;
        cerr << "  Number of remaining complete edge masks by last 1:      " << std::setw(15) << "[";
        for (size_t i = 0; i < filteredEdgeMasksCompleteByLastOne.size(); i += 1) {
            if (filteredEdgeMasksCompleteByLastOne[i].size() > 0) {
                cerr << i - 1 << " : " << filteredEdgeMasksCompleteByLastOne[i].size();
                if (i < filteredEdgeMasksCompleteByLastOne.size()-1) {
                    cerr << ", ";
                }
            }
        }
        cerr << "]" << endl;
        cerr << "  Number of remaining empty    edge masks by last 0 edge: " << std::setw(15) << "[";
        for (size_t i = 0; i < filteredEdgeMasksEmptyByLastZero.size(); i += 1) {
            if (filteredEdgeMasksEmptyByLastZero[i].size() > 0) {
                cerr << i - 1 << " : " << filteredEdgeMasksEmptyByLastZero[i].size();
                if (i < filteredEdgeMasksEmptyByLastZero.size()-1) {
                    cerr << ", ";
                }
            }
        }
        cerr << "]" << endl;
#endif
#if R5_VERBOSE >= 3
        cerr << "  Remaining complete edge masks:                          " << std::setw(15) << filteredEdgeMasksComplete << endl;
        cerr << "  Remaining empty    edge masks:                          " << std::setw(15) << filteredEdgeMasksEmpty << endl;
        cerr << "  Remaining complete edge masks by last 1:                " << std::setw(15) << "[";
        for (std::size_t i = 0; i < filteredEdgeMasksCompleteByLastOne.size(); i += 1) {
            if (filteredEdgeMasksCompleteByLastOne[i].size() > 0) {
                cerr << i - 1 << " : " << filteredEdgeMasksCompleteByLastOne[i];
                if (i < filteredEdgeMasksCompleteByLastOne.size()-1) {
                    cerr << ", ";
                }
            }
        }
        cerr << "]" << endl;
        cerr << "  Remaining empty    edge masks by last 0:                " << std::setw(15) << "[";
        for (std::size_t i = 0; i < filteredEdgeMasksEmptyByLastZero.size(); i += 1) {
            if (filteredEdgeMasksEmptyByLastZero[i].size() > 0) {
                cerr << i - 1 << " : " << filteredEdgeMasksEmptyByLastZero[i];
                if (i < filteredEdgeMasksEmptyByLastZero.size()-1) {
                    cerr << ", ";
                }
            }
        }
        cerr << "]" << endl;
#endif

        AdjacencyMatrix<config::n> counterExample2;
        s64 recursionSteps2   = 0;
        s64 coloringsChecked2 = 0;
        s64 edgeMaskChecks2   = 0;

        bool all;
        if (config::r == 1 || config::s == 1) {
            all = true;
        } else if (config::n == 1) {
            all = false;
        } else {
            // Now we can assume n >= 2 and therefore e >= 1

            // enumerate all graphs of size `n` and find out whether all have a complete subgraph of size `r` or an empty subgraph of size `s`
            all = allColoringsHaveCompleteOrEmptySubgraph(uniqueBase, filteredEdgeMasksCompleteByLastOne, filteredEdgeMasksEmptyByLastZero, &counterExample2, &recursionSteps2, &coloringsChecked2, &edgeMaskChecks2);
        }

        auto t6  = std::chrono::steady_clock::now();
        auto t56 = std::chrono::duration<double>(t6 - t5).count();

        t56Min  = t56Min != -1 ? std::min(t56Min, t56) : t56;
        t56Max  = t56Max != -1 ? std::max(t56Max, t56) : t56;
        t56Sum += t56;

#if R5_VERBOSE >= 2
        cerr << "  Check all colorings:                                    " << std::setw(15 + 4) << std::fixed << t56 << " seconds" << endl;
        cerr << "  Number of recursion steps:                              " << std::setw(15) << recursionSteps2   << endl;
        cerr << "  Number of colorings checked:                            " << std::setw(15) << coloringsChecked2 << endl;
        cerr << "  Number of edge mask checks:                             " << std::setw(15) << edgeMaskChecks2   << endl;
#endif

        recursionSteps   += recursionSteps2;
        coloringsChecked += coloringsChecked2;
        edgeMaskChecks   += edgeMaskChecks2;

#if R5_VERBOSE >= 2
        cerr << "]" << endl << endl;
#endif

        if (all == false) {
            counterExample = counterExample2;
            allColoringsHaveCompleteOrEmptySubgraph_ = false;
            break;
        }
    }

    auto t456RealEnd = std::chrono::steady_clock::now();
    auto t456Real    = std::chrono::duration<double>(t456RealEnd - t456RealStart).count();

    cerr << "Filter, sort subgraph edge masks:             " << std::setw(15 + 3) << std::fixed
            << "min "   << std::setw(4 + 4) << t45Min
            << ", avg " << std::setw(4 + 4) << t45Sum / uniqueBaseMasks.size()
            << ", max " << std::setw(4 + 4) << t45Max
            << ", sum " << std::setw(4 + 4) << t45Sum
            << " seconds" << endl;

    cerr << "Check all colorings:                          " << std::setw(15 + 3) << std::fixed
            << "min "   << std::setw(4 + 4) << t56Min
            << ", avg " << std::setw(4 + 4) << t56Sum / uniqueBaseMasks.size()
            << ", max " << std::setw(4 + 4) << t56Max
            << ", sum " << std::setw(4 + 4) << t56Sum
            << " seconds" << endl;

    cerr << "Filter, Sort, Check colorings (real):         " << std::setw(15 + 4) << t456Real << " seconds" << endl;
#if R5_VERBOSE >= 1
    cerr << "Number of recursion steps:                    " << std::setw(15) << recursionSteps << endl;
    cerr << "Number of colorings checked:                  " << std::setw(15) << coloringsChecked << endl;
    cerr << "Number of edge mask checks:                   " << std::setw(15) << edgeMaskChecks << endl;
#endif
    cerr << "Total time:                                   " << std::setw(15 + 4) << std::chrono::duration<double>(t456RealEnd - t1).count() << " seconds" << endl;

    cerr << endl;

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
