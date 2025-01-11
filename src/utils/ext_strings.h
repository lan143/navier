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
