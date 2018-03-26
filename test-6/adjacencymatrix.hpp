#ifndef ADJACENCYMATRIX_HPP
#define ADJACENCYMATRIX_HPP

#include "prereqs.hpp"

#include <sstream>

#include "adjacencymatrixindexer.hpp"

template<s64 Nodes, bool Triangular, typename Enable = void>
class BaseAdjacencyMatrix;

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix<Nodes, Triangular, std::enable_if_t<(Nodes >= 0)>> {
public:
    constexpr s64 nodes()    const { return Nodes < 0 ? 0 : Nodes; }
    constexpr s64 edges()    const { return Nodes * (Nodes - 1) / 2; }
    constexpr s64 bits()     const { return bits_; }
    constexpr s64 elements() const { return elements_; }

    constexpr bool compile_time() const { return true; }

    // TODO set main diagonal to 0, if not triangular on initialization (compile time)

private:
    static constexpr s64 bitsPerElement = sizeof(u64)*8;
    static constexpr s64 bits_     = Triangular ? Nodes*(Nodes-1) / 2 : Nodes*Nodes;
    static constexpr s64 elements_ = (bits_-1) / bitsPerElement + 1;

    u64 _v[elements_];
};

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix<Nodes, Triangular, std::enable_if_t<(Nodes < 0)>> {
public:

    using Indexer = AdjacencyMatrixIndexer<-1, Triangular>;

    s64 nodes()    const { return _nodes; }
    s64 edges()    const { return _nodes * (_nodes - 1) / 2; }
    s64 bits()     const { return Triangular ? _nodes*(_nodes-1) / 2 : _nodes*_nodes; }
    s64 elements() const { return (bits()-1) / bitsPerElement + 1; }

    constexpr bool compile_time() const { return false; }

    BaseAdjacencyMatrix(s64 nodes_) : _nodes(nodes_) {
        _v = (u64*) malloc(sizeof(u64) * elements());

        if (Triangular == false) { // set main diagonal to 0
            for (s64 n = 0; n < _nodes; n += 1) {
                unsetEdge(n, n);
            }
        }
    }

    bool edge(s64 column, s64 row) const {
        s64 i = Indexer::index(column, row, _nodes);
        return (bool) ((_v[i/bitsPerElement]>>(i%bitsPerElement))&0x1);
    }

    bool edgeChecked(s64 column, s64 row) const {
        ASSERT(row    != column);
        ASSERT(column >= 0);
        ASSERT(column <= _nodes-1);
        ASSERT(row    >= 0);
        ASSERT(row    <= _nodes-1);
        return edge(column, row);
    }

    void unsetEdge(s64 column, s64 row) {
        s64 i = Indexer::index(column, row, _nodes);
        s64 element = i / bitsPerElement;
        s64 indexInElement = i % bitsPerElement;
        _v[element] &= ~(((u64)1)<<indexInElement);

        // preserve symmetry by swapping row and column and repeating
        if (Triangular == false) {
            i = Indexer::index(row, column, _nodes);
            element = i / bitsPerElement;
            indexInElement = i % bitsPerElement;
            _v[element] &= ~(((u64)1)<<indexInElement);
        }
    }

    void setEdge(s64 column, s64 row) {
        s64 i = Indexer::index(column, row, _nodes);
        s64 element = i / bitsPerElement;
        s64 indexInElement = i % bitsPerElement;
        _v[element] |= ((u64)1)<<(indexInElement);

        // preserve symmetry by swapping row and column and repeating
        if (Triangular == false) {
            i = Indexer::index(row, column, _nodes);
            element = i / bitsPerElement;
            indexInElement = i % bitsPerElement;
            _v[element] |= ((u64)1)<<indexInElement;
        }
    }

    void unsetEdgeChecked(s64 column, s64 row) {
        ASSERT(row    != column);
        ASSERT(column >= 0);
        ASSERT(column <= _nodes-1);
        ASSERT(row    >= 0);
        ASSERT(row    <= _nodes-1);
        unsetEdge(column, row);
    }

    void setEdgeChecked(s64 column, s64 row) {
        ASSERT(row    != column);
        ASSERT(column >= 0);
        ASSERT(column <= _nodes-1);
        ASSERT(row    >= 0);
        ASSERT(row    <= _nodes-1);
        setEdge(column, row);
    }

    std::string print(bool multiline = false, std::string indent = "") const {
        std::ostringstream o;

        if(Triangular == true) {

            if (multiline == false) {
                for (s64 c = 1; c < _nodes; c += 1) {
                    for (s64 r = 0; r < c; r += 1) {
                        o << edge(c, r);
                    }
                    o << ' ';
                }
            } else {
                for (s64 r = 0; r < _nodes-1; r += 1) {
                    o << indent;
                    for (s64 a = 0; a < r; a += 1) {
                        o << ' ';
                    }
                    for (s64 c = r+1; c < _nodes; c += 1) {
                        o << edge(c, r);
                    }
                    o << std::endl;
                }
            }
        } else {
            for (s64 r = 0; r < _nodes; r += 1) {
                if (multiline) { o << indent; }
                for(s64 c = 0; c < _nodes; c += 1) {
                    o << edge(c, r);
                }
                if (multiline == false) {
                    o << ' ';
                } else {
                    o << std::endl;
                }
            }
        }

        return o.str();
    }

    ~BaseAdjacencyMatrix() {
        free(_v);
    }

private:
    s64 _nodes;
    u64* _v;

    static constexpr s64 bitsPerElement = sizeof(u64)*8;
};

template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrix : public BaseAdjacencyMatrix<Nodes, Triangular> {
public:
    constexpr AdjacencyMatrix() {}

    template<typename = std::enable_if_t<Nodes < 0>>
    AdjacencyMatrix(s64 nodes) : BaseAdjacencyMatrix<Nodes, Triangular>(nodes) {
    }
};

#endif
