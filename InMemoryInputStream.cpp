#include "InMemoryStreams.hpp"
#include <cstring>
using namespace JIO;

InMemoryInputStream::InMemoryInputStream(void *data, u8 offset, u8 length) :
data(UBoundsCheck<void*>(data, offset, length)),
position(0),
count(length) {
}

int InMemoryInputStream::read() {
    u1 *tmp = reinterpret_cast<u1*> (data);
    return position < count ? tmp[position++] : -1;
}

s8 InMemoryInputStream::read(void *buf, s8 offset, s8 length) {
    if (length == 0) {
        return 0;
    }

    char *cdata = SBoundsCheck<char*>(buf, offset, length);

    if (position >= count) {
        return -1;
    }

    s8 avail = count - position;
    if (avail > length) {
        avail = length;
    }

    u1 *tmp = reinterpret_cast<u1*> (data);

    std::memcpy(cdata, tmp + position, avail);

    position += avail;
    return avail;
}

s8 InMemoryInputStream::skip(s8 n) {
    s8 k = count - position;
    if (n < k) {
        k = n < 0 ? 0 : n;
    }

    position += k;
    return k;
}

s8 InMemoryInputStream::available() {
    return count - position;
}
