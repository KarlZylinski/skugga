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

struct PermanentMemoryStorage
{
    unsigned char* start;
    unsigned char* head;
    unsigned capacity;
};

static PermanentMemoryStorage pms;

void permanent_memory_blob_init(void* start, unsigned capacity)
{
    memzero(&pms, PermanentMemorySize);
    pms.start = (unsigned char*)start;
    pms.head = pms.start;
    pms.capacity = capacity;
}

void* permanent_alloc(unsigned size, unsigned align)
{
    Assert(mem_ptr_diff(pms.start, pms.head) + size + align <= pms.capacity, "Out of permanent memory.");
    void* p = mem_align_forward(pms.head, align);
    pms.head += size + align;
    return p;
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
    TempMemoryHeader* prev_for_allocator;
    unsigned offset_to_next;
};

static void* temp_memory_blob_alloc(unsigned size, void* allocator_latest, unsigned align)
{
    const unsigned header_align = alignof(TempMemoryHeader);
    const unsigned header_size = sizeof(TempMemoryHeader);
    const unsigned diff_to_header_size = sizeof(unsigned);
    Assert(mem_ptr_diff(tms.start, tms.head + align + header_align + size + header_size + diff_to_header_size) < tms.capacity, "Out of temp memory");
    TempMemoryHeader* tmh = (TempMemoryHeader*)mem_align_forward(tms.head, header_align);
    tmh->freed = false;

    if (allocator_latest != nullptr)
        tmh->prev_for_allocator = (TempMemoryHeader*)mem_ptr_sub(allocator_latest, *(unsigned*)mem_ptr_sub(allocator_latest, diff_to_header_size));
    else
        tmh->prev_for_allocator = nullptr;
    
    if (tms.head == tms.start)
        tmh->prev = nullptr;

    tms.head += header_align + header_size + align + diff_to_header_size + size;
    tmh->offset_to_next = mem_ptr_diff(tmh, tms.head);

    // Set next block's prev to this one.
    if (mem_ptr_diff(tms.start, tms.head + header_align + diff_to_header_size + header_size) < tms.capacity)
    {
        TempMemoryHeader* next_header = (TempMemoryHeader*)mem_align_forward(tms.head, header_align);
        next_header->prev = tmh;
    }
    
    // The reason we add the diff_to_header is so we know how far back the header is, since the diff caused by the alignment varies.
    void* after_header = mem_ptr_add(tmh, header_size + diff_to_header_size);
    void* ptr_return = mem_align_forward(after_header, align);
    unsigned diff_to_header = mem_ptr_diff(tmh, ptr_return);
    *(unsigned*)mem_ptr_sub(ptr_return, diff_to_header_size) = diff_to_header;
    return ptr_return;
}

static void temp_memory_blob_dealloc(void* ptr)
{
    if (ptr == nullptr)
        return;

    // There is an unsigned int just before the data which says how long back the header lives.
    unsigned diff_to_header = *(unsigned*)mem_ptr_sub(ptr, sizeof(unsigned));
    TempMemoryHeader* tmh = (TempMemoryHeader*)mem_ptr_sub(ptr, diff_to_header);
    tmh->freed = true;

    TempMemoryHeader* free_for_alloc = tmh;
    // Free all blocks for the allocator that owns it.
    while (free_for_alloc != nullptr)
    {
        free_for_alloc->freed = true;
        free_for_alloc = free_for_alloc->prev_for_allocator;
    }

    // We did not end up at last block. Just quit. Some other dealloaction will trigger the rewind.
    if (mem_ptr_add(tmh, tmh->offset_to_next) != tms.head)
        return;

    // Continue backwards in temp memory, rewinding other freed blocks.
    while (tmh != nullptr && tmh->freed)
        tmh = tmh->prev;

    // If tmh is null, then we arrived at start of memory
    tms.head = tmh == nullptr
        ? tms.head = tms.start
        : (unsigned char*)mem_ptr_add(tmh, tmh->offset_to_next);
}

void* temp_allocator_alloc(Allocator* allocator, unsigned size, unsigned align)
{
    void* p = temp_memory_blob_alloc(size, allocator->last_alloc, align);
    Assert(p != nullptr, "Failed to allocate memory.");
    allocator->last_alloc = p;
    return p;
}

void temp_allocator_dealloc(Allocator* allocator, void* ptr)
{
}

void temp_allocator_dealloc_all(Allocator* allocator)
{
    if (allocator->last_alloc == nullptr)
        return;

    temp_memory_blob_dealloc(allocator->last_alloc);
}

#if defined(MEMORY_TRACING_ENABLE)
static const unsigned max_captured_callstacks = 1024;

static void add_captured_callstack(CapturedCallstack* callstacks, const CapturedCallstack& cc)
{
    for (unsigned i = 0; i < max_captured_callstacks; ++i)
    {
        if (!callstacks[i].used)
        {
            callstacks[i] = cc;
            return;
        }
    }

    Error("Out of callstacks. Increase max_captured_callstacks in memory.h.");
}

static void remove_captured_callstack(CapturedCallstack* callstacks, void* p)
{
    for (unsigned i = 0; i < max_captured_callstacks; ++i) {
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
    for (unsigned i = 0; i < max_captured_callstacks; ++i)
    {
        if (!callstacks[i].used)
            continue;

        callstack_print("Memory leak stack trace", callstacks + i);
    }
}
#endif

void* heap_allocator_alloc(Allocator* allocator, unsigned size, unsigned align)
{
    ++allocator->num_allocations;
    void* p = malloc(size);

    #ifdef MEMORY_TRACING_ENABLE
        if (allocator->captured_callstacks == nullptr)
        {
            unsigned cc_size = sizeof(CapturedCallstack) * max_captured_callstacks;
            allocator->captured_callstacks = (CapturedCallstack*)malloc(cc_size);
            memset(allocator->captured_callstacks, 0, cc_size);
        }

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
