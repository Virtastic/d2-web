// 1.13c D2Common+0x38CD0: EAX = stat key, ECX = stat list (this), stack word = request (byte at +4 holds flag bits), ret 4. The result starts as the value of the
// key in the list's own sorted {key, value} array (struct {ptr, i16 count} at list+0x24; 0x382B0 finds the index with EDI = key; 0 when absent). A list whose flags
// (+0x10) are non-negative returns that. Otherwise it adds the same lookup in every child of the chain (head +0x3C, next +0x2C): a child is skipped when the request's
// flag byte ANDed with the word at +8 of the table the module global 0x890B0 points at is non-zero and the child flag bit 30 is set; a child with bit 31 searches the
// array struct at +0x48, else the one at +0x24. The child search is the same signed lower-bound loop, inline. 0x382B0 is a seam.
#include <cstdint>
#include <cstring>

struct D2Common38CD0Env {
    int32_t (*find)(uint32_t arrayStruct, uint32_t key);   // 0x382B0 (EAX = struct, EDI = key)
};

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline int32_t Rs16(uint32_t a) { int16_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
}

extern "C" int32_t D2Common_38CD0_Raw(int32_t key, uint32_t list, uint32_t req, uint32_t maskTable, const D2Common38CD0Env* env)
{
    int32_t total = 0;
    const int32_t idx = env->find(list + 0x24, static_cast<uint32_t>(key));
    if (idx >= 0) {
        const uint32_t e = Rd(list + 0x24) + static_cast<uint32_t>(idx) * 8;
        if (e != 0) total = static_cast<int32_t>(Rd(e + 4));
    }
    if (static_cast<int32_t>(Rd(list + 0x10)) >= 0) return total;
    const uint32_t flag = ((*reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(req + 4))) & Rd(maskTable + 8));
    for (uint32_t c = Rd(list + 0x3C); c != 0; c = Rd(c + 0x2C)) {
        if (flag != 0 && (Rd(c + 0x10) & 0x40000000u) != 0) continue;
        const uint32_t s = (static_cast<int32_t>(Rd(c + 0x10)) < 0) ? c + 0x48 : c + 0x24;
        const uint32_t arr = Rd(s);
        int32_t lo = 0, hi = Rs16(s + 4);
        int32_t val = 0;
        if (hi > 0) {
            do {
                const int32_t mid = (hi - lo) / 2 + lo;
                const int32_t k = static_cast<int32_t>(Rd(arr + static_cast<uint32_t>(mid) * 8));
                if (key > k) lo = mid + 1;
                else if (key < k) hi = mid;
                else { val = static_cast<int32_t>(Rd(arr + static_cast<uint32_t>(mid) * 8 + 4)); break; }
            } while (lo < hi);
        }
        total += val;
    }
    return total;
}
