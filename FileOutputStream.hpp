#ifndef FILEOUTPUTSTREAM_HPP
#define FILEOUTPUTSTREAM_HPP

#include <fstream>
#include "Streams.hpp"
#include "File.hpp"

namespace JIO {

    class FileOutputStream : public OutputStream {
    public:
        FileOutputStream(const File file, bool append);

        inline FileOutputStream(std::string path, bool append) :
        FileOutputStream(File(path), append) {
        }

        using OutputStream::write;
        virtual void write(u1 byte) override;
        virtual void write(const void *buf, s8 offset, s8 length) override;
        virtual void flush() override;

        virtual ~FileOutputStream();
    private:
        const File file;
        std::ofstream output;
        FileOutputStream(const FileOutputStream& orig);
    };
}

#endif /* FILEOUTPUTSTREAM_HPP */

