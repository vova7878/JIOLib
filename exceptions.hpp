#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <sstream>
#include  <typeinfo>

namespace JIO {

#define _src_location __FILE__, __func__, __LINE__

    inline void print(std::ostringstream &buf) {
    }

    template<typename One>
    inline void print(std::ostringstream &buf, One value) {
        buf << value;
    }

    template<typename First, typename... T>
    inline void print(std::ostringstream &buf, First value, T... other) {
        buf << value;
        print(buf, other...);
    }

    template<typename... T>
    inline std::string formatMsg(T... msg) noexcept {
        try {
            std::ostringstream buf;
            print(buf, msg...);
            return buf.str();
        } catch (...) {
            return "";
        }
    }

    inline std::string formatException(const char *file, const char *func,
            int line, const char *exname, std::string why) noexcept {
        try {
            std::ostringstream buf;
            buf << exname;
            if (why.length() != 0) {
                buf << ": " << why;
            }
            buf << "\n\tat " << func << "(" << file << ":" << line << ")";
            return buf.str();
        } catch (...) {
            return exname;
        }
    }

    class JException : public std::exception {
        const std::string msg;

    public:

        inline JException(
                const char *file, const char *func, int line,
                std::string why = "",
                const char *exname = "JException") :
        msg(formatException(file, func, line, exname, why)) {
        }

        virtual const char* what() const noexcept {
            return msg.c_str();
        }
    };

    class IndexOutOfBoundsException : public JException {
    public:

        inline IndexOutOfBoundsException(
                const char *file, const char *func, int line,
                std::string why = "",
                const char *exname = "IndexOutOfBoundsException") :
        JException(file, func, line, why, exname) {
        }
    };

    class IOException : public JException {
    public:

        inline IOException(
                const char *file, const char *func, int line,
                std::string why = "",
                const char *exname = "IOException") :
        JException(file, func, line, why, exname) {
        }
    };

    class EOFException : public IOException {
    public:

        inline EOFException(
                const char *file, const char *func, int line,
                std::string why = "",
                const char *exname = "EOFException") :
        IOException(file, func, line, why, exname) {
        }
    };
}

#endif /* EXCEPTIONS_HPP */

