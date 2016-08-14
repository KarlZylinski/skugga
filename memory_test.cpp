#include <assert.h>
#include <stdlib.h>
#include "helpers.h"
#include "memory.cpp"

int main()
{
    unsigned temp_memory_size = 1024 * 1024 * 100;
    void* temp_memory_block = malloc(temp_memory_size);
    temp_memory_blob_init(temp_memory_block, temp_memory_size);

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
    
    {
        Allocator ta1 = create_temp_allocator();
        unsigned s = 128;
        unsigned char* p1 = (unsigned char*)ta1.alloc(s);
        memset(p1, 0xff, s);
        {
            Allocator ta2 = create_temp_allocator();
            unsigned char* p2 = (unsigned char*)ta2.alloc(s);
            memset(p2, 0xfe, s);
        }
        unsigned char* p3 = (unsigned char*)ta1.alloc(s);
        memset(p3, 0xfd, s);
        unsigned char* p4 = (unsigned char*)ta1.alloc(s);
        memset(p4, 0xfc, s);
    }

    {
        Allocator ta = create_temp_allocator();
        unsigned s = 128;
        unsigned char* p1 = (unsigned char*)ta.alloc(s, 4);
        memset(p1, 0xab, s);
        assert(uintptr_t(p1) % 4 == 0);
        unsigned char* p2 = (unsigned char*)ta.alloc(s);
        memset(p2, 0xac, s);
        assert(uintptr_t(p2) % DefaultMemoryAlign == 0);
        unsigned char* p3 = (unsigned char*)ta.alloc(s, 128);
        memset(p3, 0xad, s);
        assert(uintptr_t(p3) % 128 == 0);
        unsigned char* p4 = (unsigned char*)ta.alloc(s, 36);
        memset(p4, 0xae, s);
        assert(uintptr_t(p4) % 36 == 0);
        unsigned char* p5 = (unsigned char*)ta.alloc(s, 21);
        memset(p5, 0xaf, s);
        assert(uintptr_t(p5) % 21 == 0);
    }

    assert(tms.head == tms.start);

    {
        Allocator a = create_heap_allocator();
        unsigned s = 128;
        unsigned char* p1 = (unsigned char*)a.alloc(s, 4);
        memset(p1, 0xab, s);
        assert(uintptr_t(p1) % 4 == 0);
        unsigned char* p2 = (unsigned char*)a.alloc(s);
        memset(p2, 0xac, s);
        assert(uintptr_t(p2) % DefaultMemoryAlign == 0);
        unsigned char* p3 = (unsigned char*)a.alloc(s, 128);
        memset(p3, 0xad, s);
        assert(uintptr_t(p3) % 128 == 0);
        unsigned char* p4 = (unsigned char*)a.alloc(s, 36);
        memset(p4, 0xae, s);
        assert(uintptr_t(p4) % 36 == 0);
        unsigned char* p5 = (unsigned char*)a.alloc(s, 21);
        memset(p5, 0xaf, s);
        assert(uintptr_t(p5) % 21 == 0);
        a.dealloc(p1);
        a.dealloc(p2);
        a.dealloc(p3);
        a.dealloc(p4);
        a.dealloc(p5);
    }
}