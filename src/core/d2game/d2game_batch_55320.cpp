// src/core/d2game/d2game_batch_55320.cpp -- 1.13c D2Game.dll 0x55320 (player-save header reader, v0x47..0x60)
// transcribed from the cached Ghidra decompile. Base 0x6FC20000. Callees are seams in retail order; the
// ports that exist (0x2813 stricmp, 0x49A20, 0x49A80, 0x49D70, 0x53720, 0xE1D90, 0x7B0A0, 0x494E0, 0xB23A0)
// are wired by the caller, anything not wired is left null and must be weak-guarded there (not here).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game55320Env {
    int      (*stricmp)(const char* a, const char* b);                         // 0x2813
    char*    (*sub49A20)();                                                    // 0x49A20: realm/account name
    uint32_t (*arenaFlags)();                                                  // 0xB23A0
    void     (*sub49D70)(uint32_t act);                                        // 0x49D70
    uint32_t (*sub49A80)();                                                    // 0x49A80
    void*    (*subE1D90)(uint32_t z0, uint32_t cls, uint32_t z1, uint32_t z2, void* self);  // 0xE1D90
    void     (*sub53720)(void* unit);                                          // 0x53720
    void     (*sub7B0A0)(uint32_t v, void* unit);                              // 0x7B0A0
    void     (*setSkillHotKey)(uint32_t idx, uint32_t skill, uint32_t flag, uint32_t unk);  // 0x494E0
    uint32_t dataTablesBA0;   // *(sgptDataTables + 0xBA0)
    uint32_t flag6FD31C3C;    // DAT_6fd31c3c
};

static uint32_t hk32(uint32_t v) { return (v < 0x9c || v > 0xd8) ? (v == 0xff ? 0xffffffffu : v) : v + 0x41; }

// src = param_2 (0x82-byte header), self = param_3, cl = param_4
// 1.13c D2Game+0x55320
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_55320(const uint8_t* src, uint8_t* self, uint8_t* cl,
    void** pUnit, uint32_t* o6, uint32_t* o7, uint32_t* o8, uint32_t* o9, uint32_t* o10, uint32_t* o11, uint32_t* o12,
    const D2Game55320Env* env)
{
    uint8_t h[0x82]; std::memcpy(h, src, 0x82);
    auto u16 = [&](int o) { uint16_t v; std::memcpy(&v, h + o, 2); return v; };
    auto u32 = [&](int o) { uint32_t v; std::memcpy(&v, h + o, 4); return v; };
    *o12 = 0;
    std::memset(cl + 10, 0, 2);
    if (u32(0) != 0xaa55aa55u) { if (!env->flag6FD31C3C) return 1; *o6 = 1; return 0; }
    if (u16(0x20) != 0x82) { if (env->flag6FD31C3C) { *o6 = 1; return 0; } return 0xe; }
    uint32_t ver = u32(4); *o7 = ver;
    if (ver < 0x47 || ver > 0x60) { if (!env->flag6FD31C3C) return 1; *o6 = 1; return 9; }
    h[0x17] = 0;
    char* nm = env->sub49A20(); nm[0xf] = 0;
    if (env->stricmp(nm, (const char*)h + 8) != 0) { *o6 = 0; return 1; }
    uint32_t fl = u32(0x18), bit5 = fl & 0x20;
    if (!bit5) { if (*(int32_t*)(self + 0x70)) return 0x17; }
    else if (!*(int32_t*)(self + 0x70)) return 0x18;
    if (!(fl & 4)) { if (env->arenaFlags() & 0x800) return 0x13; }
    else { if (fl & 8) return 0x15; if (!(env->arenaFlags() & 0x800)) return 0x14; }
    uint32_t lv = fl >> 8 & 0x1f;
    if (lv < 8 && self[0x6d] > 1) return 0x12;
    if ((bit5 && lv < 5 && self[0x6d]) || (lv < 4 && self[0x6d])) return 0x11;
    char name[28]; std::memcpy(name, h + 0x5f, 27); name[27] = 0;
    std::memcpy(cl + 0x45e, h + 0x59, 2);
    uint32_t v45 = u32(0x5b) & 0xffffff; std::memcpy(cl + 0x460, &v45, 4);
    std::strcpy((char*)cl + 0x464, name);
    cl[0x481] = h[0x7c]; cl[0x480] = h[0x7b]; cl[0x482] = h[0x7d];
    if (fl & 1) { *o6 = 1; uint16_t w = (uint16_t)fl & 0xfffe; std::memcpy(cl + 10, &w, 2); return 0; }
    cl[0x45c] = fl >> 0xd & 1;
    *o6 = 0; std::memcpy(cl + 10, &fl, 2);
    uint8_t b48 = h[0x58], hi = b48 >> 4;
    cl[8] = (uint8_t)u16(0x22);
    if ((b48 & 0xf) > 4 || hi > 2) return 9;
    if (self[0x6a] == 3 && *(int32_t*)(self + 0x84) == 0 && self[0x6d] == hi) std::memcpy(self + 0x7c, h + 0x7e, 4);
    if (self[0x6d] == hi) env->sub49D70(b48 & 0xf); else cl[0x1ac] = 0;
    void* unit = env->subE1D90(0, u16(0x22), 0, 0, self);
    *pUnit = unit;
    env->sub53720(unit);
    env->sub7B0A0(env->sub49A80(), *pUnit);
    *o8 = 0x10; *o9 = env->dataTablesBA0;
    if (u16(0x1e) > 0x10 || (int32_t)u16(0x1c) > (int32_t)env->dataTablesBA0) {
        if (env->flag6FD31C3C) { *o6 = 1; return 0; }
        return 1;
    }
    if (u16(0x1e) != 0x10) *o8 = u16(0x1e);
    if (u16(0x1c) != env->dataTablesBA0) *o9 = u16(0x1c);
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t v = h[0x46 + i * 2];
        uint32_t s = (v < 0x9c || v > 0xd8) ? (v == 0xff ? 0xffffu : v) : v + 0x41;
        env->setSkillHotKey(i, s, h[0x47 + i * 2], 0xffffffffu);
    }
    *o10 = hk32(h[0x56]);
    *o11 = hk32(h[0x57]);
    *o12 = u16(0x20);
    return 0;
}
