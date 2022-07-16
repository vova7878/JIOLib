#ifndef CHECKS_HPP
#define CHECKS_HPP

#include "jtypes.hpp"
#include "exceptions.hpp"

namespace JIO {

    constexpr const u8 ZERO = 0;
    constexpr const u8 POINTER_MASK = (~ZERO)>>((8 - sizeof (void*)) * 8);

    inline void throwIOBE(u8 offset, u8 length) {
        throw JIO::IndexOutOfBoundsException("Too big offset(",
                offset, ") or length(", length, ")");
    }

    inline void checkRange(u8 offset, u8 length, u8 capacity) {
        if ((offset > capacity)
                || (length > capacity)
                || ((offset + length) > capacity)
                || ((offset + length) < offset)) {
            throw JIO::IndexOutOfBoundsException("Range [",
                    offset, ", ", offset, " + ", length,
                    ") out of bounds for capacity ", capacity);
        }
    }

    inline u8 checkBounds(u8 data, u8 offset, u8 length) {
        u8 offdata = data + offset;
        u8 enddata = offdata + length;
        if ((offdata | enddata) > POINTER_MASK) {
            throwIOBE(offset, length);
        }
        if (data > offdata || offdata > enddata) {
            throwIOBE(offset, length);
        }
        return offdata;
    }

    template<typename T = void*>
    inline T checkUBounds(const void *data, u8 offset, u8 length) {
        u8 intdata = reinterpret_cast<u8> (data);
        u8 offdata = checkBounds(intdata, offset, length);
        return reinterpret_cast<T> (offdata);
    }

    template<typename T = void*>
    inline T checkSBounds(const void *data, s8 offset, s8 length) {
        if ((offset | length) < 0) {
            throw JIO::IndexOutOfBoundsException("Negative offset(",
                    offset, ") or length(", length, ")");
        }
        return checkUBounds<T>(data, offset, length);
    }
}

#endif /* CHECKS_HPP */

