// src/core/d2game/d2game_cf810.cpp -- 1.13c D2Game.dll 0xCF810 (town-portal style warp: same act -> spawn the class-0x3B
// object beside the unit's free spot and hand it to 0xA22E0; other act or no level -> 0x9E170). Transcribed from the disassembly.
// stdcall (game, unit, level, arg3), ret 0x10, no result. Callees are seams in retail's call order.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2GameCF810Env {
    uint32_t (*a56e)(uint32_t v);                                    // 0xA56E: level -> act (low byte)
    uint32_t (*room)(uint32_t unit);                                 // 0xA478
    uint32_t (*a556)(uint32_t room);                                 // 0xA556: room -> level
    uint32_t (*aade)(uint32_t actWord);                              // 0xAADE: act -> town level (retail pushes a dword whose low byte is the act)
    void     (*sub9E170)(uint32_t eax, uint32_t edi, uint32_t game, uint32_t arg3);   // 0x9E170: EAX = unit, EDI = level, ret 8
    uint32_t (*a454)(uint32_t path);                                 // 0xA454: path x
    uint32_t (*a3b8)(uint32_t path);                                 // 0xA3B8: path y
    uint32_t (*a808)(uint32_t room);                                 // 0xA808
    void     (*subDFFD0)(uint32_t esi, uint32_t mode, uint32_t unit);// 0xDFFD0: ESI = unit, stack (0x18, unit), ret 8
    uint32_t (*ae98)(uint32_t room, uint32_t* in, uint32_t* out, uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // 0xAE98 free spot
    uint32_t (*sub1280)(uint32_t eax, uint32_t ebx, uint32_t edi);  // 0x1280: EAX = room, EBX = x, EDI = y: the room holding (x, y)
    uint32_t (*allocUnit)(uint32_t pool, uint32_t type);             // 0xA6B8
    void     (*initSeed)(uint32_t esi, uint32_t edi);                // 0xE0110 SUNIT_InitSeed
    uint32_t (*alloc)(uint32_t ecx, uint32_t edx, uint32_t file, uint32_t line, uint32_t z);   // 0xA2E6
    uint32_t (*a520)(uint32_t cls);                                  // 0xA520
    void     (*a724)(uint32_t unit);                                 // 0xA724
    void     (*sub2F300)(uint32_t esi, uint32_t id, uint32_t room, uint32_t x, uint32_t y);    // 0x2F300, ret 0x10
    void     (*unitAdd)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t x, uint32_t game, uint32_t one);  // 0xE0F40, ret 0xC
    void     (*a4a2)(uint32_t unit, uint32_t v);                     // 0xA4A2
    uint32_t (*subA22E0)(uint32_t game, uint32_t u, uint32_t level, uint32_t roomLevel);   // 0xA22E0, ret 0x10
    uint32_t (*aee0)(uint32_t unit);                                 // 0xAEE0
    void     (*aec8)(uint32_t unit, uint32_t v);                     // 0xAEC8 (low byte meaningful)
    void     (*a51a)(uint32_t room, uint32_t z);                     // 0xA51A
};

static inline uint32_t cf_rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void cf_wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

// 1.13c D2Game+0xCF810
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_CF810(uint32_t game, uint32_t unit, uint32_t level, uint32_t arg3, const D2GameCF810Env* e)
{
    uint8_t act = (uint8_t)e->a56e(level);
    uint32_t room = e->room(unit);
    const uint8_t ract = (uint8_t)e->a56e(e->a556(room));
    if (!level) { e->sub9E170(unit, e->aade(ract), game, arg3); return; }
    if (act != ract) { e->sub9E170(unit, level, game, arg3); return; }
    const uint32_t path = cf_rd32(unit + 0x2c);
    int32_t t = (int32_t)cf_rd32(unit);
    uint32_t px = (t == 2 || (t > 3 && t <= 5)) ? cf_rd32(path + 0xc) : (path ? e->a454(path) : 0);
    const uint32_t x0 = px - 5;
    t = (int32_t)cf_rd32(unit);
    const uint32_t y0 = (t == 2 || (t > 3 && t <= 5)) ? cf_rd32(cf_rd32(unit + 0x2c) + 0x10)
                                                      : (cf_rd32(unit + 0x2c) ? e->a3b8(cf_rd32(unit + 0x2c)) : 0);
    if (!room) _exit(-1);                                        // assert 0xE42
    if (e->a808(room)) { e->subDFFD0(unit, 0x18, unit); return; }
    act = (uint8_t)e->a56e(level);
    const uint32_t rlev = e->a556(room);
    if (act != (uint8_t)e->a56e(rlev)) _exit(-1);               // assert 0xE5C
    uint32_t pt[2] = { x0, y0 };
    if (!e->ae98(room, pt, pt, 3, 0x3e01, 0xc01, 0)) return;
    const uint32_t x = pt[0], y = pt[1];
    const uint32_t room2 = e->sub1280(room, x, y);
    if (!room2) return;
    const uint32_t u = e->allocUnit(cf_rd32(game + 0x1c), 2);
    if (u) {
        cf_wr32(u + 0x0, 2);
        cf_wr32(u + 0x4, 0x3b);
        cf_wr32(u + 0x8, cf_rd32(game + 0x1c));
        cf_wr32(u + 0x80, game);
        uint32_t f = cf_rd32(u + 0xc8);
        f = cf_rd32(game + 0x70) ? (f | 0x2000000) : (f & ~0x2000000u);
        cf_wr32(u + 0xc8, f);
        cf_wr32(u + 0xc8, f | 0x4000000);
        const uint8_t uact = (uint8_t)e->a56e(e->a556(room2));
        *(uint8_t*)(uintptr_t)(u + 0x18) = uact;
        cf_wr32(u + 0x1c, cf_rd32(game + 0xbc + uact * 4));
        cf_wr32(u + 0x74, 0);
        e->initSeed(game + 0xd0, u);
        cf_wr32(u + 0xc4, cf_rd32(u + 0xc4) | 0x10);
        cf_wr32(u + 0xd0, 0xb);
        uint32_t id = cf_rd32(game + 0x98) + 1;
        if (id == 0xffffffffu) id = 1;
        cf_wr32(game + 0x98, id);
        const uint32_t p = e->alloc(cf_rd32(game + 0x1c), 0x38, 0x6fd1b02cu, 0x265, 0);
        cf_wr32(u + 0x14, p);
        std::memset((void*)(uintptr_t)p, 0, 0x38);
        const uint32_t v = e->a520(0x3b);
        cf_wr32(cf_rd32(u + 0x14), v);
        cf_wr32(u + 0x10, 1);
        e->a724(u);
        e->sub2F300(u, id, room2, x, y);
        e->unitAdd(room2, y, u, x, game, 1);
    }
    e->a4a2(u, 1);
    const uint32_t placed = e->subA22E0(game, u, level, rlev);
    if (!placed) return;
    e->aec8(placed, (uint8_t)e->aee0(placed) | 3);
    e->a51a(room2, 0);
    e->a51a(e->room(placed), 0);
}
