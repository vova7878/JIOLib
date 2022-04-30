#ifndef INTEGER_HPP
#define INTEGER_HPP

#include <cstdint>

constexpr inline size_t lowestOneBit(size_t i) {
    // HD, Section 2-1
    return i & -i;
}

constexpr inline bool isOneBit(size_t i) {
    return lowestOneBit(i) == i;
}

constexpr inline size_t numberOfLeadingZeros2Bit(uint8_t i) {
    return !i ? 2 : (1 - (i >> 1));
}

constexpr inline size_t numberOfLeadingZeros4Bit(uint8_t i) {
    return (i >= 1 << 2) ? numberOfLeadingZeros2Bit(i >> 2) :
            numberOfLeadingZeros2Bit(i) + 2;
}

constexpr inline size_t numberOfLeadingZeros(uint8_t i) {
    return (i >= 1 << 4) ? numberOfLeadingZeros4Bit(i >> 4) :
            numberOfLeadingZeros4Bit(i) + 4;
}

constexpr inline size_t numberOfLeadingZeros(uint16_t i) {
    return (i >= 1 << 8) ? numberOfLeadingZeros(uint8_t(i >> 8)) :
            numberOfLeadingZeros(uint8_t(i)) + 8;
}

constexpr inline size_t numberOfLeadingZeros(uint32_t i) {
    return (i >= 1 << 16) ? numberOfLeadingZeros(uint16_t(i >> 16)) :
            numberOfLeadingZeros(uint16_t(i)) + 16;
}

constexpr inline size_t numberOfLeadingZeros(uint64_t i) {
    return (i >= uint64_t(1) << 32) ? numberOfLeadingZeros(uint32_t(i >> 32)) :
            numberOfLeadingZeros(uint32_t(i)) + 32;
}

template <typename T>
constexpr inline size_t logb2(T value) {
    return sizeof (T) * 8 - numberOfLeadingZeros(value);
}

enum IType {
    illegal = 0,
    native,
    pow2,
    s4,
    other
};

constexpr IType getIntegerType(size_t size) {
    return (size == 1 || size == 2 || size == 4 || size == 8) ? IType::native :
            (isOneBit(size) ? IType::pow2 :
            (size == 0 ? IType::illegal : IType::other));
}

template<size_t size, bool sig, IType = getIntegerType(size)>
struct Integer_Impl;

template<typename T>
struct Operators_Impl;

template<typename U, typename S>
class Integer_S;

template<typename U, typename S>
class Integer_U {
private:
    U value;
    const static U shmask = (U(~U(0)))>>(numberOfLeadingZeros(U(sizeof (U) * 8)) + 1);
public:

    inline Integer_U() : value(0) { }

    inline Integer_U(U n) : value(n) { }

    inline Integer_U(Integer_S<U, S> n) : value(U(n)) { }

    template<typename T>
    explicit inline operator T() const {
        return T(value);
    }

    inline Integer_U operator/(const Integer_U other) const {
        return value / other.value;
    }

    inline Integer_U operator%(const Integer_U other) const {
        return value % other.value;
    }

    inline Integer_U operator>>(const Integer_U other) const {
        return value >> (other.value & shmask);
    }

    friend Operators_Impl<Integer_U<U, S>>;
};

template<typename U, typename S>
class Integer_S {
private:
    U value;
    static const U shmask = (U(~U(0)))>>(numberOfLeadingZeros(U(sizeof (U) * 8)) + 1);
public:

    inline Integer_S() : value(0) { }

    inline Integer_S(S n) : value(n) { }

    inline Integer_S(Integer_U<U, S> n) : value(U(n)) { }

    template<typename T>
    explicit inline operator T() const {
        return T(S(value));
    }

    inline Integer_S operator/(const Integer_S other) const {
        return S(value) / S(other.value);
    }

    inline Integer_S operator%(const Integer_S other) const {
        return S(value) % S(other.value);
    }

    inline Integer_S operator>>(const Integer_S other) const {
        return S(value) >> S(other.value & shmask);
    }

    friend Operators_Impl<Integer_S<U, S>>;
};

template<typename T>
struct Operators_Impl : public T {
    using T::T;

    inline Operators_Impl(const T &obj) : T(obj) { }

    inline Operators_Impl operator+(const Operators_Impl other) const {
        return Operators_Impl::value + other.value;
    }

    inline Operators_Impl operator-(const Operators_Impl other) const {
        return Operators_Impl::value - other.value;
    }

    inline Operators_Impl operator*(const Operators_Impl other) const {
        return Operators_Impl::value * other.value;
    }

    inline Operators_Impl operator|(const Operators_Impl other) const {
        return Operators_Impl::value | other.value;
    }

    inline Operators_Impl operator&(const Operators_Impl other) const {
        return Operators_Impl::value & other.value;
    }

    inline Operators_Impl operator^(const Operators_Impl other) const {
        return Operators_Impl::value ^ other.value;
    }

    inline Operators_Impl operator<<(const Operators_Impl other) const {
        return Operators_Impl::value << (other.value & T::shmask);
    }
};

template<>
struct Integer_Impl<1, false> {
    typedef Operators_Impl<Integer_U<uint8_t, int8_t>> type;
};

template<>
struct Integer_Impl<1, true> {
    typedef Operators_Impl<Integer_S<uint8_t, int8_t>> type;
};

template<>
struct Integer_Impl<2, false> {
    typedef Operators_Impl<Integer_U<uint16_t, int16_t>> type;
};

template<>
struct Integer_Impl<2, true> {
    typedef Operators_Impl<Integer_S<uint16_t, int16_t>> type;
};

template<>
struct Integer_Impl<4, false> {
    typedef Operators_Impl<Integer_U<uint32_t, int32_t>> type;
};

template<>
struct Integer_Impl<4, true> {
    typedef Operators_Impl<Integer_S<uint32_t, int32_t>> type;
};

template<>
struct Integer_Impl<8, false> {
    typedef Operators_Impl<Integer_U<uint64_t, int64_t>> type;
};

template<>
struct Integer_Impl<8, true> {
    typedef Operators_Impl<Integer_S<uint64_t, int64_t>> type;
};

/*template<size_t size, bool sig>
struct Integer_Impl <size, sig, IType::pow2> {
    typedef Operators_Impl<Integer_S<uint64_t, int64_t>> type;
};*/

template<size_t size, bool sig>
using Integer = typename Integer_Impl<size, sig>::type;

#endif /* INTEGER_HPP */