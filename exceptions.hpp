#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

class OutOfBoundsException : public std::exception {
    const char *msg;

public:

    inline OutOfBoundsException(const char* why) : msg(why) {
    }
    
    inline OutOfBoundsException() : msg("Out of bounds") {
    }

    virtual const char* what() const noexcept {
        return msg;
    }
};

class IOException : public std::exception {
    const char *msg;

public:

    inline IOException(const char* why) : msg(why) {
    }
    
    inline IOException() : msg("IO error") {
    }

    virtual const char* what() const noexcept {
        return msg;
    }
};

#endif /* EXCEPTIONS_HPP */

