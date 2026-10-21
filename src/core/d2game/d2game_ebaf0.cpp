// src/core/d2game/d2game_ebaf0.cpp -- 1.13c D2Game.dll 0xEBAF0, transcribed from the cached Ghidra
// decompile. Alpha lane: implementation + marker only. Register-only args (in_EAX = flags,
// unaff_ESI = client record) are explicit; unmapped callees and module globals are host seams.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2GameEBAF0Env {
    uint32_t        guard6FD31C40;                                   // DAT_6fd31c40
    void          (*getFileTime)(uint64_t* out);                     // (*(DAT_6fd31c3c+0x54))(), result in local_8
    int32_t       (*compareFileTime)(const void* a, const void* b);  // kernel32 CompareFileTime
    uint32_t      (*sub_B23A0)(void);                                // 0xB23A0, flags word
    uint32_t      (*ordinal10265)(uint32_t id);                      // 0xA2DA
    void          (*ordinal10024)(const char* module, uint32_t id);  // 0xA2C2
};

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

// Returns 0 on success, else the join-validation error code. param_1 +400 is a FILETIME.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EBAF0(const uint8_t* param_1, uint32_t in_EAX, const uint8_t* esi, const D2GameEBAF0Env* env) {  // 1.13c D2Game+0xEBAF0
    if ((in_EAX & 0x20) == 0) {
        if (rd32(esi, 0x70) != 0) return 9;
    } else if (rd32(esi, 0x70) == 0) {
        return 8;
    }
    if (env->guard6FD31C40 != 0) {
        uint64_t local_8 = 0;
        env->getFileTime(&local_8);
        if ((in_EAX & 0x40) != 0 && env->compareFileTime(param_1 + 400, &local_8) > 0) {
            if (rd32(esi, 0x74) == 0) return 0x19;
        } else if (rd32(esi, 0x74) != 0) {
            return 0x1a;
        }
    }
    if ((in_EAX & 4) == 0) {
        const uint32_t inner = rd32(esi, 0x1d28);
        if (inner == 0) {
            static const char kModule[] = "D2Game.dll";  // 0x6FD19FF9
            env->ordinal10024(kModule, env->ordinal10265(0x139));
            _exit(-1);
        }
        if ((rd32((const uint8_t*)(uintptr_t)inner, 8) & 0x800) != 0) return 0xc;
    } else {
        if ((in_EAX & 8) != 0) return 10;
        if ((env->sub_B23A0() & 0x800) == 0) return 0xb;
    }
    const uint32_t uVar2 = in_EAX >> 8 & 0x1f;
    const bool b20 = (in_EAX & 0x20) != 0;
    const uint8_t m = esi[0x6d];
    if (m < 2) {
        if (m != 0 && ((b20 && uVar2 < 5) || uVar2 < 4)) return 0xd;
    } else if ((b20 && uVar2 < 10) || uVar2 < 8) {
        return 0xe;
    }
    return 0;
}
