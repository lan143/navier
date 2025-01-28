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

std::vector<std::string> split(const std::string& input, const std::string& delimiter)
{
    std::string s = input;
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;

    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    tokens.push_back(s);

    return tokens;
}