#pragma once

#include "prereqs.hpp"

#include <chrono>
#include <functional>
#include <iterator>
#include <numeric>
#include <utility>

#include "adjacencymatrix.hpp"
#include "packeduinttuple.hpp"

#include "checkramseygraphcount.hpp"

using std::cerr;
using std::cout;
using std::endl;

using r5::AdjacencyMatrix;
using r5::AdjacencyMatrixIndexer;
using r5::Size;

// Returns an array of adjacency matrices.
// Returns the set of all possible graphs that have exactly one complete subgraph of size `subGraphSize`.
// TODO only enumerate the ones for the current extension.
// I.e. Probably the last node has to be true
template <Size edges, Size nodes, Size subGraphSize>
std::vector<AdjacencyMatrix<nodes>> subGraphEdgeMasks() {

    std::vector<AdjacencyMatrix<nodes>> masks(nChooseK(nodes, subGraphSize));

    std::array<bool, nodes> nodeMask{};

    for (Size i = 0; i < subGraphSize; i += 1) {
        nodeMask[i] = true;
    }
    for (Size i = subGraphSize; i < nodes; i += 1) {
        nodeMask[i] = false;
    }

    // cerr << "nodemask " << nodeMask << endl;

    s64 p = 0;

    do {
        masks[p].unsetAllEdges();

        for (Size e = 0; e < edges; e += 1) {
            auto pair = AdjacencyMatrixIndexer<nodes>::reverse(e);
            Size c = pair.first;
            Size r = pair.second;

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
template <Size edges, Size nodes, Size subGraphSize>
std::vector<AdjacencyMatrix<nodes>> invertSubgraphEdgeMasks(const std::vector<AdjacencyMatrix<nodes>>& edgeMasks) {
    R5_DEBUG_ASSERT(edgeMasks.size() == nChooseK(nodes, subGraphSize));
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
template <Size edges, Size nodes, bool digit>
std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1> subGraphEdgeMasksByLastDigit(const std::vector<AdjacencyMatrix<nodes>>& edgeMasks) {
    std::array<std::vector<AdjacencyMatrix<nodes>>, edges + 1> ret;

    for (const auto& mask : edgeMasks) {
        Size last = -1;  // offset everything by +1 so that we can use -1, and don't have to worry about array[nextEdge-1]
        for (Size i = mask.edges() - 1; i >= 0; i -= 1) {
            if (mask.edge(i) == digit) {
                last = i;
                break;
            }
        }
        ret[last + 1].push_back(mask);
    }

    return ret;
}

template<Size nodes>
std::vector<AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices5(const std::vector<AdjacencyMatrix<nodes>>& graphs) {

#if R5_VERBOSE >= 1
    s64 graphCombinations  = 0;
    s64 graphCombinations2 = 0;
    s64 recursionSteps     = 0;
    s64 permutationChecks  = 0;
    s64 fixedNodesSum      = 0;
#endif

    constexpr Size maxEdgeDegree      = nodes >= 1 ? nodes-1 : 0;
    constexpr Size maxTriangleDegree  = nodes >= 2 ? (nodes-1)*(nodes-2)/2 : 0;
    constexpr Size nodesBits          = r5::staticLog2Ceil(nodes+1);
    constexpr Size edgeDegreeBits     = r5::staticLog2Ceil(maxEdgeDegree+1);
    constexpr Size triangleDegreeBits = r5::staticLog2Ceil(maxTriangleDegree+1);
    using DegreeTuple = r5::PackedUIntTuple<edgeDegreeBits, triangleDegreeBits, triangleDegreeBits>;
    using DegreeHistogramEntry = r5::PackedUIntTuple<edgeDegreeBits, triangleDegreeBits, triangleDegreeBits, nodesBits>;
    using AdjacencyMatrixProperties = std::array<DegreeHistogramEntry, nodes> /*gDegreeHistogram*/;

    struct NodesByDegree {
        // TODO play with these 3 to reduce RAM usage
        using KeyType      = DegreeTuple;
        using NodeType     = u8;
        using IndexType    = u8;
        using KeysSizeType = u8;

        using KeysContainerType  = std::array<KeyType  , nodes>;
        using IndexContainerType = std::array<IndexType, nodes>;
        using NodesContainerType = std::array<NodeType , nodes>;

        static_assert(nodes < 256, "u8 is only enough for 255 nodes. Need to parameterize NodeType");

        KeysContainerType  keys;
        IndexContainerType indices;
        NodesContainerType nodes_;
        KeysSizeType       keysSize;

        using NodesConstIterator = decltype(nodes_)::const_iterator;

        R5_NOINLINE static std::size_t keyFind1(const KeysContainerType& keys, KeysSizeType keysSize, KeyType key) {
            auto it = std::find(keys.cbegin(), keys.cbegin() + keysSize, key); // Note: It seems using nodes is faster than keySize here
            return std::distance(keys.cbegin(), it);
        }

        // returns begin and end indices for nodes_
        R5_NOINLINE std::tuple<NodesConstIterator, NodesConstIterator> find(const KeyType& key) const {
            auto i = keyFind1(keys, keysSize, key);
            auto beginIt = nodes_.cbegin() + indices[i];
            auto endIt   =  i+1 < nodes ? (nodes_.cbegin() + indices[i+1]) : nodes_.cend();
            return std::make_tuple(beginIt, endIt);
        }

        // returns begin and end indices for nodes_
        R5_NOINLINE NodeType findFirstNode(const KeyType& key) const {
            auto [beginIt, endIt] = find(key);
            return *beginIt;
        }

        std::string dump() const {
            std::ostringstream o;
            // TODO convert up from u8, because the characters are printed wrongly.
            o << "keysEnd " << keysSize << " keys " << keys << " indices " << indices << " nodes " << nodes_;
            return o.str();
        }
    };

    //NodesByDegree nodesByDegree2Dummy;
    //std::cerr << "AAA  n " << nodes << " med " << maxEdgeDegree << " mtd " << maxTriangleDegree
    //        << " edb "<< edgeDegreeBits << " tdb " << triangleDegreeBits << " allbits " << edgeDegreeBits+2*triangleDegreeBits
    //        << " sizeof DegreeTuple "<< sizeof(DegreeTuple)
    //        << " sizeof AdjacencyMatrixProperties = " << sizeof(AdjacencyMatrixProperties)
    //        << " (" << nodes << " x " << sizeof(DegreeHistogramEntry) << " + padding, "
    //        << " sizeof map pair " << sizeof(std::pair<AdjacencyMatrixProperties, std::vector<int>>)
    //        << " sizeof map vector entry " << sizeof(std::tuple<AdjacencyMatrix<nodes>, NodesByDegree>)
    //        << " sizeof NodesByDegree " << sizeof(NodesByDegree) << " = " << sizeof(nodesByDegree2Dummy.keys) << " + " << sizeof(nodesByDegree2Dummy.indices) << "+ " << sizeof(nodesByDegree2Dummy.nodes_) << " + " << sizeof(nodesByDegree2Dummy.keysSize) << std::endl;

    std::unordered_map<
        AdjacencyMatrixProperties,
        std::vector<
            std::tuple<
                AdjacencyMatrix<nodes> /*g*/,
                NodesByDegree /*gNodesByDegree*/
            >
        >
    > uniqueGraphs(graphs.size());

#if R5_VERBOSE >= 1
    // TODO update this to reflect that we are dealing with pairs of Key, Value
    auto uniqueGraphsSize = [](const auto& uniqueGraphs) {
        std::size_t keysByteSize = sizeof(AdjacencyMatrixProperties) * uniqueGraphs.size();
        std::size_t valuesByteSize = 0;
        for (const auto& [k, v] : uniqueGraphs) {
            valuesByteSize += sizeof(v);
            if (v.size() > 0) {
                valuesByteSize += v.size() * sizeof(decltype(v[0]));
            }
        }
        return std::make_pair(keysByteSize, valuesByteSize);
    };
#endif

    s64 uniqueGraphsCount = 0;

    std::vector<std::tuple<s64 /*i*/, Size /*m*/>> stack;
    stack.reserve(nodes*(nodes-1));

    for (const auto& g : graphs) {

#if R5_VERBOSE >= 4
        cerr << "g " << g << endl;
#endif

        std::array<Size, nodes> gEdgeDegrees{};
        std::array<Size, nodes> gTriangleDegrees{};
        std::array<Size, nodes> gEmptyTriangleDegrees{};
        for (Size n = 0; n < nodes; n += 1) {
            for (Size m = 0; m < n; m += 1) {
                auto nm = g.edge(n, m);
                if (nm) {
                    gEdgeDegrees[n] += 1;
                    gEdgeDegrees[m] += 1;
                }

                for (Size j = 0; j < m; j += 1) {
                    if (nm && g.edge(m, j) && g.edge(n, j)) {
                        gTriangleDegrees[n] += 1;
                        gTriangleDegrees[m] += 1;
                        gTriangleDegrees[j] += 1;
                    } else if (nm == false && g.edge(m, j) == false && g.edge(n, j) == false) {
                        gEmptyTriangleDegrees[n] += 1;
                        gEmptyTriangleDegrees[m] += 1;
                        gEmptyTriangleDegrees[j] += 1;
                    }
                }
            }
        }

        std::vector<Size> gEmptyOrFullNodes{};
        std::array<DegreeTuple, nodes> gDegrees{};
        for (Size n = 0; n < nodes; n += 1) {
            if (gEdgeDegrees[n] == 0 || gEdgeDegrees[n] == nodes-1) { gEmptyOrFullNodes.emplace_back(n); }
            gDegrees[n] = DegreeTuple({gEdgeDegrees[n], gTriangleDegrees[n], gEmptyTriangleDegrees[n]});
        }

        auto gDegreesSorted = gDegrees;
        std::sort(gDegreesSorted.begin(), gDegreesSorted.end());

        // Builds gDegreeHistogram, and the keys and indices without nodes of gNodesByDegree
        std::array<DegreeHistogramEntry, nodes> gDegreeHistogram{};
        NodesByDegree gNodesByDegree{};
        std::size_t gDegreeHistogramSize = 0;
        typename DegreeTuple::ElementType runningDegreeMultiplicity = 1;
        for (std::size_t i = 1; i <= nodes; ++i) {
            if (i == nodes || gDegreesSorted[i-1] != gDegreesSorted[i]) {
                gDegreeHistogram[gDegreeHistogramSize] = DegreeHistogramEntry({gDegreesSorted[i-1].template get<0>(), gDegreesSorted[i-1].template get<1>(), gDegreesSorted[i-1].template get<2>(), runningDegreeMultiplicity});
                runningDegreeMultiplicity = 1;
                gNodesByDegree.keys[gDegreeHistogramSize] = gDegreesSorted[i-1];
                gDegreeHistogramSize += 1;
                if (gDegreeHistogramSize < nodes) {
                    gNodesByDegree.indices[gDegreeHistogramSize] = i;
                }
            } else {
                runningDegreeMultiplicity += 1;
            }
        }

        gNodesByDegree.keysSize = (typename NodesByDegree::KeysSizeType) gDegreeHistogramSize;

        // Fill in the nodes to gNodesByDegree.nodes_
        std::size_t nZeroJ;
        for (std::size_t n = 0; n < nodes; ++n) {
            const auto& degreeTuple = gDegrees[n];
            auto [ it, itEnd ] = gNodesByDegree.find(degreeTuple);
            auto j = (std::size_t) std::distance(gNodesByDegree.nodes_.cbegin(), it);
            if (n == 0) { nZeroJ = j; }
            while ((gNodesByDegree.nodes_[j] != 0) || (gNodesByDegree.nodes_[j] == 0 && n != 0 && j == nZeroJ)) {
                ++j;
            }
            gNodesByDegree.nodes_[j] = n;
        }

#if R5_VERBOSE >= 4
            cerr << "  gNodesByDegree " << gNodesByDegree << endl;
#endif

        // Note: Sort elements by lowest multiplicity first. <-- this is because multiplicity is the last (left most) entry in the packed uint tuple
        //       This is needed for traversing all degreeTuples by multiplicity below, mainly for the traversal order (low multiplicity -> high multiplicty)
        //       This sorting only sorts the elements on the left that are not (0,0,0), i.e. leaves all (0,0,0) on the right side
        auto gDegreeHistogramItEnd_ = std::begin(gDegreeHistogram); // Use this iterator as max for gDegreeHistogram, to skip the (0,0,0) on the right side
        std::advance(gDegreeHistogramItEnd_, gDegreeHistogramSize);
        std::sort(std::begin(gDegreeHistogram), gDegreeHistogramItEnd_);
        auto gDegreeHistogramItEnd = std::cbegin(gDegreeHistogram); // reset the iterator after sorting, just in case. May not be required.
        std::advance(gDegreeHistogramItEnd, gDegreeHistogramSize);

        bool isUnique = true;

        const AdjacencyMatrixProperties& gProperties = gDegreeHistogram;

        const auto uniqueGraphsIt = uniqueGraphs.find(gProperties);
        if (uniqueGraphsIt == uniqueGraphs.cend()) {
#if R5_VERBOSE >= 4
            cerr << "  unique properties" << endl;
#endif
            isUnique = true;
        } else {

            // Traversal Order:
            // 1) Assign nodes of degree 0 or nodes-1 to any node of the same degree
            // 2) Assign nodes of a unique degree to the one possible option
            // 3) Traverse in order of lowest degree multiplicity first.
            //    I.e. the node degree that is rarest comes first,
            //    nodes with the most common degree come last.
            //    This slims the traversal tree. Smaller fan-out first, bigger fan-out later.
            std::array<Size, nodes> traversalOrder;
            std::array<bool, nodes> fixedNodes{};

            Size firstNotEmptyOrFullNodeIndex = 0;
            for (Size n : gEmptyOrFullNodes) {
                traversalOrder[firstNotEmptyOrFullNodeIndex] = n;
                R5_DEBUG_ASSERT(fixedNodes[n] == false);
                fixedNodes[n] = true;
                firstNotEmptyOrFullNodeIndex += 1;
            }

            // Note: this could be faster if we unpacked DegreeTuple in one sweep
            auto fromDegreeHistogramEntry = [](const DegreeHistogramEntry& e) {
                return std::make_tuple<DegreeTuple, Size>(DegreeTuple({e.template get<0>(), e.template get<1>(), e.template get<2>()}), (Size)e.template get<3>());
            };

            // Slightly unintuitive way to iterate over nodes of unique degree. But it's better than using .find() and/or unpacking fromDegreeHistogramEntry()
            Size firstNotUniqueDegreeMultiplicityNodeIndex = firstNotEmptyOrFullNodeIndex;
            std::size_t previousIndex = 0;
            for (std::size_t i = 1; i <= nodes; ++i) {
                std::size_t currentIndex = i < nodes ? gNodesByDegree.indices[i] : (std::size_t) nodes;
                if (currentIndex - previousIndex == 1) {
                    auto n = gNodesByDegree.nodes_[previousIndex];
                    traversalOrder[firstNotUniqueDegreeMultiplicityNodeIndex] = n;
                    if (fixedNodes[n] == false) {
                        fixedNodes[n] = true;
                        firstNotUniqueDegreeMultiplicityNodeIndex += 1;
                        if (firstNotUniqueDegreeMultiplicityNodeIndex == nodes) { break; }
                    }
                }
                previousIndex = currentIndex;
            }

            Size firstNotFixedNodeIndex = firstNotUniqueDegreeMultiplicityNodeIndex;

            R5_VERBOSE_1(fixedNodesSum += firstNotFixedNodeIndex);

            Size traversedNode = firstNotFixedNodeIndex;
            for (auto it = std::cbegin(gDegreeHistogram); it != gDegreeHistogramItEnd; ++it) {
                const auto [degreeTuple, multiplicity] = fromDegreeHistogramEntry(*it);
                if (multiplicity < 2) { continue; }
                auto [ nodesIt, nodesEndIt ] = gNodesByDegree.find(degreeTuple);
                for (; nodesIt != nodesEndIt; ++nodesIt) {
                    Size n = (Size) *nodesIt;
                    if (fixedNodes[n] == true) { continue; }
                    traversalOrder[traversedNode] = n;
                    traversedNode += 1;
                }
            }

#if R5_VERBOSE >= 4
            cerr << "  traversal order " << traversalOrder  << " firstNotFixedNodeIndex " << firstNotFixedNodeIndex << endl;
#endif

            // for each recorded unique graph h with the same degree histogram as g
            // (g and h cannot be isomorphic if the node degrees differ)
            for (const auto& [ h, hNodesByDegree ] : uniqueGraphsIt->second) {

                R5_VERBOSE_1(graphCombinations += 1);

                std::array<bool, nodes> assignedNodes{};

#if R5_VERBOSE >= 4
                cerr << "  h " << h << " hNodesByDegree " << hNodesByDegree << endl;
#endif

                // Idea: You could check permutations while assigning them for an early exit here -- But this could make SIMD optimizations less interesting
                // Note: This could be split up into empty/full nodes and others. Others would have to be of unique degree, i.e. no inner for-loop. In testing I saw no benefit, though.
                int fixedNodes = 0;
                std::array<Size, nodes> permutation{};
                for (Size i = 0; i < firstNotUniqueDegreeMultiplicityNodeIndex; i += 1) {
                    Size n = traversalOrder[i];
                    const auto& degreeTuple = gDegrees[n];
                    auto [ candidateNodesIt, candidateNodesItEnd ] = hNodesByDegree.find(degreeTuple);
                    if (candidateNodesIt == hNodesByDegree.nodes_.cend()) { break; }
                    for(; candidateNodesIt != candidateNodesItEnd; ++candidateNodesIt) {
                        Size m = (Size) *candidateNodesIt;
                        if (assignedNodes[m] == false) {
                            assignedNodes[m] = true;
                            permutation[n] = m;
                            fixedNodes += 1;
                            break;
                        }
                    }
                }

                if (fixedNodes < firstNotFixedNodeIndex) {
#if R5_VERBOSE >= 4
                    cerr << "    early mismatch: could not assign all fixed nodes" << endl;
#endif
                    isUnique = true;
                    continue;
                }

                R5_DEBUG_ASSERT(firstNotFixedNodeIndex == firstNotUniqueDegreeMultiplicityNodeIndex);

                bool match = true;
                for (Size i = firstNotEmptyOrFullNodeIndex; i < firstNotFixedNodeIndex; i += 1) {
                    Size n = traversalOrder[i];
                    for (Size j = 0; j < i; j += 1) {
                        Size m = traversalOrder[j];
                        R5_DEBUG_ASSERT(n != m)
                        R5_DEBUG_ASSERT(permutation[n] != permutation[m])
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
#if R5_VERBOSE >= 4
                    cerr << "    early mismatch: permutation check failed" << endl;
#endif
                    isUnique = true;
                    continue;
                } else if (firstNotFixedNodeIndex == nodes) {
#if R5_VERBOSE >= 4
                    cerr << "    complete early match" << endl;
#endif
                    isUnique = false;
                    break;
                }

                R5_VERBOSE_1(graphCombinations2 += 1);

#if R5_VERBOSE >= 4
                cerr << "    possibly isomorphic" << endl;
#endif

                stack.clear();
                const auto& degreeTuple = gDegrees[traversalOrder[firstNotFixedNodeIndex]];
                auto [ it, itEnd ] = hNodesByDegree.find(degreeTuple);
                for (; it != itEnd; ++it) {
                    Size m = (Size) *it;
                    if (assignedNodes[m] == false) {
                        stack.emplace_back(std::make_tuple(firstNotFixedNodeIndex, m));
                    }
                }

                while (stack.empty() == false) {

#if R5_VERBOSE >= 4
                    cerr << "    stack " << stack << endl;
#endif

                    R5_VERBOSE_1(recursionSteps += 1);

                    auto [ i, m ] = stack.back();
                    Size n = traversalOrder[i];

#if R5_VERBOSE >= 4
                    cerr << "    " << i << " n " << n << endl;
#endif

                    if (assignedNodes[m] == true) {
                        assignedNodes[m] = false;
                        stack.pop_back();
                        continue;
                    }

                    permutation[n] = m;

#if R5_VERBOSE >= 4
                    cerr << "    permutation " << permutation << endl;
#endif

                    R5_VERBOSE_1(permutationChecks += 1);

                    bool match = true;
                    for (Size j = firstNotEmptyOrFullNodeIndex; j < i; j += 1) {
                        Size o = traversalOrder[j];
                        if (g.edge(n, o) != h.edge(m, permutation[o])) {
                            match = false;
                            break;
                        }
                    }

                    if (match == false) {
                        stack.pop_back();
                        continue;
                    } else if (i == nodes-1) {
                        isUnique = false;
#if R5_VERBOSE >= 4
                        cerr << "    isomorphic, permutation" << permutation << endl;
#endif
                        break;
                    } else {

                        assignedNodes[m] = true;

                        const auto& degreeTuple_ = gDegrees[traversalOrder[i+1]];
                        auto [ it_, itEnd_ ] = hNodesByDegree.find(degreeTuple_);
                        for (; it_ != itEnd_; ++it_) {
                            Size m_ = (Size) *it_;
                            if (assignedNodes[m_] == false) {
                                stack.emplace_back(std::make_tuple(i+1, m_));
                            }
                        }
                    }
                }

                if (isUnique == false) {
                    break;
                }
            }
        }

        if (isUnique) {
#if R5_VERBOSE >= 4
            cerr << "  unique g " << g << endl;
#endif
            uniqueGraphs[gProperties].emplace_back(std::make_tuple(g, std::move(gNodesByDegree)));
            uniqueGraphsCount += 1;
        }
    }

#if R5_VERBOSE >= 1
    std::size_t maxSize = 0;
    for (const auto& v : uniqueGraphs) {
        maxSize = std::max(maxSize, v.second.size());
    }

    cerr << "  Average fixed nodes:                     " << std::setw(15 + 4) << std::fixed << double(fixedNodesSum) / double(graphs.size()) << endl;
    cerr << "  Unique properties:                       " << std::setw(15) << uniqueGraphs.size() << endl;
    auto [keysByteSize_, valuesByteSize_] = uniqueGraphsSize(uniqueGraphs);
    float keysByteSize = float(keysByteSize_) / (1024*1024);
    float valuesByteSize = float(valuesByteSize_) / (1024*1024);

    cerr << "  Unique graphs keys byte size             " << std::setw(15 + 4) << std::fixed << keysByteSize << " MiB"<< endl;
    cerr << "  Unique graphs values byte size           " << std::setw(15 + 4) << std::fixed << valuesByteSize << " MiB" << endl;
    cerr << "  Max graphs per property:                 " << std::setw(15) << maxSize << endl;
    cerr << "  Graph combinations checked:              " << std::setw(15) << graphCombinations << endl;
    cerr << "  Graph combinations requiring traversal:  " << std::setw(15) << graphCombinations2 << endl;
    cerr << "  Recursion steps:                         " << std::setw(15) << recursionSteps << endl;
    cerr << "  Permutation checks:                      " << std::setw(15) << permutationChecks << endl;
#endif

    std::vector<AdjacencyMatrix<nodes>> ret;
    ret.reserve(uniqueGraphsCount);
#if R5_VERBOSE >= 2
        cerr << "  Unique graphs:" << endl;
#endif
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


// Note: Partial specialization of function templates is not allowed. Therefore use struct to wrap the function.
template<Size r, Size s, Size n, typename Enable = void>
struct RamseyGraphs {
    static std::vector<AdjacencyMatrix<n>> f(const std::vector<AdjacencyMatrix<n-1>>& smallerRamseyGraphs) {
        R5_STATIC_ASSERT(r >= 1);
        R5_STATIC_ASSERT(s >= 1);

        constexpr auto e = AdjacencyMatrix<n>().edges();

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

            std::array<Size, e + 1> stack;
            Size stackTop = 1;
            stack[0] = AdjacencyMatrix<n-1>().edges(); // set   edge x
            stack[1] = AdjacencyMatrix<n-1>().edges(); // unset edge x

            while (stackTop >= 0) {

                R5_DEBUG_ASSERT(stackTop < Size(stack.size()));

                // cerr << stackTop << stack << endl;

                R5_VERBOSE_1(recursionSteps += 1);
                // TODO the number of recursion steps is about 2x of what it should be
                // Maybe this DFS needs fixing

                Size edge = stack[stackTop];
                stackTop -= 1;

                coloring.toggleEdge(edge);

                // cerr << "  " << coloring << " edge " << edge << endl;

                if (coloring.edge(edge) == true) {

                    if (n >= r) { // avoids matching subgraphs larger than the to-be-checked graph
                        // Compare this graph against all appropriate complete subgraphs.
                        // Appropriate means every graph whos complete subgraph's laste edge is exactly the lastly enumerated edge
                        bool foundCompleteSubgraph = false;
                        const auto& currentEdgeMasksComplete = edgeMasksCompleteByLastOne[edge + 1];
                        R5_VERBOSE_1(if (currentEdgeMasksComplete.size() > 0) { coloringsChecked += 1; });
                        for (std::size_t i = 0; i < currentEdgeMasksComplete.size(); i += 1) {
                            R5_VERBOSE_1(edgeMaskChecks += 1);
                            if ((coloring & currentEdgeMasksComplete[i]) == currentEdgeMasksComplete[i]) { // NOTE: this can be speed up by testing more directly
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
                        R5_VERBOSE_1(if (currentEdgeMasksEmpty.size() > 0) { coloringsChecked += 1; });
                        for (std::size_t i = 0; i < currentEdgeMasksEmpty.size(); i += 1) {
                            R5_VERBOSE_1(edgeMaskChecks += 1);
                            if ((coloring | currentEdgeMasksEmpty[i]) == currentEdgeMasksEmpty[i]) { // NOTE: this can be speed up by testing more directly
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
        float nonUniqueRamseyGraphsByteSize = float(nonUniqueRamseyGraphs.size() * AdjacencyMatrix<n>{}.byteSize()) / (1024*1024);
        cerr << "  Non-unique Ramsey graphs byte size:      " << std::setw(15 + 4) << std::fixed << nonUniqueRamseyGraphsByteSize << " MiB" << endl;
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
        float ramseyGraphsByteSize = float(ramseyGraphs.size() * AdjacencyMatrix<n>{}.byteSize()) / (1024*1024);
        cerr << "  Ramsey graphs byte size:                 " << std::setw(15 + 4) << std::fixed << ramseyGraphsByteSize << " MiB" << endl;
        cerr << "  Total memory usage:                      " << std::setw(15 + 4) << std::fixed << double(r5::memoryUsage()) / (1024*1024) << " MiB" << endl;
#if R5_VERBOSE >= 2
        Size minEdges = std::numeric_limits<Size>::max();
        Size maxEdges = -1;
        for (const auto& g : ramseyGraphs) {
            Size edges = 0;
            for (Size i = 0; i < n; i += 1) {
                for (Size j = 0; j < i; j += 1) {
                    edges += g.edge(i, j);
                }
            }
            minEdges = std::min(edges, minEdges);
            maxEdges = std::max(edges, maxEdges);
        }

        cerr << "  Min, max edge count:                     " << std::setw(15) << (std::to_string(minEdges) + ", " + std::to_string(maxEdges)) << endl;
        cerr << "  Ramsey graphs:                           " << std::setw(15) << ramseyGraphs << endl;
#endif
        cerr << endl;
#endif

        checkRamseyGraphCount(r, s, n, ramseyGraphs.size());

        return ramseyGraphs;
    }
};

template<Size r, Size s>
struct RamseyGraphs<r, s, 1, std::enable_if_t<(r > 1 && s > 1)>> {
    static std::vector<AdjacencyMatrix<1>> f(const std::vector<AdjacencyMatrix<0>>&) { return {AdjacencyMatrix<1>()}; }
};
template<Size r>
struct RamseyGraphs<r, 1, 1> {
    static std::vector<AdjacencyMatrix<1>> f(const std::vector<AdjacencyMatrix<0>>&) { return {}; }
};
template<Size s>
struct RamseyGraphs<1, s, 1, std::enable_if_t<(s > 1)>> {
    static std::vector<AdjacencyMatrix<1>> f(const std::vector<AdjacencyMatrix<0>>&) { return {}; }
};

template<Size r, Size s, Size n>
std::vector<AdjacencyMatrix<n>> ramseyGraphs() {

    auto ramseyGraphs2 = []<Size... NodesMinusOne>(std::integer_sequence<Size, NodesMinusOne...> /*nodeCountSequence*/) {
        auto allGraphs = std::make_tuple(std::vector<AdjacencyMatrix<0>>{}, std::vector<AdjacencyMatrix<NodesMinusOne+1>>{} ...);

        ([&]{
            auto smallerGraphs = std::get<NodesMinusOne>(allGraphs);
            std::get<NodesMinusOne+1>(allGraphs) = RamseyGraphs<r, s, NodesMinusOne+1>::f(smallerGraphs);
        }(), ...);

        return std::get<n>(allGraphs);
    };

    return ramseyGraphs2(std::make_integer_sequence<Size, n>{});
}
