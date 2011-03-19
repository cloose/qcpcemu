#ifndef LOGGER_H
#define LOGGER_H

#include "types.h"


class Logger
{
public:
    static Logger* getInstance();

    void logProgramCounter(word_t pc);

private:
    Logger();
};

#endif // LOGGER_H
