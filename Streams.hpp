#ifndef INPUTSTREAM_HPP
#define INPUTSTREAM_HPP

#include <algorithm>
#include "exceptions.hpp"
#include "jtypes.hpp"

namespace JIO {

    class InputStream {
    public:
        /**
         * Читает байт и возвращает значение в промежутке от 0 до 255.
         * Если достигнут конец данных, будет возвращено -1.
         */
        virtual int read() = 0;

        /**
         * Пытается прочитать <code>length</code> байт в <code>buf</code>
         * с нулевым смещением. Возвращается количество прочитанных байт.
         * Если конец данных достигнут до того как было прочитано length байт,
         * чтение прекращается. При достижении конца данных до начала чтения,
         * будет возвращено специальное знаение -1
         */
        inline virtual s8 read(void *buf, s8 length) {
            return read(buf, 0, length);
        }

        /**
         * Пытается прочитать <code>length</code> байт в <code>buf</code> со
         * смещением <code>offset</code>. Возвращается количество прочитанных
         * байт. Если конец данных достигнут до того как было прочитано length
         * байт, чтение прекращается. При достижении конца данных до начала
         * чтения, будет возвращено специальное знаение -1
         */
        virtual s8 read(void *buf, s8 offset, s8 length) {
            if ((offset | length) < 0) {
                throw OutOfBoundsException("Negative offset or length");
            }
            if (length == 0) {
                return 0;
            }

            u1 *data = reinterpret_cast<u1*> (buf);

            int c = read();
            if (c == -1) {
                return -1;
            }
            data[offset] = c;

            s8 i = 1;
            for (; i < length; i++) {
                c = read();
                if (c == -1) {
                    break;
                }
                data[offset + i] = c;
            }
            return i;
        }

        inline virtual s8 available() {
            return 0;
        }

        virtual s8 skip(s8 count) {
            const static s8 SKIP_BUFFER_SIZE = 2048;
            static u1 SKIP_BUFFER[SKIP_BUFFER_SIZE];
            if (count <= 0) {
                return 0;
            }
            s8 remaining = count;
            s8 nr;

            s8 size = std::min(SKIP_BUFFER_SIZE, remaining);
            while (remaining > 0) {
                nr = read(SKIP_BUFFER, 0, std::min(size, remaining));
                if (nr < 0) {
                    break;
                }
                remaining -= nr;
            }

            return count - remaining;
        }

        inline virtual ~InputStream() {
        };
    };

    class OutputStream {
    public:
        virtual void write(u1 byte) = 0;

        inline virtual void write(const void *buf, s8 length) {
            write(buf, 0, length);
        }

        virtual void write(const void *buf, s8 offset, s8 length) {
            if ((offset | length) < 0) {
                throw OutOfBoundsException("Negative offset or length");
            }

            const u1 *data = reinterpret_cast<const u1*> (buf);

            for (s8 i = 0; i < length; i++) {
                write(data[offset + i]);
            }
        }

        inline virtual void flush() {
        }

        inline virtual ~OutputStream() {
        };
    };
}

#endif /* INPUTSTREAM_HPP */

