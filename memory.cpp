#include "memory.h"
#include "types.h"
#include <stdlib.h>

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

struct TempMemoryStorage
{
    unsigned char* start;
    unsigned char* head;
    unsigned capacity;
};

static TempMemoryStorage tms;

void temp_memory_blob_init(void* start, unsigned capacity)
{
    memzero(&tms, TempMemoryStorage);
    tms.start = (unsigned char*)start;
    tms.head = tms.start;
    tms.capacity = capacity;
}

struct TempMemoryHeader
{
    bool freed;
    TempMemoryHeader* prev;
    TempMemoryHeader* next_for_allocator;
    unsigned size;
};

void* temp_memory_blob_alloc(unsigned size, TempMemoryHeader* allocator_latest, unsigned align)
{
    Assert(mem_ptr_diff(tms.start, tms.head + align + size + sizeof(TempMemoryHeader)) < tms.capacity, "Out of temp memory");
    TempMemoryHeader* tmh = (TempMemoryHeader*)mem_align_forward(tms.head, align);
    tmh->freed = false;
    tmh->size = size + align;
    tmh->next_for_allocator = nullptr;

    if (allocator_latest != nullptr)
        allocator_latest->next_for_allocator = tmh;
    
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

    // Free all blocks for the allocator that owns it.
    while (tmh != nullptr)
    {
        tmh->freed = true;

        if (tmh->next_for_allocator == nullptr)
            break;

        tmh = tmh->next_for_allocator;
    }

    // We did not end up at last block. Just quit. Some other dealloaction will trigger the rewind.
    if (tmh != mem_ptr_sub(tms.head, tmh->size + sizeof(TempMemoryHeader)))
        return;

    // Continue backwards in temp memory, rewinding other freed blocks.
    while (tmh != nullptr && tmh->freed)
        tmh = tmh->prev;

    tms.head = tmh == nullptr
        ? tms.head = tms.start
        : (unsigned char*)mem_ptr_add(tmh, tmh->size);
}

void* temp_allocator_alloc(Allocator* allocator, unsigned size)
{
    void* p = temp_memory_blob_alloc(
        size,
        allocator->last_alloc == nullptr
            ? nullptr
            : (TempMemoryHeader*)mem_ptr_sub(allocator->last_alloc, sizeof(TempMemoryHeader)),
        DefaultMemoryAlign
    );
    Assert(p != nullptr, "Failed to allocate memory.");
    allocator->last_alloc = p;
    allocator->first_alloc = allocator->first_alloc == nullptr ? p : allocator->first_alloc;
    return p;
}

void temp_allocator_dealloc(Allocator* allocator, void* ptr)
{
}

void temp_allocator_dealloc_all(Allocator* allocator)
{
    if (allocator->first_alloc == nullptr)
        return;

    temp_memory_blob_dealloc(allocator->first_alloc);
}

#if defined(MEMORY_TRACING_ENABLE)
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
#endif

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

Allocator create_temp_allocator()
{
    Allocator a = {};
    a.alloc_internal = temp_allocator_alloc;
    a.dealloc_internal = temp_allocator_dealloc;
    a.out_of_scope = temp_allocator_dealloc_all;
    return a;
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
