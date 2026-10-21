// src/core/d2game/d2game_batch_ed250.cpp -- 1.13c D2Game.dll 0xED250 (player-save header reader)
// transcribed from the cached Ghidra decompile. Implementation + marker only. Base 0x6FC20000.
//
// Register/stack-only arguments are explicit: end = in_EAX, self = `this` (ECX), cl = unaff_EBX,
// cur = param_1 (cursor in/out), unitOut = param_2/unaff_retaddr slot. Callees are seams in retail order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameED250Env {
    uint32_t (*ordinal10229)(const uint8_t* buf, uint32_t len);            // checksum
    const char* (*sub49A20)();
    int  (*stricmp)(const char* a, const char* b);                         // 0x2813
    int  (*subEBAF0)(void* cl);                                            // 0xEBAF0
    void (*sub49AE0)(uint32_t v);                                          // 0x49AE0
    void (*sub49D70)(uint32_t act);                                        // 0x49D70
    uint32_t (*sub49A80)();                                                // 0x49A80
    void* (*subE1D90)(uint32_t z0, uint32_t cls, uint32_t z1, uint32_t z2, void* self);  // 0xE1D90
    void (*subEBA40)(void* unit);                                          // 0xEBA40
    void (*sub7B0A0)(uint32_t v, void* unit);                              // 0x7B0A0
    void (*subEB5D0)(const uint16_t* src, uint32_t* out2);                 // 0xEB5D0 -> {skill, flag}
    void (*setSkillHotKey)(uint32_t idx, uint32_t skill, uint32_t flag);   // 0x494E0
    uint8_t* (*ordinal10920)(void* unit);
};

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_ED250(uint32_t end, uint8_t* self, uint8_t* cl, uint32_t* cur, uint32_t* p2, void** unitOut, uint32_t expectCrc, uint32_t* retSlot, const D2GameED250Env* env) {  // 1.13c D2Game+0xED250
    uint8_t* b = (uint8_t*)(uintptr_t)*cur;
    *p2 = 0;
    *(uint16_t*)(cl + 10) = 0;
    if (end < (uint32_t)(uintptr_t)b + 0x14f) return 4;
    uint32_t* slot = (uint32_t*)(b + 0xc);
    uint32_t saved = *slot; *slot = 0;
    if (env->ordinal10229(b, end - (uint32_t)(uintptr_t)b) != expectCrc) return 6;
    if (end - (uint32_t)(uintptr_t)b != *(uint32_t*)(b + 8)) return 5;
    uint32_t ver = *(uint32_t*)(b + 4);
    if (ver > 0x5b && ver < 0x61) {
        b[0x23] = 0;
        const char* s = env->sub49A20();
        if (!s) return 3;
        if (env->stricmp(s, (const char*)(b + 0x14)) == 0) {
            int r = env->subEBAF0(cl);
            if (r) return r;
            uint8_t cls = b[0x28];
            if (cls < 8) {
                cl[8] = cls;
                uint32_t fl = *(uint32_t*)(b + 0x24);
                if (fl & 1) {
                    *(uint32_t*)(b + 0x24) = fl & ~1u;
                    *(uint16_t*)(cl + 10) = (uint16_t)fl & 0xfffe;
                    *retSlot = 0;   // retail stores a stack temp at b+0x14f (unaligned)
                    return 2;
                }
                cl[0x45c] = b[0x10] & 1;
                *(uint16_t*)(cl + 10) = *(uint16_t*)(b + 0x24);
                env->sub49AE0(*(uint32_t*)(b + 0x2c));
                uint8_t d = b[self[0x6d] + 0xa8];
                uint8_t a = d & 0x7f; if (a > 4) a = 0;
                env->sub49D70(a);
                if (self[0x6a] == 3 && *(int32_t*)(self + 0x84) == 0 && (d & 0x80))
                    *(uint32_t*)(self + 0x7c) = *(uint32_t*)(b + 0xab);
                void* unit = env->subE1D90(0, cls, 0, 0, self);
                *unitOut = unit;
                env->subEBA40(unit);
                env->sub7B0A0(env->sub49A80(), *unitOut);
                const uint16_t* p = (const uint16_t*)(b + 0x38);
                for (uint32_t i = 0; i < 0x10; i++, p += 2) {
                    uint32_t o[2]; env->subEB5D0(p, o);
                    env->setSkillHotKey(i, o[0], o[1]);
                }
                uint8_t* inv = env->ordinal10920(*unitOut);
                if (inv) {
                    static const uint32_t off[4] = {0x78, 0x7c, 0x80, 0x84};
                    static const uint32_t d0[4] = {0x74, 0x70, 0x84, 0x80}, d1[4] = {0x7c, 0x78, 0x8c, 0x88};
                    for (int k = 0; k < 4; k++) {
                        uint32_t o[2]; env->subEB5D0((const uint16_t*)(b + off[k]), o);
                        *(uint32_t*)(inv + d0[k]) = o[0]; *(uint32_t*)(inv + d1[k]) = o[1];
                    }
                    cl[0x480] = b[0xcf]; cl[0x481] = 0; cl[0x482] = 0;
                    *(uint32_t*)(uintptr_t)saved = 0x10;  // *piVar4 = loop counter (16)
                    return 0;
                }
            }
            return 4;
        }
    }
    return 7;
}
