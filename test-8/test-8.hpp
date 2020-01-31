#pragma once

#include "prereqs.hpp"

#include <chrono>
#include <functional>
#include <numeric>
#include <map>
#include <unordered_map>

//#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/adjacency_matrix.hpp>
//#include <boost/graph/isomorphism.hpp>

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

    } while (std::prev_permutation(std::begin(nodeMask), std::end(nodeMask)));

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
//        std::array<s64, edges>  gDegreeHistogram{}; // FIXME edges -> nodes
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

//template<s64 nodes>
//std::vector<AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices3(const std::vector<AdjacencyMatrix<nodes>>& graphs) {
//
//    constexpr s64 edges = AdjacencyMatrix<nodes>().edges();
//
//    using BoostGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
//    //using BoostGraph = boost::adjacency_matrix<boost::undirectedS>;
//
//    std::vector<std::tuple<AdjacencyMatrix<nodes>, BoostGraph, std::array<s64, edges>>> uniqueGraphs;
//
//    for (const auto& g : graphs) {
//
//        BoostGraph gBoost(nodes);
//        for (s64 n = 0; n < nodes; n += 1) {
//            for (s64 m = n+1; m < nodes; m += 1) {
//                if (g.edge(n, m)) {
//                    boost::add_edge(n, m, gBoost);
//                }
//            }
//        }
//
//        std::array<s64, nodes>  gDegrees{};
//        std::array<s64, edges>  gDegreeHistogram{}; // FIXME edges -> nodes
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
//        for (const auto& t : uniqueGraphs) {
//
//            const auto& hBoost           = std::get<1>(t);
//            const auto& hDegreeHistogram = std::get<2>(t);
//
//            if (gDegreeHistogram != hDegreeHistogram) { continue; }
//
//            if (boost::isomorphism(gBoost , hBoost)) {
//                isUnique = false;
//                break;
//            }
//        }
//
//        if (isUnique) {
//            uniqueGraphs.push_back(std::make_tuple(g, gBoost, gDegreeHistogram));
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

template<s64 nodes>
std::vector<AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices4(const std::vector<AdjacencyMatrix<nodes>>& graphs) {

    s64 graphCombinations = 0; if (graphCombinations) {} // (void) x; didn't work for unknown reasons
    s64 recursionSteps    = 0; (void) recursionSteps;
    s64 permutationChecks = 0; (void) permutationChecks;

    // Tests whether the permutation makes g match h
    // Only tests edges to/from the newly assigned node `n`
    auto match = [](const AdjacencyMatrix<nodes>& g, const AdjacencyMatrix<nodes>& h, s64 n, const std::array<s64, nodes>& permutation) {
        for (s64 m = 0; m < n; m += 1) {
            if (g.edge(n, m) != h.edge(permutation[n], permutation[m])) {
                return false;
            }
        }
        return true;
    };

    std::function<bool(s64, const std::array<s64, nodes>&, const std::array<std::vector<s64>, nodes>&, const AdjacencyMatrix<nodes>&, const AdjacencyMatrix<nodes>&, const std::array<s64, nodes>&)> isIsomorphic = [&isIsomorphic, &match, &recursionSteps, &permutationChecks](
            s64 n,                                                      // next node of g to be mapped
            const std::array<s64, nodes>& permutation,                  // current permutation
            const std::array<std::vector<s64>, nodes>& hAvailableNodes, // possible nodes where to map n to
            const AdjacencyMatrix<nodes>& g,
            const AdjacencyMatrix<nodes>& h,
            const std::array<s64, nodes>& gDegrees) -> bool {

        R5_VERBOSE_1(recursionSteps += 1);

        if (n == nodes) {
            return true;
        }

        const auto& hAvailableNodesDegreeOfN = hAvailableNodes[gDegrees[n]];

        // for each node m in h that has the same degree as n in g
        for (std::size_t i = 0; i < hAvailableNodesDegreeOfN.size(); ++i) {

            s64 m = hAvailableNodesDegreeOfN[i];

            // map n to g
            auto newPermutation = permutation;
            newPermutation[n] = m;

            R5_VERBOSE_1(permutationChecks += 1);

            // does every previously mapped node's edge with n match under the permutation
            // g(0..n-1, n) == h(permutation[0..n-1], permutation[n])
            if (match(g, h, n, newPermutation) == false) {
                continue;
            }

            auto newHAvailableNodes = hAvailableNodes;
            newHAvailableNodes[gDegrees[n]].erase(std::begin(newHAvailableNodes[gDegrees[n]]) + i);

            if (isIsomorphic(n+1, newPermutation, newHAvailableNodes, g, h, gDegrees) == true) {
                return true;
            }
        }

        return false;
    };

    constexpr s64 edges = AdjacencyMatrix<nodes>().edges();

    // Note: std::map might not be great long-term. unordered_map?
    std::map<std::array<s64, nodes>, std::vector<std::tuple<AdjacencyMatrix<nodes>, std::array<std::vector<s64>, nodes>>>> uniqueGraphs;

    for (const auto& g : graphs) {

        std::array<s64, nodes> gDegrees{};
        std::array<s64, nodes> gDegreeHistogram{};
        using AmIndexer = r5::AdjacencyMatrixIndexer<nodes>;

        for (s64 e = 0; e < edges; e += 1) {
            auto cr = AmIndexer::reverse(e);
            gDegrees[cr.first]  += g.edge(e);
            gDegrees[cr.second] += g.edge(e);
        }

        for (s64 d : gDegrees) {
            gDegreeHistogram[d] += 1;
        }

        std::array<std::vector<s64>, nodes> gNodesByDegree{};
        for (s64 n = 0; n < nodes; n += 1) {
            gNodesByDegree[gDegrees[n]].push_back(n);
        }

        bool isUnique = true;

        auto it = uniqueGraphs.find(gDegreeHistogram);
        if (it == std::end(uniqueGraphs)) {
            isUnique = true;
        } else {
            // for each recorded unique graph with the same degree histogram as g
            // (g and h cannot be isomorphic if the node degrees differ)
            for (const auto& t : it->second) {

                R5_VERBOSE_1(graphCombinations += 1);

                const auto& h              = std::get<0>(t);
                const auto& hNodesByDegree = std::get<1>(t);

                if (isIsomorphic(0, std::array<s64, nodes>{}, hNodesByDegree, g, h, gDegrees) == true) {
                    isUnique = false;
                    break;
                }
            }
        }

        if (isUnique) {
            uniqueGraphs[gDegreeHistogram].push_back(std::make_tuple(g, gNodesByDegree));
        }
    }

#if R5_VERBOSE >= 1
    std::size_t maxSize = 0;
    for (const auto& v : uniqueGraphs) {
        maxSize = std::max(maxSize, v.second.size());
    }

    cerr << "  Unique degree histograms:                " << std::setw(15) << uniqueGraphs.size() << endl;
    cerr << "  Max graphs per degree histogram:         " << std::setw(15) << maxSize << endl;
    cerr << "  Graph combinations checked               " << std::setw(15) << graphCombinations << endl;
    cerr << "  Recursion steps                          " << std::setw(15) << recursionSteps << endl;
    cerr << "  Permutation checks                       " << std::setw(15) << permutationChecks << endl;
#endif

    std::vector<AdjacencyMatrix<nodes>> ret;
    for (const auto& v : uniqueGraphs) {
#if R5_VERBOSE >= 2
        cerr << "    " << v.first << " : " << v.second.size() << endl;
#endif
        for (const auto& t : v.second) {
            ret.push_back(std::get<0>(t));
        }
    }

#if R5_VERBOSE >= 1
    cerr << endl;
#endif

    return ret;
}

template<s64 nodes>
std::vector<AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices5(const std::vector<AdjacencyMatrix<nodes>>& graphs) {

#if R5_VERBOSE >= 1
    s64 graphCombinations  = 0;
    s64 graphCombinations2 = 0;
    s64 recursionSteps     = 0;
    s64 permutationChecks  = 0;
    s64 fixedNodesSum      = 0;
#endif

    constexpr s64 edges = AdjacencyMatrix<nodes>().edges();

    // Note: std::map might not be great long-term. unordered_map?
    std::map<std::array<s64, nodes> /*degree histogram*/, std::vector<std::tuple<AdjacencyMatrix<nodes>/*g*/, std::array<std::vector<s64>, nodes>>/*gNodesByDegree*/>> uniqueGraphs;
    s64 uniqueGraphsCount = 0;

    std::vector<std::tuple<s64 /*i*/, s64 /*m*/, bool /*traverse*/>> stack;
    stack.reserve(nodes*2);

    for (const auto& g : graphs) {

        using AmIndexer = r5::AdjacencyMatrixIndexer<nodes>;

        std::array<s64, nodes> gDegrees{};
        for (s64 e = 0; e < edges; e += 1) {
            auto cr = AmIndexer::reverse(e);
            gDegrees[cr.first]  += g.edge(e);
            gDegrees[cr.second] += g.edge(e);
        }

        std::array<s64, nodes> gDegreeHistogram{}; // degree -> degree multiplicity
        for (s64 d : gDegrees) {
            gDegreeHistogram[d] += 1;
        }

        std::array<std::set<s64>, nodes+1> gDegreeHistogramReverse{}; // degree multiplicty -> set of degrees
        for (std::size_t i = 0; i < nodes; i += 1) {
            gDegreeHistogramReverse[gDegreeHistogram[i]].insert(i);
        }

        std::array<std::vector<s64>, nodes> gNodesByDegree{};
        for (s64 n = 0; n < nodes; n += 1) {
            gNodesByDegree[gDegrees[n]].emplace_back(n);
        }


        // Traversal Order:
        // 1) Assign nodes of degree 0 to any node of degree 0
        // 2) Assign nodes of degree nodes-1 to any node of degree nodes-1
        // 3) Assign nodes of a unique degree to the one possible option
        // 4) Traverse in order of lowest degree multiplicity first.
        //    I.e. the node degree that is rarest comes first,
        //    nodes with the most common degree come last.
        //    This slims the traversal tree. Smaller fan-out first, bigger fan-out later.
        std::array<s64, nodes> traversalOrder;

        std::size_t firstNotFixedNodeIndex = 0;
        for (s64 n : gNodesByDegree[0]) {
            traversalOrder[firstNotFixedNodeIndex] = n;
            firstNotFixedNodeIndex += 1;
        }

        for (s64 n : gNodesByDegree[nodes-1]) {
            traversalOrder[firstNotFixedNodeIndex] = n;
            firstNotFixedNodeIndex += 1;
        }

        // Not that j = 1 and < nodes-1 skips the above two cases
        for (std::size_t d = 1; d < nodes-1; ++d) {
            if (gDegreeHistogram[d] == 1) {
                R5_DEBUG_ASSERT(gNodesByDegree[d].size() == 1);
                traversalOrder[firstNotFixedNodeIndex] = gNodesByDegree[d][0];
                firstNotFixedNodeIndex += 1;
            }
        }

        R5_VERBOSE_1(fixedNodesSum += firstNotFixedNodeIndex);

        std::size_t traversedNode = firstNotFixedNodeIndex;
        for (std::size_t degreeMultiplicity = 2; degreeMultiplicity <= nodes; degreeMultiplicity += 1) {
            for (s64 d : gDegreeHistogramReverse[degreeMultiplicity]) {
                if (d == 0 || d == nodes-1) { continue; }
                for (s64 n : gNodesByDegree[d]) {
                    traversalOrder[traversedNode] = n;
                    traversedNode += 1;
                }
            }
        }

        // cerr << "  traversal order " << traversalOrder  << " firstNotFixedNodeIndex " << firstNotFixedNodeIndex << endl;

        bool isUnique = true;

        auto it = uniqueGraphs.find(gDegreeHistogram);
        if (it == std::end(uniqueGraphs)) {
            // cerr << "  unique degree histogram" << endl;
            isUnique = true;
        } else {
            // for each recorded unique graph h with the same degree histogram as g
            // (g and h cannot be isomorphic if the node degrees differ)
            for (auto& t : it->second) {

                R5_VERBOSE_1(graphCombinations += 1);

                const auto& h        = std::get<0>(t);
                auto hAvailableNodes = std::get<1>(t); // copy

                // cerr << "  h " << h << " hAvailableNodes " << hAvailableNodes << endl;

                std::array<s64, nodes> permutation{};
                for (std::size_t i = 0; i < firstNotFixedNodeIndex; i += 1) {
                    s64 n = traversalOrder[i];
                    s64 m = hAvailableNodes[gDegrees[n]].back();
                    permutation[n] = m;
                    hAvailableNodes[gDegrees[n]].pop_back();
                }

                bool match = true;
                for (std::size_t i = 0; i < firstNotFixedNodeIndex; i += 1) {
                    s64 n = traversalOrder[i];
                    for (std::size_t j = 0; j < i; j += 1) {
                        s64 m = traversalOrder[j];
                        if (g.edge(n, m) != h.edge(permutation[n], permutation[m])) {
                            match = false;
                            break;
                        }
                    }

                    if (match == false) {
                        break;
                    }
                }
                R5_VERBOSE_1(permutationChecks += 1);

                if (match == false) {
                    // cerr << "    early mismatch" << endl;
                    continue;
                } else if (firstNotFixedNodeIndex == nodes) {
                    // cerr << "    complete early match" << endl;
                    isUnique = false;
                    break;
                }

                R5_VERBOSE_1(graphCombinations2 += 1);

                // cerr << "    possibly isomorphic" << endl;

                stack.clear();
                for (s64 m : hAvailableNodes[gDegrees[traversalOrder[firstNotFixedNodeIndex]]]) {
                    stack.emplace_back(std::make_tuple(firstNotFixedNodeIndex, m, true));
                }

                while (stack.empty() == false) {

                    // cerr << "    stack " << stack << endl;

                    R5_VERBOSE_1(recursionSteps += 1);

                    s64 i;
                    s64 m;
                    bool traverse;

                    std::tie(i, m, traverse) = stack.back();
                    s64 n = traversalOrder[i];

                    // cerr << "  " << i << " n " << n << endl;

                    if (traverse == false) {
                        hAvailableNodes[gDegrees[n]].emplace_back(m);
                        stack.pop_back();
                        continue;
                    }

                    permutation[n] = m;

                    // cerr << "    permutation " << permutation << endl;

                    R5_VERBOSE_1(permutationChecks += 1);

                    bool match = true;
                    for (s64 j = 0; j < i; j += 1) {
                        s64 m = traversalOrder[j];
                        if (g.edge(n, m) != h.edge(permutation[n], permutation[m])) {
                            match = false;
                            break;
                        }
                    }

                    if (match == false) {
                        stack.pop_back();
                        continue;
                    } else if (i == nodes-1) {
                        isUnique = false;
                        // cerr << "    isomorphic" << endl;
                        break;
                    } else {

                        auto& hAvailableNodesDegreeN = hAvailableNodes[gDegrees[n]];
                        hAvailableNodesDegreeN.erase(std::find(std::begin(hAvailableNodesDegreeN), std::end(hAvailableNodesDegreeN), m));

                        std::get<2>(stack.back()) = false;

                        for (s64 m_ : hAvailableNodes[gDegrees[traversalOrder[i+1]]]) {
                            stack.emplace_back(std::make_tuple(i+1, m_, true));
                        }
                    }
                }

                if (isUnique == false) {
                    break;
                }
            }
        }

        if (isUnique) {
            // cerr << "  unique g " << g << endl;
            uniqueGraphs[gDegreeHistogram].emplace_back(std::make_tuple(g, gNodesByDegree));
            uniqueGraphsCount += 1;
        }
    }

#if R5_VERBOSE >= 1
    std::size_t maxSize = 0;
    for (const auto& v : uniqueGraphs) {
        maxSize = std::max(maxSize, v.second.size());
    }

    cerr << "  Average fixed nodes:                     " << std::setw(15 + 4) << std::fixed << fixedNodesSum / (double) graphs.size() << endl;
    cerr << "  Unique degree histograms:                " << std::setw(15) << uniqueGraphs.size() << endl;
    cerr << "  Max graphs per degree histogram:         " << std::setw(15) << maxSize << endl;
    cerr << "  Graph combinations checked               " << std::setw(15) << graphCombinations << endl;
    cerr << "  Graph combinations requiring traversal   " << std::setw(15) << graphCombinations2 << endl;
    cerr << "  Recursion steps                          " << std::setw(15) << recursionSteps << endl;
    cerr << "  Permutation checks                       " << std::setw(15) << permutationChecks << endl;
#endif

    std::vector<AdjacencyMatrix<nodes>> ret;
    ret.reserve(uniqueGraphsCount);
    for (const auto& v : uniqueGraphs) {
#if R5_VERBOSE >= 2
        cerr << "    " << v.first << " : " << v.second.size() << endl;
#endif
        for (const auto& t : v.second) {
            ret.emplace_back(std::get<0>(t));
        }
    }

#if R5_VERBOSE >= 1
    cerr << endl;
#endif

    // std::sort(std::begin(ret), std::end(ret)); // FIXME/NOTE: Use this to properly order ramsey graphs, but this slows everything down by 10% or so

    return ret;
}


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

        R5_VERBOSE_1(auto t1 = std::chrono::steady_clock::now());

        std::vector<AdjacencyMatrix<n>> edgeMasksComplete;
        std::vector<AdjacencyMatrix<n>> edgeMasksEmpty;
        if (n >= r) {
            edgeMasksComplete = subGraphEdgeMasks<e, n, r>();
        }
        if (n >= s) {
            edgeMasksEmpty    = invertSubgraphEdgeMasks<e, n, s>(subGraphEdgeMasks<e, n, s>());
        }
        R5_VERBOSE_1(auto t2 = std::chrono::steady_clock::now());
        R5_VERBOSE_1(auto t12 = std::chrono::duration<double>(t2 - t1).count());

        // TODO only generate the subgraphs interesting for the current extension

        std::array<std::vector<AdjacencyMatrix<n>>, e + 1> edgeMasksCompleteByLastOne = subGraphEdgeMasksByLastDigit<e, n, true>(edgeMasksComplete);
        std::array<std::vector<AdjacencyMatrix<n>>, e + 1> edgeMasksEmptyByLastZero   = subGraphEdgeMasksByLastDigit<e, n, false>(edgeMasksEmpty);

        R5_VERBOSE_1(auto t3 = std::chrono::steady_clock::now());
        R5_VERBOSE_1(auto t23 = std::chrono::duration<double>(t3 - t2).count());

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

        R5_VERBOSE_1(auto t4 = std::chrono::steady_clock::now());

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

        R5_VERBOSE_1(auto t5 = std::chrono::steady_clock::now());
        R5_VERBOSE_1(auto t45 = std::chrono::duration<double>(t5 - t4).count());

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

        R5_VERBOSE_1(auto t6 = std::chrono::steady_clock::now());
        auto ramseyGraphs = uniqueAdjacencyMatrices5(nonUniqueRamseyGraphs);
        R5_VERBOSE_1(auto t7 = std::chrono::steady_clock::now());

        R5_VERBOSE_1(auto t67 = std::chrono::duration<double>(t7 - t6).count());

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

