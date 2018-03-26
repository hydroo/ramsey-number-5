#ifndef ADJACENCYMATRIXINDEXER_HPP
#define ADJACENCYMATRIXINDEXER_HPP

#include "prereqs.hpp"

#include <cmath>

#include <utility>

template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrixIndexer {
public:
    template<typename = std::enable_if_t<Nodes >= 0>>
    static constexpr s64 index(s64 column, s64 row) {
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

    template<typename = std::enable_if_t<Nodes >= 0>>
    static constexpr s64 indexChecked(s64 row, s64 column) {
        ASSERT(row    >= 0);
        ASSERT(row    <= Nodes-1);
        ASSERT(column >= 0);
        ASSERT(column <= Nodes-1);
        return index(row, column);
    }

    template<typename = std::enable_if_t<Nodes >= 0>>
    static constexpr std::pair<s64, s64> reverse(s64 i) {
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

    template<typename = std::enable_if_t<Nodes >= 0>>
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

    template<typename = std::enable_if_t<Nodes < 0>>
    static s64 index(s64 column, s64 row, s64 nodes) {
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

    template<typename = std::enable_if_t<Nodes < 0>>
    static s64 indexChecked(s64 row, s64 column, s64 nodes) {
        ASSERT(row    >= 0);
        ASSERT(row    <= nodes-1);
        ASSERT(column >= 0);
        ASSERT(column <= nodes-1);
        return index(row, column, nodes);
    }

    template<typename = std::enable_if_t<Nodes < 0>>
    static std::pair<s64, s64> reverse(s64 i, s64 nodes) {
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

    template<typename = std::enable_if_t<Nodes < 0>>
    static std::pair<s64, s64> reverseChecked(s64 i, s64 nodes) {
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
