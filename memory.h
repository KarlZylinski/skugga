#pragma once

#include "types.h"

//#define MEMORY_TRACING_ENABLE

#if defined(MEMORY_TRACING_ENABLE)
    #include "callstack_capturer.h"
    #define MaxCaputredCallstacks 1024
#endif

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
    void* last_alloc;
    void* first_alloc;
    unsigned num_allocations;

    #if defined(MEMORY_TRACING_ENABLE)
        CapturedCallstack* captured_callstacks;
    #endif
};

static const unsigned DefaultMemoryAlign = 8;
unsigned mem_ptr_diff(void* ptr1, void* ptr2);
void* mem_ptr_add(void* ptr1, unsigned offset);
void* mem_ptr_sub(void* ptr1, unsigned offset);
void* mem_align_forward(void* p, unsigned align);

void temp_memory_blob_init(void* start, unsigned capacity);
const unsigned TempMemorySize = 1024 * 1024 * 1024;

void heap_allocator_check_clean(Allocator* allocator);

Allocator create_temp_allocator();
Allocator create_heap_allocator();
