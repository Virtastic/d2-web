// D2Client find-by-id in the global list (head = DAT_6fbcbc14). Retail 32-bit pointers; memory reads go through the rd seam.
#include <cstdint>
typedef uint32_t (*d2c_rd32_fn)(uint32_t addr);
// 1.13c D2Client+0x9D630 (ECX unused; id=param_2; head=[0x1CBC14]; entry+0x10 id, +0x38 sublist {id,next}, +0x80 next)
uint32_t d2c_9d630(d2c_rd32_fn rd, uint32_t head, int32_t id) {
    if (id == -1) return 0;
    for (uint32_t e = head; e; e = rd(e + 0x80)) {
        if ((int32_t)rd(e + 0x10) == id) return e;
        for (uint32_t n = rd(e + 0x38); n; n = rd(n + 4))
            if ((int32_t)rd(n) == id) return e;
    }
    return 0;
}
