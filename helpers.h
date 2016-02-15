#pragma once

#include <string.h>
#include <math.h>

#define memzero(ptr, type) memset(ptr, 0, sizeof(type))
#define SmallNumber 0.0000001f

bool almost_equal(f32 f1, f32 f2)
{
    return fabs(f2 - f1) < SmallNumber;
}