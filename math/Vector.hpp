/*
 * Copyright (c) 2023 Vladimir Kozelkov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <type_traits>
#include <ostream>

// -std=c++17
namespace JIO {

    template<typename, size_t>
    struct Vector;

    inline namespace detail {

        namespace micro_std {
            template<typename T, typename TT = std::remove_reference_t<T>>
            using forward_ret_t =
                    std::conditional_t<std::is_lvalue_reference_v<T>, TT &, TT &&>;

            template<typename T, typename TT>
            inline forward_ret_t<T> forward(TT &&v) noexcept {
                return static_cast<T &&> (v);
            }
        }

        template<typename T, size_t size>
        constexpr auto is_vector_f(Vector<T, size>) {
            return std::true_type{};
        };

        template<typename T>
        constexpr auto is_vector_f(T) {
            return std::false_type{};
        };

        template<typename T>
        using is_vector = decltype(is_vector_f(std::declval<std::remove_cv<T>>()));

        template<size_t... index, typename F, typename... Tp>
        constexpr void
        apply_sequence(std::integer_sequence<size_t, index...>, F &&f, Tp &&... args) {
            (void(micro_std::forward<F>(f)(index, micro_std::forward<Tp>(args)...)), ...);
        }

        template<size_t length, typename F, typename... Tp>
        constexpr void apply_sequence(F &&f, Tp &&... args) {
            apply_sequence(std::make_index_sequence<length>(),
                           micro_std::forward<F>(f), micro_std::forward<Tp>(args)...);
        }

    } // namespace detail

    template<typename T1, size_t size1, typename T2, size_t size2>
    constexpr void copy(const Vector<T1, size1> &src, size_t src_pos,
                        Vector<T2, size2> &dst, size_t dst_pos, size_t length) {
        for (size_t i = 0; i < length; i++) {
            dst[dst_pos + i] = static_cast<T1>(src[src_pos + i]);
        }
    }

    template<size_t src_pos, size_t dst_pos, size_t length,
            typename T1, size_t size1, typename T2, size_t size2>
    constexpr void copy(const Vector<T1, size1> &src, Vector<T2, size2> &dst) {
        detail::apply_sequence<length>([](auto index, auto &&src, auto &&dst) {
            dst[dst_pos + index] = static_cast<T1>(src[src_pos + index]);
        }, src, dst);
    }

    template<typename T, size_t size>
    class Vector {
        static_assert(size > 0, "vector size must be greater than 0");
        static_assert(!std::is_reference<T>::value, "vector can`t contain reference");
        static_assert(!is_vector<T>::value, "vector can`t contain another vector");

    private:
        T data[size];

        template<size_t index>
        constexpr void assign() {
            static_assert(index == size, "Wrong arguments length");
        }

        template<size_t index, typename T2, size_t size2, typename... Tp>
        constexpr void assign(const Vector<T2, size2> &v, Tp... arr) {
            static_assert(index + size2 <= size, "Wrong arguments length");
            copy<0, index, size2>(v, *this);
            assign<index + size2>(arr...);
        }

        template<size_t index, typename T2, typename... Tp>
        constexpr void assign(const T2 &v, Tp... arr) {
            static_assert(index + 1 <= size, "Wrong arguments length");
            T tmp = static_cast<T>(v);
            if constexpr (index == 0 && sizeof...(Tp) == 0) {
                apply_sequence<size>([](auto i, auto &&out, auto &&tmp) {
                    out[i] = tmp;
                }, *this, tmp);
            } else {
                data[index] = tmp;
                assign<index + 1>(arr...);
            }
        }

    public:
        constexpr Vector() {
            assign<0>(T{});
        }

        template<typename... Tp>
        constexpr explicit Vector(Tp... arr) {
            assign<0>(arr...);
        }

        constexpr const T &operator[](size_t index) const {
            return data[index];
        }

        constexpr T &operator[](size_t index) {
            return data[index];
        }

        template<typename, size_t>
        friend
        class Vector;
    };

    template<typename T, size_t size, typename CharT, typename Traits>
    inline std::basic_ostream<CharT, Traits> &operator<<(
            std::basic_ostream<CharT, Traits> &out, const Vector<T, size> &v) {
        out << '{';
        for (size_t i = 0; i < size; i++) {
            if (i) {
                out << ',' << ' ';
            }
            out << v[i];
        }
        out << '}';
        return out;
    }

#define BIN_VV_OPERATOR(op)                                                                \
    template<typename T1, typename T2, size_t size>                                        \
    constexpr auto operator op(const Vector<T1, size>& v1, const Vector<T2, size>& v2) {   \
        Vector<decltype(std::declval<T1>() op std::declval<T2>()), size> out;              \
        detail::apply_sequence<size>([](auto index, auto&& v1, auto&& v2, auto&& out) {    \
            out[index] = v1[index] op v2[index];                                           \
            }, v1, v2, out);                                                               \
        return out;                                                                        \
    }

    BIN_VV_OPERATOR(+)

    BIN_VV_OPERATOR(-)

    BIN_VV_OPERATOR(*)

    BIN_VV_OPERATOR(/)

    BIN_VV_OPERATOR(%)

    BIN_VV_OPERATOR(|)

    BIN_VV_OPERATOR(&)

    BIN_VV_OPERATOR(^)

    BIN_VV_OPERATOR(&&)

    BIN_VV_OPERATOR(||)

    BIN_VV_OPERATOR(<<)

    BIN_VV_OPERATOR(>>)

#define BIN_VT_TV_OPERATOR(op)                                                             \
    template<typename T1, typename T2, size_t size>                                        \
    constexpr auto operator op(const Vector<T1, size>& v1, const T2& v2) {                 \
        Vector<decltype(std::declval<T1>() op std::declval<T2>()), size> out;              \
        detail::apply_sequence<size>([](auto index, auto&& v1, auto&& v2, auto&& out) {    \
            out[index] = v1[index] op v2;                                                  \
            }, v1, v2, out);                                                               \
        return out;                                                                        \
    }                                                                                      \
    /*template<typename T1, typename T2, size_t size>                                        \
    constexpr auto operator op(const T1& v1, const Vector<T2, size>& v2) {                 \
        Vector<decltype(std::declval<T1>() op std::declval<T2>()), size> out;              \
        detail::apply_sequence<size>([](auto index, auto&& v1, auto&& v2, auto&& out) {    \
            out[index] = v1 op v2[index];                                                  \
            }, v1, v2, out);                                                               \
        return out;                                                                        \
    }*/

    BIN_VT_TV_OPERATOR(+)

    BIN_VT_TV_OPERATOR(-)

    BIN_VT_TV_OPERATOR(*)

    BIN_VT_TV_OPERATOR(/)

    BIN_VT_TV_OPERATOR(%)

    BIN_VT_TV_OPERATOR(|)

    BIN_VT_TV_OPERATOR(&)

    BIN_VT_TV_OPERATOR(^)

    BIN_VT_TV_OPERATOR(&&)

    BIN_VT_TV_OPERATOR(||)

    BIN_VT_TV_OPERATOR(<<)

    BIN_VT_TV_OPERATOR(>>)

#define UNARY_V_OPERATOR(op)                                                   \
    template<typename T, size_t size>                                          \
    constexpr auto operator op(const Vector<T, size>& v) {                     \
        Vector<decltype(op std::declval<T>()), size> out;                      \
        detail::apply_sequence<size>([](auto index, auto&& v, auto&& out) {    \
            out[index] = op v[index];                                          \
            }, v, out);                                                        \
        return out;                                                            \
    }

    UNARY_V_OPERATOR(+)

    UNARY_V_OPERATOR(-)

    UNARY_V_OPERATOR(~)

#define ASSIGN_VV_OPERATOR(op)                                                                  \
    template<typename T1, typename T2, size_t size>                                             \
    constexpr Vector<T1, size>& operator op(Vector<T1, size>& v1, const Vector<T2, size>& v2) { \
        detail::apply_sequence<size>([](auto index, auto&& v1, auto&& v2) {                     \
            v1[index] op v2[index];                                                             \
            }, v1, v2);                                                                         \
        return v1;                                                                              \
    }

    ASSIGN_VV_OPERATOR(+=)

    ASSIGN_VV_OPERATOR(-=)

    ASSIGN_VV_OPERATOR(*=)

    ASSIGN_VV_OPERATOR(/=)

    ASSIGN_VV_OPERATOR(%=)

    ASSIGN_VV_OPERATOR(|=)

    ASSIGN_VV_OPERATOR(&=)

    ASSIGN_VV_OPERATOR(^=)

    ASSIGN_VV_OPERATOR(<<=)

    ASSIGN_VV_OPERATOR(>>=)

#define ASSIGN_VT_OPERATOR(op)                                                    \
    template<typename T1, typename T2, size_t size>                               \
    constexpr Vector<T1, size>& operator op(Vector<T1, size>& v1, const T2& v2) { \
        detail::apply_sequence<size>([](auto index, auto&& v1, auto&& v2) {       \
            v1[index] op v2;                                                      \
            }, v1, v2);                                                           \
        return v1;                                                                \
    }

    ASSIGN_VT_OPERATOR(+=)

    ASSIGN_VT_OPERATOR(-=)

    ASSIGN_VT_OPERATOR(*=)

    ASSIGN_VT_OPERATOR(/=)

    ASSIGN_VT_OPERATOR(%=)

    ASSIGN_VT_OPERATOR(|=)

    ASSIGN_VT_OPERATOR(&=)

    ASSIGN_VT_OPERATOR(^=)

    ASSIGN_VT_OPERATOR(<<=)

    ASSIGN_VT_OPERATOR(>>=)

#define BIN_VV_F(name, fn)                                                              \
    template<typename T1, typename T2, size_t size>                                     \
    constexpr auto name(const Vector<T1, size>& v1, const Vector<T2, size>& v2) {       \
        Vector<decltype(fn(std::declval<T1>(), std::declval<T2>())), size> out;         \
        detail::apply_sequence<size>([](auto index, auto&& v1, auto&& v2, auto&& out) { \
            out[index] = fn(v1[index], v2[index]);                                      \
            }, v1, v2, out);                                                            \
        return out;                                                                     \
    }

#define UNARY_V_F(name, fn)                                                 \
    template<typename T, size_t size>                                       \
    constexpr auto name(const Vector<T, size>& v) {                         \
        Vector<decltype(fn(std::declval<T>())), size> out;                  \
        detail::apply_sequence<size>([](auto index, auto&& v, auto&& out) { \
            out[index] = fn(v[index]);                                      \
            }, v, out);                                                     \
        return out;                                                         \
    }

    UNARY_V_F(abs, std::abs)

    UNARY_V_F(sqrt, std::sqrt)

    UNARY_V_F(sin, std::sin)

    UNARY_V_F(cos, std::cos)

    UNARY_V_F(tan, std::tan)

    UNARY_V_F(asin, std::asin)

    UNARY_V_F(acos, std::acos)

    UNARY_V_F(atan, std::atan)

    UNARY_V_F(floor, std::floor)

    //TODO: simplify?
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    constexpr auto fract(T v) {
        return v - std::floor(v);
    }

    UNARY_V_F(fract, fract)

    template<typename T1, typename T2, size_t size>
    constexpr auto dot(const Vector<T1, size> &v1, const Vector<T2, size> &v2) {
        using R = decltype(std::declval<T1>() * std::declval<T2>());
        R out = R{};
        detail::apply_sequence<size>([](auto index, auto &&v1, auto &&v2, auto &&out) {
            out += v1[index] * v2[index];
        }, v1, v2, out);
        return out;
    }

    template<typename T, size_t size>
    constexpr T length(const Vector<T, size> &v) {
        return std::sqrt(dot(v, v));
    }

    template<size_t size>
    constexpr bool any(const Vector<bool, size> &v) {
        bool out = false;
        for (size_t i = 0; (!out) && (i < size); i++) {
            out |= v[i];
        }
        return out;
    }

    template<size_t size>
    constexpr bool all(const Vector<bool, size> &v) {
        bool out = true;
        for (size_t i = 0; out && (i < size); i++) {
            out &= v[i];
        }
        return out;
    }

    template<typename T1, size_t size, typename T2, typename T3>
    constexpr auto clamp(const Vector<T1, size> &v, const T2 &lo2, const T3 &hi3) {
        T1 lo = static_cast<T1>(lo2), hi = static_cast<T1>(hi3);
        Vector<T1, size> out;
        detail::apply_sequence<size>([](auto index, auto &&v, auto hi, auto lo, auto &&out) {
            T1 value = v[index];
            out[index] = value < lo ? lo : (value > hi ? hi : value);
        }, v, hi, lo, out);
        return out;
    }

    template<typename T1, size_t size, typename T2>
    constexpr auto mix(const Vector<T1, size> &v1, const Vector<T1, size> &v2, const T2 &m) {
        Vector<T1, size> out;
        detail::apply_sequence<size>([](auto index, auto &&v1, auto &&v2, auto m, auto &&out) {
            out[index] = v1[index] + (v1[index] - v2[index]) * m;
        }, v1, v2, m, out);
        return out;
    }

    template<typename T1, size_t size, typename T2, typename T3, typename T4, typename T5>
    constexpr Vector<T1, size> remap(const Vector<T1, size> &v, const T2 &lo_in2,
                                     const T3 &hi_in3, const T4 &lo_out4, const T5 &hi_out5) {
        T1 lo_in = static_cast<T1>(lo_in2), hi_in = static_cast<T1>(hi_in3);
        T1 lo_out = static_cast<T1>(lo_out4), hi_out = static_cast<T1>(hi_out5);
        return (clamp(v, lo_in, hi_in) - lo_in) *
               (hi_out - lo_out) / (hi_in - lo_in) + lo_out;
    }

} // namespace JIO

#endif /* VECTOR_HPP */