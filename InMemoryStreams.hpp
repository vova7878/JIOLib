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

        inline virtual ~InMemoryInputStream() override { }
    private:
        void *const data;
        u8 position;
        const u8 count;
        InMemoryInputStream(const InMemoryInputStream&);
        InMemoryInputStream& operator=(const InMemoryInputStream&);
    };

    /*class InMemoryOutputStream : public OutputStream {
    public:

        inline InMemoryOutputStream() {
        }

        using OutputStream::write;
        virtual void write(u1 byte) override;
        virtual void write(const void *buf, s8 offset, s8 length) override;
        virtual void flush() override;

        virtual ~InMemoryOutputStream() override;
    private:
        InMemoryOutputStream(const InMemoryOutputStream&);
        InMemoryOutputStream& operator=(const InMemoryOutputStream&);
    };*/
}

#endif /* INMEMORYSTREAMS_HPP */

