#include "ext_strings.h"

std::string formatString(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int size = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (size < 0) {
        throw std::runtime_error("Error during formatting");
    }

    std::vector<char> buffer(size + 1);

    va_start(args, format);
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    va_end(args);

    return std::string(buffer.data());
}