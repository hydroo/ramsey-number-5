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
template<s64 nodes>
std::vector<r5::AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices();

template<>
std::vector<r5::AdjacencyMatrix<1>> uniqueAdjacencyMatrices() {
    r5::AdjacencyMatrix<1> m;
    m.unsetAllEdges();
    return std::vector<r5::AdjacencyMatrix<1>>{m};
}

template<s64 nodes>
std::vector<r5::AdjacencyMatrix<nodes>> uniqueAdjacencyMatrices() {

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
    auto smallerUniqueMatrices = uniqueAdjacencyMatrices<nodes-1>();

    std::vector<r5::AdjacencyMatrix<nodes>> mp(nodePermutationCount); // permuted matrices, one per nodePermutation

#if R5_VERBOSE >= 1
    std::cerr << "uniqueAdjacencyMatrices<" << nodes << ">:" << std::endl;
    std::cerr << "  Size of AdjacencyMatrix<nodes>            " << sizeof(mp[0]) << " bytes" << std::endl;
    std::cerr << "  Node permutations (nodes!)                " << nodePermutationCount << std::endl;
    std::cerr << "  Size of mp                                " << nodePermutationCount*sizeof(mp[0]) << " bytes"<< std::endl;
    std::cerr << "  Smaller unique matrices                   " << smallerUniqueMatrices.size() << std::endl;
    std::cerr << "  Edges per smaller unique matrix           " << (nodes-1)*(nodes-2)/2 << std::endl;
    std::cerr << "  Total iterations to set all initial edges " << smallerUniqueMatrices.size() * nodePermutationCount * (nodes-1)*(nodes-2)/2 << std::endl;
    std::cerr << "  Total recursion steps                     " << smallerUniqueMatrices.size() << " * 2^" << column << " = " << smallerUniqueMatrices.size() * (s64) std::pow(2, column) << std::endl;
    std::cerr << std::endl;
#endif

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

    std::vector<r5::AdjacencyMatrix<nodes>> ret(uniqueGraphs.size());
    // reverse order, because it is sorted like 111 11 1 in the set
    auto i = ret.size()-1;
    for (const auto& g : uniqueGraphs) {
        ret[i] = g;
        i -= 1;
    }

    return ret;
}

#endif // UNIQUE_HPP
