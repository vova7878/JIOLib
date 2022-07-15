#ifndef INTEGER_COMMON_HPP
#define INTEGER_COMMON_HPP

#include "Integer.hpp"

namespace JIO {
    template<bool A>
    using k_enable_if_t = typename std::enable_if<A, bool>::type;

#define k_enable_if(B) k_enable_if_t<(B)> = false

    template <typename T, T... values>
    struct k_array_t {
        const T data[sizeof...(values)] = {values...};

        constexpr inline const T operator[](size_t index) const {
            return data[index];
        }
    };

    template<typename T, T value, T... values>
    constexpr inline auto k_append(k_array_t<T, values...>) {
        return k_array_t<T, value, values...>();
    }

    constexpr char k_digits[36] = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };

    template<char c, size_t index = 0, k_enable_if(k_digits[index] == c)>
    constexpr inline auto k_indexOfDigit() {
        return index;
    }

    template<char c, size_t index = 0, k_enable_if(k_digits[index] != c)>
    constexpr inline auto k_indexOfDigit() {
        return k_indexOfDigit<c, index + 1 > ();
    }

    constexpr inline size_t k_make_pow2(size_t i) {
        if (i == 0) {
            return 0;
        }
        size_t log = logb2(i);
        if (i == (size_t(1) << (log - 1))) {
            return i;
        }
        return 1 << log;
    }

    constexpr size_t k_bits_to_bytes(size_t bits, bool sig) {
        return k_make_pow2((bits + 7 + (sig ? 1 : 0)) / 8);
    }

    template<bool sig, int bit_per_symbol, int first, int... nums>
    constexpr inline auto k_get_bytes() {
        return k_bits_to_bytes(sizeof...(nums) * bit_per_symbol +
                (4 - _numberOfLeadingZeros4Bit_h(uint8_t(first))), sig);
    }

    template<bool sig, int... nums, size_t bytes = k_get_bytes<sig, 4, nums...>()>
    constexpr inline auto k_parseHex(k_array_t<int, nums...> arr) {
        using I = Integer<bytes, sig>;
        I out;
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out <<= 4;
            out |= I(arr[i]);
        }
        return out;
    }

    template<bool sig, size_t bytes, int... nums>
    constexpr inline auto k_parseDec_h(k_array_t<int, nums...> arr) {
        using I = Integer<bytes, sig>;
        I out;
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out *= I(10);
            out += I(arr[i]);
        }
        return out;
    }

    template<bool sig, int... nums, size_t tmp_bytes =
    k_bits_to_bytes(sizeof...(nums) * 4, false),
    size_t bytes = k_bits_to_bytes(
            tmp_bytes * 8 - k_parseDec_h<false, tmp_bytes>(
            k_array_t<int, nums...>()).numberOfLeadingZeros(), sig)>
    constexpr inline auto k_parseDec(k_array_t<int, nums...> arr) {
        return k_parseDec_h<sig, bytes>(arr);
    }

    template<bool sig, int... nums, size_t bytes = k_get_bytes<sig, 3, nums...>()>
    constexpr inline auto k_parseOct(k_array_t<int, nums...> arr) {
        using I = Integer<bytes, sig>;
        I out;
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out <<= 3;
            out |= I(arr[i]);
        }
        return out;
    }

    template<bool sig, int... nums, size_t bytes = k_get_bytes<sig, 1, nums...>()>
    constexpr inline auto k_parseBin(k_array_t<int, nums...> arr) {
        using I = Integer<bytes, sig>;
        I out;
        for (size_t i = 0; i<sizeof...(nums); i++) {
            out <<= 1;
            out |= I(arr[i]);
        }
        return out;
    }

    template<int min, int max, int value>
    constexpr inline bool k_contains() {
        return value >= min && value <= max;
    }

    template<int min, int max>
    constexpr inline auto k_verify() {
        return k_array_t<int>();
    }

    template<int min, int max, char a, char... chars,
    k_enable_if((k_contains<min, max, k_indexOfDigit<a>()>()))>
    constexpr inline auto k_verify() {
        return k_append<int, k_indexOfDigit<a>()> (k_verify<min, max, chars...>());
    }

    template<int min, int max, char a, char... chars, k_enable_if(a != '0')>
    constexpr inline auto k_filter() {
        return k_verify<min, max, a, chars...>();
    }

    template<int min, int max, char a, char... chars, k_enable_if(a == '0')>
    constexpr inline auto k_filter() {
        return k_filter<min, max, chars...>();
    }

    template<bool sig, char a, char b, char... chars, k_enable_if((a == '0') &&
            !(b == 'b' || b == 'B' || b == 'x' || b == 'X'))>
    constexpr inline auto k_parse() {
        return k_parseOct<sig>(k_filter<0, 7, b, chars...>());
    }

    template<bool sig, char a, char b, char... chars, k_enable_if((a == '0') &&
            (b == 'b' || b == 'B'))>
    constexpr inline auto k_parse() {
        return k_parseBin<sig>(k_filter<0, 1, chars...>());
    }

    template<bool sig, char a, char b, char... chars, k_enable_if((a == '0') &&
            (b == 'x' || b == 'X'))>
    constexpr inline auto k_parse() {
        return k_parseHex<sig>(k_filter<0, 15, chars...>());
    }

    template<bool sig, char a, char... chars, k_enable_if(a != '0')>
    constexpr inline auto k_parse() {
        return k_parseDec<sig>(k_filter<0, 10, a, chars...>());
    }

    template<bool sig, char a, k_enable_if(a == '0')>
    constexpr inline auto k_parse() {
        return Integer<1, sig>(0);
    }

    template<char... chars>
    constexpr inline auto operator""_UI() {
        return k_parse<false, chars...>();
    }

    template<char... chars>
    constexpr inline auto operator""_SI() {
        return k_parse<true, chars...>();
    }

#undef k_enable_if
}

#endif /* INTEGER_COMMON_HPP */

