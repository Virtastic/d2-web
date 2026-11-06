// 1.13c D2Client+0x3E7F0: the skill-list walk of the skill-select popup. stdcall (unit, x, p3, p4, p5), ret 0x14.
// Five passes (list row 0..4) over the unit's skills; the x cursor restarts at `x` each pass, the y cursor starts at
// [0xDBC4C] - 0x56 and steps up 0x30 on the first drawn skill of a later row. Every callee goes through Env (a test
// harness can stand in for them; the engine wires the D2Common skill getters and the three draw routines).
#include <cstdint>
#include <cstring>
namespace d2client_3e7f0 {
struct Env {
    uint32_t (*first)(uint32_t unit);                                  // thunk 0xCE98, 1 word
    uint32_t (*next)(uint32_t skill);                                  // thunk 0xC75A, 1 word
    uint32_t (*txt)(uint32_t skill);                                   // thunk 0xC640, ECX = skill: the SkillsTxt-like record (int16 id at +0)
    uint32_t (*flags)(uint32_t skill);                                 // thunk 0xC628, 1 word
    int32_t (*charges)(uint32_t skill, uint32_t* a, uint32_t z0, uint32_t z1, uint32_t* b);   // thunk 0xC766, 5 words; a, b are out-words (-1, 0 before)
    int32_t (*count)(uint32_t skill);                                  // thunk 0xCE9E, 1 word
    int32_t (*state)(uint32_t unit, uint32_t skill);                   // thunk 0xCB74, 2 words
    uint32_t (*level)(uint32_t unit, uint32_t skill, uint32_t one, uint32_t pre);   // thunk 0xC718, 3 words; `pre` = the word pushed ahead of it for 0x3E770 (b)
    void (*f3e490)(int32_t x, uint32_t txt, uint32_t a, uint32_t b, int32_t mode, int32_t y, int32_t p3);      // EAX = x, 6 words
    int32_t (*f3cfa0)(int32_t x, int32_t y, uint32_t skill, int32_t p3, int32_t mode);                         // EAX = x, EDX = y, ESI = skill, 2 words; returns the new x
    void (*f3e770)(int32_t x, int32_t y, uint32_t txt, int32_t mode, uint32_t lvl, uint32_t b);               // EAX = x, ESI = y, 4 words
    void (*f3d8a0)(uint32_t txt, int32_t mode, int32_t cnt, int32_t x, int32_t y);                             // 5 words
    const uint8_t* dt;        // *sgptDataTables (via IAT 0xCE474): +0xB8C desc rows (0x120), +0xB94 count, +0xB98 skill rows (0x23C), +0xBA0 count
    const uint8_t* bm;        // gdwBitMasks (via IAT 0xCEE50): bytes +0x10, +0x18 read
    const int32_t* height;    // 0xDBC4C
    const uint32_t* player;   // 0x11BBFC
    const uint32_t* cmpA;     // 0x1197F8 (unsigned compare)
    const uint32_t* cmpB;     // 0x11985C
    int32_t* rowY;            // 0x119D74..0x119D80: the y of rows 1..4
};
static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline int16_t rd16(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }

void sub_3E7F0(const Env& e, uint32_t unit, int32_t x0, int32_t p3, int32_t p4, int32_t p5) {
    int32_t y = *e.height - 0x56;
    const int32_t step = (p3 ? 0x60 : 0) - 0x30;
    int32_t last = 0;
    uint8_t seen[256]; std::memset(seen, 0, sizeof seen);
    const uint32_t dt = (uint32_t)(uintptr_t)e.dt;
    for (int32_t row = 0; row < 5; ++row) {
        int32_t x = x0;
        for (uint32_t sk = e.first(unit); sk; ) {
            const uint32_t nx = e.next(sk);
            int32_t cnt = 0;
            const uint32_t d = e.txt(sk);
            do {
                if (!d) break;
                const int32_t id = rd16(d);
                if (id < 0 || id >= (int32_t)rd32(dt + 0xBA0)) break;
                const uint32_t sr = (uint32_t)id * 0x23Cu + rd32(dt + 0xB98);
                if (!sr) break;
                const int32_t di = rd16(sr + 0x194);
                if (di < 0 || di >= (int32_t)rd32(dt + 0xB94)) break;
                const uint32_t ds = (uint32_t)di * 0x120u + rd32(dt + 0xB8C);
                if (!ds) break;
                if (p3 && !(e.bm[0x18] & rd8(d + 7))) break;
                if (e.bm[0x10] & rd8(d + 4)) break;
                if (e.flags(sk) & 8) break;
                uint32_t a = 0xFFFFFFFFu, b = 0;
                int32_t k = e.charges(sk, &a, 0, 0, &b);
                if (!p3 && !k) { cnt = e.count(sk); if (cnt > 0 && (e.bm[0x10] & rd8(d + 8))) k = 1; }   // a non-positive count stays in EBX and still picks 0x3D8A0
                if (row < 4) { if (k || row != (int32_t)(int8_t)rd8(ds + 5)) break; }
                else if (!k) break;
                const int32_t q = e.state(unit, sk);
                int32_t mode;
                if (q) mode = q == 6 ? 4 : 1;
                else mode = (unit == *e.player && *e.cmpA < *e.cmpB) ? 1 : 0;
                if (last < row) { y -= 0x30; last = row; }
                const uint8_t pool = rd8(ds + 6);
                if (pool) { if (seen[pool]) break; seen[pool] = 1; }
                if (!p4) {
                    if (!p5) e.f3e490(x, d, a, b, mode, y, p3);
                    x = e.f3cfa0(x, y, sk, p3, mode);
                } else {
                    if (!cnt) e.f3e770(x, y, d, mode, e.level(unit, sk, 1, b), b);
                    else e.f3d8a0(d, mode, cnt, x, y);
                    x += step;
                }
                if (row >= 1) e.rowY[row - 1] = y;
            } while (false);
            sk = nx;
        }
    }
}
}
