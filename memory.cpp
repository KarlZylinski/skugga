#include "types.h"

struct TempMemoryHeader
{
    bool freed;
    TempMemoryHeader* prev;
    uint32 size;
};

struct TempMemoryStorage
{
    uint8* start;
    uint8* head;
    uint32 capacity;
};


namespace memory
{

static const uint32 default_align = 8;

unsigned ptr_diff(void* ptr1, void* ptr2)
{
    return (unsigned)((uint8*)ptr2 - (uint8*)ptr1);
}

void* ptr_add(void* ptr1, uint32 offset)
{
    return (void*)((uint8*)ptr1 + offset);
}

void* ptr_sub(void* ptr1, uint32 offset)
{
    return (void*)((uint8*)ptr1 - offset);
}

void* align_forward(void* p, uint32 align = default_align)
{
    uintptr_t pi = uintptr_t(p);
    const uint32 mod = pi % align;

    if (mod)
        pi += (align - mod);

    return (void *)pi;
}

}

namespace temp_memory
{

TempMemoryStorage tms;

void init(void* start, unsigned capacity)
{
    memzero(&tms, TempMemoryStorage);
    tms.start = (uint8*)start;
    tms.head = tms.start;
    tms.capacity = capacity;
}

void* alloc(uint32 size, uint32 align = memory::default_align)
{
    Assert(memory::ptr_diff(tms.start, tms.head + align + size + sizeof(TempMemoryHeader)) < tms.capacity, "Out of temp memory");
    TempMemoryHeader* tmh = (TempMemoryHeader*)memory::align_forward(tms.head, align);
    tmh->freed = false;
    tmh->size = size;
    
    if (tms.head == tms.start)
        tmh->prev = nullptr;

    tms.head += sizeof(TempMemoryHeader) + size + align;

    // Set next block's prev to this one.
    if (memory::ptr_diff(tms.start, tms.head + sizeof(TempMemoryHeader) + align) < tms.capacity)
    {
        TempMemoryHeader* next_header = (TempMemoryHeader*)memory::align_forward(tms.head, align);
        next_header->prev = tmh;
    }

    return memory::ptr_add(tmh, sizeof(TempMemoryHeader));
}

void dealloc(void* ptr)
{
    TempMemoryHeader* tmh = (TempMemoryHeader*)memory::ptr_sub(ptr, sizeof(TempMemoryHeader));
    tmh->freed = true;

    if (memory::ptr_add(tmh, sizeof(TempMemoryHeader) + tmh->size) != tms.head)
        return;

    // Continue backwards in temp memory, rewinding other freed blocks.

    while (tmh != nullptr && tmh->freed)
    {
        tmh = tmh->prev;
    }

    tms.head = tmh == nullptr ? tms.start : (uint8*)tmh;
}

}

/*
struct Allocator
{
    ~Allocator()
    {
        if (out_of_scope)
            out_of_scope(this);
    }

    void*(*alloc)(Allocator* alloc, unsigned size);
    void(*out_of_scope)(Allocator* alloc);

    uint8* mem;
    uint32 capacity;
};


namespace memory
{

unsigned ptr_diff(void* ptr1, void* ptr2)
{
    return (unsigned)((uint8*)ptr2 - (uint8*)ptr1);
}

}

TempMemoryStorage global_temp_memory;

namespace temp_memory
{

void init(void* start, unsigned capacity)
{
    memset(&global_temp_memory, 0, sizeof(TempMemoryStorage));
    global_temp_memory.start = (uint8*)start;
    global_temp_memory.capacity = capacity;
}

void* alloc(unsigned size)
{
    Assert(memory::ptr_diff(global_temp_memory.start, global_temp_memory.start + global_temp_memory.head_offset + size) < global_temp_memory.capacity, "Out of temp memory");
    void* p = global_temp_memory.start;
    global_temp_memory.head_offset += size;
    return p;
}

void out_of_scope(Allocator* allocator)
{

}

Allocator create()
{
    Allocator allocator = {0};
    allocator.alloc = alloc;
    allocator.out_of_scope = allocator_out_of_scope;
}

void reset()
{
    global_temp_memory.head_offset = 0;
}

}*/