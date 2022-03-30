#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

class OutOfBoundsException : public std::exception {
    const char *msg;

public:

    inline OutOfBoundsException(const char* why) : msg(why) {
    }

    virtual const char* what() const noexcept {
        return msg;
    }
};

#endif /* EXCEPTIONS_HPP */

