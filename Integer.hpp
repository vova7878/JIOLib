#ifndef INTEGER_HPP
#define INTEGER_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>

constexpr inline size_t lowestOneBit(size_t i) {
    // HD, Section 2-1
    return i & -i;
}

constexpr inline bool isOneBit(size_t i) {
    return lowestOneBit(i) == i;
}

template<typename T>
constexpr const T& max(const T &v1, const T &v2) {
    return v1 < v2 ? v2 : v1;
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
    u4array,
    other
};

constexpr IType getIntegerType(size_t size) {
    return (size == 1 || size == 2 || size == 4 || size == 8) ? native :
            (size == 0 ? illegal :
            (isOneBit(size) ? pow2 :
            ((size & 3) == 0 ? u4array : other)));
}

template<size_t size, bool = (size > sizeof (unsigned int))>
struct SHValue {
    typedef unsigned int type;
};

template<size_t size>
struct SHValue<size, true> {
    typedef size_t type;
};

template<size_t size>
using SHType = typename SHValue<size>::type;

template<size_t size, bool sig>
struct Integer;

template<size_t size, bool sig, IType = getIntegerType(size)>
struct Integer_Impl;

template<typename T>
struct Operators_Impl;

template<typename U, typename S>
class Integer_S;

template<typename UT, typename ST>
class Integer_U {
private:
    typedef ST S;
    typedef UT U;
    typedef SHType<sizeof (U) > M;
    const static M shmask = sizeof (U) * 8 - 1;
    U value;
public:

    explicit constexpr inline Integer_U() : value(0) { }

    explicit constexpr inline Integer_U(const U n) : value(n) { }

    constexpr inline Integer_U operator/(const Integer_U &other) const {
        return Integer_U(value / other.value);
    }

    constexpr inline Integer_U operator%(const Integer_U &other) const {
        return Integer_U(value % other.value);
    }

    constexpr inline Integer_U operator>>(const M other) const {
        return Integer_U(value >> (other & shmask));
    }

    constexpr inline bool operator>(const Integer_U &other) const {
        return value > other.value;
    }

    constexpr inline bool operator<(const Integer_U &other) const {
        return value < other.value;
    }

    constexpr inline bool operator>=(const Integer_U &other) const {
        return value >= other.value;
    }

    constexpr inline bool operator<=(const Integer_U &other) const {
        return value <= other.value;
    }

    template<size_t size, bool sig>
    friend class Integer;

    friend Operators_Impl<Integer_U<U, S>>;
};

template<typename UT, typename ST>
class Integer_S {
private:
    typedef ST S;
    typedef UT U;
    typedef SHType<sizeof (U) > M;
    static const M shmask = sizeof (U) * 8 - 1;
    U value;
public:

    constexpr explicit inline Integer_S() : value(0) { }

    constexpr explicit inline Integer_S(const S n) : value(n) { }

    constexpr inline Integer_S operator/(const Integer_S &other) const {
        return Integer_S(S(value) / S(other.value));
    }

    constexpr inline Integer_S operator%(const Integer_S &other) const {
        return Integer_S(S(value) % S(other.value));
    }

    constexpr inline Integer_S operator>>(const M other) const {
        return Integer_S(S(value) >> other & shmask);
    }

    constexpr inline bool operator>(const Integer_S &other) const {
        return S(value) > S(other.value);
    }

    constexpr inline bool operator<(const Integer_S &other) const {
        return S(value) < S(other.value);
    }

    constexpr inline bool operator>=(const Integer_S &other) const {
        return S(value) >= S(other.value);
    }

    constexpr inline bool operator<=(const Integer_S &other) const {
        return S(value) <= S(other.value);
    }

    template<size_t size, bool sig>
    friend class Integer;

    friend Operators_Impl<Integer_S<U, S>>;
};

template<typename T>
struct Operators_Impl : public T {
    using T::T;

    constexpr inline Operators_Impl(const T &obj) : T(obj) { }

    constexpr inline Operators_Impl operator+() const {
        return *this;
    }

    constexpr inline Operators_Impl operator-() const {
        return Operators_Impl(-T::value);
    }

    constexpr inline Operators_Impl operator+(const Operators_Impl &other) const {
        return Operators_Impl(T::value + other.value);
    }

    constexpr inline Operators_Impl operator-(const Operators_Impl &other) const {
        return Operators_Impl(T::value - other.value);
    }

    constexpr inline Operators_Impl operator*(const Operators_Impl &other) const {
        return Operators_Impl(T::value * other.value);
    }

    constexpr inline Operators_Impl operator|(const Operators_Impl &other) const {
        return Operators_Impl(T::value | other.value);
    }

    constexpr inline Operators_Impl operator&(const Operators_Impl &other) const {
        return Operators_Impl(T::value & other.value);
    }

    constexpr inline Operators_Impl operator^(const Operators_Impl &other) const {
        return Operators_Impl(T::value ^ other.value);
    }

    constexpr inline Operators_Impl operator<<(const typename T::M other) const {
        return Operators_Impl(T::value << (other & T::shmask));
    }

    constexpr inline bool operator==(const Operators_Impl &other) const {
        return T::value == other.value;
    }

    constexpr inline bool operator!=(const Operators_Impl &other) const {
        return T::value != other.value;
    }

    constexpr inline Operators_Impl operator~() const {
        return Operators_Impl(~T::value);
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

private:

    constexpr inline Operators_Impl p1() const {
        return Operators_Impl(T::value + 1);
    }

    constexpr inline Operators_Impl m1() const {
        return Operators_Impl(T::value - 1);
    }

    template<size_t size, bool sig>
    friend class Integer;
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

template<size_t half, bool sig>
struct Pow2_Integer_Base;

template<size_t half>
struct Pow2_Integer_Base<half, false> {
private:
    typedef Integer<half, true> S;
    typedef Integer<half, false> U;
    typedef SHType<half> M;
    static const M shmask = half * 8 - 1;
    U low, high;
public:

    constexpr explicit inline Pow2_Integer_Base() : low(), high() { }

    constexpr explicit inline Pow2_Integer_Base(const U low) : low(low), high() { }

    constexpr explicit inline Pow2_Integer_Base(const S low) :
    low(low), high((low < S()) ? ~U() : U()) { }

    constexpr explicit inline Pow2_Integer_Base(const U low, const U high) :
    low(low), high(high) { }

    constexpr inline bool operator>(const Pow2_Integer_Base &other) const {
        return (S(high) > S(other.high)) ? true :
                ((S(high) < S(other.high)) ? false :
                (low > other.low));
    }

    constexpr inline bool operator<(const Pow2_Integer_Base &other) const {
        return (S(high) < S(other.high)) ? true :
                ((S(high) > S(other.high)) ? false :
                (low < other.low));
    }

    constexpr inline bool operator>=(const Pow2_Integer_Base &other) const {
        return (S(high) > S(other.high)) ? true :
                ((S(high) < S(other.high)) ? false :
                (low >= other.low));
    }

    constexpr inline bool operator<=(const Pow2_Integer_Base &other) const {
        return (S(high) < S(other.high)) ? true :
                ((S(high) > S(other.high)) ? false :
                (low <= other.low));
    }

    friend Pow2_Integer_Impl<half, false>;

    template<size_t size2, bool sig2>
    friend class Integer;
};

template<size_t half>
struct Pow2_Integer_Base<half, true> {
private:
    typedef Integer<half, true> S;
    typedef Integer<half, false> U;
    typedef SHType<half> M;
    static const M shmask = half * 8 - 1;
    U low, high;
public:

    constexpr explicit inline Pow2_Integer_Base() : low(), high() { }

    constexpr explicit inline Pow2_Integer_Base(const U low) : low(low), high() { }

    constexpr explicit inline Pow2_Integer_Base(const S low) :
    low(low), high((low < S()) ? ~S() : S()) { }

    constexpr explicit inline Pow2_Integer_Base(const U low, const S high) :
    low(low), high(high) { }

    constexpr inline bool operator>(const Pow2_Integer_Base &other) const {
        return (high > other.high) ? true :
                ((high < other.high) ? false :
                (low > other.low));
    }

    constexpr inline bool operator<(const Pow2_Integer_Base &other) const {
        return (high < other.high) ? true :
                ((high > other.high) ? false :
                (low < other.low));
    }

    constexpr inline bool operator>=(const Pow2_Integer_Base &other) const {
        return (high > other.high) ? true :
                ((high < other.high) ? false :
                (low >= other.low));
    }

    constexpr inline bool operator<=(const Pow2_Integer_Base &other) const {
        return (high < other.high) ? true :
                ((high > other.high) ? false :
                (low <= other.low));
    }

    template<size_t size2, bool sig2>
    friend class Pow2_Integer_Impl;

    template<size_t size2, bool sig2>
    friend class Integer;
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

    constexpr inline static I p1_helper(const U low, const U high) {
        return I(low, low == U() ? high.p1() : high);
    }

    constexpr inline static I plus_helper(
            const U low, const U oldlow, const U high) {
        return I(low, low < oldlow ? high.p1() : high);
    }

    constexpr inline I p1() const {
        return p1_helper(T::low.p1(), T::high);
    }

    constexpr inline I m1() const {
        return I(T::low.m1(), (T::low == U()) ? T::high.m1() : T::high);
    }
public:
    using T::T;

    constexpr inline Pow2_Integer_Impl(const T &obj) : T(obj) { }

    constexpr inline U getLow() const {
        return T::low;
    }

    constexpr inline U getHigh() const {
        return T::high;
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

    constexpr inline I operator+() const {
        return *this;
    }

    constexpr inline I operator-() const {
        return (~ * this).p1();
    }

    constexpr inline I operator+(const I &other) const {
        return plus_helper(T::low + other.low, T::low, T::high + other.high);
    }

    constexpr inline I operator-(const I &other) const {
        return *this +(-other);
    }

    /*inline Operators_Impl operator*(const Operators_Impl other) const {
        return Operators_Impl::value * other.value;
    }

    inline Operators_Impl operator<<(const Operators_Impl other) const {
        return Operators_Impl::value << (other.value & T::shmask);
    }*/

    constexpr inline I operator|(const I &other) const {
        return I(T::low | other.low, T::high | other.high);
    }

    constexpr inline I operator&(const I &other) const {
        return I(T::low & other.low, T::high & other.high);
    }

    constexpr inline I operator^(const I &other) const {
        return I(T::low ^ other.low, T::high ^ other.high);
    }

    constexpr inline I operator~() const {
        return I(~T::low, ~T::high);
    }

    constexpr inline bool operator==(const I &other) const {
        return (T::low == other.low) && (T::high == other.high);
    }

    constexpr inline bool operator!=(const I &other) const {
        return (T::low != other.low) || (T::high != other.high);
    }

    template<size_t size2, bool sig2>
    friend class Integer;
};

template<bool A>
using enable_if_t = typename std::enable_if<A, bool>::type;

#define enable_if(B) enable_if_t<(B)> = false

template<typename T, size_t size, enable_if(!std::is_integral<T>::value)>
constexpr inline bool can_upcast() {
    return false;
}

template<typename T, size_t size, enable_if(std::is_integral<T>::value)>
constexpr inline bool can_upcast() {
    return size >= sizeof (T);
}

template<typename T, enable_if(!std::is_integral<T>::value)>
constexpr inline bool is_integral() {
    return false;
}

template<typename T, enable_if(std::is_integral<T>::value)>
constexpr inline bool is_integral() {
    return true;
}

template<typename T, enable_if(!std::is_signed<T>::value)>
constexpr inline bool is_signed() {
    return false;
}

template<typename T, enable_if(std::is_signed<T>::value)>
constexpr inline bool is_signed() {
    return true;
}

template<typename U, typename S, bool sig, typename V, enable_if(sig)>
constexpr inline S castUS(V value) {
    return S(value);
}

template<typename U, typename S, bool sig, typename V, enable_if(!sig)>
constexpr inline U castUS(V value) {
    return U(value);
}

template<size_t size, bool sig>
struct Integer {
private:
    using V = typename Integer_Impl<size, sig>::type;
    V value;

    template<size_t size2, bool sig2,
    enable_if((getIntegerType(size) == native) &&
            (getIntegerType(size2) == native))>
    constexpr inline Integer<size2, sig2> upcast() const {
        using I = Integer<size2, sig2>;
        return typename I::V(castUS<typename I::V::U, typename I::V::S,
                sig, typename V::U > (value.value));
    }

    template<size_t size2, bool sig2,
    enable_if((size == size2) && (getIntegerType(size) == pow2))>
    constexpr inline Integer<size, sig2> upcast() const {
        using I = Integer<size2, sig2>;
        return I(typename I::V(value.low, value.high));
    }

    template<size_t size2, bool sig2,
    enable_if((size != size2) &&
            ((getIntegerType(size) == native) ||
            (getIntegerType(size) == pow2)) &&
            (getIntegerType(size2) == pow2))>
    constexpr inline Integer<size2, sig2> upcast() const {
        using I = Integer<size2, sig2>;
        return typename I::V(castUS<typename I::V::U, typename I::V::S,
                sig, Integer<size, sig> > (*this));
    }

    template<size_t size2, bool sig2,
    enable_if((getIntegerType(size) == native) &&
            (getIntegerType(size2) == native))>
    constexpr inline Integer<size2, sig2> downcast() const {
        using I = Integer<size2, sig2>;
        return typename I::V(typename I::V::U(value.value));
    }

    template<size_t size2, bool sig2,
    enable_if((getIntegerType(size) == pow2) &&
            (getIntegerType(size2) == native))>
    constexpr inline Integer<size2, sig2> downcast() const {
        using I = Integer<size2, sig2>;
        return I(value.low);
    }

    constexpr inline Integer p1() const {
        return value.p1();
    }

    constexpr inline Integer m1() const {
        return value.m1();
    }

    constexpr inline Integer(const V n) : value(n) { }
public:

    constexpr inline static size_t SIZE() {
        return size;
    };

    constexpr inline static bool IS_SIGNED() {
        return sig;
    };

    constexpr inline static Integer ZERO() {
        return Integer();
    };

    constexpr inline static Integer ONE() {
        return ZERO().p1();
    };

    constexpr inline Integer() : value() { }

    template<typename T, enable_if((getIntegerType(size) == native) &&
            (can_upcast<T, size>()))>
    constexpr inline Integer(const T n) : value(n) { }

    template<typename T, enable_if((getIntegerType(size) != native) &&
            (can_upcast<T, size>()))>
    constexpr inline Integer(const T n) :
    value(Integer(Integer<sizeof (T), is_signed<T>()>(n)).value) { }

    template<size_t size2, bool sig2,
    enable_if(size2 >= size)>
    constexpr inline operator Integer<size2, sig2>() const {
        return upcast<size2, sig2>();
    }

    template<size_t size2, bool sig2,
    enable_if(size2 < size)>
    constexpr explicit inline operator Integer<size2, sig2>() const {
        return downcast<size2, sig2>();
    }

    template<typename T, enable_if((is_integral<T>())&&
            (getIntegerType(size) == native) &&
            (sizeof (T) >= size))>
    constexpr inline operator T() const {
        return T(castUS<typename V::U, typename V::S,
                sig, typename V::U > (value.value));
    }

    template<typename T, enable_if((is_integral<T>())&&
            (getIntegerType(size) == native) &&
            (sizeof (T) < size))>
    constexpr explicit inline operator T() const {
        return T(value.value);
    }

    template<typename T, enable_if((is_integral<T>())&&
            (getIntegerType(size) != native) &&
            (sizeof (T) < size))>
    constexpr explicit inline operator T() const {
        return T(Integer<sizeof (T), is_signed<T>()>(*this));
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator+(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator-(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator*(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator/(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator%(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator|(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator&(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<max(size1, size2), sig1 && sig2> operator^(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr bool operator==(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr bool operator!=(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr bool operator<=(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr bool operator>=(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr bool operator<(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr bool operator>(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<size1, sig1> operator<<(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<typename T, enable_if((is_integral<T>()))>
    constexpr inline Integer operator<<(const T v2) const {
        return value << SHType<size>(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    friend constexpr Integer<size1, sig1> operator>>(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2);

    template<typename T, enable_if((is_integral<T>()))>
    constexpr inline Integer operator>>(const T v2) const {
        return value >> SHType<size>(v2);
    }

    constexpr inline Integer operator~() const {
        return ~value;
    }

    constexpr inline Integer operator-() const {
        return -value;
    }

    constexpr inline Integer operator+() const {
        return *this;
    }

    template<size_t size2, bool sig2>
    friend class Integer;


    template<size_t size2, bool sig2>
    friend class Pow2_Integer_Impl;
};

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator+(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value + R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator+(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) + R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator+(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) + R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator-(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value - R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator-(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) - R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator-(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) - R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator*(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value * R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator*(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) * R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator*(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) * R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator/(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value / R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator/(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) / R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator/(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) / R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator%(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value % R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator%(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) % R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator%(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) % R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator|(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value | R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator|(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) | R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator|(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) | R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator&(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value & R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator&(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) & R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator&(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) & R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<max(size1, size2), sig1 && sig2> operator^(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value ^ R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator^(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) ^ R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline Integer<max(size1, sizeof (T)), sig1 && (is_signed<T>())>
operator^(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) ^ R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline bool operator==(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value == R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator==(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) == R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator==(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) == R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline bool operator!=(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value != R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator!=(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) != R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator!=(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) != R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline bool operator<=(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value <= R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator<=(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) <= R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator<=(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) <= R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline bool operator>=(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value >= R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator>=(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) >= R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator>=(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) >= R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline bool operator<(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value < R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator<(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) < R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator<(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) < R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline bool operator>(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    using R = Integer < max(size1, size2), sig1 && sig2>;
    return R(v1).value > R(v2).value;
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator>(const Integer<size1, sig1> &v1, const T v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) > R(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline bool operator>(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer < max(size1, sizeof (T)), sig1 && (is_signed<T>())>;
    return R(v1) > R(v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<size1, sig1> operator<<(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    return v1.value << SHType<size1>(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline T operator<<(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer <sizeof (T), (is_signed<T>())>;
    return T(R(v1) << v2);
}

template<size_t size1, bool sig1, size_t size2, bool sig2>
constexpr inline Integer<size1, sig1> operator>>(
        const Integer<size1, sig1> &v1,
        const Integer<size2, sig2> &v2) {
    return v1.value >> SHType<size1>(v2);
}

template<size_t size1, bool sig1, typename T,
enable_if((is_integral<T>()))>
constexpr inline T operator>>(const T v1, const Integer<size1, sig1> &v2) {
    using R = Integer <sizeof (T), (is_signed<T>())>;
    return T(R(v1) >> v2);
}

#endif /* INTEGER_HPP */
