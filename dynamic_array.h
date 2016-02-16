#pragma once

struct Allocator;

struct DynamicArray
{
    uint8* data;
    uint32 size;
    uint32 capacity;
    Allocator* allocator;
};


namespace dynamic_array
{

void* array_push_raw(DynamicArray* da, uint32 size);
DynamicArray create(Allocator* allocator);
void destroy(DynamicArray* da);

}

#define array_push(array, type) (type*)dynamic_array::array_push_raw(array, sizeof(type))
#define array_num(array, type) (array.size / sizeof(type))
#define array_raw(array, type) (type*)array.data
#define array_get(array, type, index) ((type*)(array.data + sizeof(type) * index))