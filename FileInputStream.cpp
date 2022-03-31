#include "FileInputStream.hpp"

using namespace JIO;

FileInputStream::FileInputStream(const File f) :
file(f),
input() {
    input.open(file.getPath(), std::ios::in | std::ios::binary);
}

int FileInputStream::read() {
    return input.get();
}

s8 FileInputStream::read(void *buf, s8 offset, s8 length) {
    if ((offset | length) < 0) {
        throw OutOfBoundsException("Negative offset or length");
    }
    if (length == 0) {
        return 0;
    }
    if (input.peek() == -1) {
        return -1;
    }
    std::streamsize n = length;
    char *data = reinterpret_cast<char*> (buf);
    return input.readsome(data + offset, n);
}

s8 FileInputStream::available() {
    //TODO данные должны быть на основе длинны файла
    return !(input.peek() == -1);
}

FileInputStream::~FileInputStream() {
    input.close();
}

