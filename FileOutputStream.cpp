#include "FileStreams.hpp"

using namespace JIO;

FileOutputStream::FileOutputStream(const File f, bool append) :
file(f),
output() {
    std::ios::openmode mode = std::ios::out | std::ios::binary;
    if (append) {
        mode |= std::ios::in | std::ios::ate;
    }
    output.open(file.getPath(), mode);

    if (output.fail()) {
        throw IOException(_src_location, "Unable to open file");
    }
}

void FileOutputStream::write(u1 byte) {
    output.put(byte);

    if (output.fail()) {
        throw IOException(_src_location, "Write error");
    }
}

void FileOutputStream::write(const void *buf, s8 offset, s8 length) {
    const char *data = checkSBounds<const char*>(buf, offset, length);

    output.write(data, length);

    if (output.fail()) {
        throw IOException(_src_location, "Write error");
    }
}

void FileOutputStream::flush() {
    output.flush();

    if (output.fail()) {
        throw IOException(_src_location, "Flush error");
    }
}

FileOutputStream::~FileOutputStream() {
    output.close();
}
