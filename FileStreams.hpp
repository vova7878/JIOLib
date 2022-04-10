#ifndef FILESTREAMS_HPP
#define FILESTREAMS_HPP

#include <fstream>
#include "Streams.hpp"
#include "File.hpp"

namespace JIO {

    class FileInputStream : public InputStream {
    public:
        FileInputStream(const File file);

        inline FileInputStream(std::string path) :
        FileInputStream(File(path)) { }

        using InputStream::read;
        virtual int read() override;
        virtual s8 read(void *buf, s8 offset, s8 length) override;
        virtual s8 available() override;

        virtual ~FileInputStream();
    private:
        const File file;
        std::ifstream input;
        FileInputStream(const FileInputStream&);
        FileInputStream& operator=(const FileInputStream&);
    };

    class FileOutputStream : public OutputStream {
    public:
        FileOutputStream(const File file, bool append);

        inline FileOutputStream(std::string path, bool append) :
        FileOutputStream(File(path), append) { }

        using OutputStream::write;
        virtual void write(u1 byte) override;
        virtual void write(const void *buf, s8 offset, s8 length) override;
        virtual void flush() override;

        virtual ~FileOutputStream();
    private:
        const File file;
        std::ofstream output;
        FileOutputStream(const FileOutputStream&);
        FileOutputStream& operator=(const FileOutputStream&);
    };
}

#endif /* FILESTREAMS_HPP */

