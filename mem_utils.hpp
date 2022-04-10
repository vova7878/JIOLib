#ifndef MEM_UTILS_HPP
#define MEM_UTILS_HPP

#include <cstring>
#include "checks.hpp"

namespace JIO {

    inline void copyBytesWithCheck(void *dst, size_t dstPos,
            const void *src, size_t srcPos, size_t length) {
        void *d = checkUBounds<void*>(dst, dstPos, length);
        const void *s = checkUBounds<const void*>(src, srcPos, length);
        std::memmove(d, s, length);
    }

    inline void copyBytes(void *dst, size_t dstPos,
            const void *src, size_t srcPos, size_t length) {
        char *d = reinterpret_cast<char*> (dst) + dstPos;
        const char *s = reinterpret_cast<const char*> (src) + srcPos;
        std::memmove(d, s, length);
    }
}

#endif /* MEM_UTILS_HPP */

