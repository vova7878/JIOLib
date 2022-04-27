#pragma once

#include <cstdint>
#include "Endian.hpp"

namespace JIO {

    static_assert(sizeof (void*) <= 8, "Too big pointer size");

    typedef int8_t s1;
    typedef int16_t s2;
    typedef int32_t s4;
    typedef int64_t s8;

    typedef EInteger<int16_t, true> s2_le;
    typedef EInteger<int32_t, true> s4_le;
    typedef EInteger<int64_t, true> s8_le;

    typedef EInteger<int16_t, false> s2_be;
    typedef EInteger<int32_t, false> s4_be;
    typedef EInteger<int64_t, false> s8_be;

    typedef uint8_t u1;
    typedef uint16_t u2;
    typedef uint32_t u4;
    typedef uint64_t u8;

    typedef EInteger<uint16_t, true> u2_le;
    typedef EInteger<uint32_t, true> u4_le;
    typedef EInteger<uint64_t, true> u8_le;

    typedef EInteger<uint16_t, false> u2_be;
    typedef EInteger<uint32_t, false> u4_be;
    typedef EInteger<uint64_t, false> u8_be;

    typedef float f4;
    static_assert(sizeof (f4) == 4, "Size of float is not 4 byte");

    typedef double f8;
    static_assert(sizeof (f8) == 8, "Size of double is not 8 byte");

}
