#ifndef ADJACENCYMATRIX_HPP
#define ADJACENCYMATRIX_HPP

#include "prereqs.hpp"

#include <sstream>

#include "adjacencymatrixindexer.hpp"

namespace r5 {

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix {
protected:
    static constexpr s64 bitsPerElement = sizeof(u64)*8;

    using Indexer = AdjacencyMatrixIndexer<Nodes, Triangular>;

    static constexpr s64 nodes   (s64 n) { return n; }
    static constexpr s64 edges   (s64 n) { return n*(n-1)/2; }
    static constexpr s64 bits    (s64 n) { return Triangular ? edges(n) : n*n; }
    static constexpr s64 elements(s64 n) { return (bits(n)-1) / bitsPerElement + 1; }

    static constexpr void initialize(s64 n, u64* v) {
        unsetAllEdges(n, v);
    }

    static constexpr void assign(const u64* m, s64 nodes, u64* v) {
        r5::copy(m, elements(nodes), v);
    }

    template<s64 NodesM>
    static constexpr void assign(const u64* m, s64 nodesM, s64 nodesV, u64* v) {
        if (Triangular == true) {
            r5::copy(m, elements(nodesM), v);
        } else {
            for (s64 c = 1; c < nodesM; c += 1) {
                for (s64 r = 0; r < c; r += 1) {
                    if (edge(AdjacencyMatrixIndexer<NodesM, Triangular>::index(c, r, nodesM), m) == true) {
                        setEdge(c, r, nodesV, v);
                    } else {
                        unsetEdge(c, r, nodesV, v);
                    }
                }
            }
        }
    }

    static constexpr void bitwiseOr(const u64* v1, const u64* v2, s64 nodes, u64* v) {
        for (s64 i = 0; i < elements(nodes); i += 1) {
            v[i] = v1[i] | v2[i];
        }
    }

    static constexpr void bitwiseAnd(const u64* v1, const u64* v2, s64 nodes, u64* v) {
        for (s64 i = 0; i < elements(nodes); i += 1) {
            v[i] = v1[i] & v2[i];
        }
    }

    static constexpr void bitwiseComplement(const u64* v1, s64 nodes, u64* v) {
        for (s64 i = 0; i < elements(nodes); i += 1) {
            v[i] = ~v1[i];
        }
    }

    static constexpr bool compareEqual(const u64* v1, const u64* v2, s64 nodes) {
        bool equal = true;
        for (s64 i = 0; i < elements(nodes); i += 1) {
            equal &= v1[i] == v2[i];
        }
        return equal;
    }

    static constexpr bool compareLessThan(const u64* v1, const u64* v2, s64 nodes) {
        // Returns true if v1 has earlier 1 bits on the left than v2
        // This behavior is desirable for uniqueAdjacencyMatrices, because then
        // extensions of previous/smaller unique graphs will never change the shape
        // of the earlier graph.
        // E.g. we want 1 00 001 to be smaller than 0 01 100, because otherwise
        // extending 1 00 to 1 00 001 will result in prefering 0 01 100 as the new
        // 'smallest' representation of 1 00 001
        // This in turn means we would always have to churn through all representations
        // of each extended graph. Instead we can now stop searching for the smallest
        // representations and only have to check whether the current one is the
        // smallest, which is false as soon as we find a smaller representation.
        //
        // TODO the above note seems wrong / misleading.
        // Extending 0 00 by anything with at least one edge will obviously yield 1 xx xxx.
        // Maybe the advantage is that we can apply a permutation left to right and exit early?
        for (s64 i = 0; i < elements(nodes); i += 1) {
            if (v1[i] != v2[i]) {
                return (bool) ((v1[i]>>(__builtin_ffsll(v1[i] ^ v2[i]) - 1))&0x1);
            }
        }
        return false;
    }

    static constexpr bool edge(s64 column, s64 row, s64 nodes, const u64* v) {
        s64 e = Indexer::index(column, row, nodes);
        return (bool) ((v[e/bitsPerElement]>>(e%bitsPerElement))&0x1);
    }

    static constexpr bool edge(s64 edge_, const u64* v) {
        return (bool) ((v[edge_/bitsPerElement]>>(edge_%bitsPerElement))&0x1);
    }

    static constexpr bool edgeChecked(s64 column, s64 row, s64 nodes, const u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        return edge(column, row, nodes, v);
    }

    static constexpr bool edgeChecked(s64 edge_, s64 nodes, const u64* v) {
        R5_ASSERT(edge_ >= 0);
        R5_ASSERT(edge_ <= edges(nodes)-1);
        return edge(edge_, v);
    }

    static constexpr void unsetEdge(s64 column, s64 row, s64 nodes, u64* v) {
        auto f = [nodes, v](s64 column, s64 row) {
            s64 i = Indexer::index(column, row, nodes);
            v[i / bitsPerElement] &= ~(((u64)1)<<(i % bitsPerElement));
        };
        f(column, row);
        if (Triangular == false) { f(row, column); }
    }

    static constexpr void unsetEdge(s64 edge, s64 nodes, u64* v) {
        auto f = [v](s64 edge) {
            v[edge / bitsPerElement] &= ~(((u64)1)<<(edge % bitsPerElement));
        };
        f(edge);
        if (Triangular == false) {
            auto p = Indexer::reverse(edge, nodes);
            f(Indexer::index(p.second, p.first, nodes));
        }
    }

    static constexpr void unsetEdgeChecked(s64 column, s64 row, s64 nodes, u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        unsetEdge(column, row, nodes, v);
    }

    static constexpr void unsetEdgeChecked(s64 edge, s64 nodes, u64* v) {
        R5_ASSERT(edge >= 0);
        R5_ASSERT(edge <= edges(nodes)-1);
        unsetEdge(edge, nodes, v);
    }

    static constexpr void unsetAllEdges(s64 nodes, u64* v) {
        r5::fill_n(v, elements(nodes), (u64) 0x0000000000000000);
    }

    static constexpr void setEdge(s64 column, s64 row, s64 nodes, u64* v) {
        auto f = [nodes, v](s64 column, s64 row) {
            s64 i = Indexer::index(column, row, nodes);
            v[i / bitsPerElement] |= ((u64)1)<<(i % bitsPerElement);
        };
        f(column, row);
        if (Triangular == false) { f(row, column); }
    }

    static constexpr void setEdge(s64 edge, s64 nodes, u64* v) {
        auto f = [v](s64 edge) {
            v[edge / bitsPerElement] |= ((u64)1)<<(edge % bitsPerElement);
        };
        f(edge);
        if (Triangular == false) {
            auto p = Indexer::reverse(edge, nodes);
            f(Indexer::index(p.second, p.first, nodes));
        }
    }

    static constexpr void setEdgeChecked(s64 column, s64 row, s64 nodes, u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        setEdge(column, row, nodes, v);
    }

    static constexpr void setEdgeChecked(s64 edge, s64 nodes, u64* v) {
        R5_ASSERT(edge >= 0);
        R5_ASSERT(edge <= edges(nodes)-1);
        setEdge(edge, nodes, v);
    }

    static constexpr void setAllEdges(s64 nodes, u64* v) {
        unsetAllEdges(nodes, v);
        for (s64 n = 0; n < nodes; n += 1) {
            for (s64 m = 0; m < n; m += 1) {
                setEdge(n, m, nodes, v);
            }
        }
    }

    static constexpr void toggleEdge(s64 column, s64 row, s64 nodes, u64* v) {
        auto f = [nodes, v](s64 column, s64 row) {
            s64 i = Indexer::index(column, row, nodes);
            v[i / bitsPerElement] ^= ((u64)1)<<(i % bitsPerElement);
        };
        f(column, row);
        if (Triangular == false) { f(row, column); }
    }

    static constexpr void toggleEdge(s64 edge, s64 nodes, u64* v) {
        auto f = [v](s64 edge) {
            v[edge / bitsPerElement] ^= ((u64)1)<<(edge % bitsPerElement);
        };
        f(edge);
        if (Triangular == false) {
            auto p = Indexer::reverse(edge, nodes);
            f(Indexer::index(p.second, p.first, nodes));
        }
    }

    static constexpr void toggleEdgeChecked(s64 column, s64 row, s64 nodes, u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        toggleEdge(column, row, nodes, v);
    }

    static constexpr void toggleEdgeChecked(s64 edge, s64 nodes, u64* v) {
        R5_ASSERT(edge >= 0);
        R5_ASSERT(edge <= edges(nodes)-1);
        toggleEdge(edge, nodes, v);
    }

    static constexpr s64 edgeCount(s64 nodes, const u64* v) {
        s64 ret = 0;
        for (s64 i = 0; i < elements(nodes); i += 1) {
            ret += (s64) __builtin_popcountll(v[i]);
        }

        if(Triangular == false) {
            ret /= 2;
        }

        return ret;
    }

    static std::string print(bool multiline, std::string indent, s64 nodes, const u64* v) {
        std::ostringstream o;

        if(Triangular == true) {
            if (multiline == false) {
                for (s64 c = 1; c < nodes; c += 1) {
                    for (s64 r = 0; r < c; r += 1) {
                        o << edge(c, r, nodes, v);
                    }
                    if (c < nodes-1) { o << ' '; }
                }
            } else {
                for (s64 r = 0; r < nodes-1; r += 1) {
                    o << indent;
                    for (s64 a = 0; a < r; a += 1) {
                        o << ' ';
                    }
                    for (s64 c = r+1; c < nodes; c += 1) {
                        o << edge(c, r, nodes, v);
                    }
                    o << std::endl;
                }
            }
        } else {
            for (s64 r = 0; r < nodes; r += 1) {
                if (multiline) { o << indent; }
                for(s64 c = 0; c < nodes; c += 1) {
                    o << edge(c, r, nodes, v);
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
};

template<s64 Nodes, bool Triangular, typename Enable = void>
class BaseAdjacencyMatrix2;

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix2<Nodes, Triangular, std::enable_if_t<(Nodes >= 0)>> : public BaseAdjacencyMatrix<Nodes, Triangular> {
public:

    using Base    = BaseAdjacencyMatrix<Nodes, Triangular>;
    using Indexer = typename Base::Indexer;

    constexpr s64 nodes()    const { return Base::nodes   (Nodes); }
    constexpr s64 edges()    const { return Base::edges   (Nodes); }
    constexpr s64 bits()     const { return Base::bits    (Nodes); }
    constexpr s64 elements() const { return Base::elements(Nodes); }

    // constructor
    constexpr BaseAdjacencyMatrix2() {
        Base::initialize(Nodes, _v);
    }

    // copy constructors
    constexpr BaseAdjacencyMatrix2(const BaseAdjacencyMatrix2& m) {
        Base::assign(m._v, Nodes, _v);
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 != -1 && Nodes2 < Nodes>>
    constexpr BaseAdjacencyMatrix2(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        if (m.elements() < elements()) {
            Base::unsetAllEdges(Nodes, _v);
        }
        Base::template assign<Nodes2>(m._v, Nodes2, Nodes, _v);
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 == -1>, int dummy = 0>
    BaseAdjacencyMatrix2(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        R5_ASSERT(m.nodes() <= Nodes);
        if (m.elements() < elements()) {
            Base::unsetAllEdges(Nodes, _v);
        }
        Base::template assign<Nodes2>(m._v, m.nodes(), Nodes, _v);
    }

    // assignment operators
    constexpr BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2& m) {
        Base::assign(m._v, Nodes, _v);
        return *this;
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 != -1 && Nodes2 < Nodes>>
    constexpr BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        if (m.elements() < elements()) {
            Base::unsetAllEdges(Nodes, _v);
        }
        Base::template assign<Nodes2>(m._v, Nodes2, Nodes, _v);
        return *this;
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 == -1>, int dummy = 0>
    BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        R5_ASSERT(m.nodes() <= Nodes);
        if (m.elements() < elements()) {
            Base::unsetAllEdges(Nodes, _v);
        }
        Base::template assign<Nodes2>(m._v, m.nodes(), Nodes, _v);
        return *this;
    }

    constexpr bool edge(s64 column, s64 row) const {
        return Base::edge(column, row, Nodes, _v);
    }

    constexpr bool edge(s64 edge_) const {
        return Base::edge(edge_, _v);
    }

    constexpr bool edgeChecked(s64 column, s64 row) const {
        return Base::edgeChecked(column, row, Nodes, _v);
    }

    constexpr bool edgeChecked(s64 edge) const {
        return Base::edgeChecked(edge, Nodes, _v);
    }

    constexpr void unsetEdge(s64 column, s64 row) {
        Base::unsetEdge(column, row, Nodes, _v);
    }

    constexpr void unsetEdge(s64 edge) {
        Base::unsetEdge(edge, Nodes, _v);
    }

    constexpr void unsetEdgeChecked(s64 column, s64 row) {
        Base::unsetEdgeChecked(column, row, Nodes, _v);
    }

    constexpr void unsetEdgeChecked(s64 edge) {
        Base::unsetEdgeChecked(edge, Nodes, _v);
    }

    constexpr void unsetAllEdges() {
        Base::unsetAllEdges(Nodes, _v);
    }

    constexpr void setEdge(s64 column, s64 row) {
        Base::setEdge(column, row, Nodes, _v);
    }

    constexpr void setEdge(s64 edge) {
        Base::setEdge(edge, Nodes, _v);
    }

    constexpr void setEdgeChecked(s64 column, s64 row) {
        Base::setEdgeChecked(column, row, Nodes, _v);
    }

    constexpr void setEdgeChecked(s64 edge) {
        Base::setEdgeChecked(edge, Nodes, _v);
    }

    constexpr void setAllEdges() {
        Base::setAllEdges(Nodes, _v);
    }

    constexpr void toggleEdge(s64 column, s64 row) {
        Base::toggleEdge(column, row, Nodes, _v);
    }

    constexpr void toggleEdge(s64 edge) {
        Base::toggleEdge(edge, Nodes, _v);
    }

    constexpr void toggleEdgeChecked(s64 column, s64 row) {
        Base::toggleEdgesChecked(column, row, Nodes, _v);
    }

    constexpr void toggleEdgeChecked(s64 edge) {
        Base::toggleEdgesChecked(edge, Nodes, _v);
    }

    constexpr s64 edgeCount() const {
        return Base::edgeCount(Nodes, _v);
    }

    std::string print(bool multiline = false, std::string indent = "") const {
        return Base::print(multiline, indent, Nodes, _v);
    }

    template<s64 Nodes2, bool Triangular2, typename Enable2>
    friend class BaseAdjacencyMatrix2;

protected:
    template<s64 Nodes2>
    constexpr void bitwiseOr(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m, BaseAdjacencyMatrix2* ret) const {
        Base::bitwiseOr(_v, m._v, Nodes, ret->_v);
    }

    template<s64 Nodes2>
    constexpr void bitwiseAnd(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m, BaseAdjacencyMatrix2* ret) const {
        Base::bitwiseAnd(_v, m._v, Nodes, ret->_v);
    }

    constexpr void bitwiseComplement(BaseAdjacencyMatrix2* ret) const {
        Base::bitwiseComplement(_v, Nodes, ret->_v);
    }

    template<s64 Nodes2>
    constexpr bool operator==(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) const {
        return Base::compareEqual(_v, m._v, Nodes);
    }

    template<s64 Nodes2>
    constexpr bool operator<(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) const {
        return Base::compareLessThan(_v, m._v, Nodes);
    }

private:
    u64 _v[Base::elements(Nodes)]{};
};

template<s64 Nodes, bool Triangular>
class BaseAdjacencyMatrix2<Nodes, Triangular, std::enable_if_t<(Nodes == -1)>> : public BaseAdjacencyMatrix<Nodes, Triangular> {
public:
    using Base    = BaseAdjacencyMatrix<Nodes, Triangular>;
    using Indexer = typename Base::Indexer;

    s64 nodes()    const { return Base::nodes   (_nodes); }
    s64 edges()    const { return Base::edges   (_nodes); }
    s64 bits()     const { return Base::bits    (_nodes); }
    s64 elements() const { return Base::elements(_nodes); }

    // constructor
    BaseAdjacencyMatrix2(s64 nodes_) : _nodes(nodes_) {
        allocate();
        Base::initialize(_nodes, _v);
    }

    // copy constructors
    BaseAdjacencyMatrix2(const BaseAdjacencyMatrix2& m) : _nodes(m._nodes) {
        allocate();
        Base::assign(m._v, _nodes, _v);
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 >= 0>>
    BaseAdjacencyMatrix2(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) : _nodes(Nodes2) {
        allocate();
        Base::assign(m._v, _nodes, _v);
    }

    // assignment operators
    BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2& m) {
        deallocate();
        _nodes = m._nodes;
        allocate();
        Base::assign(m._v, _nodes, _v);
        return *this;
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 >= 0>>
    BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        deallocate();
        _nodes = Nodes2;
        allocate();
        Base::assign(m._v, _nodes, _v);
        return *this;
    }

    bool edge(s64 column, s64 row) const {
        return Base::edge(column, row, _nodes, _v);
    }

    bool edge(s64 edge_) const {
        return Base::edge(edge_, _v);
    }

    bool edgeChecked(s64 column, s64 row) const {
        return Base::edgeChecked(column, row, _nodes, _v);
    }

    bool edgeChecked(s64 edge) const {
        return Base::edgeChecked(edge, _nodes, _v);
    }

    void unsetEdge(s64 column, s64 row) {
        Base::unsetEdge(column, row, _nodes, _v);
    }

    void unsetEdge(s64 edge) {
        Base::unsetEdge(edge, _nodes, _v);
    }

    void unsetEdgeChecked(s64 column, s64 row) {
        Base::unsetEdgeChecked(column, row, _nodes, _v);
    }

    void unsetEdgeChecked(s64 edge) {
        Base::unsetEdgeChecked(edge, _nodes, _v);
    }

    void unsetAllEdges() {
        Base::unsetAllEdges(_nodes, _v);
    }

    void setEdge(s64 column, s64 row) {
        Base::setEdge(column, row, _nodes, _v);
    }

    void setEdge(s64 edge) {
        Base::setEdge(edge, _nodes, _v);
    }

    void setEdgeChecked(s64 column, s64 row) {
        Base::setEdgeChecked(column, row, _nodes, _v);
    }

    void setEdgeChecked(s64 edge) {
        Base::setEdgeChecked(edge, _nodes, _v);
    }

    void setAllEdges() {
        Base::setAllEdges(_nodes, _v);
    }

    void toggleEdge(s64 column, s64 row) {
        Base::toggleEdge(column, row, _nodes, _v);
    }

    void toggleEdge(s64 edge) {
        Base::toggleEdge(edge, _nodes, _v);
    }

    void toggleEdgeChecked(s64 column, s64 row) {
        Base::toggleEdgeChecked(column, row, _nodes, _v);
    }

    void toggleEdgeChecked(s64 edge) {
        Base::toggleEdgeChecked(edge, _nodes, _v);
    }

    s64 edgeCount() const {
        return Base::edgeCount(_nodes, _v);
    }

    std::string print(bool multiline = false, std::string indent = "") const {
        return Base::print(multiline, indent, _nodes, _v);
    }

    ~BaseAdjacencyMatrix2() {
        deallocate();
    }

    template<s64 Nodes2, bool Triangular2, typename Enable2>
    friend class BaseAdjacencyMatrix2;

protected:
    template<s64 Nodes2>
    void bitwiseOr(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m, BaseAdjacencyMatrix2* ret) const {
        Base::bitwiseOr(_v, m._v, _nodes, ret->_v);
    }

    template<s64 Nodes2>
    void bitwiseAnd(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m, BaseAdjacencyMatrix2* ret) const {
        Base::bitwiseAnd(_v, m._v, _nodes, ret->_v);
    }

    void bitwiseComplement(BaseAdjacencyMatrix2* ret) const {
        Base::bitwiseComplement(_v, _nodes, ret->_v);
    }

    template<s64 Nodes2>
    bool operator==(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) const {
        return Base::compareEqual(_v, m._v, _nodes);
    }

    template<s64 Nodes2>
    bool operator<(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) const {
        return Base::compareLessThan(_v, m._v, _nodes);
    }

private:
    void allocate() {
        _v = (u64*) malloc(sizeof(u64) * elements());
    }

    void deallocate() {
        free(_v);
    }

private:
    s64 _nodes;
    u64* _v;
};

template<s64 Nodes, bool Triangular = true>
class AdjacencyMatrix : public BaseAdjacencyMatrix2<Nodes, Triangular> {
public:

    using Base = BaseAdjacencyMatrix2<Nodes, Triangular>;

    // default constructor only for compile-time
    template<s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0>, int dummy = 0>
    constexpr AdjacencyMatrix() : Base() {}

    // nodes constructor only for runtime
    template<s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>>
    AdjacencyMatrix(s64 nodes) : Base(nodes) {}

    // copy constructors
    // Note: The bottom template would not be used as a copy constructor
    constexpr AdjacencyMatrix(const AdjacencyMatrix& m)           : Base(m) {}
    template<s64 Nodes2, typename = std::enable_if_t<Nodes == -1 || Nodes2 <= Nodes>>
    constexpr AdjacencyMatrix(const AdjacencyMatrix<Nodes2, Triangular>& m) : Base(m) {}

    // assignment operators
    // Note: The bottom template would not be used as an assignment operator
    constexpr AdjacencyMatrix& operator=(const AdjacencyMatrix& m) {
        Base::operator=(m);
        return *this;
    }
    template<s64 Nodes2, typename = std::enable_if_t<Nodes == -1 || Nodes2 <= Nodes>>
    AdjacencyMatrix& operator=(const AdjacencyMatrix<Nodes2, Triangular>& m) {
        Base::operator=(m);
        return *this;
    }

    // bitwise or operators
    // Note: There are two different operators because the AdjacencyMatrix constructors are different for the two cases
    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0 && (Nodes2 == Nodes || Nodes2 == -1)>>
    constexpr AdjacencyMatrix operator|(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        R5_ASSERT(m.nodes() == Base::nodes());
        AdjacencyMatrix ret;
        Base::bitwiseOr(m, &ret);
        return ret;
    }

    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>, int dummy = 0>
    AdjacencyMatrix operator|(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        R5_ASSERT(m.nodes() == Base::nodes());
        AdjacencyMatrix ret(m.nodes());
        Base::bitwiseOr(m, &ret);
        return ret;
    }

    // bitwise and operators
    // Note: There are two different operators because the AdjacencyMatrix constructors are different for the two cases
    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0 && (Nodes2 == Nodes_ || Nodes2 == -1)>>
    constexpr AdjacencyMatrix operator&(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        R5_ASSERT(m.nodes() == Base::nodes());
        AdjacencyMatrix ret;
        Base::bitwiseAnd(m, &ret);
        return ret;
    }

    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>, int dummy = 0>
    AdjacencyMatrix operator&(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        R5_ASSERT(m.nodes() == Base::nodes());
        AdjacencyMatrix ret(m.nodes());
        Base::bitwiseAnd(m, &ret);
        return ret;
    }

    // bitwise complement operators
    // Note: There are two different operators because the AdjacencyMatrix constructors are different for the two cases
    template<s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ >= 0>>
    constexpr AdjacencyMatrix operator~() const {
        AdjacencyMatrix ret;
        Base::bitwiseComplement(&ret);
        return ret;
    }

    template<s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes_ == -1>, int dummy = 0>
    AdjacencyMatrix operator~() const {
        AdjacencyMatrix ret(Base::nodes());
        Base::bitwiseComplement(&ret);
        return ret;
    }

    // compare equal operator
    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes2 == Nodes_ || Nodes_ == -1 || Nodes2 == -1>>
    constexpr bool operator==(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        R5_ASSERT(m.nodes() == Base::nodes());
        return Base::operator==(m);
    }

    // compare not-equal operator
    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes2 == Nodes_ || Nodes_ == -1 || Nodes2 == -1>>
    constexpr bool operator!=(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        return !operator==(m);
    }

    // compare less than operator
    // Note: The least significant bit is on the left (in print())
    template<s64 Nodes2, s64 Nodes_ = Nodes, typename = std::enable_if_t<Nodes2 == Nodes_ || Nodes_ == -1 || Nodes2 == -1>>
    constexpr bool operator<(const AdjacencyMatrix<Nodes2, Triangular>& m) const {
        R5_ASSERT(m.nodes() == Base::nodes());
        return Base::operator<(m);
    }
};

template<s64 Nodes, bool Triangular>
std::ostream& operator<< (std::ostream& o, const r5::AdjacencyMatrix<Nodes, Triangular>& m) {
    o << m.print();
    return o;
}

} // namespace r5

#endif
