#include <assert.h>
#include <stdlib.h>
#include "helpers.h"
#include "memory.cpp"

int main()
{
    unsigned temp_memory_size = 1024 * 1024 * 100;
    void* temp_memory_block = malloc(temp_memory_size);
    temp_memory::init(temp_memory_block, temp_memory_size);
    
    uint32 s = 128;
    uint8* p1 = (uint8*)temp_memory::alloc(s);
    memset(p1, 0xda, s);
    uint8* p2 = (uint8*)temp_memory::alloc(s);
    memset(p2, 0xfe, s);
    uint8* p3 = (uint8*)temp_memory::alloc(s);
    memset(p3, 0x3e, s);
    temp_memory::dealloc(p2);
    for (uint32 i = 0; i < s; ++i)
    {
        assert(p1[i] == 0xda);
    }
    temp_memory::dealloc(p1);
    temp_memory::dealloc(p3);
    assert(temp_memory::tms.head == temp_memory::tms.start);
}