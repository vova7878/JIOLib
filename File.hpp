#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <experimental/filesystem>
#include "jtypes.hpp"

namespace fs = std::experimental::filesystem::v1;

namespace JIO {

    class File final {
    public:

        inline File(const fs::path pathname) : path(pathname) {
        }
        File(const File& orig) = default;
        ~File() = default;

        bool exists() const noexcept;
        bool isDirectory() const noexcept;
        bool isFile() const noexcept;
        bool isRegularFile() const noexcept;
        bool isAbsolute() const;
        bool mkDir() const noexcept;
        bool mkDirs() const noexcept;
        bool mkFile() const noexcept; //todo
        bool renameTo(File dest) const noexcept;
        bool remove() const noexcept;
        bool removeAll() const noexcept;
        u8 length() const noexcept;
        fs::path getPath() const noexcept;
        File getAbsoluteFile() const;
        fs::path getAbsolutePath() const;
        File getCanonicalFile() const;
        fs::path getCanonicalPath() const;
        std::vector<File> listFiles() const;
    private:
        const fs::path path;
    };
}

#endif /* FILE_HPP */

