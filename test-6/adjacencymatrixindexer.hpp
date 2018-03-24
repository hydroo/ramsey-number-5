#ifndef ADJACENCYMATRIXINDEXER_HPP
#define ADJACENCYMATRIXINDEXER_HPP

#include "prereqs.hpp"

#include <cmath>

#include <utility>

template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrixIndexer {
public:
    static constexpr s64 index(s64 row, s64 column) {
        if(Triangular == true) {
            if (row == column) {
                return -1;
            } else {
                if (row > column) {
                    auto t = row;
                    row = column;
                    column = t;
                }
                return (Nodes*(Nodes-1)/2) - (Nodes-row)*((Nodes-row)-1)/2 + column - row - 1;
            }
        } else {
            return row*Nodes + column;
        }
    }

    static constexpr s64 indexChecked(s64 row, s64 column) {
        ASSERT(row    >= 0);
        ASSERT(row    <= Nodes-1);
        ASSERT(column >= 0);
        ASSERT(column <= Nodes-1);
        return index(row, column);
    }

    static constexpr std::pair<s64, s64> reverse(s64 i) {
        if (Triangular == true) {
            s64 row = Nodes - 2 - std::floor(std::sqrt(-8*i + 4*Nodes*(Nodes-1)-7)/2.0 - 0.5);
            s64 column = i + row + 1 - Nodes*(Nodes-1)/2 + (Nodes-row)*((Nodes-row)-1)/2;
            return std::make_pair(row, column);
        } else {
            s64 column = i % Nodes;
            s64 row = (i - column) / Nodes;
            return std::make_pair(row, column);
        }
    }

    static constexpr std::pair<s64, s64> reverseChecked(s64 i) {
        if (Triangular == true) {
            ASSERT(i >= 0);
            ASSERT(i <= Nodes*(Nodes-1)/2);
        } else {
            ASSERT(i >= 0);
            ASSERT(i <= Nodes*Nodes-1);
        }
        return reverse(i);
    }

    AdjacencyMatrixIndexer()                              = delete;
    AdjacencyMatrixIndexer(const AdjacencyMatrixIndexer&) = delete;
    AdjacencyMatrixIndexer(AdjacencyMatrixIndexer&&)      = delete;
};

#endif // ADJACENCYMATRIXINDEXER_HPP
