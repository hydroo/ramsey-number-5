#ifndef ADJACENCYMATRIXINDEXER_HPP
#define ADJACENCYMATRIXINDEXER_HPP

#include "prereqs.hpp"

#include <cmath>

#include <utility>

namespace r5 {

/* Indexing into a adjacency matrix of a simple undirected graph.
 *
 * Provides index(), indexChecked(), reverse(), reverseChecked()
 *
 * Nodes >= 0 -> node count     known at compile time: functions omit 'nodes' argument
 * Nodes <  0 -> node count not known at compile time: functions have 'nodes' argument
 */
template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrixIndexer {
public:
    // third parameter unifies interface with the functions below that have 'nodes', fourth one is because
    // only three would be an illegal (equal) overload
    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 >= 0>>
    static constexpr s64 index(s64 column, s64 row, s64 = -1, void* = 0) {
        return index_(column, row, Nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 >= 0>>
    static constexpr s64 indexChecked(s64 column, s64 row, s64 = -1, void* = 0) {
        return indexChecked_(column, row, Nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 >= 0>>
    static constexpr std::pair<s64, s64> reverse(s64 i, s64 = -1, void* = 0) {
        return reverse_(i, Nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 >= 0>>
    static constexpr std::pair<s64, s64> reverseChecked(s64 i, s64 = -1, void* = 0) {
        return reverseChecked_(i, Nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 == -1>>
    static s64 index(s64 column, s64 row, s64 nodes) {
        return index_(column, row, nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 == -1>>
    static s64 indexChecked(s64 column, s64 row, s64 nodes) {
        return indexChecked_(column, row, nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 == -1>>
    static std::pair<s64, s64> reverse(s64 i, s64 nodes) {
        return reverse_(i, nodes);
    }

    template<s64 Nodes2 = Nodes, typename = std::enable_if_t<Nodes2 == -1>>
    static std::pair<s64, s64> reverseChecked(s64 i, s64 nodes) {
        return reverseChecked_(i, nodes);
    }

private:
    static constexpr s64 index_(s64 column, s64 row, s64 nodes) {
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

    static constexpr s64 indexChecked_(s64 column, s64 row, s64 nodes) {
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        return index_(column, row, nodes);
    }

    static constexpr std::pair<s64, s64> reverse_(s64 i, s64 nodes) {
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

    static constexpr std::pair<s64, s64> reverseChecked_(s64 i, s64 nodes) {
        if (Triangular == true) {
            R5_ASSERT(i >= 0);
            R5_ASSERT(i <= nodes*(nodes-1)/2);
        } else {
            R5_ASSERT(i >= 0);
            R5_ASSERT(i <= nodes*nodes-1);
        }
        return reverse_(i, nodes);
    }

private:
    AdjacencyMatrixIndexer()                              = delete;
    AdjacencyMatrixIndexer(const AdjacencyMatrixIndexer&) = delete;
    AdjacencyMatrixIndexer(AdjacencyMatrixIndexer&&)      = delete;
};

} // namespace r5

#endif // ADJACENCYMATRIXINDEXER_HPP
