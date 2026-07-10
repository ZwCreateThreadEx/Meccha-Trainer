#pragma once

#include <string>

enum class Level
{
    info,
    warn,
    error,
    success
};

namespace Logger
{
    void Log(Level level, const std::string& message);
}