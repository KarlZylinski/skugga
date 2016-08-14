#include "memory.h"
#include "types.h"
#include <stdlib.h>

struct TempMemoryHeader
{
    bool freed;
    TempMemoryHeader* prev;
    unsigned size;
};

struct TempMemoryStorage
{
    unsigned char* start;
    unsigned char* head;
    unsigned capacity;
};

unsigned mem_ptr_diff(void* ptr1, void* ptr2)
{
    return (unsigned)((unsigned char*)ptr2 - (unsigned char*)ptr1);
}

void* mem_ptr_add(void* ptr1, unsigned offset)
{
    return (void*)((unsigned char*)ptr1 + offset);
}

void* mem_ptr_sub(void* ptr1, unsigned offset)
{
    return (void*)((unsigned char*)ptr1 - offset);
}

void* mem_align_forward(void* p, unsigned align)
{
    uintptr_t pi = uintptr_t(p);
    const unsigned mod = pi % align;

    if (mod)
        pi += (align - mod);

    return (void *)pi;
}

static TempMemoryStorage tms;

void temp_memory_blob_init(void* start, unsigned capacity)
{
    memzero(&tms, TempMemoryStorage);
    tms.start = (unsigned char*)start;
    tms.head = tms.start;
    tms.capacity = capacity;
}

void* temp_memory_blob_alloc(unsigned size, unsigned align)
{
    Assert(mem_ptr_diff(tms.start, tms.head + align + size + sizeof(TempMemoryHeader)) < tms.capacity, "Out of temp memory");
    TempMemoryHeader* tmh = (TempMemoryHeader*)mem_align_forward(tms.head, align);
    tmh->freed = false;
    tmh->size = size + align;
    
    if (tms.head == tms.start)
        tmh->prev = nullptr;

    tms.head += sizeof(TempMemoryHeader) + size + align;

    // Set next block's prev to this one.
    if (mem_ptr_diff(tms.start, tms.head + sizeof(TempMemoryHeader) + align) < tms.capacity)
    {
        TempMemoryHeader* next_header = (TempMemoryHeader*)mem_align_forward(tms.head, align);
        next_header->prev = tmh;
    }

    return mem_ptr_add(tmh, sizeof(TempMemoryHeader));
}

void temp_memory_blob_dealloc(void* ptr)
{
    if (ptr == nullptr)
        return;

    TempMemoryHeader* tmh = (TempMemoryHeader*)mem_ptr_sub(ptr, sizeof(TempMemoryHeader));
    tmh->freed = true;

    // Are we the last block?
    if (mem_align_forward(mem_ptr_add(ptr, tmh->size)) != tms.head)
        return;

    // Continue backwards in temp memory, rewinding other freed blocks.
    while (tmh != nullptr && tmh->freed)
    {
        tmh = tmh->prev;
    }

    tms.head = tmh == nullptr ? tms.start : (unsigned char*)tmh;
}

void* temp_allocator_alloc(Allocator* allocator, unsigned size)
{
    Assert(allocator->num_allocations + 1 < Allocator::MaxAllocations, "Too many allocations.");
    void* p = temp_memory_blob_alloc(size);
    Assert(p != nullptr, "Failed to allocate memory.");
    allocator->allocations[allocator->num_allocations++] = p;
    return p;
}

void temp_allocator_dealloc(Allocator* allocator, void* ptr)
{
    if (ptr == nullptr)
        return;

    temp_memory_blob_dealloc(ptr);

    for (unsigned i = 0; i < allocator->num_allocations; ++i)
    {
        if (allocator->allocations[i] == ptr)
        {
            unsigned last_index = allocator->num_allocations - 1;
            if (allocator->num_allocations > 1 && i != last_index)
            {
                allocator->allocations[i] = allocator->allocations[last_index];
                allocator->allocations[last_index] = nullptr;
            }
            else
            {
                allocator->allocations[i] = nullptr;
            }

            --allocator->num_allocations;
            break;
        }
    }
}

void temp_allocator_dealloc_all(Allocator* allocator)
{
    for (unsigned char i = 0; i < allocator->num_allocations; ++i)
    {
        temp_memory_blob_dealloc(allocator->allocations[i]);
    }

    memset(allocator->allocations, 0, sizeof(void*) * Allocator::MaxAllocations);
    allocator->num_allocations = 0;
}

static void add_captured_callstack(CapturedCallstack* callstacks, const CapturedCallstack& cc)
{
    for (unsigned i = 0; i < MaxCaputredCallstacks; ++i)
    {
        if (!callstacks[i].used)
        {
            callstacks[i] = cc;
            return;
        }
    }

    Error("Out of callstacks. Increase MaxCaputredCallstacks in memory.h.");
}

static void remove_captured_callstack(CapturedCallstack* callstacks, void* p)
{
    for (unsigned i = 0; i < MaxCaputredCallstacks; ++i) {
        if (callstacks[i].ptr == p)
        {
            callstacks[i].used = false;
            return;
        }
    }

    Error("Failed to find callstack in remove_captured_callstack.");
}

static void ensure_captured_callstacks_unused(CapturedCallstack* callstacks)
{
    for (unsigned i = 0; i < MaxCaputredCallstacks; ++i)
    {
        if (!callstacks[i].used)
            continue;

        callstack_print("Memory leak stack trace", callstacks + i);
    }
}

void* heap_allocator_alloc(Allocator* allocator, unsigned size)
{
    ++allocator->num_allocations;
    void* p = malloc(size);

    #ifdef MEMORY_TRACING_ENABLE
        add_captured_callstack(allocator->captured_callstacks, callstack_capture(1, p));
    #endif

    return p;
}

void heap_allocator_dealloc(Allocator* allocator, void* ptr)
{
    if (ptr == nullptr)
        return;

    #ifdef MEMORY_TRACING_ENABLE
        remove_captured_callstack(allocator->captured_callstacks, ptr);
    #endif

    free(ptr);
    --allocator->num_allocations;
}

void heap_allocator_check_clean(Allocator* allocator)
{
    #ifdef MEMORY_TRACING_ENABLE
        ensure_captured_callstacks_unused(allocator->captured_callstacks);
    #endif

    Assert(allocator->num_allocations == 0, "Heap allocator not clean on shutdown.");
}

Allocator create_heap_allocator()
{
    Allocator a = {};

    #ifdef MEMORY_TRACING_ENABLE
        unsigned cc_size = sizeof(CapturedCallstack) * MaxCaputredCallstacks;
        a.captured_callstacks = (CapturedCallstack*)malloc(cc_size);
        memset(a.captured_callstacks, 0, cc_size);
    #endif

    a.alloc_internal = heap_allocator_alloc;
    a.dealloc_internal = heap_allocator_dealloc;
    return a;
}
