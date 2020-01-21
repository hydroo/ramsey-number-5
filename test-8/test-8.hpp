#pragma once

#include "prereqs.hpp"

#include <chrono>
#include <functional>
#include <numeric>

#include "adjacencymatrix.hpp"

using std::cerr;
using std::cout;
using std::endl;

using r5::AdjacencyMatrix;
using r5::AdjacencyMatrixIndexer;

// Returns an array of adjacency matrices.
// Returns the set of all possible graphs that have exactly one complete subgraph of size `subGraphSize`.
// TODO only enumerate the ones for the current extension.
// I.e. Probably the last node has to be true
template <s64 edges, s64 nodes, s64 subGraphSize>
std::vector<AdjacencyMatrix<nodes>> subGraphEdgeMasks() {

    std::vector<AdjacencyMatrix<nodes>> masks(nChooseK(nodes, subGraphSize));

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
        masks[p].unsetAllEdges();

        for (s64 e = 0; e < edges; e += 1) {
            auto pair = AdjacencyMatrixIndexer<nodes>::reverse(e);
            s64 c = pair.first;
            s64 r = pair.second;

            // cerr << "e " << e << ", c " << c << ", r " << r << endl;

            if (nodeMask[c] == true && nodeMask[r] == true) {
                masks[p].setEdge(c, r);
            }
        }

        // cerr << "node mask " << nodeMask << ", edge mask " << (*masks)[p] << endl;

        p += 1;

    } while (r5::prev_permutation(std::begin(nodeMask), std::end(nodeMask)));

    return masks;
}

// flips all edges
template <s64 edges, s64 nodes, s64 subGraphSize>
std::vector<AdjacencyMatrix<nodes>> invertSubgraphEdgeMasks(const std::vector<AdjacencyMatrix<nodes>>& edgeMasks) {
    R5_ASSERT(edgeMasks.size() == nChooseK(nodes, subGraphSize));
    std::vector<AdjacencyMatrix<nodes>> ret(nChooseK(nodes, subGraphSize));
    for (std::size_t i = 0; i < edgeMasks.size(); i += 1) {
        ret[i] = ~edgeMasks[i];
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

template<s64 nodes>
std::vector<AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices(const std::vector<AdjacencyMatrix<nodes>>& graphs) {

    constexpr s64 edges = AdjacencyMatrix<nodes>().edges();
    constexpr s64 permutationCount = factorial(nodes);

#if R5_VERBOSE >= 1
        cerr << "  Uniquify ramsey graphs" << endl;
        cerr << "    Number of permutations                 " << std::setw(15) << permutationCount << endl;
#endif

    auto t1 = std::chrono::steady_clock::now();

    // create all permutations and store them as maps from edges to edges
    auto edgePermutations = []() -> std::vector<std::array<s64, edges>> {

        std::vector<std::array<s64, edges>> ret(permutationCount);
        using AmIndexer = r5::AdjacencyMatrixIndexer<nodes>;

        std::array<s64, nodes> permutation;
        std::iota(std::begin(permutation), std::end(permutation), 0);

        s64 p = 0;
        do {
            for (s64 e = 0; e < edges; e += 1) {
                auto cr = AmIndexer::reverse(e);
                ret[p][AmIndexer::index(permutation[cr.first], permutation[cr.second])] = e;
            }

            p += 1;
        } while (std::next_permutation(std::begin(permutation), std::end(permutation)));

        return ret;
    }();

    auto t2 = std::chrono::steady_clock::now();
    auto t12 = std::chrono::duration<double>(t2 - t1).count();
#if R5_VERBOSE >= 1
        cerr << "    Create permutations:                   " << std::setw(15 + 4) << std::fixed << t12 << " seconds" << endl;
#if R5_VERBOSE >= 2
        if (nodes < 5) {
            cerr << "    Edge permutations:                     " << std::setw(15) << edgePermutations << endl;
        }
#if R5_VERBOSE >= 3
        cerr << "    Not canonical graphs:" << endl;
#endif
#endif
#endif

    std::vector<AdjacencyMatrix<nodes>> ret;

    auto t3 = std::chrono::steady_clock::now();
    for (const auto& g : graphs) {

        bool isCanonical = true;

        for (const auto& permutation : edgePermutations) {

            for (s64 e = 0; e < edges; e += 1) {

                const bool e1 = g.edge(permutation[e]);
                const bool e2 = g.edge(e);

                if (e1 != e2) {
                    isCanonical = e1 < e2;

#if R5_VERBOSE >= 3
                    if (isCanonical == false) {
        cerr << "      " << g << " " << " e: " << e << " p[e]: " << permutation[e] << " p: "<< permutation << endl;
                    }
#endif

                    break;
                }
            }

            if (isCanonical == false) {
                break;
            }
        }

        if (isCanonical == true) {
            ret.push_back(g);
        }
    }
    auto t4  = std::chrono::steady_clock::now();
    auto t34 = std::chrono::duration<double>(t4 - t3).count();
#if R5_VERBOSE >= 1
        cerr << "    Check canonicity:                      " << std::setw(15 + 4) << std::fixed << t34 << " seconds" << endl;
#endif

    return ret;
}

//template<s64 nodes>
//std::vector<AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices2(const std::vector<AdjacencyMatrix<nodes>>& graphs) {
//
//    constexpr s64 edges = AdjacencyMatrix<nodes>().edges();
//
//    std::vector<std::tuple<AdjacencyMatrix<nodes>, std::array<s64, nodes>, std::array<s64, edges>>> uniqueGraphs;
//
//    for (const auto& g : graphs) {
//
//        std::array<s64, nodes>  gDegrees{};
//        std::array<s64, edges>  gDegreeHistogram{};
//        using AmIndexer = r5::AdjacencyMatrixIndexer<nodes>;
//
//        for (s64 e = 0; e < edges; e += 1) {
//            auto cr = AmIndexer::reverse(e);
//            gDegrees[cr.first]  += g.edge(e);
//            gDegrees[cr.second] += g.edge(e);
//        }
//
//        for (s64 d : gDegrees) {
//            gDegreeHistogram[d] += 1;
//        }
//
//        bool isUnique = true;
//
//        for (const auto& t : uniqueGraphs) {
//
//            auto h                = std::get<0>(t);
//            auto hDegrees         = std::get<1>(t);
//            auto hDegreeHistogram = std::get<2>(t);
//
//            if (gDegreeHistogram != hDegreeHistogram) { continue; }
//
//            std::array<s64, nodes> permutation;
//            std::iota(std::begin(permutation), std::end(permutation), 0);
//
//            do {
//
//                bool sameDegrees = true;
//                for (s64 n = 0; n < nodes; n += 1) {
//                    if (gDegrees[permutation[n]] != hDegrees[n]) {
//                        sameDegrees = false;
//                        break;
//                    }
//                }
//                if (sameDegrees == false) { continue; }
//
//                bool isIsomorph = true;
//                for (s64 e = 0; e < edges; e += 1) {
//                    auto cr = AmIndexer::reverse(e);
//                    if (g.edge(AmIndexer::index(permutation[cr.first], permutation[cr.second])) != h.edge(e)) {
//                        isIsomorph = false;
//                        break;
//                    }
//                }
//
//                if (isIsomorph) {
//                    isUnique = false;
//                    break;
//                }
//
//            } while (std::next_permutation(std::begin(permutation), std::end(permutation)));
//
//            if (isUnique == false) {
//                break;
//            }
//        }
//
//        if (isUnique) {
//            uniqueGraphs.push_back(std::make_tuple(g, gDegrees, gDegreeHistogram));
//        }
//    }
//
//    std::vector<AdjacencyMatrix<nodes>> ret;
//    for (const auto& t : uniqueGraphs) {
//        ret.push_back(std::get<0>(t));
//    }
//
//    return ret;
//}

template<s64 r, s64 s, s64 n, typename Enable = void>
struct RamseyGraphs {
    static std::vector<AdjacencyMatrix<n>> f() {
        R5_STATIC_ASSERT(r >= 1);
        R5_STATIC_ASSERT(s >= 1);

        constexpr auto e = AdjacencyMatrix<n>().edges();

        auto smallerRamseyGraphs = RamseyGraphs<r, s, n-1>::f();

#if R5_VERBOSE >= 1
        cerr << "Ramsey(" << r << "," << s << ")-graphs with " << n << " vertices" << endl;
#endif

        auto t1 = std::chrono::steady_clock::now();

        std::vector<AdjacencyMatrix<n>> edgeMasksComplete;
        std::vector<AdjacencyMatrix<n>> edgeMasksEmpty;
        if (n >= r) {
            edgeMasksComplete = subGraphEdgeMasks<e, n, r>();
        }
        if (n >= s) {
            edgeMasksEmpty    = invertSubgraphEdgeMasks<e, n, s>(subGraphEdgeMasks<e, n, s>());
        }
        auto t2 = std::chrono::steady_clock::now();
        auto t12 = std::chrono::duration<double>(t2 - t1).count();

        // TODO only generate the subgraphs interesting for the current extension

        std::array<std::vector<AdjacencyMatrix<n>>, e + 1> edgeMasksCompleteByLastOne = subGraphEdgeMasksByLastDigit<e, n, true>(edgeMasksComplete);
        std::array<std::vector<AdjacencyMatrix<n>>, e + 1> edgeMasksEmptyByLastZero   = subGraphEdgeMasksByLastDigit<e, n, false>(edgeMasksEmpty);

        auto t3 = std::chrono::steady_clock::now();
        auto t23 = std::chrono::duration<double>(t3 - t2).count();

#if R5_VERBOSE >= 1
        cerr << "  Create subgraph edge masks:              " << std::setw(15 + 4) << std::fixed << t12 << " seconds" << endl;
        cerr << "  Sort subgraph edge masks by last digit:  " << std::setw(15 + 4) << std::fixed << t23 << " seconds" << endl;
        cerr << "  Number of complete edge masks by last 1: " << std::setw(15) << "[";
        for (size_t i = 0; i < edgeMasksCompleteByLastOne.size(); i += 1) {
            if (edgeMasksCompleteByLastOne[i].size() > 0) {
                cerr << i - 1 << " : " << edgeMasksCompleteByLastOne[i].size();
                if (i < edgeMasksCompleteByLastOne.size()-1) {
                    cerr << ", ";
                }
            }
        }
        cerr << "]" << endl;
        cerr << "  Number of empty    edge masks by last 0: " << std::setw(15) << "[";
        for (size_t i = 0; i < edgeMasksEmptyByLastZero.size(); i += 1) {
            if (edgeMasksEmptyByLastZero[i].size() > 0) {
                cerr << i - 1 << " : " << edgeMasksEmptyByLastZero[i].size();
                if (i < edgeMasksEmptyByLastZero.size()-1) {
                    cerr << ", ";
                }
            }
        }
        cerr << "]" << endl;
        cerr << endl;
        cerr << "  Smaller Ramsey graphs:                   " << std::setw(15) << smallerRamseyGraphs.size() << endl;
        cerr << "  New edges to fill:                       " << std::setw(15) << n-1 << endl;
        cerr << "  Possible combinations:                   " << std::setw(15) << smallerRamseyGraphs.size()*s64(std::pow(2, n-1)) << " # " << smallerRamseyGraphs.size() << " * 2^" << n-1 << endl;
        cerr << endl;
#endif

        auto t4 = std::chrono::steady_clock::now();

        std::vector<AdjacencyMatrix<n>> nonUniqueRamseyGraphs;

#if R5_VERBOSE >= 1
        s64 recursionSteps   = 0;
        s64 coloringsChecked = 0;
        s64 edgeMaskChecks   = 0;
#endif

        for (const AdjacencyMatrix<n-1>& graph : smallerRamseyGraphs) {

            // cerr << "  " << graph << endl;

            AdjacencyMatrix<n> coloring(graph);

            // *** Start DFS ***

            std::array<s64, e + 1> stack;
            s64 stackTop = 1;
            stack[0] = AdjacencyMatrix<n-1>().edges(); // set   edge x
            stack[1] = AdjacencyMatrix<n-1>().edges(); // unset edge x

            while (stackTop >= 0) {

                R5_DEBUG_ASSERT(stackTop < (s64) stack.size());

                // cerr << stackTop << stack << endl;

                R5_VERBOSE_1(recursionSteps += 1);
                // TODO the number of recursion steps is about 2x of what it should be
                // Maybe this DFS needs fixing

                s64 edge = stack[stackTop];
                stackTop -= 1;

                coloring.toggleEdge(edge);

                // cerr << "  " << coloring << " edge " << edge << endl;

                if (coloring.edge(edge) == true) {

                    if (n >= r) { // avoids matching subgraphs larger than the to-be-checked graph
                        // Compare this graph against all appropriate complete subgraphs.
                        // Appropriate means every graph whos complete subgraph's laste edge is exactly the lastly enumerated edge
                        bool foundCompleteSubgraph = false;
                        const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[edge + 1];
                        R5_VERBOSE_1(if (edgeMasksCompleteByLastOne.size() > 0) { coloringsChecked += 1; });
                        for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                            R5_VERBOSE_1(edgeMaskChecks += 1);
                            if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) {
                                foundCompleteSubgraph = true;
                                break;
                            }
                        }
                        if (foundCompleteSubgraph) { continue; }
                    }

                } else {

                    if (n >= s) { // avoids matching subgraphs larger than the to-be-checked graph
                        // Do the same for empty subgraphs
                        bool foundEmptySubgraph = false;
                        const auto& currentEdgeMasksEmpty = edgeMasksEmptyByLastZero[edge + 1];
                        R5_VERBOSE_1(if (edgeMasksEmptyByLastZero.size() > 0) { coloringsChecked += 1; });
                        for (std::size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
                            R5_VERBOSE_1(edgeMaskChecks += 1);
                            if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) {
                                foundEmptySubgraph = true;
                                break;
                            }
                        }
                        if (foundEmptySubgraph) { continue; }
                    }

                }

                if (edge < e - 1) {
                    stack[stackTop+1] = edge + 1; // set   edge + 1
                    stack[stackTop+2] = edge + 1; // unset edge + 1
                    stackTop += 2;
                } else {
                    // If this graph is completely enumerated and no complete or empty subgraph has been found,
                    // return false and provide this graph as a counter example.
                    R5_VERBOSE_1(coloringsChecked += 1);
                    nonUniqueRamseyGraphs.push_back(coloring);

                    // cerr << "    " << coloring << " has no complete or empty subgraphs" << endl;
                }
            }
        }

        auto t5 = std::chrono::steady_clock::now();
        auto t45 = std::chrono::duration<double>(t5 - t4).count();

#if R5_VERBOSE >= 1
        cerr << "  Check all colorings:                     " << std::setw(15 + 4) << std::fixed << t45 << " seconds" << endl;
        cerr << "  Number of recursion steps:               " << std::setw(15) << recursionSteps << endl;
        cerr << "  Number of colorings checked:             " << std::setw(15) << coloringsChecked << endl;
        cerr << "  Number of edge mask checks:              " << std::setw(15) << edgeMaskChecks   << endl;
        cerr << "  Non-unique Ramsey graphs:                " << std::setw(15) << nonUniqueRamseyGraphs.size() << endl;
#if R5_VERBOSE >= 2
        cerr << "  Non-unique Ramsey graphs:                " << std::setw(15) << nonUniqueRamseyGraphs << endl;
#endif
        cerr << endl;
#endif

        auto t6 = std::chrono::steady_clock::now();
        auto ramseyGraphs = uniqueAdjacencyMatrices(nonUniqueRamseyGraphs);
        auto t7 = std::chrono::steady_clock::now();

        auto t67 = std::chrono::duration<double>(t7 - t6).count();

#if R5_VERBOSE >= 1
        cerr << "  Uniquify Ramsey graphs:                  " << std::setw(15 + 4) << std::fixed << t67 << " seconds" << endl;
        cerr << "  Ramsey graphs:                           " << std::setw(15) << ramseyGraphs.size() << endl;
#if R5_VERBOSE >= 2
        cerr << "  Ramsey graphs:                           " << std::setw(15) << ramseyGraphs << endl;
#endif
        cerr << endl;
#endif

        return ramseyGraphs;
    }
};

template<s64 r, s64 s>
struct RamseyGraphs<r, s, 1, std::enable_if_t<(r > 1 && s > 1)>> {
    static std::vector<AdjacencyMatrix<1>> f() { return {AdjacencyMatrix<1>()}; }
};
template<s64 r>
struct RamseyGraphs<r, 1, 1> {
    static std::vector<AdjacencyMatrix<1>> f() { return {}; }
};
template<s64 s>
struct RamseyGraphs<1, s, 1, std::enable_if_t<(s > 1)>> {
    static std::vector<AdjacencyMatrix<1>> f() { return {}; }
};

