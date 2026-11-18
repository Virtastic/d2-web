// D2sound list-length helper: counts nodes of a singly linked list (next at +4); 32-bit retail pointers.
#include <cstdint>
#include <cstring>
// 1.13c D2sound+0x08690
int d2sound_list_count_8690(const void* node)
{
    int n = 0;
    while (node) {
        ++n;
        uint32_t next; std::memcpy(&next, (const uint8_t*)node + 4, 4);
        node = (const void*)(uintptr_t)next;
    }
    return n;
}
