#include "FileOutputStream.hpp"

using namespace JIO;

FileOutputStream::FileOutputStream(const File f, bool append) :
file(f),
output() {
    output.open(file.getPath(), std::ios::out | std::ios::binary);
    if(output.fail()){
        throw IOException("Unable to open file");
    }
}

void FileOutputStream::write(u1 byte) {
    output.put(byte);
    if(output.fail()){
        throw IOException("Write error");
    }
}

void FileOutputStream::write(const void *buf, s8 offset, s8 length) {
    if ((offset | length) < 0) {
        throw OutOfBoundsException("Negative offset or length");
    }

    const char *data = reinterpret_cast<const char*> (buf);

    output.write(data + offset, length);
    
    if(output.fail()){
        throw IOException("Write error");
    }
}

void FileOutputStream::flush() {
    output.flush();
    if(output.fail()){
        throw IOException("Flush error");
    }
}

FileOutputStream::~FileOutputStream() {
    output.close();
}
