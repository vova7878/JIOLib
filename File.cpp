#include "File.hpp"

using namespace JIO;

bool File::exists() const noexcept {
    std::error_code ignore;
    return fs::exists(path, ignore);
}

bool File::isDirectory() const noexcept {
    std::error_code ignore;
    return fs::is_directory(path, ignore);
}

bool File::isFile() const noexcept {
    return exists()&& !isDirectory();
}

bool File::isRegularFile() const noexcept {
    std::error_code ignore;
    return fs::is_regular_file(path, ignore);
}

fs::path File::getAbsolutePath() const {
    return fs::absolute(path);
}

File File::getAbsoluteFile() const {
    return File(getAbsolutePath());
}

fs::path File::getCanonicalPath() const {
    return fs::canonical(path);
}

File File::getCanonicalFile() const {
    return File(getCanonicalPath());
}

fs::path File::getPath() const noexcept {
    return path;
}

bool File::isAbsolute() const {
    return path.is_absolute();
}

bool File::mkDir() const noexcept {
    std::error_code ignore;
    return fs::create_directory(path, ignore);
}

bool File::mkDirs() const noexcept {
    std::error_code ignore;
    return fs::create_directories(path, ignore);
}

bool File::renameTo(File dest) const noexcept {
    std::error_code err;
    fs::rename(path, dest.path, err);
    return !bool(err);
}

u8 File::length() const noexcept {
    std::error_code err;
    u8 out = fs::file_size(path, err);
    return bool(err) ? 0 : out;
}

std::vector<File> File::listFiles() const {
    std::vector<File> out;
    for (const fs::directory_entry &child : fs::directory_iterator(path)) {
        out.push_back(File(child.path()));
    }
    return out;
}

bool File::remove() const noexcept {
    std::error_code ignore;
    return fs::remove(path, ignore);
}