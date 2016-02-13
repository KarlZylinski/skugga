#pragma once

#include <cassert>

#define InvalidHandle (unsigned)-1

typedef unsigned char uint8;

#define Assert(cond, msg) assert(cond && msg)