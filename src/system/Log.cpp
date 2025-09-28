#include "Log.h"

#include <iostream>


std::ostream& Log::info(const char* const tag)
{
    return std::cout << "[i][" << tag << "] ";
}

std::ostream& Log::warning(const char* const tag)
{
    return std::cout << "[w][" << tag << "] ";
}

std::ostream& Log::error(const char* const tag)
{
    return std::cerr << "[e][" << tag << "] ";
}

std::ostream& Log::debug(const char* const tag)
{
    return std::cout << "[d][" << tag << "] ";
}
