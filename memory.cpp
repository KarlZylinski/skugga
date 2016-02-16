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
    tmh->size = size + align;
    
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
    if (ptr == nullptr)
        return;

    TempMemoryHeader* tmh = (TempMemoryHeader*)memory::ptr_sub(ptr, sizeof(TempMemoryHeader));
    tmh->freed = true;

    // Are we the last block?
    if (memory::align_forward(memory::ptr_add(ptr, tmh->size)) != tms.head)
        return;

    // Continue backwards in temp memory, rewinding other freed blocks.
    while (tmh != nullptr && tmh->freed)
    {
        tmh = tmh->prev;
    }

    tms.head = tmh == nullptr ? tms.start : (uint8*)tmh;
}

}

namespace temp_allocator
{

namespace internal
{

void* alloc(Allocator* allocator, unsigned size)
{
    Assert(allocator->num_allocations + 1 < Allocator::MaxAllocations, "Too many allocations.");
    void* p = temp_memory::alloc(size);
    Assert(p != nullptr, "Failed to allocate memory.");
    allocator->allocations[allocator->num_allocations++] = p;
    return p;
}

void dealloc(Allocator* allocator, void* ptr)
{
    if (ptr == nullptr)
        return;
    
    temp_memory::dealloc(ptr);

    for (uint32 i = 0; i < allocator->num_allocations; ++i)
    {
        if (allocator->allocations[i] == ptr)
        {
            uint32 last_index = allocator->num_allocations - 1;
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

void dealloc_all(Allocator* allocator)
{
    for (uint8 i = 0; i < allocator->num_allocations; ++i)
    {
        temp_memory::dealloc(allocator->allocations[i]);
    }

    memset(allocator->allocations, 0, sizeof(void*) * Allocator::MaxAllocations);
    allocator->num_allocations = 0;
}

}

}

#define create_temp_allocator() { temp_allocator::internal::alloc, temp_allocator::internal::dealloc, temp_allocator::internal::dealloc_all }

