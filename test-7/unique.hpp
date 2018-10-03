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

    constexpr s64 edges = nodes*(nodes-1)/2;
    constexpr s64 nodePermutationCount = factorial(nodes);

    // create all permutations and store them as maps from edges to edges
    auto reverseEdgePermutations = []() -> std::vector<std::array<s64, edges>> {

        std::vector<std::array<s64, edges>> ret(nodePermutationCount);
        using AmIndexer = r5::AdjacencyMatrixIndexer<nodes>;

        std::array<s64, nodes> permutation;
        for (std::size_t n = 0; n < nodes; n += 1) {
            permutation[n] = n;
        }

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

    std::set<r5::AdjacencyMatrix<nodes>> uniqueGraphs;

    constexpr s64 column = nodes-1;

    // Take the unique adjacency matrices of size nodes-1
    auto smallerUniqueMatrices = uniqueAdjacencyMatrices<nodes-1>();

#if R5_VERBOSE >= 1
    std::cerr << "uniqueAdjacencyMatrices<" << nodes << ">:" << std::endl;
    std::cerr << "  Size of AdjacencyMatrix<nodes>            " << sizeof(r5::AdjacencyMatrix<nodes>) << " bytes" << std::endl;
    std::cerr << "  Node permutations (nodes!)                " << nodePermutationCount << std::endl;
    std::cerr << "  Smaller unique matrices                   " << smallerUniqueMatrices.size() << std::endl;
    const s64 totalRecursionSteps = smallerUniqueMatrices.size() * (s64) std::pow(2, column);
    std::cerr << "  Total recursion steps                     " << smallerUniqueMatrices.size() << " * 2^" << column << " = " << totalRecursionSteps << std::endl;
    const s64 maximumGraphChecks = totalRecursionSteps * nodePermutationCount;
    std::cerr << "  Maximum graph checks                      " << smallerUniqueMatrices.size() << " * 2^" << column << " * " << nodePermutationCount << " = " << maximumGraphChecks << std::endl;

    s64 graphChecks = 0;
#endif

    // For each unique adjacency matrix of size nodes-1
    for (const auto& p : smallerUniqueMatrices) {

        r5::AdjacencyMatrix<nodes> m(p);

        // Start the DFS to enumerate all the remaining edges.
        // The remaining edges are in the last column 'column' and in rows 0 to column-1
        std::array<s64, nodes> stack;
        s64 stackTop = 1;
        stack[0] = r5::AdjacencyMatrixIndexer<nodes>::index(column, 0); //   set first edge
        stack[1] = r5::AdjacencyMatrixIndexer<nodes>::index(column, 0); // unset first edge

        while (stackTop >= 0) {

            R5_DEBUG_ASSERT(stackTop < (s64) stack.size());

            s64 edge = stack[stackTop];
            stackTop -= 1;

            m.toggleEdge(edge);

            if (edge < edges-1) { // still enumerating

                stack[stackTop+1] = edge + 1;
                stack[stackTop+2] = edge + 1;
                stackTop += 2;

            } else { // DFS leaf

                bool isCanonical = true;

                for (const auto& permutation : reverseEdgePermutations) {

                    for (s64 e = 0; e < edges; e += 1) {

                        const bool e1 = m.edge(permutation[e]);
                        const bool e2 = m.edge(e);

                        if (e1 != e2) {
                            isCanonical = e1 < e2;
                            break;
                        }
                    }

#if R5_VERBOSE >= 1
                    graphChecks += 1;
#endif

                    if (isCanonical == false) {
                        break;
                    }
                }

                if (isCanonical == true) {
                    uniqueGraphs.insert(m);
                }
            }
        }
    }

#if R5_VERBOSE >= 1
    R5_ASSERT(graphChecks >= totalRecursionSteps);
    R5_ASSERT(graphChecks <= maximumGraphChecks);
    std::cerr << "  Actual graph checks                       " << graphChecks << std::endl;
    std::cerr << std::endl;
#endif

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
