#ifndef ADJACENCYMATRIX_HPP
#define ADJACENCYMATRIX_HPP

#include "prereqs.hpp"

template<s64 Nodes, bool Triangular, typename Enable = void>
struct BaseAdjacencyMatrix;

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix<Nodes, Triangular, std::enable_if_t<(Nodes >= 0)>> {
public:
    constexpr s64 nodes()    const { return Nodes < 0 ? 0 : Nodes; }
    constexpr s64 edges()    const { return Nodes * (Nodes - 1) / 2; }
    constexpr s64 bits()     const { return bits_; }
    constexpr s64 elements() const { return elements_; }

    constexpr bool compile_time() const { return true; }

private:
    static constexpr s64 bits_     = Triangular ? Nodes*(Nodes-1) / 2 : Nodes*Nodes;
    static constexpr s64 elements_ = (bits_-1) / 64 + 1;

    u64 _v[elements_];
};

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix<Nodes, Triangular, std::enable_if_t<(Nodes < 0)>> {
public:
    s64 nodes()    const { return _nodes; }
    s64 edges()    const { return _nodes * (_nodes - 1) / 2; }
    s64 bits()     const { return Triangular ? _nodes*(_nodes-1) / 2 : _nodes*_nodes; }
    s64 elements() const { return (bits()-1) / 64 + 1; }

    constexpr bool compile_time() const { return false; }

    BaseAdjacencyMatrix(size_t nodes_) : _nodes(nodes_) {
        _v = (u64*) malloc(sizeof(u64) * elements());
    }

    ~BaseAdjacencyMatrix() {
        free(_v);
    }

private:
    s64 _nodes;
    u64* _v;
};

template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrix : public BaseAdjacencyMatrix<Nodes, Triangular> {
public:
    constexpr AdjacencyMatrix() {}

    template<s64 Nodes2 = Nodes>
    AdjacencyMatrix(int nodes, typename std::enable_if<(Nodes2 < 0)>::type* = 0) : BaseAdjacencyMatrix<Nodes, Triangular>(nodes) {
    }
};

#endif
