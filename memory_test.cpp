#include <assert.h>
#include <stdlib.h>
#include "helpers.h"
#include "memory.cpp"

int main()
{
    unsigned temp_memory_size = 1024 * 1024 * 100;
    void* temp_memory_block = malloc(temp_memory_size);
    temp_memory::init(temp_memory_block, temp_memory_size);
    
    {
        unsigned s = 128;
        unsigned char* p1 = (unsigned char*)temp_memory::alloc(s);
        memset(p1, 0xda, s);
        unsigned char* p2 = (unsigned char*)temp_memory::alloc(s);
        memset(p2, 0xfe, s);
        unsigned char* p3 = (unsigned char*)temp_memory::alloc(s);
        memset(p3, 0x3e, s);
        temp_memory::dealloc(p2);
        for (unsigned i = 0; i < s; ++i)
        {
            assert(p1[i] == 0xda);
        }
        temp_memory::dealloc(p1);
        temp_memory::dealloc(p3);
        assert(temp_memory::tms.head == temp_memory::tms.start);
    }

    {
        Allocator ta = create_temp_allocator();
        unsigned s = 128;
        unsigned char* p1 = (unsigned char*)ta.alloc(s);
        memset(p1, 0xaa, s);
        unsigned char* p2 = (unsigned char*)ta.alloc(s);
        memset(p2, 0xbb, s);
        unsigned char* p3 = (unsigned char*)ta.alloc(s);
        memset(p3, 0xcc, s);
    }
    
    assert(temp_memory::tms.head == temp_memory::tms.start);
}