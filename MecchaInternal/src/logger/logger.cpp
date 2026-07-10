#include "logger.hpp"

#include <Windows.h>
#include <iostream>

namespace Logger
{
    void Log(Level level, const std::string& message)
    {
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

        WORD defaultColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        WORD levelColor = defaultColor;

        switch (level)
        {
        case Level::info:
            levelColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            std::cout << "[";
            SetConsoleTextAttribute(console, levelColor);
            std::cout << "INFO";
            SetConsoleTextAttribute(console, defaultColor);
            std::cout << "] ";
            break;

        case Level::warn:
            levelColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            std::cout << "[";
            SetConsoleTextAttribute(console, levelColor);
            std::cout << "WARN";
            SetConsoleTextAttribute(console, defaultColor);
            std::cout << "] ";
            break;

        case Level::error:
            levelColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
            std::cout << "[";
            SetConsoleTextAttribute(console, levelColor);
            std::cout << "ERROR";
            SetConsoleTextAttribute(console, defaultColor);
            std::cout << "] ";
            break;

        case Level::success:
            levelColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            std::cout << "[";
            SetConsoleTextAttribute(console, levelColor);
            std::cout << "SUCCESS";
            SetConsoleTextAttribute(console, defaultColor);
            std::cout << "] ";
            break;
        }

        std::cout << message << std::endl;
    }
}