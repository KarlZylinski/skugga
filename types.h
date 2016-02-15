#pragma once

#include <cassert>

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef float f32;
typedef double f64;
typedef wchar_t wchar;

#define InvalidHandle (uint32)-1

#define Assert(cond, msg) assert(cond && msg)