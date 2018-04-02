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
         // set main diagonal to 0
         if (Triangular == false) {
            for (s64 i = 0; i < n; i += 1) {
                unsetEdge(i, i, n, v);
            }
        }
    }

    static constexpr void assign(const u64* m, s64 nodes, u64* v) {
        r5::copy(m, elements(nodes), v);
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

    static constexpr bool edge(s64 column, s64 row, s64 nodes, const u64* v) {
        s64 i = Indexer::index(column, row, nodes);
        return (bool) ((v[i/bitsPerElement]>>(i%bitsPerElement))&0x1);
    }

    static constexpr bool edgeChecked(s64 column, s64 row, s64 nodes, const u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        return edge(column, row, nodes, v);
    }

    static constexpr void unsetEdge(s64 column, s64 row, s64 nodes, u64* v) {
        auto f = [nodes, v](s64 column, s64 row) {
            s64 i = Indexer::index(column, row, nodes);
            s64 element = i / bitsPerElement;
            s64 indexInElement = i % bitsPerElement;
            v[element] &= ~(((u64)1)<<indexInElement);
        };
        f(column, row);
        if (Triangular == false) { f(row, column); }
    }

    static constexpr void unsetEdgeChecked(s64 column, s64 row, s64 nodes, u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        unsetEdge(column, row, nodes, v);
    }

    static constexpr void unsetAllEdges(s64 nodes, u64* v) {
        r5::fill_n(v, elements(nodes), (u64) 0x0000000000000000);
    }

    static constexpr void setEdge(s64 column, s64 row, s64 nodes, u64* v) {
        auto f = [nodes, v](s64 column, s64 row) {
            s64 i = Indexer::index(column, row, nodes);
            s64 element = i / bitsPerElement;
            s64 indexInElement = i % bitsPerElement;
            v[element] |= ((u64)1)<<(indexInElement);
        };
        f(column, row);
        if (Triangular == false) { f(row, column); }
    }

    static constexpr void setEdgeChecked(s64 column, s64 row, s64 nodes, u64* v) {
        R5_ASSERT(row    != column);
        R5_ASSERT(column >= 0);
        R5_ASSERT(column <= nodes-1);
        R5_ASSERT(row    >= 0);
        R5_ASSERT(row    <= nodes-1);
        setEdge(column, row, nodes, v);
    }

    static constexpr void setAllEdges(s64 nodes, u64* v) {
        r5::fill_n(v, elements(nodes), (u64) 0xffffffffffffffff);
    }

    static std::string print(bool multiline, std::string indent, s64 nodes, const u64* v) {
        std::ostringstream o;

        if(Triangular == true) {

            if (multiline == false) {
                for (s64 c = 1; c < nodes; c += 1) {
                    for (s64 r = 0; r < c; r += 1) {
                        o << edge(c, r, nodes, v);
                    }
                    o << ' ';
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

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 == -1>>
    BaseAdjacencyMatrix2(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        R5_ASSERT(m.nodes() == Nodes);
        Base::assign(m._v, Nodes, _v);
    }

    // assignment operators
    constexpr BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2& m) {
        Base::assign(m._v, Nodes, _v);
        return *this;
    }

    template<s64 Nodes2, typename = std::enable_if_t<Nodes2 == -1>>
    BaseAdjacencyMatrix2& operator=(const BaseAdjacencyMatrix2<Nodes2, Triangular>& m) {
        R5_ASSERT(m.nodes() == Nodes);
        Base::assign(m._v, Nodes, _v);
        return *this;
    }

    constexpr bool edge(s64 column, s64 row) const {
        return Base::edge(column, row, Nodes, _v);
    }

    constexpr bool edgeChecked(s64 column, s64 row) const {
        return Base::edgeChecked(column, row, Nodes, _v);
    }

    constexpr void unsetEdge(s64 column, s64 row) {
        Base::unsetEdge(column, row, Nodes, _v);
    }

    constexpr void unsetEdgeChecked(s64 column, s64 row) {
        Base::unsetEdgeChecked(column, row, Nodes, _v);
    }

    constexpr void unsetAllEdges() {
        Base::unsetAllEdges(Nodes, _v);
    }

    constexpr void setEdge(s64 column, s64 row) {
        Base::setEdge(column, row, Nodes, _v);
    }

    constexpr void setEdgeChecked(s64 column, s64 row) {
        Base::setEdgesChecked(column, row, Nodes, _v);
    }

    constexpr void setAllEdges() {
        Base::setAllEdges(Nodes, _v);
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

    bool edgeChecked(s64 column, s64 row) const {
        return Base::edgeChecked(column, row, _nodes, _v);
    }

    void unsetEdge(s64 column, s64 row) {
        Base::unsetEdge(column, row, _nodes, _v);
    }

    void unsetEdgeChecked(s64 column, s64 row) {
        Base::unsetEdgeChecked(column, row, _nodes, _v);
    }

    void unsetAllEdges() {
        Base::unsetAllEdges(_nodes, _v);
    }

    void setEdge(s64 column, s64 row) {
        Base::setEdge(column, row, _nodes, _v);
    }

    void setEdgeChecked(s64 column, s64 row) {
        Base::setEdgeChecked(column, row, _nodes, _v);
    }

    void setAllEdges() {
        Base::setAllEdges(_nodes, _v);
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
    template<s64 Nodes2>
    AdjacencyMatrix(const AdjacencyMatrix<Nodes2, Triangular>& m) : Base(m) {}

    // assignment operators
    // Note: The bottom template would not be used as an assignment operator
    constexpr AdjacencyMatrix& operator=(const AdjacencyMatrix& m) {
        Base::operator=(m);
        return *this;
    }
    template<s64 Nodes2>
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
};

} // namespace r5

#endif
