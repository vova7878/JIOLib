#ifndef CHECKS_HPP
#define CHECKS_HPP

#include "jtypes.hpp"
#include "exceptions.hpp"

constexpr const u8 ZERO = 0;
constexpr const u8 POINTER_MASK = (~ZERO)>>((8 - sizeof (void*)) * 8);

inline u8 checkBounds(u8 data, u8 offset, u8 length) {
    u8 offdata = data + offset;
    u8 enddata = offdata + length;
    if ((offdata | enddata) > POINTER_MASK) {
        throw JIO::OutOfBoundsException(_src_location, "Too big offset or length");
    }
    if (data > offdata || data > enddata) {
        throw JIO::OutOfBoundsException(_src_location, "Too big offset or length");
    }
    return offdata;
}

template<typename T>
inline T checkUBounds(const void *data, u8 offset, u8 length) {
    if ((offset | length) > POINTER_MASK) {
        throw JIO::OutOfBoundsException(_src_location, "Too big offset or length");
    }
    u8 intdata = reinterpret_cast<u8> (data);
    u8 offdata = checkBounds(intdata, offset, length);
    return reinterpret_cast<T> (offdata);
}

template<typename T>
inline T checkSBounds(const void *data, s8 offset, s8 length) {
    if ((offset | length) < 0) {
        throw JIO::OutOfBoundsException(_src_location, "Negative offset or length");
    }
    return checkUBounds<T>(data, offset, length);
}

#endif /* CHECKS_HPP */

