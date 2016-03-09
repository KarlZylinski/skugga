#pragma once
#include <cassert>
#define InvalidHandle (unsigned)-1
#define Assert(cond, msg) assert(cond && msg)

typedef wchar_t wchar;