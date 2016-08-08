#pragma once

#include <string.h>
#include <math.h>
#include "types.h"

#define memzero(ptr, type) memset(ptr, 0, sizeof(type))
#define SmallNumber 0.0000001f
#define IsValidRRHandle(rrh) rrh.h != InvalidHandle

inline bool almost_equal(float f1, float f2)
{
    return fabs(f2 - f1) < SmallNumber;
}

inline int max(int i1, int i2)
{
    return i1 > i2 ? i1 : i2;
}