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
    auto nodePermutations = []() -> std::array<std::array<s64, nodes>, factorial(nodes)> {

        std::array<std::array<s64, nodes>, factorial(nodes)> ret;

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

    auto previous = uniqueAdjacencyMatrices<nodes-1>();

    std::set<r5::AdjacencyMatrix<nodes>> uniqueGraphs;

    // s64 compares = 0;
    // s64 edgeSets = 0;

    for (const auto& p : previous) {

        r5::AdjacencyMatrix<nodes> m;
        m.unsetAllEdges();

        for (s64 e = 0; e < p.edges(); e += 1) {
            if (p.edge(e) == true) {
                m.setEdge(e);
            } else {
                m.unsetEdge(e);
            }
        }

        std::array<s64, m.edges() + 1> stack;
        s64 stackTop = 1;
        stack[0] = p.edges();
        stack[1] = p.edges();

        while (stackTop >= 0) {

            R5_DEBUG_ASSERT(stackTop < (s64) stack.size());

            s64 edge = stack[stackTop];
            stackTop -= 1;

            m.toggleEdge(edge);

            r5::AdjacencyMatrix<nodes> smallest = m;

            // std::cerr << m << std::endl;

            for (const auto& np : nodePermutations) {

                r5::AdjacencyMatrix<nodes> mp;

                for (s64 c = 1; c < m.nodes(); c += 1) {
                    for (s64 r = 0; r < c; r += 1) {
                        // edgeSets += 1;

                        if (m.edge(c, r) == true) {
                            mp.setEdge(np[c], np[r]);
                        } else {
                            mp.unsetEdge(np[c], np[r]);
                        }
                    }
                }

                // std::cerr << "  " << mp << std::endl;

                // compares += 1;

                smallest = std::min(smallest, mp);
            }

            uniqueGraphs.insert(smallest); // deduplicates

            if (edge < m.edges() - 1) {
                stack[stackTop+1] = edge + 1; // set   edge + 1
                stack[stackTop+2] = edge + 1; // unset edge + 1
                stackTop += 2;
            }
        }
    }

    // std::cerr << __func__ << " n: " << nodes << ", compares: " << std::setw(15) << compares << ", edge sets: " << std::setw(15) << edgeSets << std::endl;

    std::vector<r5::AdjacencyMatrix<nodes>> ret;
    for (const auto& g : uniqueGraphs) {
        ret.push_back(g);
    }

    return ret;
}

#endif // UNIQUE_HPP
