#include "dynamic_array.h"
#include "memory.h"

namespace dynamic_array
{

namespace internal
{

void grow(DynamicArray* da, uint32 min_remaining_capacity)
{
    uint8* old_data = da->data;
    uint32 new_capacity = max(da->capacity * 2, da->size + min_remaining_capacity);
    da->data = (uint8*)da->allocator->alloc(new_capacity);
    memcpy(da->data, old_data, da->size);
    da->allocator->dealloc(old_data);
    da->capacity = new_capacity;
}

} // namespace internal

void* array_push_raw(DynamicArray* da, uint32 size)
{
    if (da->data == nullptr || da->size + size > da->capacity)
    {
        internal::grow(da, size);
    }

    uint8* p = da->data + da->size;
    da->size += size;
    memset(p, 0, size);
    return p;
}

DynamicArray create(Allocator* allocator)
{
    DynamicArray da = {};
    da.allocator = allocator;
    return da;
}

void destroy(DynamicArray* da)
{
    da->allocator->dealloc(da->data);
}

} // namespace dynamic_array
