#pragma once

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
template<Size Nodes, bool Triangular = true>
class AdjacencyMatrixIndexer {
public:
    // third parameter unifies interface with the functions below that have 'nodes', fourth one is because
    // only three would be an illegal (equal) overload
    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0>>
    static constexpr Size index(Size column, Size row, Size = -1, void* = 0) {
        return index_(column, row, Nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0>>
    static constexpr Size indexChecked(Size column, Size row, Size = -1, void* = 0) {
        return indexChecked_(column, row, Nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0>>
    static constexpr std::pair<Size, Size> reverse(Size i, Size = -1, void* = 0) {
        return reverse_(i, Nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0>>
    static constexpr std::pair<Size, Size> reverseChecked(Size i, Size = -1, void* = 0) {
        return reverseChecked_(i, Nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>>
    static Size index(Size column, Size row, Size nodes) {
        return index_(column, row, nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>>
    static Size indexChecked(Size column, Size row, Size nodes) {
        return indexChecked_(column, row, nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>>
    static std::pair<Size, Size> reverse(Size i, Size nodes) {
        return reverse_(i, nodes);
    }

    template<Size Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>>
    static std::pair<Size, Size> reverseChecked(Size i, Size nodes) {
        return reverseChecked_(i, nodes);
    }

private:
    static constexpr Size index_(Size column, Size row, Size nodes) {
        if(Triangular == true) {
            if (column == row) {
                return -1;
            } else {
                if (column < row) {
                    std::swap(column, row);
                }
                return column*(column-1)/2 + row;
            }
        } else {
            return column*nodes + row;
        }
    }

    static constexpr Size indexChecked_(Size column, Size row, Size nodes) {
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        return index_(column, row, nodes);
    }

    // sqrt cannot be used at compile time --> not constexpr for Triangular matrices
    template<bool Triangular_ = Triangular, std::enable_if_t<Triangular_ == true, bool> = true>
    static std::pair<Size, Size> reverse_(Size i, Size nodes) {
        (void) nodes;
        Size column = Size(floor(0.5 + sqrt(0.25 + double(2*i))));
        Size row    = i - column*(column-1)/2;
        return std::make_pair(column, row);
    }

    template<bool Triangular_ = Triangular, std::enable_if_t<Triangular_ == false, bool> = true>
    static constexpr std::pair<Size, Size> reverse_(Size i, Size nodes) {
        Size row    = i % nodes;
        Size column = (i - row) / nodes;
        return std::make_pair(column, row);
    }

    static constexpr std::pair<Size, Size> reverseChecked_(Size i, Size nodes) {
        if (Triangular == true) {
            R5_ASSERT(i >= 0);
            R5_ASSERT(i <= nodes*(nodes-1)/2);
        } else {
            R5_ASSERT(i >= 0);
            R5_ASSERT(i <= nodes*nodes);
        }
        return reverse_(i, nodes);
    }

private:
    AdjacencyMatrixIndexer()                              = delete;
    AdjacencyMatrixIndexer(const AdjacencyMatrixIndexer&) = delete;
    AdjacencyMatrixIndexer(AdjacencyMatrixIndexer&&)      = delete;
};

} // namespace r5
