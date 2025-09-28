#pragma once

#include <ostream>
#include <string>

class Log {
public:
    static std::ostream& info(const char* const tag);
    static std::ostream& warning(const char* const tag);
    static std::ostream& error(const char* const tag);
    static std::ostream& debug(const char* const tag);
};
