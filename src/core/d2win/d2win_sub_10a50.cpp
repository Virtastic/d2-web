// 1.13c D2Win+0x10A50 (491 bytes): draw a type-2 control's current cel(s) and advance its animation
// frame. __fastcall ECX = control. Seams: step = D2Win+0x10930 (ESI = obj), cmp10086 = D2CMP #10086
// (frame count), initCel = #10021, a = #10069, b = #10002, draw = #10041, tick = GetTickCount,
// fatal = Fog #10265/#10024 + _exit(-1) (type != 2).
// The decompile's iStack_58 is never written (uninitialised stack): modelled as 0, so the second draw
// never fires.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint32_t at(uint32_t a) { return rd32((void*)(uintptr_t)a, 0); }

struct D2Win10A50Env {
    void     (*step)(void* obj);
    int32_t  (*cmp10086)(uint32_t file);
    uint32_t (*initCel)(void* ctx);
    void     (*a10069)(uint32_t h);
    int32_t  (*b10002)(uint32_t h);
    void     (*draw)(void* ctx, int32_t x, uint32_t y, uint32_t color, uint32_t pal, uint32_t z);
    uint32_t (*tick)();
    void     (*fatal)();
};

// 1.13c D2Win+0x10A50
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10a50(void* o, const D2Win10A50Env* env, uint32_t esi) {
    if (rd32(o, 0) != 2) { env->fatal(); return 0; }
    uint8_t ctx[0x50] = {0}; // iStack_4c at +0; iStack_18 (file) at +0x34
    if (rd32(o, 0x04) != 0 || rd32(o, 0x40) != 0) {
        int32_t x = (int32_t)rd32(o, 0x10);
        env->step(o);
        const uint32_t tbl = rd32(o, 0x40), st = rd32(o, 0x50);
        uint32_t file = 0, pal, file2 = 0, f2 = 0;
        int32_t frames, dur;
        if (tbl == 0) {
            file = rd32(o, 0x04); pal = rd32(o, 0x54);
            frames = env->cmp10086(file);
            dur = (int32_t)rd32(o, 0x44);
            wr32(ctx, 0, 0);
            wr32(ctx, 0x34, file);
            uint32_t h = env->initCel(ctx);
            env->a10069(h);
            x -= env->b10002(h);
        } else {
            uint32_t p0 = at(tbl + st * 16), p1 = at(tbl + st * 16 + 4);
            if (p0) file = at(p0);
            if (p1) f2 = at(p1);
            pal = rd32(o, 0x54);
            frames = env->cmp10086(file);
            dur = (int32_t)at(st * 16 + 0xC + tbl);
        }
        if (file) { wr32(ctx, 0, rd32(o, 0x4C)); wr32(ctx, 0x34, file); env->draw(ctx, x, esi, 0xFFFFFFFFu, pal, 0); }
        if (file2) { wr32(ctx, 0, rd32(o, 0x4C)); wr32(ctx, 0x34, file2); env->draw(ctx, x, esi, 0xFFFFFFFFu, f2, 0); }
        if (dur != 0 && frames > 1 && rd32(o, 0x5C) != 0) {
            uint32_t per = (uint32_t)dur * 1000u / 25u;
            uint32_t el = (env->tick() - rd32(o, 0x48)) / per;
            if (st != 2 && st != 4) wr32(o, 0x4C, el % (uint32_t)(frames - 1));
            else { if ((int32_t)el >= frames - 1) el = (uint32_t)(frames - 1); wr32(o, 0x4C, el); }
        }
    }
    return 1;
}
