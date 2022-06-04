#include "InMemoryStreams.hpp"
#include "mem_utils.hpp"

using namespace JIO;

InMemoryInputStream::InMemoryInputStream(void *data, u8 offset, u8 length) :
data(checkUBounds<void*>(data, offset, length)),
position(0),
count(length) { }

int InMemoryInputStream::read() {
    u1 *tmp = reinterpret_cast<u1*> (data);
    s8 tmp_pos = position + 1;
    if (tmp_pos <= count) {
        position = tmp_pos;
        return tmp[tmp_pos];
    }
    return -1;
}

s8 InMemoryInputStream::read(void *buf, s8 offset, s8 length) {
    if (length == 0) {
        return 0;
    }

    checkSBounds(buf, offset, length);

    s8 tmp_pos = position;
    if (tmp_pos >= count) {
        return -1;
    }

    s8 avail = count - tmp_pos;
    if (avail > length) {
        avail = length;
    }

    //Переполнение невозможно
    copyBytes(buf, offset, data, tmp_pos, avail);

    position = tmp_pos + avail;
    return avail;
}

s8 InMemoryInputStream::skip(s8 n) {
    s8 tmp_pos = position;
    s8 k = count - tmp_pos;
    if (n < k) {
        k = n < 0 ? 0 : n;
    }

    position = tmp_pos + k;
    return k;
}

s8 InMemoryInputStream::available() {
    return count - position;
}
