// 1.13c D2Client+0x9D160 (87 B, EAX = chain head, returns the sorted head in EAX): insertion sort of an item
// chain (next @+0x80) by the signed dword @+0x18, equal keys keep the later node first.
#include <emscripten.h>
#include <cstdint>
static inline uint32_t* W(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
extern "C" uint32_t d2c_port_9d160(uint32_t head) {
    uint32_t sorted = 0;
    while (head != 0) {
        uint32_t nx = W(head)[0x80 / 4];
        if (sorted != 0) {
            uint32_t p = sorted, prev = 0, at;
            do {
                at = p;
                if ((int32_t)W(at)[0x18 / 4] < (int32_t)W(head)[0x18 / 4]) break;
                p = W(at)[0x80 / 4]; prev = at;
            } while (W(at)[0x80 / 4] != 0);
            if (prev != 0) { W(head)[0x80 / 4] = W(prev)[0x80 / 4]; W(prev)[0x80 / 4] = head; head = nx; continue; }
        }
        W(head)[0x80 / 4] = sorted; sorted = head; head = nx;
    }
    return sorted;
}
