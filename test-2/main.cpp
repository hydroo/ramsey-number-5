#include "prereqs.hpp"

// --- Static Setup ----------------------------------------------------------
#ifndef T2_COLOR0
#error "T2_COLOR0 not defined"
#endif

#ifndef T2_NODES
#error "T2_NODES not defined"
#endif

constexpr int color0SubgraphSize = T2_COLOR0;
constexpr int color1SubgraphSize = T2_COLOR1;
constexpr int nodes = T2_NODES;

static_assert(nodes >= std::min(color0SubgraphSize, color0SubgraphSize));

// ---------------------------------------------------------------------------
using EdgeColor = int;

constexpr int colors = 2;
constexpr int edges = nodes * (nodes - 1) / 2;
// constexpr int colorings = pow(colors, edges); // too big for basic datatypes

void test2();

int main(int argc, char** args) {
    (void)argc;
    (void)args;

    test2();

    return 0;
}

void test2() {
    std::cerr << "problem:   R(" << color0SubgraphSize << ", " << color1SubgraphSize << ") <= " << nodes << " ?" << std::endl;

    std::cerr << "nodes:     " << nodes << std::endl;
    std::cerr << "edges:     " << edges << "                  # = nodes(nodes-1)/2" << std::endl;
    // std::cerr << "colorings: " << colorings << "                  # = colors^edges" << std::endl; // too big for basic datatypes

    std::bitset<edges> coloring;  // all zero by default

    auto nextColoring = [&coloring]() -> bool {
        for (auto i = 0; i < edges; i += 1) {
            coloring.flip(i);
            if (coloring.test(i) == true) {
                return true;
            }
        }
        return false;
    };


    auto hasCompleteSubGraph = [&coloring]() {

        //int n = nodes;
        //int k = subGraphSize;

        //std::vector<int> mask(k, 1);
        //mask.resize(n, 0);

        //// std::vector<int> indices(subGraphSize);

        //do {
        //    // for (int i = 0, j = 0; i < mask.size(); i += 1) {
        //    //    if (mask[i] == 1) {
        //    //        //indices[j] = i;
        //    //        j += 1;
        //    //    }
        //    //}

        //    // std::cerr << "  indices: "<< indices << std::endl;

        //    bool complete = true;

        //    for (std::vector<EdgeColor>::size_type i = 0; i < coloring.size(); i += 1) {
        //        int j = i;
        //        int y = 0;
        //        int w = nodes - 1;
        //        while (j >= w && w > 0) {
        //            j -= w;
        //            w -= 1;
        //            y += 1;
        //        }
        //        int x = j + 1 + y;

        //        // if (mask[x] == 1 && mask[y] == 1) {
        //        //     std::cerr << "    index: " << i << " : " << x << ", " << y << " color: " << coloring[i] << std::endl;
        //        // }

        //        if (mask[x] == 1 && mask[y] == 1) {
        //            if (coloring[i] != color) {
        //                complete = false;
        //            }
        //        }
        //    }

        //    if (complete == true) {
        //        return true;
        //    }

        //} while (std::prev_permutation(mask.begin(), mask.end()));

        //return false;

        return true;
    };


    bool allColoringsHaveCompleteColor0OrColor1Subgraphs = true;

    do {
        //printAdjacencyMatrix(std::cerr, coloring, nodes, "   " );
        std::cerr << coloring << std::endl;

        allColoringsHaveCompleteColor0OrColor1Subgraphs = hasCompleteSubGraph();

        if (allColoringsHaveCompleteColor0OrColor1Subgraphs == false) {
            break;
        }

    } while (nextColoring());


    if (allColoringsHaveCompleteColor0OrColor1Subgraphs) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}
