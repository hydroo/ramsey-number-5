#ifndef ADJACENCYMATRIXINDEXER_HPP
#define ADJACENCYMATRIXINDEXER_HPP

#include "prereqs.hpp"

#include <cmath>

#include <utility>

template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrixIndexer {
public:
    template<s64 Nodes2 = Nodes>
    static constexpr s64 index(s64 column, s64 row, typename std::enable_if<Nodes2 >= 0>::type* = 0) {
        if(Triangular == true) {
            if (column == row) {
                return -1;
            } else {
                if (column < row) {
                    auto t = column;
                    column = row;
                    row = t;
                }
                return column*(column-1)/2 + row;
            }
        } else {
            return column*Nodes + row;
        }
    }

    template<s64 Nodes2 = Nodes>
    static constexpr s64 indexChecked(s64 row, s64 column, typename std::enable_if<Nodes2 >= 0>::type* = 0) {
        ASSERT(row    >= 0);
        ASSERT(row    <= Nodes-1);
        ASSERT(column >= 0);
        ASSERT(column <= Nodes-1);
        return index(row, column);
    }

    template<s64 Nodes2 = Nodes>
    static constexpr std::pair<s64, s64> reverse(s64 i, typename std::enable_if<Nodes2 >= 0>::type* = 0) {
        if (Triangular == true) {
            s64 column = (s64) floor(0.5 + sqrt(0.25 + 2*i));
            s64 row    = i - column*(column-1)/2;
            return std::make_pair(column, row);
        } else {
            s64 row    = i % Nodes;
            s64 column = (i - row) / Nodes;
            return std::make_pair(column, row);
        }
    }

    template<s64 Nodes2 = Nodes>
    static constexpr std::pair<s64, s64> reverseChecked(s64 i, typename std::enable_if<Nodes2 >= 0>::type* = 0) {
        if (Triangular == true) {
            ASSERT(i >= 0);
            ASSERT(i <= Nodes*(Nodes-1)/2);
        } else {
            ASSERT(i >= 0);
            ASSERT(i <= Nodes*Nodes-1);
        }
        return reverse(i);
    }

    template<s64 Nodes2 = Nodes>
    static s64 index(s64 column, s64 row, s64 nodes, typename std::enable_if<Nodes2 < 0>::type* = 0) {
        if(Triangular == true) {
            if (column == row) {
                return -1;
            } else {
                if (column < row) {
                    auto t = column;
                    column = row;
                    row = t;
                }
                return column*(column-1)/2 + row;
            }
        } else {
            return column*nodes + row;
        }
    }

    template<s64 Nodes2 = Nodes>
    static s64 indexChecked(s64 row, s64 column, s64 nodes, typename std::enable_if<Nodes2 < 0>::type* = 0) {
        ASSERT(row    >= 0);
        ASSERT(row    <= nodes-1);
        ASSERT(column >= 0);
        ASSERT(column <= nodes-1);
        return index(row, column, nodes);
    }

    template<s64 Nodes2 = Nodes>
    static std::pair<s64, s64> reverse(s64 i, s64 nodes, typename std::enable_if<Nodes2 < 0>::type* = 0) {
        if (Triangular == true) {
            s64 column = (s64) floor(0.5 + sqrt(0.25 + 2*i));
            s64 row    = i - column*(column-1)/2;
            return std::make_pair(column, row);
        } else {
            s64 row    = i % nodes;
            s64 column = (i - row) / nodes;
            return std::make_pair(column, row);
        }
    }

    template<s64 Nodes2 = Nodes>
    static std::pair<s64, s64> reverseChecked(s64 i, s64 nodes, typename std::enable_if<Nodes2 < 0>::type* = 0) {
        if (Triangular == true) {
            ASSERT(i >= 0);
            ASSERT(i <= nodes*(nodes-1)/2);
        } else {
            ASSERT(i >= 0);
            ASSERT(i <= nodes*nodes-1);
        }
        return reverse(i, nodes);
    }

    AdjacencyMatrixIndexer()                              = delete;
    AdjacencyMatrixIndexer(const AdjacencyMatrixIndexer&) = delete;
    AdjacencyMatrixIndexer(AdjacencyMatrixIndexer&&)      = delete;
};

#endif // ADJACENCYMATRIXINDEXER_HPP
