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
    FILE* file_handle = fopen(filename, "r");

    if (file_handle == nullptr)
        return {false};

    fseek(file_handle, 0, SEEK_END);
    uint32 filesize = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);

    if (filesize == 0)
        return {false};

    uint8* data = (uint8*)alloc->alloc(uint32(filesize));

    if (data == nullptr)
        return {false};

    fread(data, 1, filesize, file_handle);
    fclose(file_handle);
    File file = {};
    file.data = data;
    file.size = filesize;
    return {true, file};
}

bool write(const File& file, const char* filename)
{
    FILE* file_handle = fopen(filename, "w");

    if (file_handle == nullptr)
        return false;

    fwrite(file.data, 1, file.size, file_handle);
    fclose(file_handle);
    return true;
}

} // namespace file
