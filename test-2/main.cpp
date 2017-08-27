#include "prereqs.hpp"

// --- Static Setup ----------------------------------------------------------
#include "config.hpp"

static_assert(nodes >= std::max(color0SubgraphSize, color1SubgraphSize)); // code breaks if I allow smaller ones. Don't want to fix right now

// ---------------------------------------------------------------------------
using EdgeColor = int;

constexpr int colors = 2;
constexpr int edges = nodes * (nodes - 1) / 2;

template<int subGraphSize>
std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)> subGraphEdgeMasks() {

    std::array<std::bitset<edges>, nChooseK(nodes, subGraphSize)> edgeMasks;

    bool nodeMask[nodes]; // plain array because previousPermutation in prereqs.hpp is like that for constexpr compatibility

    for (int i = 0; i < subGraphSize; i += 1) {
        nodeMask[i] = true;
    }
    for (int i = subGraphSize; i < nodes; i += 1) {
        nodeMask[i] = false;
    }

    int p = 0;

    do {

        for (int e = 0; e < edges; e += 1) {
            int j = e;
            int y = 0;
            int w = nodes - 1;
            while (j >= w && w > 0) {
                j -= w;
                w -= 1;
                y += 1;
            }
            int x = j + 1 + y;

            //std::cerr << "  e " << e << " x " << x << " y " << y << std::endl;

            if (nodeMask[x] == true && nodeMask[y] == true) {
                edgeMasks[p][e] = true;
            } else {
                edgeMasks[p][e] = false;
            }
        }

        //std::cerr << arrayToString(nodeMask, nodes) << " \"" << std::string(b) << "\"" << std::endl;

        p += 1;

    } while (previousPermutation(nodeMask, nodes));

    return edgeMasks;
}


void test2();

void checkResult(bool result);

int main(int argc, char** args) {
    (void)argc;
    (void)args;

    test2();

    return 0;
}


void test2() {
    std::cerr << "problem:                     R(" << color0SubgraphSize << ", " << color1SubgraphSize << ") <= " << nodes << " ?" << std::endl;

    std::cerr << "color 0 subgraph size:       " << color0SubgraphSize << std::endl;
    std::cerr << "color 1 subgraph size:       " << color1SubgraphSize << std::endl;
    std::cerr << "number of color 0 subgraphs: " << nChooseK(nodes, color0SubgraphSize) << std::endl;
    std::cerr << "number of color 1 subgraphs: " << nChooseK(nodes, color1SubgraphSize) << std::endl;
    std::cerr << "nodes:                       " << nodes << std::endl;
    std::cerr << "edges:                       " << edges << "                  # = nodes(nodes-1)/2" << std::endl;
    std::cerr << "edge colorings:              " << constPow<int64_t>(2, edges) << "                  # = 2^edges" << std::endl;
    // std::cerr << "colorings: " << colorings << "                  # = colors^edges" << std::endl; // too big for basic datatypes

    std::bitset<edges> coloring;  // all zero by default

    auto masks0 = subGraphEdgeMasks<color0SubgraphSize>();
    auto masks1 = subGraphEdgeMasks<color1SubgraphSize>();

    //std::cerr << masks0 << masks1 << std::endl;

    auto nextColoring = [&coloring]() -> bool {
        for (auto i = 0; i < edges; i += 1) {
            coloring.flip(i);
            if (coloring.test(i) == true) {
                return true;
            }
        }
        return false;
    };


    auto hasCompleteOrEmptySubgraph = [&coloring, masks0, masks1]() {

        std::bitset<edges> tmp;

        for (auto mask0 : masks0) {
            tmp = ~coloring & mask0;
            if (tmp == mask0) {
                return true;
            }
        }

        for (auto mask1 : masks1) {
            tmp = coloring & mask1;
            if (tmp == mask1) {
                return true;
            }
        }

        return false;
    };


    bool allColoringsHaveCompleteOrEmptySubgraph = true;

    do {
        //printAdjacencyMatrix(std::cerr, coloring, nodes, "   " );
        //std::cerr << coloring << std::endl;

        allColoringsHaveCompleteOrEmptySubgraph = hasCompleteOrEmptySubgraph();

        if (allColoringsHaveCompleteOrEmptySubgraph == false) {
            break;
        }

    } while (nextColoring());


    if (allColoringsHaveCompleteOrEmptySubgraph) {
        std::cout << "R(" << color0SubgraphSize << ", " << color1SubgraphSize << ") <= " << nodes << std::endl;
    } else {
        std::cout << "R(" << color0SubgraphSize << ", " << color1SubgraphSize << ") >  " << nodes << std::endl;
    }

    checkResult(allColoringsHaveCompleteOrEmptySubgraph);
}

void checkResult(bool result) {

    std::vector<std::vector<int>> truths{{1, 1,  1},
                                         {1, 2,  1},
                                         {1, 3,  1},
                                         {1, 4,  1},
                                         {1, 5,  1},
                                         {2, 1,  1},
                                         {2, 2,  2},
                                         {2, 3,  3},
                                         {2, 4,  4},
                                         {2, 5,  5},
                                         {3, 1,  1},
                                         {3, 2,  3},
                                         {3, 3,  6},
                                         {3, 4,  9},
                                         {3, 5, 14},
                                         {4, 1,  1},
                                         {4, 2,  2},
                                         {4, 3,  9},
                                         {4, 4, 18},
                                         {4, 5, 25},
                                         {5, 1,  1},
                                         {5, 2,  2},
                                         {5, 3, 14},
                                         {5, 4, 25},
                                         //{5, 5, 43-48}
                                        };

    for (auto truth : truths) {
        if (color0SubgraphSize == truth[0] && color1SubgraphSize == truth[1]) {
            if (nodes < truth[2]) {
                if (result == true) {
                    std::cerr << "wrong result" << std::endl;
                    std::abort();
                }
            } else if (nodes >= truth[2]) {
                if (result == false) {
                    std::cerr << "wrong result" << std::endl;
                    std::abort();
                }
            }
        }
    }
}
