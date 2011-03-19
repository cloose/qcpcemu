#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>


class NotImplementedException : public std::exception
{
public:
    NotImplementedException() : std::exception() {}
    NotImplementedException(const std::string& amessage) throw() : message(amessage) {}
    virtual ~NotImplementedException() throw() {}

    virtual const char* what() const throw() { return message.c_str(); }

private:
    std::string message;
};

#endif // EXCEPTIONS_H
