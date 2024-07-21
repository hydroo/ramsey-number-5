#pragma once

#include <array>
#include <numeric>
#include <boost/integer/static_log2.hpp>

#include "prereqs.hpp"

namespace r5 {

template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T staticLog2Ceil(T n_);
template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T staticLog2Floor(T n);
template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr T staticPow2(T n);

template<std::size_t... Bits>
class PackedUIntTuple {
private:
    template<std::size_t Bytes> struct ElementTypeSelector    { };
    template<>                  struct ElementTypeSelector<1> { using Type = uint8_t; };
    template<>                  struct ElementTypeSelector<2> { using Type = uint16_t; };
    template<>                  struct ElementTypeSelector<4> { using Type = uint32_t; };
    template<>                  struct ElementTypeSelector<8> { using Type = uint64_t; };

    template<std::size_t Size_>
    static constexpr std::array<std::size_t, Size_> bitOffsetsFromSizes(const std::array<std::size_t, Size_>& bitSizes) {
        std::array<std::size_t, Size_> ret{};
        std::exclusive_scan(bitSizes.cbegin(), bitSizes.cend(), ret.begin(), 0);
        return ret;
    }

    template<typename T, std::size_t Size_>
    static constexpr std::array<T, Size_> bitMasksFromSizes(const std::array<std::size_t, Size_>& bitSizes) {
        std::array<T, Size_> ret{};

        for (std::size_t i = 0; i < Size_; ++i) {
            ret[i] = T(staticPow2(bitSizes[i]))-1;
        }

        return ret;
    }

    template<typename T, std::size_t Size_>
    static std::array<uint64_t, Size_> convertArrayForPrinting(const std::array<T, Size_>& a) {
        std::array<uint64_t, Size_> ret;
        for (std::size_t i = 0; i < a.size(); ++i) {
            ret[i] = uint64_t(a[i]);
        }
        return ret;
    }

    static constexpr std::size_t                    _size           = sizeof...(Bits);
    static_assert(_size > 0, "Need at least one entry, or add the necessary code to support empty tuples");
    static constexpr std::array<std::size_t, _size> _bitSizes{Bits...};
    static constexpr std::array<std::size_t, _size> _bitOffsets     = bitOffsetsFromSizes(_bitSizes);
    static constexpr std::size_t                    _bitSum         = std::accumulate(std::cbegin(_bitSizes), std::cend(_bitSizes), 0);
    static constexpr std::size_t                    _byteSum        = (_bitSum / 8) + ((_bitSum % 8 == 0) ? 0 : 1);
    static_assert(_byteSum <= 8, "TODO: add support for arrays of elements");
    static constexpr std::size_t                    _byteSumRounded = r5::staticPow2(r5::staticLog2Ceil(_byteSum));

public:
    using ElementType = ElementTypeSelector<_byteSumRounded>::Type;

private:
    static constexpr std::array<ElementType, _size> _bitMasks = bitMasksFromSizes<ElementType, _size>(_bitSizes);

public:
    PackedUIntTuple() { clear(); };

    template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    PackedUIntTuple(const T(&a)[_size]) {
        clear();
        for (std::size_t i = 0; i < _size; ++i) {
            R5_DEBUG_ASSERT(a[i] <= T(_bitMasks[i]));
            _e |= (ElementType(a[i]) & _bitMasks[i]) << _bitOffsets[i];
        }
    }

    PackedUIntTuple(const PackedUIntTuple& t) : _e(t._e) {}
    ~PackedUIntTuple() {}

    PackedUIntTuple& operator=(const PackedUIntTuple& rhs) { _e = rhs._e; return *this; }

    bool operator==(const PackedUIntTuple& rhs) const { return _e == rhs._e; }
    bool operator!=(const PackedUIntTuple& rhs) const { return _e != rhs._e; }
    bool operator<(const PackedUIntTuple& rhs)  const { return _e  < rhs._e; }
    bool operator>(const PackedUIntTuple& rhs)  const { return _e  > rhs._e; }

    void clear() { _e = 0; }

    ElementType data() const { return _e; }

    template<std::size_t Index>
    ElementType get() const {
        static_assert(Index < _size);
        return (_e >> _bitOffsets[Index]) & _bitMasks[Index];
    }

    ElementType get(std::size_t i) const {
        return (_e >> _bitOffsets[i]) & _bitMasks[i];
    }

    template<std::size_t Index, typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    void set(T e) {
        static_assert(Index < _size);
        R5_DEBUG_ASSERT(e <= T(_bitMasks[Index]));
        _e &= ~(_bitMasks[Index] << _bitOffsets[Index]);
        _e |= (e&_bitMasks[Index]) << _bitOffsets[Index];
    }

    template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    void set(std::size_t i, T e) {
        R5_DEBUG_ASSERT(i < _size);
        R5_DEBUG_ASSERT(e <= T(_bitMasks[i]));
        _e &= ~(_bitMasks[i] << _bitOffsets[i]);
        _e |= (e&_bitMasks[i]) << _bitOffsets[i];
    }

    std::size_t size() const { return _size; }

    std::string dump(std::string indent = "") const {
        std::ostringstream o;
        o << indent;
        o << "sizeof "           << sizeof(*this);
        o << " _size "           << _size;
        o << " _bitSizes "       << _bitSizes;
        o << " _bitOffsets "     << _bitOffsets;
        o << " _bitSum "         << _bitSum;
        o << " _byteSum "        << _byteSum;
        o << " _byteSumRounded " << _byteSumRounded;
        o << " _bitMasks "       << convertArrayForPrinting(_bitMasks);
        return o.str();
    }

    ElementType _e; // TODO P1 support arrays
};

template <std::size_t... Bits>
std::ostream& operator<<(std::ostream& o, const r5::PackedUIntTuple<Bits...>& t) {
    o << '(';
    for (s64 i = 0; i < s64(t.size())-1; i += 1) {
        o << t.get(i) << ", ";
    }
    if (t.size() > 0) {
        o << t.get(t.size()-1);
    }
    o << ')';
    return o;
}

} // namespace r5

template <std::size_t... Bits>
struct std::hash<r5::PackedUIntTuple<Bits...>> {
    std::size_t operator()(const r5::PackedUIntTuple<Bits...>& t) const {
        return (std::size_t) t.data();
    }
};

template <std::size_t... Bits>
struct boost::hash<r5::PackedUIntTuple<Bits...>> {
    std::size_t operator()(const r5::PackedUIntTuple<Bits...>& t) const {
        return (std::size_t) t.data();
    }
};

/* *** helpers ************************************************************** */

namespace r5 {

namespace {
template<typename T>
    constexpr T staticLog2CeilRecurse(T n) {
        if   (n <= 1 ) { return  0; }
        else           { return  1 + staticLog2CeilRecurse(n / 2); }
    }
}

template<typename T, typename>
constexpr T staticLog2Ceil(T n) {
    if      (n <  0) { return -1; }
    else if (n == 0) { return  0; }
    auto nRounded = staticPow2(staticLog2Floor(n));
    auto offset = n == nRounded ? 0 : 1;
    return offset + staticLog2CeilRecurse(nRounded);
}

template<typename T, typename>
constexpr T staticLog2Floor(T n) {
    if      (n  < 0 ) { return -1; }
    else if (n <= 1 ) { return  0; }
    else              { return  1 + staticLog2Floor(n / 2); }
}

template<typename T, typename>
constexpr T staticPow2(T n) {
    return n <= 0 ? 1 : 2 * staticPow2(n - 1);
}

} // namespace r5