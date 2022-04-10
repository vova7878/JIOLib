#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <sstream>
#include  <typeinfo>

namespace JIO {

    inline void print(std::ostringstream &buf) { }

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
    std::string formatMsg(const char *exname, T... msg) noexcept {
        try {
            std::ostringstream buf;
            buf << exname;
            if (sizeof...(msg) != 0) {
                buf << ": ";
                print(buf, msg...);
            }
            return buf.str();
        } catch (...) {
            return exname;
        }
    }

    class JException : public std::exception {
        const std::string msg;
    protected:

        inline JException(std::string msg) : msg(msg) { }

    public:

        template<typename... T>
        inline JException(T... why) :
        msg(formatMsg("JException", why...)) { }

        virtual const char* what() const noexcept {
            return msg.c_str();
        }
    };

    class IndexOutOfBoundsException : public JException {
    protected:

        inline IndexOutOfBoundsException(std::string msg) :
        JException(msg) { }

    public:

        template<typename... T>
        inline IndexOutOfBoundsException(T... why) :
        JException(formatMsg("IndexOutOfBoundsException", why...)) { }
    };

    class IOException : public JException {
    protected:

        inline IOException(std::string msg) :
        JException(msg) { }

    public:

        template<typename... T>
        inline IOException(T... why) :
        JException(formatMsg("IOException", why...)) { }
    };

    class EOFException : public IOException {
    protected:

        inline EOFException(std::string msg) :
        IOException(msg) { }

    public:

        template<typename... T>
        inline EOFException(T... why) :
        IOException(formatMsg("EOFException", why...)) { }
    };
}

#endif /* EXCEPTIONS_HPP */

