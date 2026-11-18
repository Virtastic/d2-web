// 1.13c D2Win+0x10A50 (491 bytes): draw a type-2 control's current cel(s) and advance its animation
// frame. __fastcall ECX = control; x = +0x0C, y = +0x10. Seams: step = D2Win+0x10930 (ESI = control),
// frames = D2CMP #10086 (thunk 0x774A), cel = thunk 0x7744 (the cel of a 0x48-byte draw context),
// offY / offX = thunks 0x775C / 0x7756 (the cel's offsets), draw = D2Gfx #10041 (thunk 0x77CE, stdcall 6),
// tick = GetTickCount, fatal = Fog #10265/#10024 + _exit(-1) (type != 2).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint32_t at(uint32_t a) { return rd32((void*)(uintptr_t)a, 0); }

struct D2Win10A50Env {
    void     (*step)(void* obj);
    int32_t  (*frames)(uint32_t file);
    uint32_t (*cel)(void* ctx);
    int32_t  (*offY)(uint32_t cel);
    int32_t  (*offX)(uint32_t cel);
    void     (*draw)(void* ctx, int32_t x, int32_t y, uint32_t color, uint32_t pal, uint32_t z);
    uint32_t (*tick)();
    void     (*fatal)();
};

// 1.13c D2Win+0x10A50
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10a50(void* o, const D2Win10A50Env* env) {
    if (rd32(o, 0) != 2) { env->fatal(); return 0; }
    uint8_t ctx[0x48] = {0};   // +0 frame, +0x34 file
    if (rd32(o, 0x04) == 0 && rd32(o, 0x40) == 0) return 1;
    int32_t x = (int32_t)rd32(o, 0x0C), y = (int32_t)rd32(o, 0x10);
    env->step(o);
    const uint32_t tbl = rd32(o, 0x40);
    uint32_t file = 0, file2 = 0, pal, pal2;
    int32_t frames, dur;
    if (tbl) {
        const uint32_t e = tbl + rd32(o, 0x50) * 16;
        const uint32_t p0 = at(e), p1 = at(e + 4);
        if (p0) file = at(p0);
        if (p1) file2 = at(p1);
        pal2 = at(e + 8);
        pal = rd32(o, 0x54);
        frames = env->frames(file);
        dur = (int32_t)at(tbl + rd32(o, 0x50) * 16 + 0xC);
    } else {
        file = rd32(o, 0x04);
        pal = pal2 = rd32(o, 0x54);
        frames = env->frames(file);
        dur = (int32_t)rd32(o, 0x44);
        wr32(ctx, 0x34, file);
        const uint32_t c = env->cel(ctx);
        y -= env->offY(c);
        x -= env->offX(c);
    }
    if (file) { wr32(ctx, 0, rd32(o, 0x4C)); wr32(ctx, 0x34, file); env->draw(ctx, x, y, 0xFFFFFFFFu, pal, 0); }
    if (file2) { wr32(ctx, 0, rd32(o, 0x4C)); wr32(ctx, 0x34, file2); env->draw(ctx, x, y, 0xFFFFFFFFu, pal2, 0); }
    if (dur != 0 && frames > 1 && rd32(o, 0x5C) != 0) {
        const uint32_t per = (uint32_t)((int32_t)((uint32_t)dur * 1000u) / 25);
        const uint32_t st = rd32(o, 0x50);
        const uint32_t el = (env->tick() - rd32(o, 0x48)) / per;
        const uint32_t last = (uint32_t)(frames - 1);
        if (st != 2 && st != 4) wr32(o, 0x4C, el % last);
        else wr32(o, 0x4C, (int32_t)el >= (int32_t)last ? last : el);
    }
    return 1;
}
