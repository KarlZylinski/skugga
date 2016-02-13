struct TempMemory
{
    uint8* start;
    unsigned capacity;
    unsigned head_offset;
};


namespace memory
{

unsigned ptr_diff(void* ptr1, void* ptr2)
{
    return (unsigned)((uint8*)ptr2 - (uint8*)ptr1);
}

}

TempMemory global_temp_memory;

namespace temp_memory
{

void init(void* start, unsigned capacity)
{
    memset(&global_temp_memory, 0, sizeof(TempMemory));
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

void reset()
{
    global_temp_memory.head_offset = 0;
}

}