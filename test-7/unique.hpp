#ifndef UNIQUE_HPP
#define UNIQUE_HPP

#include "prereqs.hpp"

#include <set>

#include "adjacencymatrix.hpp"

constexpr s64 factorial(s64 n) {
    if (n == 0) { return 1; }
    else        { return factorial(n-1) * n; }
}

// Generates all unique (up to isomorphism) adjacency matrices of size `nodes`
template<s64 nodes, s64 nodes2 = 0, typename = std::enable_if_t<nodes == 1>>
std::vector<r5::AdjacencyMatrix<1>> uniqueAdjacencyMatrices(
        const std::vector<r5::AdjacencyMatrix<nodes2>>& edgeMasksComplete = std::vector<r5::AdjacencyMatrix<nodes2>>(),
        const std::vector<r5::AdjacencyMatrix<nodes2>>& edgeMasksEmpty    = std::vector<r5::AdjacencyMatrix<nodes2>>()) {

    if ((edgeMasksComplete.size() > 0 || edgeMasksEmpty.size() > 0) && (nodes2 == 0 || nodes2 == 1)) {
        return std::vector<r5::AdjacencyMatrix<1>>{};
    } else {
        return std::vector<r5::AdjacencyMatrix<1>>{r5::AdjacencyMatrix<1>()};
    }
}

template<s64 nodes, s64 nodes2 = 0, typename = std::enable_if_t<(nodes > 1)>>
std::vector<r5::AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices(
        const std::vector<r5::AdjacencyMatrix<nodes2>>& edgeMasksComplete = std::vector<r5::AdjacencyMatrix<nodes2>>(),
        const std::vector<r5::AdjacencyMatrix<nodes2>>& edgeMasksEmpty    = std::vector<r5::AdjacencyMatrix<nodes2>>()) {

    constexpr s64 nodePermutationCount = factorial(nodes);

    // create all node permutations
    auto nodePermutations = []() -> std::vector<std::array<s64, nodes>> {

        std::vector<std::array<s64, nodes>> ret(nodePermutationCount);

        std::array<s64, nodes> nodeList;
        for (std::size_t n = 0; n < nodeList.size(); n += 1) {
            nodeList[n] = n;
        }

        s64 p = 0;
        do {
            ret[p] = nodeList;
            p += 1;
        } while (std::next_permutation(std::begin(nodeList), std::end(nodeList)));

        return ret;
    }();

    // Cache column, row to edge indexing
    // Without this edge(c, r) and setEdge(c, r) would be substantially slower
    constexpr auto indexer = []() -> std::array<std::array<s64, nodes>, nodes> {
        std::array<std::array<s64, nodes>, nodes> ret{};
        using AmIndexer = r5::AdjacencyMatrixIndexer<nodes>;
        for (s64 c = 0; c < nodes; c += 1) {
            for (s64 r = 0; r < nodes; r += 1) {
                ret[c][r] = AmIndexer::index(c, r);
            }
        }
        return ret;
    }();

    std::set<r5::AdjacencyMatrix<nodes>> uniqueGraphs;

    constexpr s64 column = nodes-1;

    // Take the unique adjacency matrices of size nodes-1
    auto smallerUniqueMatrices = uniqueAdjacencyMatrices<nodes-1, nodes2>();

    std::vector<r5::AdjacencyMatrix<nodes>> mp(nodePermutationCount); // permuted matrices, one per nodePermutation

    // For each unique adjacency matrix of size nodes-1
    for (const auto& p : smallerUniqueMatrices) {

        r5::AdjacencyMatrix<nodes> m(p);
        std::fill(std::begin(mp), std::end(mp), r5::AdjacencyMatrix<nodes>());

        // apply the node permutations for the smaller unique graphs (size nodes-1)
        for (std::size_t i = 0; i < nodePermutationCount; i += 1) {
            const auto& np = nodePermutations[i];
            for (s64 c = 1; c < p.nodes(); c += 1) {
                for (s64 r = 0; r < c; r += 1) {
                    if (p.edge(c*(c-1)/2 + r) == true) {
                        mp[i].setEdge(indexer[np[c]][np[r]]);
                    }
                }
            }
        }

        // Start the DFS to enumerate all the remaining edges.
        // The remaining edges are in the last column 'column' and in rows 0 to column-1
        std::array<s64, column + 1> stack;
        s64 stackTop = 1;
        stack[0] = 0; //   set first edge
        stack[1] = 0; // unset first edge

        while (stackTop >= 0) {

            R5_DEBUG_ASSERT(stackTop < (s64) stack.size());

            s64 row = stack[stackTop];
            stackTop -= 1;

            m.toggleEdge(indexer[column][row]);
            for (std::size_t i = 0; i < nodePermutationCount; i += 1) {
                const auto& np = nodePermutations[i];
                mp[i].toggleEdge(indexer[np[column]][np[row]]);
            }

            if (row < column - 1) { // still enumerating

                stack[stackTop+1] = row + 1;
                stack[stackTop+2] = row + 1;
                stackTop += 2;

            } else { // DFS leaf

                bool isCanonical = true;
                for (s64 i = 0; i < nodePermutationCount; i += 1) {
                    if (mp[i] < m) {
                        isCanonical = false;
                        break;
                    }
                }

                if (isCanonical) {
                    uniqueGraphs.insert(m);
                }
            }
        }
    }

    // std::cerr << uniqueGraphs << " nodes " << nodes << ", nodes2 " << nodes2 << std::endl;

    constexpr s64 nodesMax = std::max(nodes, nodes2);

    std::vector<r5::AdjacencyMatrix<nodesMax>> edgeMasksComplete2(edgeMasksComplete.size());
    std::vector<r5::AdjacencyMatrix<nodesMax>> edgeMasksEmpty2   (edgeMasksEmpty.size());

    for (std::size_t i = 0; i < edgeMasksComplete.size(); i += 1) {
        edgeMasksComplete2[i] = edgeMasksComplete[i];
    }
    for (std::size_t i = 0; i < edgeMasksEmpty.size(); i += 1) {
        edgeMasksEmpty2   [i] = edgeMasksEmpty   [i];
    }

    std::vector<r5::AdjacencyMatrix<nodes>> ret;
    for (auto i = uniqueGraphs.crbegin(); i != uniqueGraphs.crend(); ++i) {

        r5::AdjacencyMatrix<nodesMax> gc(*i);
        r5::AdjacencyMatrix<nodesMax> ge(*i);

        for (s64 j = i->edges(); j < ge.edges(); j += 1) {
            ge.setEdge(j);
        }

        bool foundEdgeMaskMatch = false;

        for (std::size_t j = 0; j < edgeMasksComplete2.size(); j += 1) {
            if ((gc & edgeMasksComplete2[j]) == edgeMasksComplete2[j]) {
                // std::cerr << "  & g " << g << "gc " << gc << ", m "<< e << std::endl;
                foundEdgeMaskMatch = true;
                break;
            }
        }

        if (foundEdgeMaskMatch == false) {
            for (std::size_t j = 0; j < edgeMasksEmpty2.size(); j += 1) {
                if ((ge | edgeMasksEmpty2[j]) == edgeMasksEmpty2[j]) {
                    // std::cerr << "  | g " << g << ", ge " << ge << ", m "<< e << std::endl;
                    foundEdgeMaskMatch = true;
                    break;
                }
            }
        }

        if (foundEdgeMaskMatch == false) {
            ret.push_back(*i);
        }
    }

    return ret;
}

#endif // UNIQUE_HPP
