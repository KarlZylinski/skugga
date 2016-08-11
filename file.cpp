#include "file.h"
#include <stdio.h>
#include "memory.h"

namespace file
{

LoadedFile load(Allocator* alloc, const wchar* filename)
{
    FILE* file_handle = _wfopen(filename, L"rb");

    if (file_handle == nullptr)
        return {false};

    fseek(file_handle, 0, SEEK_END);
    unsigned filesize = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);

    if (filesize == 0)
        return {false};

    unsigned char* data = (unsigned char*)alloc->alloc(unsigned(filesize));

    if (data == nullptr)
        return {false};

    fread(data, 1, filesize, file_handle);
    fclose(file_handle);
    File file = {};
    file.data = data;
    file.size = filesize;
    return {true, file};
}

bool write(const File& file, const wchar* filename)
{
    FILE* file_handle = _wfopen(filename, L"wb");

    if (file_handle == nullptr)
        return false;

    fwrite(file.data, 1, file.size, file_handle);
    fclose(file_handle);
    return true;
}

} // namespace file
