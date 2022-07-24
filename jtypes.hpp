#pragma once

#include <cstdint>

namespace JIO {

    static_assert(sizeof (void*) <= 8, "Too big pointer size");

    typedef int8_t s1;
    typedef int16_t s2;
    typedef int32_t s4;
    typedef int64_t s8;

    typedef uint8_t u1;
    typedef uint16_t u2;
    typedef uint32_t u4;
    typedef uint64_t u8;

    typedef float f4;
    static_assert(sizeof (f4) == 4, "Size of float is not 4 byte");
    typedef double f8;
    static_assert(sizeof (f8) == 8, "Size of double is not 8 byte");

}
