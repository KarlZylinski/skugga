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

LoadedFile load(Allocator* alloc, const wchar* filename);
bool write(const File& file, const wchar* filename);

} // namespace file
