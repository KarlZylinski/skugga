#pragma once

#include "memory.h"

struct File
{
    unsigned char* data;
    unsigned size;
};

struct LoadedFile
{
    bool valid;
    File file;
};

namespace file
{

LoadedFile load(Allocator* alloc, const char* filename);
bool write(const File& file, const char* filename);

} // namespace file
