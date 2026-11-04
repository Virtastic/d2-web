// D2Game+0x31E20 -- place a new object (unit type 2) at a seeded free spot inside a room.
// stack (game, classId, sizeX, sizeY, room) ret 0x14; returns the unit or 0.
// #a6d0(room, &box{x,y,w,h}); w < 2 or h < 2 -> 0. Up to 5 tries, seed = *(game+0x10f0):
//   cx = roll(w - sizeX - 1) + x, cy = roll(h - sizeY - 1) + y (inline roll; range <= 0 gives 0, seed untouched)
//   accept when cx, cy != 0, x+1 <= (u16)cx < x+w-1, y+1 <= (u16)cy < y+h-1 and
//   #ae38(room, cx, cy, sizeX+6, sizeY+6, 0x3f11) returns 0 (low 16 bits).
// Then a null room is fatal (0x54e); the unit comes from #a6b8(game+0x1c, 2) and is filled in field by field
// (see the spec); 0x2F300 gets (esi unit, ebx game; game+0x98 after its bump, room, cx, cy) and
// 0xE0F40 (ecx cy, edx unit, eax room; cx, game, 1).
#include <cstdint>
#include <cstring>

struct D2Game31E20Env {
    void     (*fatal)(uint32_t line);
    void     (*a6d0)(uint32_t room, uint32_t box);
    uint16_t (*ae38)(uint32_t room, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t mask);
    uint32_t (*a6b8)(uint32_t pool, uint32_t type);
    uint32_t (*a556)(uint32_t room);
    uint8_t  (*a56e)(uint32_t level);
    void     (*subE0110)(uint32_t esi, uint32_t edi);
    uint32_t (*a2e6)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t zero);
    uint32_t file;                                   // retail "..\Source\D2Game\UNIT\SUnit.cpp" address
    uint32_t (*a520)(uint32_t classId);
    void     (*a724)(uint32_t unit);
    void     (*sub2F300)(uint32_t esi, uint32_t ebx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    void     (*subE0F40)(uint32_t ecx, uint32_t edx, uint32_t eax, uint32_t s1, uint32_t s2, uint32_t s3);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

static uint32_t roll(uint32_t seed, int32_t range)
{
    if (range <= 0) return 0;
    const uint64_t s = (uint64_t)rd32(seed) * 0x6ac690c5u + rd32(seed + 4);
    wr32(seed, (uint32_t)s); wr32(seed + 4, (uint32_t)(s >> 32));
    const uint32_t n = (uint32_t)range;
    return (n & (n - 1)) ? (uint32_t)s % n : ((uint32_t)s & (n - 1));
}

// 1.13c D2Game+0x31E20
extern "C" uint32_t d2g_31e20(uint32_t game, uint32_t classId, int32_t sizeX, int32_t sizeY,
                                                           uint32_t room, const D2Game31E20Env* env)
{
    int32_t box[4] = { 0, 0, 0, 0 };
    env->a6d0(room, (uint32_t)(uintptr_t)box);
    const int32_t bx = box[0], by = box[1], bw = box[2], bh = box[3];
    if (bw < 2 || bh < 2) return 0;
    const uint32_t seed = rd32(game + 0x10f0);
    int tries = 0;
    uint32_t cx = 0, cy = 0;
    for (;;) {
        cx = roll(seed, bw - sizeX - 1) + (uint32_t)bx;
        cy = roll(seed, bh - sizeY - 1) + (uint32_t)by;
        if (cx != 0 && cy != 0) {
            const int32_t x16 = (uint16_t)cx, y16 = (uint16_t)cy;
            if (x16 >= bx + 1 && y16 >= by + 1 && x16 < bx + bw - 1 && y16 < by + bh - 1 &&
                env->ae38(room, cx, cy, (uint32_t)(sizeX + 6), (uint32_t)(sizeY + 6), 0x3f11) == 0)
                break;
        }
        if (++tries >= 5) return 0;
    }
    if (room == 0) { env->fatal(0x54e); return 0; }
    const uint32_t u = env->a6b8(rd32(game + 0x1c), 2);
    if (u == 0) return 0;
    wr32(u, 2);
    wr32(u + 4, classId);
    wr32(u + 8, rd32(game + 0x1c));
    wr32(u + 0x80, game);
    uint32_t f = rd32(u + 0xc8);
    f = rd32(game + 0x70) ? (f | 0x2000000) : (f & 0xfdffffffu);
    wr32(u + 0xc8, f | 0x4000000);
    const uint8_t act = env->a56e(env->a556(room));
    *(uint8_t*)(uintptr_t)(u + 0x18) = act;
    wr32(u + 0x1c, rd32(game + 0xbc + act * 4));
    wr32(u + 0x74, 0);
    env->subE0110(game + 0xd0, u);
    wr32(u + 0xc4, rd32(u + 0xc4) | 0x10);
    wr32(u + 0xd0, 0xb);
    uint32_t n = rd32(game + 0x98) + 1;
    if (n == 0xffffffffu) n = 1;
    wr32(game + 0x98, n);
    const uint32_t data = env->a2e6(rd32(game + 0x1c), 0x38, env->file, 0x265, 0);
    wr32(u + 0x14, data);
    std::memset((void*)(uintptr_t)data, 0, 0x38);
    wr32(data, env->a520(classId));
    wr32(u + 0x10, 0);
    env->a724(u);
    env->sub2F300(u, game, n, room, cx, cy);
    env->subE0F40(cy, u, room, cx, game, 1);
    return u;
}
