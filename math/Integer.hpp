#ifndef INTEGER_HPP
#define INTEGER_HPP

#include <cstdint>
#include <type_traits>
#include <iostream>
#include <utility>

// -std=c++14
namespace JIO {

    template<bool A>
    using p_enable_if_t = typename std::enable_if<A, bool>::type;

#define p_enable_if(B) p_enable_if_t<(B)> = false

    template<typename T>
    constexpr inline T lowestOneBit(const T i) {
        // HD, Section 2-1
        return i & -i;
    }

    template<typename T>
    constexpr inline bool isOneBit(const T i) {
        return lowestOneBit(i) == i;
    }

    template<typename T>
    constexpr const T& max(const T &v1, const T &v2) {
        return v1 < v2 ? v2 : v1;
    }

    constexpr inline size_t p_numberOfLeadingZeros2Bit_h(uint8_t i) {
        return i ? (1 - (i >> 1)) : 2;
    }

    constexpr inline size_t p_numberOfLeadingZeros4Bit_h(uint8_t i) {
        return (i >= 1 << 2) ?
                p_numberOfLeadingZeros2Bit_h(i >> 2) :
                p_numberOfLeadingZeros2Bit_h(i) + 2;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint8_t i) {
        return (i >= 1 << 4) ?
                p_numberOfLeadingZeros4Bit_h(i >> 4) :
                p_numberOfLeadingZeros4Bit_h(i) + 4;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint16_t i) {
        return (i >= 1 << 8) ?
                p_numberOfLeadingZeros_h(uint8_t(i >> 8)) :
                p_numberOfLeadingZeros_h(uint8_t(i)) + 8;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint32_t i) {
        return (i >= 1 << 16) ?
                p_numberOfLeadingZeros_h(uint16_t(i >> 16)) :
                p_numberOfLeadingZeros_h(uint16_t(i)) + 16;
    }

    constexpr inline size_t p_numberOfLeadingZeros_h(uint64_t i) {
        return (i >= uint64_t(1) << 32) ?
                p_numberOfLeadingZeros_h(uint32_t(i >> 32)) :
                p_numberOfLeadingZeros_h(uint32_t(i)) + 32;
    }

    constexpr inline size_t p_numberOfTrailingZeros2Bit_h(uint8_t i) {
        return i ? (1 - (i & 1)) : 2;
    }

    constexpr inline size_t p_numberOfTrailingZeros4Bit_h(uint8_t i) {
        return (i & 0x3) ?
                p_numberOfTrailingZeros2Bit_h(i) :
                p_numberOfTrailingZeros2Bit_h(i >> 2) + 2;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint8_t i) {
        return (i & 0xf) ?
                p_numberOfTrailingZeros4Bit_h(i) :
                p_numberOfTrailingZeros4Bit_h(i >> 4) + 4;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint16_t i) {
        return (uint8_t(i)) ?
                p_numberOfTrailingZeros_h(uint8_t(i)) :
                p_numberOfTrailingZeros_h(uint8_t(i >> 8)) + 8;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint32_t i) {
        return (uint16_t(i)) ?
                p_numberOfTrailingZeros_h(uint16_t(i)) :
                p_numberOfTrailingZeros_h(uint16_t(i >> 16)) + 16;
    }

    constexpr inline size_t p_numberOfTrailingZeros_h(uint64_t i) {
        return (uint32_t(i)) ?
                p_numberOfTrailingZeros_h(uint32_t(i)) :
                p_numberOfTrailingZeros_h(uint32_t(i >> 32)) + 32;
    }

    enum p_IType {
        illegal = 0,
        native,
        pow2,
        array
    };

    constexpr p_IType p_getIntegerType(size_t size) {
        if (!size) {
            return illegal;
        }
        if (size == 1 || size == 2 || size == 4 || size == 8) {
            return native;
        }
        return isOneBit(size) ? pow2 : array;
    }

    template<size_t size, bool = (size > sizeof (unsigned int))>
    struct p_SHValue {
        typedef unsigned int type;
    };

    template<size_t size>
    struct p_SHValue<size, true> {
        typedef size_t type;
    };

    template<size_t size>
    using p_SHType = typename p_SHValue<size>::type;

    template<size_t size, bool sig>
    class Integer;

    template<size_t size, bool sig, p_IType = p_getIntegerType(size)>
    struct p_Integer_Impl;

    template<typename T>
    struct p_Operators_Impl;

    template<typename U, typename S>
    class p_Integer_S;

    template<typename UT, typename ST>
    class p_Integer_U {
    private:
        typedef ST S;
        typedef UT U;
        typedef p_Integer_U I;
        typedef p_SHType<sizeof (U) > M;
        constexpr static M shmask = sizeof (U) * 8 - 1;
        U value;
    public:

        constexpr inline p_Integer_U() = default;

        constexpr explicit inline p_Integer_U(const U n) : value(n) { }

        constexpr inline bool isNegative() const {
            return false;
        };

        constexpr inline bool isSNegative() const {
            return S(value) < 0;
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            U tmp = v1.value + v2.value;
            out = I(tmp);
            return tmp < v1.value;
        }

        constexpr inline static bool increment_overflow(I &value) {
            return (++value.value) == 0;
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            U tmp = v1.value - v2.value;
            out = I(tmp);
            return tmp > v1.value;
        }

        constexpr inline static bool decrement_overflow(I &value) {
            return (value.value--) == 0;
        }

        constexpr inline I operator/(const I &other) const {
            return p_Integer_U(value / other.value);
        }

        constexpr inline I operator%(const I &other) const {
            return p_Integer_U(value % other.value);
        }

        constexpr inline I operator>>(const M other) const {
            return p_Integer_U(value >> (other & shmask));
        }

        constexpr inline bool operator>(const I &other) const {
            return value > other.value;
        }

        constexpr inline bool operator<(const I &other) const {
            return value < other.value;
        }

        constexpr inline bool operator>=(const I &other) const {
            return value >= other.value;
        }

        constexpr inline bool operator<=(const I &other) const {
            return value <= other.value;
        }

        template<size_t size, bool sig>
        friend class Integer;

        friend p_Operators_Impl<p_Integer_U<U, S>>;
    };

    template<typename UT, typename ST>
    class p_Integer_S {
    private:
        typedef ST S;
        typedef UT U;
        typedef p_Integer_S I;
        typedef p_SHType<sizeof (U) > M;
        constexpr static M shmask = sizeof (U) * 8 - 1;
        U value;
    public:

        constexpr inline p_Integer_S() = default;

        constexpr explicit inline p_Integer_S(const S n) : value(n) { }

        constexpr inline bool isNegative() const {
            return S(value) < 0;
        };

        constexpr inline bool isSNegative() const {
            return S(value) < 0;
        };

        template<size_t size, size_t = size>
        struct divrem_h {

            constexpr inline static I div(const I &a, const I &b) {
                return I(S(a.value) / S(b.value));
            }

            constexpr inline static I rem(const I &a, const I &b) {
                return I(S(a.value) % S(b.value));
            }
        };

        template<size_t size>
        struct divrem_h<size, 4> {

            constexpr inline static I div(const I &a, const I &b) {
                if ((a.value == U(0x80000000L)) && (b.value == U(-1L))) {
                    return I(0x80000000L);
                }
                return I(S(a.value) / S(b.value));
            }

            constexpr inline static I rem(const I &a, const I &b) {
                if ((a.value == U(0x80000000L)) && (b.value == U(-1L))) {
                    return I(0);
                }
                return I(S(a.value) % S(b.value));
            }
        };

        template<size_t size>
        struct divrem_h<size, 8> {

            constexpr inline static I div(const I &a, const I &b) {
                if ((a.value == U(0x8000000000000000LL)) && (b.value == U(-1LL))) {
                    return I(0x8000000000000000LL);
                }
                return I(S(a.value) / S(b.value));
            }

            constexpr inline static I rem(const I &a, const I &b) {
                if ((a.value == U(0x8000000000000000LL)) && (b.value == U(-1LL))) {
                    return I(0);
                }
                return I(S(a.value) % S(b.value));
            }
        };

        constexpr inline I operator/(const I &other) const {
            return divrem_h<sizeof (U)>::div(*this, other);
        }

        constexpr inline I operator%(const I &other) const {
            return divrem_h<sizeof (U)>::rem(*this, other);
        }

        constexpr inline I operator>>(const M other) const {
            return I(S(value) >> (other & shmask));
        }

        constexpr inline bool operator>(const I &other) const {
            return S(value) > S(other.value);
        }

        constexpr inline bool operator<(const I &other) const {
            return S(value) < S(other.value);
        }

        constexpr inline bool operator>=(const I &other) const {
            return S(value) >= S(other.value);
        }

        constexpr inline bool operator<=(const I &other) const {
            return S(value) <= S(other.value);
        }

        template<size_t size, bool sig>
        friend class Integer;

        friend p_Operators_Impl<p_Integer_S<U, S>>;
    };

    template<typename T>
    struct p_Operators_Impl : public T {
    private:

        typedef p_Operators_Impl I;

    public:

        using T::T;

        constexpr inline p_Operators_Impl(const T &obj) : T(obj) { }

        constexpr inline static I ZERO() {
            return T(0);
        };

        constexpr inline bool isZero() const {
            return T::value == 0;
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            return T::add_overflow(v1, v2, out);
        }

        constexpr inline static bool increment_overflow(I &value) {
            return T::increment_overflow(value);
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            return T::sub_overflow(v1, v2, out);
        }

        constexpr inline static bool decrement_overflow(I &value) {
            return T::decrement_overflow(value);
        }

        void printv(std::ostream &out) {
            out << std::hex << uint64_t(T::value) << std::dec;
        }

        constexpr inline size_t numberOfLeadingZeros() const {
            return p_numberOfLeadingZeros_h(T::value);
        }

        constexpr inline size_t numberOfTrailingZeros() const {
            return p_numberOfTrailingZeros_h(T::value);
        }

        constexpr inline I operator+() const {
            return *this;
        }

        constexpr inline I operator-() const {
            return I(-T::value);
        }

        constexpr inline I operator+(const I &other) const {
            return I(T::value + other.value);
        }

        constexpr inline I operator-(const I &other) const {
            return I(T::value - other.value);
        }

        constexpr inline I operator*(const I &other) const {
            return I(T::value * other.value);
        }

        constexpr inline I operator|(const I &other) const {
            return I(T::value | other.value);
        }

        constexpr inline I operator&(const I &other) const {
            return I(T::value & other.value);
        }

        constexpr inline I operator^(const I &other) const {
            return I(T::value ^ other.value);
        }

        constexpr inline I operator<<(const typename T::M other) const {
            return I(T::value << (other & T::shmask));
        }

        constexpr inline bool operator==(const I &other) const {
            return T::value == other.value;
        }

        constexpr inline bool operator!=(const I &other) const {
            return T::value != other.value;
        }

        constexpr inline I operator~() const {
            return I(~T::value);
        }

    private:

        constexpr inline I p1() const {
            return I(T::value + 1);
        }

        constexpr inline I m1() const {
            return I(T::value - 1);
        }

        template<size_t size, bool sig>
        friend class Integer;
    };

    template<>
    struct p_Integer_Impl<1, false> {
        typedef p_Operators_Impl<p_Integer_U<uint8_t, int8_t>> type;
    };

    template<>
    struct p_Integer_Impl<1, true> {
        typedef p_Operators_Impl<p_Integer_S<uint8_t, int8_t>> type;
    };

    template<>
    struct p_Integer_Impl<2, false> {
        typedef p_Operators_Impl<p_Integer_U<uint16_t, int16_t>> type;
    };

    template<>
    struct p_Integer_Impl<2, true> {
        typedef p_Operators_Impl<p_Integer_S<uint16_t, int16_t>> type;
    };

    template<>
    struct p_Integer_Impl<4, false> {
        typedef p_Operators_Impl<p_Integer_U<uint32_t, int32_t>> type;
    };

    template<>
    struct p_Integer_Impl<4, true> {
        typedef p_Operators_Impl<p_Integer_S<uint32_t, int32_t>> type;
    };

    template<>
    struct p_Integer_Impl<8, false> {
        typedef p_Operators_Impl<p_Integer_U<uint64_t, int64_t>> type;
    };

    template<>
    struct p_Integer_Impl<8, true> {
        typedef p_Operators_Impl<p_Integer_S<uint64_t, int64_t>> type;
    };

    template<size_t half, bool sig>
    class p_Pow2_Integer_Impl;

    template<size_t size, bool sig>
    struct p_Integer_Impl <size, sig, p_IType::pow2> {
        typedef p_Pow2_Integer_Impl<size / 2, sig> type;
    };

    template<size_t half, bool sig>
    class p_Pow2_Integer_Base;

    template<size_t half>
    class p_Pow2_Integer_Base<half, false> {
    private:
        typedef Integer<half, true> S;
        typedef Integer<half, false> U;
        typedef p_Pow2_Integer_Base<half, false> I;
        typedef p_SHType<half * 2> M;
        constexpr static M shmask = half * 2 * 8 - 1;
        U low, high;

        constexpr inline static I rightShift(
                const I &value, const M shiftDistance) {
            if (shiftDistance == 0) {
                return value;
            }
            if (shiftDistance < (half * 8)) {
                return I((value.low >> shiftDistance) |
                        (value.high << (half * 8 - shiftDistance)),
                        value.high >> shiftDistance);
            }
            return I(value.high >> (shiftDistance - half * 8), U::ZERO());
        }
    public:

        constexpr inline p_Pow2_Integer_Base() = default;

        constexpr explicit inline p_Pow2_Integer_Base(const U &low) :
        low(low), high(U::ZERO()) { }

        constexpr explicit inline p_Pow2_Integer_Base(const S &low) :
        low(low), high(low.isSNegative() ? ~U::ZERO() : U::ZERO()) { }

        constexpr inline p_Pow2_Integer_Base(const U &low, const U &high) :
        low(low), high(high) { }

        constexpr inline bool isNegative() const {
            return false;
        };

        constexpr inline bool isSNegative() const {
            return high.isSNegative();
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            bool tmpo = U::add_overflow(v1.high, v2.high, out.high);
            if (U::add_overflow(v1.low, v2.low, out.low)) {
                return U::increment_overflow(out.high) || tmpo;
            }
            return tmpo;
        }

        constexpr inline static bool increment_overflow(I &value) {
            if (U::increment_overflow(value.low)) {
                return U::increment_overflow(value.high);
            }
            return false;
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            bool tmpo = U::sub_overflow(v1.high, v2.high, out.high);
            if (U::sub_overflow(v1.low, v2.low, out.low)) {
                return U::decrement_overflow(out.high) || tmpo;
            }
            return tmpo;
        }

        constexpr inline static bool decrement_overflow(I &value) {
            if (U::decrement_overflow(value.low)) {
                return U::decrement_overflow(value.high);
            }
            return false;
        }

        constexpr inline bool operator>(const I &other) const {
            return (high > other.high) ? true :
                    ((high < other.high) ? false :
                    (low > other.low));
        }

        constexpr inline bool operator<(const I &other) const {
            return (high < other.high) ? true :
                    ((high > other.high) ? false :
                    (low < other.low));
        }

        constexpr inline bool operator>=(const I &other) const {
            return (high > other.high) ? true :
                    ((high < other.high) ? false :
                    (low >= other.low));
        }

        constexpr inline bool operator<=(const I &other) const {
            return (high < other.high) ? true :
                    ((high > other.high) ? false :
                    (low <= other.low));
        }

        constexpr inline I operator>>(const M other) const {
            return rightShift(*this, other & shmask);
        }

        template<size_t size2, bool sig2>
        friend class p_Pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half>
    class p_Pow2_Integer_Base<half, true> {
    private:
        typedef Integer<half, true> S;
        typedef Integer<half, false> U;
        typedef p_Pow2_Integer_Base<half, true> I;
        typedef p_SHType<half * 2> M;
        constexpr static M shmask = half * 2 * 8 - 1;
        U low, high;

        inline static I rightShift(
                const I &value, const M shiftDistance) {
            if (shiftDistance == 0) {
                return value;
            }
            if (shiftDistance < (half * 8)) {
                return I((value.low >> shiftDistance) |
                        (value.high << (half * 8 - shiftDistance)),
                        S(value.high) >> shiftDistance);
            }
            return I(S(value.high) >> (shiftDistance - half * 8),
                    value.high.isSNegative() ? ~U::ZERO() : U::ZERO());
        }
    public:

        constexpr inline p_Pow2_Integer_Base() = default;

        constexpr explicit inline p_Pow2_Integer_Base(const U &low) :
        low(low), high(U::ZERO()) { }

        constexpr explicit inline p_Pow2_Integer_Base(const S &low) :
        low(low), high(low.isNegative() ? ~U::ZERO() : U::ZERO()) { }

        constexpr inline p_Pow2_Integer_Base(const U &low, const U &high) :
        low(low), high(high) { }

        constexpr inline bool isNegative() const {
            return high.isSNegative();
        };

        constexpr inline bool isSNegative() const {
            return high.isSNegative();
        };

        constexpr inline bool operator>(const I &other) const {
            return (S(high) > S(other.high)) ? true :
                    ((S(high) < S(other.high)) ? false :
                    (low > other.low));
        }

        constexpr inline bool operator<(const I &other) const {
            return (S(high) < S(other.high)) ? true :
                    ((S(high) > S(other.high)) ? false :
                    (low < other.low));
        }

        constexpr inline bool operator>=(const I &other) const {
            return (S(high) > S(other.high)) ? true :
                    ((S(high) < S(other.high)) ? false :
                    (low >= other.low));
        }

        constexpr inline bool operator<=(const I &other) const {
            return (S(high) < S(other.high)) ? true :
                    ((S(high) > S(other.high)) ? false :
                    (low <= other.low));
        }

        constexpr inline I operator>>(const M other) const {
            return rightShift(*this, other & shmask);
        }

        template<size_t size2, bool sig2>
        friend class p_Pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class Integer;
    };

    template<size_t half, bool sig>
    class p_Pow2_Integer_Impl : public p_Pow2_Integer_Base<half, sig> {
    private:
        typedef p_Pow2_Integer_Base<half, sig> T;
        typedef p_Pow2_Integer_Impl<half, sig> I;
        typedef p_Pow2_Integer_Impl<half, false> UI;
        typedef p_Pow2_Integer_Impl<half, true> SI;
        typedef typename T::S S;
        typedef typename T::U U;
        typedef typename T::M M;

        constexpr inline static I leftShift(const I &value,
                const typename T::M shiftDistance) {
            if (shiftDistance == 0) {
                return value;
            }
            if (shiftDistance < (half * 8)) {
                return I(value.low << shiftDistance,
                        (value.high << shiftDistance) |
                        (value.low >> (half * 8 - shiftDistance)));
            }
            return I(U::ZERO(), value.low << (shiftDistance - half * 8));
        }

        constexpr inline static UI divremUnsigned_h5(
                const UI &out, bool bit) {
            return (out << 1) | (bit ? UI(U(1)) : UI::ZERO());
        }

        constexpr inline static std::pair<UI, UI> divremUnsigned_h4(
                const UI &nx, const UI &ny, size_t i, const UI &out) {
            return i == 0 ? std::pair<UI, UI>(out, nx) :
                    (divremUnsigned_h2(nx, ny, i - 1, out));
        }

        constexpr inline static std::pair<UI, UI> divremUnsigned_h3(
                const UI &x, const UI &y, bool xmey, size_t i, const UI &out) {
            return divremUnsigned_h4(xmey ? x - y : x,
                    y >> 1, i, divremUnsigned_h5(out, xmey));
        }

        constexpr inline static std::pair<UI, UI> divremUnsigned_h2(
                const UI &x, const UI &y, size_t i, const UI &out) {
            return divremUnsigned_h3(x, y, x >= y, i, out);
        }

        constexpr inline static std::pair<UI, UI> divremUnsigned_h1(
                const UI &x, const UI &y, size_t xZeros, size_t yZeros) {
            return yZeros < xZeros ? std::pair<UI, UI>(UI::ZERO(), x) :
                    divremUnsigned_h2(x, y << (yZeros - xZeros),
                    yZeros - xZeros, UI::ZERO());
        }

        constexpr inline static std::pair<UI, UI> divremUnsigned(
                const UI &x, const UI &y) {
            return y.isZero() ? throw std::runtime_error("Dividing by zero") :
                    divremUnsigned_h1(x, y, x.numberOfLeadingZeros(),
                    y.numberOfLeadingZeros());
        }

        constexpr inline static std::pair<SI, SI> divremSigned_h2(
                const std::pair<UI, UI> &tmp, bool neg1, bool neg2) {
            return std::pair<SI, SI>((neg1 ? -tmp.first : tmp.first).s(),
                    (neg2 ? -tmp.second : tmp.second).s());
        }

        constexpr inline static std::pair<SI, SI> divremSigned_h1(
                const SI &x, const SI &y, bool xNeg, bool yNeg) {
            return divremSigned_h2(
                    divremUnsigned((xNeg ? -x : x).u(), (yNeg ? -y : y).u()),
                    xNeg ^ yNeg, xNeg);
        }

        constexpr inline static std::pair<SI, SI> divremSigned(
                const SI &x, const SI &y) {
            return divremSigned_h1(x, y, x.isNegative(), y.isNegative());
        }

        constexpr inline I p1() const {
            I tmp = *this;
            if (U::increment_overflow(tmp.low)) {
                ++tmp.high;
            }
            return tmp;
        }

        constexpr inline I m1() const {
            I tmp = *this;
            if (U::decrement_overflow(tmp.low)) {
                --tmp.high;
            }
            return tmp;
        }
    public:
        using T::T;

        template<bool sig2>
        constexpr inline p_Pow2_Integer_Impl(
                const p_Pow2_Integer_Impl<half, sig2> &other) :
        T(other.low, other.high) { }

        constexpr inline p_Pow2_Integer_Impl(const T &obj) : T(obj) { }

        constexpr inline SI s() const {
            return SI(T::low, T::high);
        }

        constexpr inline UI u() const {
            return UI(T::low, T::high);
        }

        constexpr inline static I ZERO() {
            return I(U::ZERO());
        };

        constexpr inline bool isZero() const {
            return T::low.isZero() && T::high.isZero();
        };

        constexpr inline static bool add_overflow(
                const I &v1, const I &v2, I &out) {
            return T::add_overflow(v1, v2, out);
        }

        constexpr inline static bool increment_overflow(I &value) {
            return T::increment_overflow(value);
        }

        constexpr inline static bool sub_overflow(
                const I &v1, const I &v2, I &out) {
            return T::sub_overflow(v1, v2, out);
        }

        constexpr inline static bool decrement_overflow(I &value) {
            return T::decrement_overflow(value);
        }

        void printv(std::ostream &out) {
            T::high.printv(out);
            out << ", ";
            T::low.printv(out);
        }

        constexpr inline size_t numberOfLeadingZeros() const {
            if (T::high.isZero()) {
                return T::low.numberOfLeadingZeros() + half * 8;
            }
            return T::high.numberOfLeadingZeros();
        }

        constexpr inline size_t numberOfTrailingZeros() const {
            if (T::low.isZero()) {
                return T::high.numberOfTrailingZeros() + half * 8;
            }
            return T::low.numberOfTrailingZeros();
        }

        constexpr inline I operator+() const {
            return *this;
        }

        constexpr inline I operator-() const {
            return (~(*this)).p1();
        }

        template<bool sig2, p_enable_if(!(sig || sig2))>
        constexpr inline I operator/(const p_Pow2_Integer_Impl<half, sig2> &other) const {
            return divremUnsigned(*this, other).first;
        }

        template<bool sig2, p_enable_if(sig && sig2)>
        constexpr inline I operator/(const p_Pow2_Integer_Impl<half, sig2> &other) const {
            return divremSigned(*this, other).first;
        }

        template<bool sig2, p_enable_if(!(sig || sig2))>
        constexpr inline I operator%(const p_Pow2_Integer_Impl<half, sig2> &other) const {
            return divremUnsigned(*this, other).second;
        }

        template<bool sig2, p_enable_if(sig && sig2)>
        constexpr inline I operator%(const p_Pow2_Integer_Impl<half, sig2> &other) const {
            return divremSigned(*this, other).second;
        }

        constexpr inline I operator+(const I &other) const {
            U tmph = T::high + other.high;
            U tmpl; //unintialized, may give warnings
            if (U::add_overflow(T::low, other.low, tmpl)) {
                ++tmph;
            }
            return I(tmpl, tmph);
        }

        constexpr inline I operator-(const I &other) const {
            U tmph = T::high - other.high;
            U tmpl = U::ZERO();
            if (U::sub_overflow(T::low, other.low, tmpl)) {
                --tmph;
            }
            return I(tmpl, tmph);
        }

        constexpr inline I operator*(const I &other) const {
            auto tmp = wmultiply(T::low, other.low).value;
            return I(tmp.low, tmp.high + T::low * other.high + T::high * other.low);
        }

        constexpr inline I operator<<(const M other) const {
            return leftShift(*this, other & T::shmask);
        }

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

    template<typename T1, typename T2>
    struct p_compare_types {
        constexpr static bool value = false;
    };

    template<typename T>
    struct p_compare_types <T, T> {
        constexpr static bool value = true;
    };

    template<typename T1, typename T2>
    constexpr inline bool p_compare_types_cv() {
        return p_compare_types<typename std::remove_cv<T1>::type,
                typename std::remove_cv<T2>::type>::value;
    }

    template<typename T>
    constexpr inline bool p_is_integral() {
        return std::is_integral<T>::value && (!p_compare_types_cv<T, bool>());
    }

    template<typename T>
    constexpr inline bool p_is_signed() {
        return std::is_signed<T>::value;
    }

    template<typename U, typename S, bool sig, typename V, p_enable_if(sig)>
    constexpr inline S p_castUS(V value) {
        return S(value);
    }

    template<typename U, typename S, bool sig, typename V, p_enable_if(!sig)>
    constexpr inline U p_castUS(V value) {
        return U(value);
    }

    template<typename T, bool sig, p_enable_if(sig)>
    constexpr inline T p_min_value() {
        return T(1) << (sizeof (T) * 8 - 1);
    }

    template<typename T, bool sig, p_enable_if(!sig)>
    constexpr inline T p_min_value() {
        return T::ZERO();
    }

    template<typename T, bool sig>
    constexpr inline T p_max_value() {
        return ~p_min_value<T, sig>();
    }

    template<size_t size, bool sig>
    class Integer {
    private:
        using V = typename p_Integer_Impl<size, sig>::type;
        V value;

        template<size_t size2, bool sig2,
        bool = (p_getIntegerType(size) == native) &&
        (p_getIntegerType(size2) == native),
        bool = (size == size2) && (p_getIntegerType(size) == pow2),
        bool = (size != size2) &&
        ((p_getIntegerType(size) == native) ||
                (p_getIntegerType(size) == pow2)) &&
        (p_getIntegerType(size2) == pow2)>
        struct upcast_h {
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, true, false, false> {

            constexpr inline static Integer<size2, sig2> upcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return typename I::V(p_castUS<typename V::U,
                        typename V::S, sig > (v.value.value));
            }
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, false, true, false> {

            constexpr inline static Integer<size2, sig2> upcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return I(typename I::V(v.value.low, v.value.high));
            }
        };

        template<size_t size2, bool sig2>
        struct upcast_h<size2, sig2, false, false, true> {

            constexpr inline static Integer<size2, sig2> upcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return typename I::V(p_castUS<typename I::V::U,
                        typename I::V::S, sig > (v));
            }
        };

        template<size_t size2, bool sig2>
        constexpr inline Integer<size2, sig2> upcast() const {
            return upcast_h<size2, sig2>::upcast(*this);
        }

        template<size_t size2, bool sig2,
        bool = (p_getIntegerType(size) == native) &&
        (p_getIntegerType(size2) == native),
        bool = (p_getIntegerType(size) == pow2) &&
        ((p_getIntegerType(size2) == pow2) ||
                (p_getIntegerType(size2) == native))>
        struct downcast_h {
        };

        template<size_t size2, bool sig2>
        struct downcast_h<size2, sig2, true, false> {

            constexpr inline static Integer<size2, sig2> downcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return typename I::V(typename I::V::U(v.value.value));
            }
        };

        template<size_t size2, bool sig2>
        struct downcast_h<size2, sig2, false, true> {

            constexpr inline static Integer<size2, sig2> downcast(const Integer &v) {
                using I = Integer<size2, sig2>;
                return I(v.value.low);
            }
        };

        template<size_t size2, bool sig2>
        constexpr inline Integer<size2, sig2> downcast() const {
            return downcast_h<size2, sig2>::downcast(*this);
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

        constexpr inline static bool is_signed() {
            return sig;
        };

        constexpr inline static Integer ZERO() {
            return V::ZERO();
        };

        constexpr inline static Integer ONE() {
            return ZERO().p1();
        };

        constexpr inline static Integer MAX_VALUE() {
            return p_max_value<Integer, sig>();
        };

        constexpr inline static Integer MIN_VALUE() {
            return p_min_value<Integer, sig>();
        };

        constexpr inline Integer() = default;

        constexpr inline bool isZero() const {
            return value.isZero();
        };

        constexpr inline bool isNegative() const {
            return value.isNegative();
        };

        constexpr inline bool isSNegative() const {
            return value.isSNegative();
        };

        constexpr inline static bool add_overflow(
                const Integer &v1, const Integer &v2, Integer &out) {
            return V::add_overflow(v1.value, v2.value, out.value);
        }

        constexpr inline static bool increment_overflow(Integer &value) {
            return V::increment_overflow(value.value);
        }

        constexpr inline static bool sub_overflow(
                const Integer &v1, const Integer &v2, Integer &out) {
            return V::sub_overflow(v1.value, v2.value, out.value);
        }

        constexpr inline static bool decrement_overflow(Integer &value) {
            return V::decrement_overflow(value.value);
        }

        constexpr inline size_t numberOfLeadingZeros() const {
            return value.numberOfLeadingZeros();
        }

        constexpr inline size_t numberOfTrailingZeros() const {
            return value.numberOfTrailingZeros();
        }

        constexpr inline Integer<size, true> s() const {
            return *this;
        }

        constexpr inline Integer<size, false> u() const {
            return *this;
        }

        void print(std::ostream &out) {
            out << "I<" << std::dec << size << ", " << (sig ? "t" : "f") << ">{";
            value.printv(out);
            out << "}";
        }

        void printv(std::ostream &out) {
            value.printv(out);
        }

        template<size_t size1, bool sig1, size_t size2, bool sig2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (size1 <= size / 2) && (size2 <= size / 2))>
        constexpr inline Integer(const Integer<size1, sig1> &low,
                const Integer<size2, sig2> &high) : value(low, high) { }

        template<typename T1, typename T2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (p_is_integral<T1>()) && (p_is_integral<T2>())&&
                (sizeof (T1) <= size / 2) && (sizeof (T2) <= size / 2))>
        constexpr inline Integer(const T1 low, const T2 high) : value(low, high) { }

        template<typename T1, size_t size2, bool sig2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (sizeof (T1) <= size / 2) && (size2 <= size / 2))>
        constexpr inline Integer(const T1 low,
                const Integer<size2, sig2> &high) : value(low, high) { }

        template<size_t size1, bool sig1, typename T2,
        p_enable_if((p_getIntegerType(size) == pow2) &&
                (size1 <= size / 2) && (sizeof (T2) <= size / 2))>
        constexpr inline Integer(const Integer<size1, sig1> &low,
                const T2 high) : value(low, high) { }

    private:

        template<typename T, p_IType = p_getIntegerType(size)>
        struct tcast_h {
        };

        template<typename T>
        struct tcast_h<T, native> {

            constexpr inline static V tcast(const T n) {
                return V(n);
            }
        };

        template<typename T>
        struct tcast_h<T, pow2> {

            constexpr inline static V tcast(const T n) {
                return Integer(Integer<sizeof (T), JIO::p_is_signed<T>()>(n)).value;
            }
        };

    public:

        template<typename T, p_enable_if((p_is_integral<T>()) && (size >= sizeof (T)))>
        constexpr inline Integer(const T n) : value(tcast_h<T>::tcast(n)) { }

        template<typename T, p_enable_if((p_is_integral<T>()) && (size < sizeof (T)))>
        constexpr explicit inline Integer(const T n) : value(tcast_h<T>::tcast(n)) { }

        template<size_t size2, bool sig2,
        p_enable_if(size2 >= size)>
        constexpr inline operator Integer<size2, sig2>() const {
            return upcast<size2, sig2>();
        }

        template<size_t size2, bool sig2,
        p_enable_if(size2 < size)>
        constexpr explicit inline operator Integer<size2, sig2>() const {
            return downcast<size2, sig2>();
        }

    private:

        template<typename T,
        bool = (p_getIntegerType(size) == native) && (sizeof (T) >= size),
        bool = (p_getIntegerType(size) == native) && (sizeof (T) < size),
        bool = (p_getIntegerType(size) != native) && (sizeof (T) < size)>
        struct pcast_h {
        };

        template<typename T>
        struct pcast_h<T, true, false, false> {

            constexpr inline static T pcast(const Integer& v) {
                return T(p_castUS<typename V::U, typename V::S,
                        sig, typename V::U > (v.value.value));
            }
        };

        template<typename T>
        struct pcast_h<T, false, true, false> {

            constexpr inline static T pcast(const Integer& v) {
                return T(v.value.value);
            }
        };

        template<typename T>
        struct pcast_h<T, false, false, true> {

            constexpr inline static T pcast(const Integer& v) {
                return T(Integer<sizeof (T), JIO::p_is_signed<T>()>(v));
            }
        };

    public:

        template<typename T, p_enable_if((p_is_integral<T>()) && (sizeof (T) >= size))>
        constexpr inline operator T() const {
            return pcast_h<T>::pcast(*this);
        }

        template<typename T, p_enable_if((p_is_integral<T>()) && (sizeof (T) < size))>
        constexpr explicit inline operator T() const {
            return pcast_h<T>::pcast(*this);
        }

        constexpr explicit inline operator bool() const {
            return !(this->isZero());
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator+(const Integer<size2, sig2> &v2) const {
            return R(*this).value + R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator-(const Integer<size2, sig2> &v2) const {
            return R(*this).value - R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator*(const Integer<size2, sig2> &v2) const {
            return R(*this).value * R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator/(const Integer<size2, sig2> &v2) const {
            return typename R::V(R(*this).value / R(v2).value);
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator%(const Integer<size2, sig2> &v2) const {
            return typename R::V(R(*this).value % R(v2).value);
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator|(const Integer<size2, sig2> &v2) const {
            return R(*this).value | R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator&(const Integer<size2, sig2> &v2) const {
            return R(*this).value & R(v2).value;
        }

        template<size_t size2, bool sig2,
        typename R = Integer<max(size, size2), sig && sig2>>
        constexpr inline R operator^(const Integer<size2, sig2> &v2) const {
            return R(*this).value ^ R(v2).value;
        }

        template<size_t size2, bool sig2>
        constexpr inline Integer operator<<(const Integer<size2, sig2> &v2) const {
            return value << p_SHType<size>(v2);
        }

        template<typename T, p_enable_if((p_is_integral<T>()))>
        constexpr inline Integer operator<<(const T v2) const {
            return value << p_SHType<size>(v2);
        }

        template<size_t size2, bool sig2>
        constexpr inline Integer operator>>(const Integer<size2, sig2> &v2) const {
            return V(value >> p_SHType<size>(v2));
        }

        template<typename T, p_enable_if((p_is_integral<T>()))>
        constexpr inline Integer operator>>(const T v2) const {
            return V(value >> p_SHType<size>(v2));
        }

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

        constexpr inline Integer operator~() const {
            return ~value;
        }

        constexpr inline Integer operator-() const {
            return -value;
        }

        constexpr inline Integer operator+() const {
            return *this;
        }

        template<size_t size1, bool sig1>
        friend constexpr Integer<size1, sig1>&
        operator++(Integer<size1, sig1>&);

        template<size_t size1, bool sig1>
        friend constexpr Integer<size1, sig1>&
        operator--(Integer<size1, sig1>&);

        template<size_t size2, bool sig2>
        friend class Integer;

        template<size_t size2, bool sig2>
        friend class p_Pow2_Integer_Impl;

        template<size_t size2, bool sig2>
        friend class p_Array_Integer_Impl;

        template<typename T>
        friend struct p_Operators_Impl;
    };

    template <typename T>
    constexpr inline size_t logb2(const T value) {
        return sizeof (T) * 8 - p_numberOfLeadingZeros_h(value);
    }

    template <size_t size, bool sig>
    constexpr inline size_t logb2(const Integer<size, sig> &value) {
        return size * 8 - value.numberOfLeadingZeros();
    }

    template<size_t size1, bool sig1>
    std::ostream& operator<<(std::ostream &out, Integer<size1, sig1> v) {
        auto f = out.flags();
        v.print(out);
        out.flags(f);
        return out;
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1>& operator++(Integer<size1, sig1> &v1) {
        return v1 = v1.p1();
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1>& operator--(Integer<size1, sig1> &v1) {
        return v1 = v1.m1();
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1> operator++(Integer<size1, sig1> &v1, int) {
        Integer<size1, sig1> tmp = v1;
        ++v1;
        return tmp;
    }

    template<size_t size1, bool sig1>
    constexpr inline Integer<size1, sig1> operator--(Integer<size1, sig1> &v1, int) {
        Integer<size1, sig1> tmp = v1;
        --v1;
        return tmp;
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator+(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) + R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator+(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) + R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator+=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 + Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator+=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 + Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator+=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) + R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator-(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) - R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator-(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) - R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator-=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 - Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator-=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 - Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator-=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) - R(v2));
    }

    template<size_t size1, p_enable_if((size1 == 1) || (size1 == 2) || (size1 == 4))>
    constexpr inline Integer<size1 * 2, false> wmultiply(
            const Integer<size1, false> &v1, const Integer<size1, false> &v2) {
        using R = Integer<size1 * 2, false>;
        return R(v1) * R(v2);
    }

    template<size_t size1, p_enable_if((size1 == 8) || p_getIntegerType(size1) == pow2)>
    constexpr inline Integer<size1 * 2, false> wmultiply(
            const Integer<size1, false> &v1,
            const Integer<size1, false> &v2) {
        using U1 = Integer < size1 / 2, false >;
        using U2 = Integer<size1, false>;
        using U4 = Integer<size1 * 2, false>;
        U1 a = U1(v1 >> (size1 * 4)), b = U1(v1);
        U1 c = U1(v2 >> (size1 * 4)), d = U1(v2);
        U2 ac = wmultiply(a, c);
        U2 bd = wmultiply(b, d);
        U1 ab, cd; //unintialized, may give warnings
        bool abo = U1::add_overflow(a, b, ab);
        bool cdo = U1::add_overflow(c, d, cd);
        U4 abcd = U4(wmultiply(ab, cd)) + ((abo && cdo) ? U4(1) << (size1 * 16) : U4::ZERO()) +
                (abo ? (U2(cd) << (size1 * 8)) : U2::ZERO()) +
                (cdo ? (U2(ab) << (size1 * 8)) : U2::ZERO());
        return U4(bd, ac) + ((abcd - ac - bd) << (size1 * 4));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator*(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) * R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator*(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) * R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator*=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 * Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator*=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 * Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator*=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) * R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator/(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) / R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator/(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) / R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator/=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 / Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator/=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 / Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator/=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) / R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator%(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) % R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator%(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) % R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator%=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 % Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator%=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 % Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator%=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) % R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator|(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) | R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator|(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) | R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator|=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 | Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator|=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 | Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator|=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) | R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator&(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) & R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator&(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) & R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator&=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 & Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator&=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 & Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator&=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) & R(v2));
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator^(const Integer<size1, sig1> &v1, const T v2) {
        return R(v1) ^ R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    typename R = Integer<max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline R operator^(const T v1, const Integer<size1, sig1> &v2) {
        return R(v1) ^ R(v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2,
    p_enable_if(size2 <= size1)>
    constexpr inline Integer<size1, sig1>& operator^=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 ^ Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (sizeof (T) <= size1))>
    constexpr inline Integer<size1, sig1>& operator^=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 ^ Integer<size1, sig1>(v2));
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()) && (size1 <= sizeof (T)))>
    constexpr inline T& operator^=(T &v1, const Integer<size1, sig1> &v2) {
        using R = Integer<sizeof (T), p_is_signed<T>()>;
        return v1 = T(R(v1) ^ R(v2));
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline bool operator==(
            const Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        using R = Integer < max(size1, size2), sig1 && sig2>;
        return R(v1).value == R(v2).value;
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator==(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) == R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator==(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
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
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator!=(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) != R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator!=(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
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
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<=(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) <= R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<=(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
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
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>=(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) >= R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>=(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
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
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) < R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator<(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
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
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>(const Integer<size1, sig1> &v1, const T v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) > R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline bool operator>(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer < max(size1, sizeof (T)), sig1 && (p_is_signed<T>())>;
        return R(v1) > R(v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T operator<<(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer <sizeof (T), (p_is_signed<T>())>;
        return T(R(v1) << v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline Integer<size1, sig1>& operator<<=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 << v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline Integer<size1, sig1>& operator<<=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 << v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T& operator<<=(T &v1, const Integer<size1, sig1> &v2) {
        return v1 = T(Integer<sizeof (T), p_is_signed<T>()>(v1) << v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T operator>>(const T v1, const Integer<size1, sig1> &v2) {
        using R = Integer <sizeof (T), (p_is_signed<T>())>;
        return T(R(v1) >> v2);
    }

    template<size_t size1, bool sig1, size_t size2, bool sig2>
    constexpr inline Integer<size1, sig1>& operator>>=(
            Integer<size1, sig1> &v1,
            const Integer<size2, sig2> &v2) {
        return v1 = (v1 >> v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline Integer<size1, sig1>& operator>>=(
            Integer<size1, sig1> &v1, const T v2) {
        return v1 = (v1 >> v2);
    }

    template<size_t size1, bool sig1, typename T,
    p_enable_if((p_is_integral<T>()))>
    constexpr inline T& operator>>=(T &v1, const Integer<size1, sig1> &v2) {
        return v1 = T(Integer<sizeof (T), p_is_signed<T>()>(v1) >> v2);
    }

#undef p_enable_if
}

#endif /* INTEGER_HPP */
