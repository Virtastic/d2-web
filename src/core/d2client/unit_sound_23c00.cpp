// 1.13c D2Client+0x23C00: stack = pUnit, plain cdecl-shaped frame.
// Unit anim-frame sound trigger: gates on unit type/mode/class, derives per-frame timing from the
// unit's anim fields (+0x44 frame, +0x48 length, +0x4C step, +0x84 start tick), plays the class sound
// (0x22FB0 -> 0xB38A70 -> 0xB37ED0), then 0x21FB0. Callees are behind the host callback, as in 22fb0.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

enum {
    OP_TYPE = 0x22430, OP_MODE = 0x223D0, OP_CLASS = 0x22490, OP_SND_RECORD = 0x22A70,
    OP_PROFILE = 0x22FB0, OP_FINAL = 0x21FB0,
    OP_ROUND = 0xB395D0, OP_PLAY = 0xB38A70, OP_VOLUME = 0xB37ED0, OP_RAND = 0xB39640,
};
// a..c are the register/stack words in call order; the host owns each callee's real convention.
typedef uint32_t (*D2Client23C00Cb)(void* ctx, uint32_t rva, uint32_t a, uint32_t b, uint32_t c);

static const auto rd32 = [](const uint8_t* p, size_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; };

// unit: host pointer to the unit bytes; nClassCount = [[0xFB7E474]+0xA80]; nNow = [0xFBC97F8];
// nPlayer = [0xFBCBBFC] (the local player unit id compared to pUnit).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_23c00(uint32_t unit, const uint8_t* u, int32_t nClassCount,
        uint32_t nNow, uint32_t nPlayer, D2Client23C00Cb cb, void* ctx) {
    uint32_t type = cb(ctx, OP_TYPE, unit, 0, 0);                         // 0x23C0D
    uint32_t mode = cb(ctx, OP_MODE, unit, 0, 0);                         // 0x23C18
    if (type == 1) {                                                      // 0x23C1D
        int32_t cls = (int32_t)cb(ctx, OP_CLASS, unit, 0, 0);
        if (cls < 0 || cls >= nClassCount) cls = -1;                      // 0x23C41
        bool gate = (mode == 2 || mode == 0xF);
        bool enter = false;
        if (cls == 0xF) { if (mode == 1) enter = true; }                  // 0x23C5A -> 0x23C76
        else if (cls == 0x6E) gate = (mode == 8);
        if (!enter && !gate) return (uint32_t)cls;                        // 0x23C70 -> exit, eax = cls
    }
    int32_t step = (int16_t)rd32(u, 0x4C);                                // movsx word
    uint32_t frame = rd32(u, 0x44), len = rd32(u, 0x48);
    if (step == 0) return frame;                                          // 0x23C90
    uint32_t scale = 2, rate = 100, lo = 0;
    uint32_t seed = 0, doRand = 0;
    if (type == 1) {
        uint32_t rec = cb(ctx, OP_SND_RECORD, unit, 0, 0);                // 0x23CB0
        if (rec) {
            const uint8_t* r = (const uint8_t*)(uintptr_t)rec;
            seed = rd32(r, 0x60) << 8; scale = rd32(r, 0x5C); doRand = rd32(r, 0x58); rate = rd32(r, 0x64);
            if (scale == 0) return rate;                                  // 0x23CD2
        }
    }
    uint32_t per = len / (scale * (uint32_t)step);                        // 0x23CE5 div
    uint32_t elapsed = nNow - rd32(u, 0x84);
    if (scale > 1 && elapsed < (uint32_t)(((uint64_t)(per * 2) * 0xAAAAAAABull) >> 33))   // (2*per)/3
        return per * 0x55555556u;                                         // eax residue, as retail
    (void)lo;
    int32_t a = (int32_t)cb(ctx, OP_ROUND, frame, seed, 0);               // 0x23D2A, ecx = seed
    int32_t b = (int32_t)cb(ctx, OP_ROUND, (uint32_t)(step + (int32_t)frame), seed, 0);
    if (a >= b) return (uint32_t)b;
    int32_t c = (int32_t)cb(ctx, OP_ROUND, frame - (uint32_t)step, seed, 0);
    if (a >= c) return (uint32_t)c;
    uint32_t flag = 0;
    if (type == 0) flag = (mode == 3); else if (type == 1) flag = (mode == 0xF);   // 0x23D63
    uint32_t base = cb(ctx, OP_PROFILE, unit, flag, 0);                   // 0x23D8F, ret 4
    uint32_t snd = cb(ctx, OP_PLAY, unit, 0, base);                       // 0x23D96, stack unit,0,0,0
    if (snd) {
        uint32_t vol = (unit == nPlayer) ? 0xFF : 200;                    // 0x23DA1
        if (elapsed > (per * 3) >> 1) vol = (uint32_t)((int32_t)(vol * 160) / 255);   // 0x23DC4
        cb(ctx, OP_VOLUME, snd, vol, 0);                                  // 0x23DE0
    }
    if (doRand && cb(ctx, OP_RAND, 100, 0, 0) < rate)                     // 0x23DF2
        cb(ctx, OP_PLAY, unit, 0, 0);
    return cb(ctx, OP_FINAL, unit, nNow, 0);                              // 0x23E10
}
