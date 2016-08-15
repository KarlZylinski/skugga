#pragma once

//#define ENABLE_MEMORY_TRACING

#if defined(ENABLE_MEMORY_TRACING)
    #include "callstack_capturer.h"
#endif

const unsigned DefaultMemoryAlign = 8;

struct Allocator
{
    ~Allocator()
    {
        if (out_of_scope)
            out_of_scope(this);
    }

    void* alloc(unsigned size, unsigned align = DefaultMemoryAlign)
    {
        return alloc_internal(this, size, align);
    }

    void dealloc(void* ptr)
    {
        dealloc_internal(this, ptr);
    }

    void*(*alloc_internal)(Allocator* alloc, unsigned size, unsigned align);
    void(*dealloc_internal)(Allocator* alloc, void* ptr);
    void(*out_of_scope)(Allocator* alloc);
    void* last_alloc;
    unsigned num_allocations;

    #if defined(ENABLE_MEMORY_TRACING)
        CapturedCallstack* captured_callstacks;
    #endif
};

unsigned mem_ptr_diff(void* ptr1, void* ptr2);
void* mem_ptr_add(void* ptr1, unsigned offset);
void* mem_ptr_sub(void* ptr1, unsigned offset);
void* mem_align_forward(void* p, unsigned align);

void permanent_memory_blob_init(void* start, unsigned capacity);
const unsigned PermanentMemorySize = 32 * 1024 * 1024;
void* permanent_alloc(unsigned size, unsigned align = DefaultMemoryAlign);

const unsigned TempMemorySize = 1024 * 1024 * 1024;
void temp_memory_blob_init(void* start, unsigned capacity);
void* temp_allocator_alloc(Allocator* allocator, unsigned size, unsigned align);
void temp_allocator_dealloc(Allocator* allocator, void* ptr);
void temp_allocator_dealloc_all(Allocator* allocator);

#define create_temp_allocator() {temp_allocator_alloc, temp_allocator_dealloc, temp_allocator_dealloc_all}

void heap_allocator_check_clean(Allocator* allocator);
void* heap_allocator_alloc(Allocator* allocator, unsigned size, unsigned align);
void heap_allocator_dealloc(Allocator* allocator, void* ptr);

#define create_heap_allocator() {heap_allocator_alloc, heap_allocator_dealloc, nullptr};
