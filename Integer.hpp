#ifndef INTEGER_HPP
#define INTEGER_HPP

#include <cstdint>

constexpr size_t lowestOneBit(size_t i) {
    // HD, Section 2-1
    return i & -i;
}

constexpr bool isOneBit(size_t i) {
    return lowestOneBit(i) == i;
}

enum IType {
    illegal = 0,
    native,
    pow2,
    s4,
    other
};

constexpr IType getIntegerType(size_t size) {
    /*if (size == 0) {
        return IType::illegal;
    }
    if (size == 1 || size == 2 || size == 4 || size == 8) {
        return IType::native;
    }
    if (isOneBit(size)) {
        return IType::pow2;
    }
    if ((size & 3) == 0) {
        return IType::s4;
    }
    return IType::other;*/
    return (size == 1 || size == 2 || size == 4 || size == 8) ? IType::native :
            (isOneBit(size) ? IType::pow2 : IType::other);
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

    friend Operators_Impl<Integer_U<U, S>>;
};

template<typename U, typename S>
class Integer_S {
private:
    U value;
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
        return Operators_Impl::value << other;
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