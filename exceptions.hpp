#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <sstream>

namespace JIO {

#define _src_location __FILE__, __func__, __LINE__

    static std::string formatException(const char* why, const char *file,
            const char *func, const char *exname, int line) noexcept {
        try {
            std::ostringstream buf;
            buf << exname;
            if (why != nullptr) {
                buf << ": " << why;
            }
            buf << "\n\tat " << func << "(" << file << ":" << line << ")";
            return buf.str();
        } catch (...) {
            return why;
        }
    }

    class JException : public std::exception {
        std::string msg;

    public:

        inline JException(
                const char *file, const char *func, int line,
                const char* why = nullptr,
                const char *exname = "JException") :
        msg(formatException(why, file, func, exname, line)) {
        }

        virtual const char* what() const noexcept {
            return msg.c_str();
        }
    };

    class OutOfBoundsException : public JException {
    public:

        inline OutOfBoundsException(
                const char *file, const char *func, int line,
                const char* why = nullptr,
                const char *exname = "OutOfBoundsException") :
        JException(file, func, line, why, exname) {
        }
    };

    class IOException : public JException {
    public:

        inline IOException(
                const char *file, const char *func, int line,
                const char* why = nullptr,
                const char *exname = "IOException") :
        JException(file, func, line, why, exname) {
        }
    };
}

#endif /* EXCEPTIONS_HPP */

