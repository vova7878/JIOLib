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

FileOutputStream::~FileOutputStream() {
    output.close();
}
