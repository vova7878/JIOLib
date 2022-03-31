#include "FileOutputStream.hpp"

using namespace JIO;

FileOutputStream::FileOutputStream(const File f, bool append) :
file(f),
output() {
    output.open(file.getPath(), std::ios::out | std::ios::binary);
}

void FileOutputStream::write(u1 byte) {
    output.put(byte);
}

void FileOutputStream::write(const void *buf, s8 offset, s8 length) {
    if ((offset | length) < 0) {
        throw OutOfBoundsException("Negative offset or length");
    }

    const char *data = reinterpret_cast<const char*> (buf);

    output.write(data + offset, length);
}

void FileOutputStream::flush() {
    output.flush();
}

FileOutputStream::~FileOutputStream() {
    output.close();
}
