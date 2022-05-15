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

struct __unused {
};

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

    template<size_t index, enable_if(index == size)>
    constexpr inline void assignOne(T e) { }

    template<size_t index, enable_if(index != size)>
    constexpr inline void assignOne(T e) {
        data[index] = e;
        assignOne < index + 1 > (e);
    }
public:

    constexpr inline Vector(__unused) { }

    constexpr inline Vector() {
        assignOne<0>(T());
    }

    template<typename Tp>
    constexpr explicit inline Vector(Tp v) {
        assignOne<0>(v);
    }

    template<typename... Tp>
    constexpr explicit inline Vector(Tp... arr) {
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

#define OP_TYPE(op, T1, T2) decltype(std::declval<T1>() op std::declval<T2>())

#define BIN_VV_OPERATOR_H(hname, op)                              \
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

#define BIN_VV_OPERATOR(hname, op)                                \
BIN_VV_OPERATOR_H(hname, op)                                      \
template<typename T1, typename T2, size_t size>                   \
constexpr inline Vector<OP_TYPE(op, T1, T2), size> operator op(   \
        const Vector<T1, size> &v1,                               \
        const Vector<T2, size> &v2) {                             \
    Vector<OP_TYPE(op, T1, T2), size> out((__unused()));          \
    hname<0>(v1, v2, out);                                        \
    return out;                                                   \
}

BIN_VV_OPERATOR(plus_h, +)
BIN_VV_OPERATOR(sub_h, -)
BIN_VV_OPERATOR(mul_h, *)
BIN_VV_OPERATOR(div_h, /)
BIN_VV_OPERATOR(rem_h, %)
BIN_VV_OPERATOR(or_h, |)
BIN_VV_OPERATOR(and_h, &)
BIN_VV_OPERATOR(xor_h, ^)
BIN_VV_OPERATOR(shl_h, <<)
BIN_VV_OPERATOR(shr_h, >>)

#undef BIN_VV_OPERATOR_H
#undef BIN_VV_OPERATOR

#define BIN_VT_OPERATOR_H(hname, op)                              \
template<size_t index, typename T1, typename T2, typename T3,     \
size_t size, enable_if(index == size)>                            \
constexpr inline void hname(const Vector<T1, size> &v1,           \
        const T2 v2, Vector<T3, size> &out) { }                   \
template<size_t index, typename T1, typename T2, typename T3,     \
size_t size, enable_if(index != size)>                            \
constexpr inline void hname(const Vector<T1, size> &v1,           \
        const T2 v2, Vector<T3, size> &out) {                     \
    out[index] = v1[index] op v2;                                 \
    hname < index + 1 > (v1, v2, out);                            \
}

#define BIN_VT_OPERATOR(hname, op)                                \
BIN_VT_OPERATOR_H(hname, op)                                      \
template<typename T1, typename T2, size_t size>                   \
constexpr inline Vector<OP_TYPE(op, T1, T2), size> operator op(   \
        const Vector<T1, size> &v1, const T2 v2) {                \
    Vector<OP_TYPE(op, T1, T2), size> out((__unused()));          \
    hname<0>(v1, v2, out);                                        \
    return out;                                                   \
}

BIN_VT_OPERATOR(plus_h, +)
BIN_VT_OPERATOR(sub_h, -)
BIN_VT_OPERATOR(mul_h, *)
BIN_VT_OPERATOR(div_h, /)
BIN_VT_OPERATOR(rem_h, %)
BIN_VT_OPERATOR(or_h, |)
BIN_VT_OPERATOR(and_h, &)
BIN_VT_OPERATOR(xor_h, ^)
BIN_VT_OPERATOR(shl_h, <<)
BIN_VT_OPERATOR(shr_h, >>)

#undef BIN_VT_OPERATOR_H
#undef BIN_VT_OPERATOR

#define BIN_TV_OPERATOR_H(hname, op)                              \
template<size_t index, typename T1, typename T2, typename T3,     \
size_t size, enable_if(index == size)>                            \
constexpr inline void hname(const T1 v1,                          \
        const Vector<T2, size> &v2, Vector<T3, size> &out) { }    \
template<size_t index, typename T1, typename T2, typename T3,     \
size_t size, enable_if(index != size)>                            \
constexpr inline void hname(const T1 v1,                          \
        const Vector<T2, size> &v2, Vector<T3, size> &out) {      \
    out[index] = v1 op v2[index];                                 \
    hname < index + 1 > (v1, v2, out);                            \
}

#define BIN_TV_OPERATOR(hname, op)                                \
BIN_TV_OPERATOR_H(hname, op)                                      \
template<typename T1, typename T2, size_t size>                   \
constexpr inline Vector<OP_TYPE(op, T1, T2), size> operator op(   \
        const T1 v1, const Vector<T2, size> &v2) {                \
    Vector<OP_TYPE(op, T1, T2), size> out((__unused()));          \
    hname<0>(v1, v2, out);                                        \
    return out;                                                   \
}

BIN_TV_OPERATOR(plus_h, +)
BIN_TV_OPERATOR(sub_h, -)
BIN_TV_OPERATOR(mul_h, *)
BIN_TV_OPERATOR(div_h, /)
BIN_TV_OPERATOR(rem_h, %)
BIN_TV_OPERATOR(or_h, |)
BIN_TV_OPERATOR(and_h, &)
BIN_TV_OPERATOR(xor_h, ^)
BIN_TV_OPERATOR(shl_h, <<)
BIN_TV_OPERATOR(shr_h, >>)

#undef BIN_VV_OPERATOR_H
#undef BIN_VV_OPERATOR

#undef OP_TYPE

#undef enable_if

#endif /* VECTOR_HPP */

