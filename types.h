#pragma once
#include <cassert>
#define InvalidHandle (unsigned)0
#define Assert(cond, msg) assert(cond && msg)
#define Error(msg) assert(false && msg)

typedef wchar_t wchar;

struct RRHandle
{
    unsigned h;
};