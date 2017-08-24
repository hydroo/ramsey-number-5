#include "prereqs.hpp"

void test1(int ramseyNumber0, int ramseyNumber1);

int main(int argc, char** args) {
    if (argc < 2 || argc > 3) {
        std::cerr << " Usage: %s <subgraph-size-0> <subgraph-size-1>" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    int ramseyNumber0 = std::stoi(args[1]);  // throws std::invalid_argument, std::out_of_range
    int ramseyNumber1;

    if (argc > 2) {
        ramseyNumber1 = std::stoi(args[2]);  // throws std::invalid_argument, std::out_of_range
    } else {
        ramseyNumber1 = ramseyNumber0;
    }

    test1(ramseyNumber0, ramseyNumber1);

    return 0;
}

using EdgeColor = int;

bool hasCompleteSubgraph(int subGraphSize, EdgeColor color, int nodes, const std::vector<EdgeColor>& coloring) {
    // std::cerr << coloring << std::endl;

    ASSERT(nodes * (nodes - 1) / 2 == (int)coloring.size());

    if (subGraphSize > nodes) {
        return false;
    }

    int n = nodes;
    int k = subGraphSize;

    std::vector<int> mask(k, 1);
    mask.resize(n, 0);

    // std::vector<int> indices(subGraphSize);

    do {
        // for (int i = 0, j = 0; i < mask.size(); i += 1) {
        //    if (mask[i] == 1) {
        //        //indices[j] = i;
        //        j += 1;
        //    }
        //}

        // std::cerr << "  indices: "<< indices << std::endl;

        bool complete = true;

        for (std::vector<EdgeColor>::size_type i = 0; i < coloring.size(); i += 1) {
            int j = i;
            int y = 0;
            int w = nodes - 1;
            while (j >= w && w > 0) {
                j -= w;
                w -= 1;
                y += 1;
            }
            int x = j + 1 + y;

            // if (mask[x] == 1 && mask[y] == 1) {
            //     std::cerr << "    index: " << i << " : " << x << ", " << y << " color: " << coloring[i] << std::endl;
            // }

            if (mask[x] == 1 && mask[y] == 1) {
                if (coloring[i] != color) {
                    complete = false;
                }
            }
        }

        if (complete == true) {
            return true;
        }

    } while (std::prev_permutation(mask.begin(), mask.end()));

    return false;
}

// The Ramsey number R(r,s) is the Smallest number of nodes n, such that
//   every 2-coloring of a graph of size n
//     has a complete subgraph of size r of color 0, or
//     a complete subgraph of size s of color 1
void test1(int ramseyNumber0, int ramseyNumber1) {
    int nodes = std::max(ramseyNumber0, ramseyNumber1);

    bool exitCondition = false;

    while (exitCondition == false) {
        int edges = nodes * (nodes - 1) / 2;
        int colorings = std::pow(2, edges);

        // std::cerr << edges << std::endl;

        std::vector<EdgeColor> coloring(edges, 0);

        exitCondition = true;

        for (int c = 0; c < colorings; c += 1) {
            bool onlyEmptyOrOnlyComplete = hasCompleteSubgraph(ramseyNumber0, 0, nodes, coloring) ||
                                           hasCompleteSubgraph(ramseyNumber1, 1, nodes, coloring);
            exitCondition &= onlyEmptyOrOnlyComplete;
            if (onlyEmptyOrOnlyComplete == false) {
                std::cerr << "nodes: " << nodes << ", subGraphSize (" << ramseyNumber0 << ", " << ramseyNumber1
                          << "), Coloring: " << coloring << " has no complete 0/1 subgraph" << std::endl;
                break;
            }

            for (int edge = edges - 1; edge >= 0; edge -= 1) {
                if (coloring[edge] == 0) {
                    coloring[edge] = 1;
                    for (int i = edge + 1; i < edges; i += 1) {
                        coloring[i] = 0;
                    }
                    break;
                }
            }
        }

        nodes += 1;
    }

    std::cout << "R(" << ramseyNumber0 << ", " << ramseyNumber1 << ") = " << nodes - 1 << std::endl;
}
