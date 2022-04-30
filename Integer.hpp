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
        return T::value + other.value;
    }

    inline Operators_Impl operator-(const Operators_Impl other) const {
        return T::value - other.value;
    }

    inline Operators_Impl operator+() const {
        return *this;
    }

    inline Operators_Impl operator-() const {
        return -T::value;
    }

    inline Operators_Impl& operator++() {
        ++T::value;
        return *this;
    }

    inline Operators_Impl& operator--() {
        --T::value;
        return *this;
    }

    inline Operators_Impl operator++(int) {
        Operators_Impl tmp = *this;
        operator++();
        return tmp;
    }

    inline Operators_Impl operator--(int) {
        Operators_Impl tmp = *this;
        operator--();
        return tmp;
    }

    inline Operators_Impl operator*(const Operators_Impl other) const {
        return T::value * other.value;
    }

    inline Operators_Impl operator|(const Operators_Impl other) const {
        return T::value | other.value;
    }

    inline Operators_Impl operator&(const Operators_Impl other) const {
        return T::value & other.value;
    }

    inline Operators_Impl operator^(const Operators_Impl other) const {
        return T::value ^ other.value;
    }

    inline Operators_Impl operator~() const {
        return ~T::value;
    }

    inline Operators_Impl operator<<(const Operators_Impl other) const {
        return T::value << (other.value & T::shmask);
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

template<size_t half, bool sig>
struct Pow2_Integer_Impl;

template<size_t size, bool sig>
struct Integer_Impl <size, sig, IType::pow2> {
    typedef Pow2_Integer_Impl<size / 2, sig> type;
};

template<size_t size, bool sig>
using Integer = typename Integer_Impl<size, sig>::type;

template<size_t half, bool sig>
struct Pow2_Integer_Base;

template<size_t half>
struct Pow2_Integer_Base<half, false> {
private:
    typedef Integer<half, true> S;
    typedef Integer<half, false> U;
    U low, high;
public:

    inline Pow2_Integer_Base() : low(0), high(0) { }

    inline Pow2_Integer_Base(U low) : low(low), high(0) { }

    inline Pow2_Integer_Base(U low, U high) : low(low), high(high) { }

    friend Pow2_Integer_Impl<half, false>;
};

template<size_t half, bool sig>
struct Pow2_Integer_Impl : Pow2_Integer_Base<half, sig> {
private:
    typedef typename Pow2_Integer_Base<half, sig>::S S;
    typedef typename Pow2_Integer_Base<half, sig>::U U;
    typedef Pow2_Integer_Base<half, sig> T;
    typedef Pow2_Integer_Impl<half, sig> I;

    inline static void increment(I &value) {
        U &tmp = ++value.low;
        if (!bool(tmp)) {
            ++value.high;
        }
    }

    inline static void decrement(I &value) {
        if (!bool(value.low)) {
            --value.high;
        }
        --value.low;
    }
public:
    using T::T;

    inline Pow2_Integer_Impl(const T &obj) : T(obj) { }

    inline U getLow() const {
        return T::low;
    }

    inline U getHigh() const {
        return T::high;
    }

    explicit inline operator U() const {
        return T::low;
    }

    explicit inline operator bool() const {
        return T::low && T::high;
    }

    inline I& operator++() {
        increment(*this);
        return *this;
    }

    inline I& operator--() {
        decrement(*this);
        return *this;
    }

    inline I operator++(int) {
        I tmp = *this;
        operator++();
        return tmp;
    }

    inline I operator--(int) {
        I tmp = *this;
        operator--();
        return tmp;
    }

    inline I operator+() const {
        return *this;
    }

    inline I operator-() const {
        I tmp = ~(*this);
        ++tmp;
        return tmp;
    }

    /*inline Operators_Impl operator+(const Operators_Impl other) const {
        return Operators_Impl::value + other.value;
    }

    inline Operators_Impl operator-(const Operators_Impl other) const {
        return Operators_Impl::value - other.value;
    }

    inline Operators_Impl operator*(const Operators_Impl other) const {
        return Operators_Impl::value * other.value;
    }

    inline Operators_Impl operator<<(const Operators_Impl other) const {
        return Operators_Impl::value << (other.value & T::shmask);
    }*/

    inline I operator|(const I other) const {
        return I(T::low | other.low, T::high | other.high);
    }

    inline I operator&(const I other) const {
        return I(T::low & other.low, T::high & other.high);
    }

    inline I operator^(const I other) const {
        return I(T::low ^ other.low, T::high ^ other.high);
    }

    inline I operator~() const {
        return I(~T::low, ~T::high);
    }
};

#endif /* INTEGER_HPP */