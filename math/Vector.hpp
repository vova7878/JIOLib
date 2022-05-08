#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cassert>
#include <type_traits>
#include <iostream>

template<typename T, size_t size>
class Vector;

template<bool A>
using enable_if_t = typename std::enable_if<A, bool>::type;

#define enable_if(B) enable_if_t<(B)> = false

template<size_t size1, size_t size2, size_t index1, size_t index2,
size_t length, size_t counter, typename T1, typename T2, enable_if(counter == length)>
constexpr inline void copy_array(T1 *array1, const T2 *array2) { }

template<size_t size1, size_t size2, size_t index1, size_t index2,
size_t length, size_t counter, typename T1, typename T2, enable_if(counter != length)>
constexpr inline void copy_array(T1 *array1, const T2 *array2) {
    array1[index1 + counter] = T1(array2[index2 + counter]);
    copy_array<size1, size2, index1, index2, length, counter + 1 > (
            array1, array2);
}

template<typename T, size_t size>
class Vector {
private:
    T data[size];

    template<size_t index>
    constexpr inline void assign() {
        static_assert(index == size, "Wrong arguments length");
    }

    template<size_t index, size_t size2, typename T2, typename... Tp>
    constexpr inline void assign(Vector<T2, size2> e, Tp... arr) {
        static_assert(index + size2 <= size, "Wrong arguments length");
        copy_array<size, size2, index, 0, size2, 0>(data, e.data);
        assign < index + size2 > (arr...);
    }

    template<size_t index, typename T2, typename... Tp>
    constexpr inline void assign(T2 e, Tp... arr) {
        static_assert(index + 1 <= size, "Wrong arguments length");
        data[index] = T(e);
        assign < index + 1 > (arr...);
    }
public:

    constexpr inline Vector() { }

    template<typename... Tp>
    constexpr inline Vector(Tp... arr) {
        assign<0>(arr...);
    }

    constexpr inline const T& operator[](size_t index) const {
        assert(index < size);
        return data[index];
    }

    constexpr inline T& operator[](size_t index) {
        assert(index < size);
        return data[index];
    }

    template<typename T1, size_t size1>
    friend class Vector;
};

template<typename T, size_t size>
std::ostream& operator<<(std::ostream &out, Vector<T, size> v) {
    out << "{";
    if (size > 0) {
        out << v[0];
        for (int i = 1; i < size; i++) {
            out << ", " << v[i];
        }
    }
    out << "}";
    return out;
}

#define BIN_OPERATOR_H(hname, op)                                 \
template<size_t index, typename T1, typename T2, typename T3,     \
size_t size, enable_if(index == size)>                            \
constexpr inline void hname(const Vector<T1, size> &v1,           \
        const Vector<T2, size> &v2, Vector<T3, size> &out) { }    \
template<size_t index, typename T1, typename T2, typename T3,     \
size_t size, enable_if(index != size)>                            \
constexpr inline void hname(const Vector<T1, size> &v1,           \
        const Vector<T2, size> &v2, Vector<T3, size> &out) {      \
    out[index] = v1[index] op v2[index];                          \
    hname < index + 1 > (v1, v2, out);                            \
}

#define BIN_OPERATOR(hname, op)                                   \
BIN_OPERATOR_H(hname, op)                                         \
template<typename T, size_t size>                                 \
constexpr inline Vector<T, size> operator op(                     \
        const Vector<T, size> &v1,                                \
        const Vector<T, size> &v2) {                              \
    Vector<T, size> out;                                          \
    hname<0>(v1, v2, out);                                        \
    return out;                                                   \
}

BIN_OPERATOR(plus_h, +)
BIN_OPERATOR(sub_h, -)
BIN_OPERATOR(mul_h, *)
BIN_OPERATOR(div_h, /)
BIN_OPERATOR(rem_h, %)
BIN_OPERATOR(or_h, |)
BIN_OPERATOR(and_h, &)
BIN_OPERATOR(xor_h, ^)

BIN_OPERATOR_H(shl_h, <<)
template<typename T1, typename T2, size_t size>
constexpr inline Vector<T1, size> operator<<(
        const Vector<T1, size> &v1,
        const Vector<T2, size> &v2) {
    Vector<T1, size> out;
    shl_h<0>(v1, v2, out);
    return out;
}

BIN_OPERATOR_H(shr_h, >>)
template<typename T1, typename T2, size_t size>
constexpr inline Vector<T1, size> operator>>(
        const Vector<T1, size> &v1,
        const Vector<T2, size> &v2) {
    Vector<T1, size> out;
    shr_h<0>(v1, v2, out);
    return out;
}

#undef BIN_OPERATOR_H
#undef BIN_OPERATOR
#undef enable_if

#endif /* VECTOR_HPP */

