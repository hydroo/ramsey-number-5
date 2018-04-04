#ifndef TEST_5_CHECK_HPP
#define TEST_5_CHECK_HPP

#include "prereqs.hpp"
#include "test-5-prereqs.hpp"

#include "adjacencymatrix.hpp"

s64 expectedResult(s64 r, s64 s);

template<s64 Nodes, bool Triangular = false>
bool hasCompleteOrEmptySubgraph(const r5::AdjacencyMatrix<Nodes, Triangular>& m, s64 completeSubgraphNodeCount, s64 emptySubgraphNodeCount) {

    s64 r = completeSubgraphNodeCount;
    s64 s = emptySubgraphNodeCount;

    auto hasSubGraph = [](const r5::AdjacencyMatrix<Nodes, Triangular>& m, s64 subgraphNodeCount, bool complete) -> bool {

        std::vector<bool> nodeMask(m.nodes());

        std::fill(std::begin(nodeMask), std::end(nodeMask), true);
        std::fill(nodeMask.begin() + subgraphNodeCount, std::end(nodeMask), false);

        do {
            std::vector<s64> indices;
            for (std::size_t i = 0; i < nodeMask.size(); i += 1) {
                if (nodeMask[i] == true) {
                    indices.push_back(i);
                }
            }

            bool has = true;
            for (std::size_t i = 0; i < indices.size(); i += 1) {
                for (std::size_t j = i + 1; j < indices.size(); j += 1) {
                    has &= complete == true ? m.edge((s64)indices[i], (s64)indices[j]) == true : m.edge((s64)indices[i], (s64)indices[j]) == false;
                }
            }

            if (has == true) {
                return true;
            }

        } while (std::prev_permutation(std::begin(nodeMask), std::end(nodeMask)));

        return false;
    };

    bool hasCompleteSubgraph;
    if (r <= m.nodes()) { hasCompleteSubgraph = hasSubGraph(m, r, true); }
    else                { hasCompleteSubgraph = false; }

    bool hasEmptySubgraph;
    if (s <= m.nodes()) { hasEmptySubgraph = hasSubGraph(m, s, false); }
    else                { hasEmptySubgraph = false; }

    return hasCompleteSubgraph | hasEmptySubgraph;
}

void test_hasCompleteOrEmptySubgraph();

#endif // TEST_5_CHECK_HPP
