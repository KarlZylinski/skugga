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

static const unsigned DefaultMemoryAlign = 8;
unsigned mem_ptr_diff(void* ptr1, void* ptr2);
void* mem_ptr_add(void* ptr1, unsigned offset);
void* mem_ptr_sub(void* ptr1, unsigned offset);
void* mem_align_forward(void* p, unsigned align = DefaultMemoryAlign);

void temp_memory_blob_init(void* start, unsigned capacity);
void* temp_memory_blob_alloc(unsigned size, unsigned align = DefaultMemoryAlign);
void temp_memory_blob_dealloc(void* ptr);
const unsigned TempMemorySize = 1024 * 1024 * 1024;

void* temp_allocator_alloc(Allocator* allocator, unsigned size);
void temp_allocator_dealloc(Allocator* allocator, void* ptr);
void temp_allocator_dealloc_all(Allocator* allocator);

void* heap_allocator_alloc(Allocator* allocator, unsigned size);
void heap_allocator_dealloc(Allocator* allocator, void* ptr);

inline Allocator create_temp_allocator()
{
    Allocator a = {};
    a.alloc_internal = temp_allocator_alloc;
    a.dealloc_internal = temp_allocator_dealloc;
    a.out_of_scope = temp_allocator_dealloc_all;
    return a;
}
 
inline Allocator create_heap_allocator()
{
    Allocator a = {};
    a.alloc_internal = heap_allocator_alloc;
    a.dealloc_internal = heap_allocator_dealloc;
    return a;
}
