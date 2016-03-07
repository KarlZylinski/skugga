#pragma once

#include "types.h"

struct Allocator
{
    ~Allocator()
    {
        if (out_of_scope)
            out_of_scope(this);
    }

    void* alloc(unsigned size)
    {
        return alloc_internal(this, size);
    }

    void dealloc(void* ptr)
    {
        dealloc_internal(this, ptr);
    }

    void*(*alloc_internal)(Allocator* alloc, unsigned size);
    void(*dealloc_internal)(Allocator* alloc, void* ptr);
    void(*out_of_scope)(Allocator* alloc);
    static const unsigned MaxAllocations = 128;
    void* allocations[MaxAllocations];
    unsigned num_allocations;
};

namespace memory 
{

static const unsigned default_align = 8;
unsigned ptr_diff(void* ptr1, void* ptr2);
void* ptr_add(void* ptr1, unsigned offset);
void* ptr_sub(void* ptr1, unsigned offset);
void* align_forward(void* p, unsigned align = default_align);

}

namespace temp_memory
{

void init(void* start, unsigned capacity);
const unsigned TempMemorySize = 1024 * 1024 * 1024;

}


namespace temp_allocator
{

void* alloc(Allocator* allocator, unsigned size);
void dealloc(Allocator* allocator, void* ptr);
void dealloc_all(Allocator* allocator);

}


#define create_temp_allocator() { temp_allocator::alloc, temp_allocator::dealloc, temp_allocator::dealloc_all }
