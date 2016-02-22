#include <stdio.h>
#include "memory.h"

struct File
{
    uint8* data;
    uint32 size;
};

struct LoadedFile
{
    bool valid;
    File file;
};

namespace file
{

LoadedFile load(Allocator* alloc, const char* filename)
{
    FILE* file_handle = fopen(filename, "rb");

    if (!file_handle)
        return {false};

    fseek(file_handle, 0, SEEK_END);
    uint32 filesize = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);
    uint8* data = (uint8*)alloc->alloc(uint32(filesize) + 1);

    if (!data)
        return {false};

    fread(data, 1, filesize, file_handle);
    data[filesize] = 0;
    fclose(file_handle);
    File file = {0};
    file.data = data;
    file.size = filesize;
    return {true, file};
}

} // namespace file
