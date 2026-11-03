// 1.13c D2Client+0x62410 (355 B, no arguments): room = Ordinal_10331([11BBFC]); Ordinal_10383(room, &rooms, &count) (D2Common near-room list);
// for each room, each node of the chain [room+0x74] / next [+0xE8]: 619E0(EAX=node). Then, with prev = [11BC14] read at entry and cur = [11BC14] now,
// both non-null: s1 = word [prev+0x22]; if more than one list node (from cur, next [+0x80]) has that word, s1 != 0xFFFF and [F16B8] != 0, every node q after prev
// whose key [q+0x10] is not in the 128-bucket table (bucket key&0x7F, entry key [+0xC], next [+0xE4], a hit with [entry] != 0 is fatal 0x5A) and whose
// word [q+0x22] is shared by more than one node (else 0xFFFF) equal to s1 runs 60AF0(EDI=q); cur is re-read after each such call.
#include <emscripten.h>
#include <cstdint>
typedef uint32_t u32;
static inline u32 r32(u32 a) { return *(const u32*)(uintptr_t)a; }
static inline uint16_t r16(u32 a) { return *(const uint16_t*)(uintptr_t)a; }
struct D2c62410Env {
    u32 (*ord10331)(u32 player);
    void (*ord10383)(u32 room, u32* rooms, int32_t* count);
    void (*f619e0)(u32 node);
    u32 (*f60af0)(u32 q);   // EDI = q
    void (*fatal)(u32 code);
};
// Returns retail's EAX: cur when prev or cur is null; 0 when the gates fail (the count walk ends on a null next, [F16B8] is 0);
// [F16B8] when prev has no next; else the last q's residue: the matched bucket entry, the high half of its key with the shared word,
// 0xFFFF, or 60AF0's result.
extern "C" EMSCRIPTEN_KEEPALIVE
u32 d2c_62410(const u32* g_bc14, u32 g_bbfc, u32 g_a16b8, const u32* g_a608, const D2c62410Env* e) {
    u32 prev = *g_bc14;
    u32 rooms = 0; int32_t cnt = 0;
    e->ord10383(e->ord10331(g_bbfc), &rooms, &cnt);
    for (int32_t i = 0; i < cnt; i++) {
        u32 room = r32(rooms + (u32)i * 4);
        for (u32 n = r32(room + 0x74); n; n = r32(n + 0xe8)) e->f619e0(n);
    }
    u32 cur = *g_bc14;
    if (!prev || !cur) return cur;
    uint16_t s1 = r16(prev + 0x22);
    int32_t c = 0;
    for (u32 p = cur; p; p = r32(p + 0x80)) if (r16(p + 0x22) == s1) c++;
    if (c <= 1 || s1 == 0xffff || !g_a16b8) return 0;
    u32 eax = g_a16b8;
    for (u32 q = r32(prev + 0x80); q; q = r32(q + 0x80)) {
        u32 key = r32(q + 0x10);
        bool hit = false;
        for (u32 p = g_a608[key & 0x7f]; p; p = r32(p + 0xe4)) {
            if (r32(p + 0xc) == key) {
                if (r32(p)) { e->fatal(0x5a); return 0; }
                hit = true; eax = p; break;
            }
        }
        if (hit) continue;
        eax = 0xffff;
        if (cur) {
            uint16_t t = r16(q + 0x22);
            int32_t c2 = 0;
            for (u32 p = cur; p; p = r32(p + 0x80)) if (r16(p + 0x22) == t) c2++;
            if (c2 > 1) eax = (key & 0xffff0000u) | t;
        }
        if (s1 == (uint16_t)eax) { eax = e->f60af0(q); cur = *g_bc14; }
    }
    return eax;
}
