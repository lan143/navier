#pragma once

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdarg>
#include <stdexcept>

extern std::string formatString(const char* format, ...);
extern std::vector<std::string> split(const std::string& input, const std::string& delimiter);