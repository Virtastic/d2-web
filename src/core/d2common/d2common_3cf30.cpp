// 1.13c D2Common+0x3CF30: stable insertion sort of 12-byte records. EAX = count (u16), stack (array, comparator), ret 8. Only a record's word at +0 and dword at +4
// move (the dword at +8 stays in its slot). The comparator is fastcall (ECX = held record copy, EDX = candidate copy, both {u16, dword}); a positive answer
// means the held record sorts after the candidate. The held copy is the largest record so far and only changes when the candidate is not smaller.
#include <cstdint>
#include <cstring>

namespace {
struct Rec { uint16_t w; uint32_t d; };
inline Rec Ld(uint8_t* a, uint32_t i) { Rec r; memcpy(&r.w, a + i * 12, 2); memcpy(&r.d, a + i * 12 + 4, 4); return r; }
inline void St(uint8_t* a, uint32_t i, const Rec& r) { memcpy(a + i * 12, &r.w, 2); memcpy(a + i * 12 + 4, &r.d, 4); }
}

extern "C" void D2Common_3CF30_Raw(uint16_t count, uint32_t array, int32_t (*cmp)(const void* held, const void* cand))
{
    if (count <= 1) return;
    uint8_t* a = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(array));
    Rec held = Ld(a, 0);
    for (uint32_t i = 1; i < count; i++) {
        Rec key = Ld(a, i);
        if (cmp(&held, &key) > 0) {
            St(a, i, held);
            uint32_t j = i - 1;
            while (j > 0) {
                Rec t = Ld(a, j - 1);
                if (cmp(&t, &key) <= 0) break;
                St(a, j, t);
                j--;
            }
            St(a, j, key);
        } else held = key;
    }
}
