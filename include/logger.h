#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

class Logger
{
public:
    Logger(Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& get()
    {
        static Logger logger;
        return logger;
    }

    void log(const std::string& message, bool isError = false);
private:
    Logger() = default;
};

#endif