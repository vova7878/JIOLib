#ifndef INMEMORYSTREAMS_HPP
#define INMEMORYSTREAMS_HPP

#include "Streams.hpp"

namespace JIO {

    class InMemoryInputStream : public InputStream {
    public:
        InMemoryInputStream(void *data, u8 offset, u8 length);

        using InputStream::read;
        virtual int read() override;
        virtual s8 read(void *buf, s8 offset, s8 length) override;
        virtual s8 skip(s8 count) override;
        virtual s8 available() override;

        inline virtual ~InMemoryInputStream() {
        }
    private:
        void* data;
        u8 position;
        u8 count;
        InMemoryInputStream(const InMemoryInputStream& orig);
    };

    /*class FileOutputStream : public OutputStream {
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
    };*/
}

#endif /* INMEMORYSTREAMS_HPP */

