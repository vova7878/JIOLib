#ifndef FILEINPUTSTREAM_HPP
#define FILEINPUTSTREAM_HPP

#include <fstream>
#include "Streams.hpp"
#include "File.hpp"

namespace JIO {

    class FileInputStream : public InputStream {
    public:
        FileInputStream(const File file);

        inline FileInputStream(std::string path) :
        FileInputStream(File(path)) {
        }

        using InputStream::read;
        virtual int read() override;
        virtual s8 read(void *buf, s8 offset, s8 length) override;
        virtual s8 available() override;

        virtual ~FileInputStream();
    private:
        const File file;
        std::ifstream input;
        FileInputStream(const FileInputStream& orig);
    };
}

#endif /* FILEINPUTSTREAM_HPP */

