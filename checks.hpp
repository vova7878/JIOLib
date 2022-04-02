#ifndef CHECKS_HPP
#define CHECKS_HPP

#include "jtypes.hpp"
#include "exceptions.hpp"

constexpr const u8 ZERO = 0;
constexpr const u8 POINTER_MASK = (~ZERO)>>((8 - sizeof (void*)) * 8);

template<typename T>
inline T UBoundsCheck(const void *data, u8 offset, u8 length) {
    if ((offset | length)&(~POINTER_MASK)) {
        throw JIO::OutOfBoundsException(_src_location, "Too big offset or length");
    }
    u8 intdata = reinterpret_cast<u8> (data);
    u8 offdata = intdata + offset;
    if (intdata > offdata || intdata > (offdata + length)) {
        throw JIO::OutOfBoundsException(_src_location, "Too big offset or length");
    }
    return reinterpret_cast<T> (offdata);
}

template<typename T>
inline T SBoundsCheck(const void *data, s8 offset, s8 length) {
    if ((offset | length) < 0) {
        throw JIO::OutOfBoundsException(_src_location, "Negative offset or length");
    }
    return UBoundsCheck<T>(data, offset, length);
}

#endif /* CHECKS_HPP */

