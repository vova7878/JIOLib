#include "FileStreams.hpp"

using namespace JIO;

FileInputStream::FileInputStream(const File f) :
file(f),
input() {
    input.open(file.getPath(), std::ios::in | std::ios::binary);

    if (input.fail()) {
        throw IOException(_src_location, "Unable to open file");
    }
}

int FileInputStream::read() {
    int out = input.get();

    if (input.fail()) {
        throw IOException(_src_location, "Read error");
    }

    return out;
}

s8 FileInputStream::read(void *buf, s8 offset, s8 length) {
    if (length == 0) {
        return 0;
    }

    char *data = SBoundsCheck<char*>(buf, offset, length);

    if (input.peek() == -1) {
        return -1;
    }

    s8 out = input.readsome(data, length);

    if (input.fail()) {
        throw IOException(_src_location, "Read error");
    }
    return out;
}

s8 FileInputStream::available() {
    std::streambuf *buf = input.rdbuf();

    if (input.fail() || buf == nullptr) {
        throw IOException(_src_location, "Unable to get available bytes");
    }

    s8 out = buf->in_avail();
    return out < 0 ? 0 : out;
}

FileInputStream::~FileInputStream() {
    input.close();
}

