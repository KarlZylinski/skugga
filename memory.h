#pragma once

struct Allocator
{
    ~Allocator()
    {
        if (out_of_scope)
            out_of_scope(this);
    }

    void* alloc(uint32 size)
    {
        return alloc_internal(this, size);
    }

    void dealloc(void* ptr)
    {
        dealloc_internal(this, ptr);
    }

    void*(*alloc_internal)(Allocator* alloc, uint32 size);
    void(*dealloc_internal)(Allocator* alloc, void* ptr);
    void(*out_of_scope)(Allocator* alloc);
    static const uint8 MaxAllocations = 128;
    void* allocations[MaxAllocations];
    uint8 num_allocations;
};

namespace temp_memory
{

const uint32 TempMemorySize = 1024 * 1024 * 1024;

}
