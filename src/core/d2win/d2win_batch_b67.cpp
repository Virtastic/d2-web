// src/core/d2win/d2win_batch_b67.cpp -- 1.13c D2Win rows 0x7C60 0xBF60 0xC180 0x164D0 from cached decompiles.
// Skipped: CRT (0x1230 0x1962 0x3F3B 0x4202 0x68FB 0x69D3 0x69FE), direct-Win32 clipboard (0x13800 0x138C0),
// 0xD880 (1.9 KB, multi-stage). Hidden register inputs are explicit leading params. Callees are seams.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const char*, uint32_t);
__attribute__((weak)) void Ordinal_10029(const char*, const char*);
__attribute__((weak)) uint32_t Ordinal_10102(void);
__attribute__((weak)) void Ordinal_10103(void);
__attribute__((weak)) void* Ordinal_10042(int32_t size, int32_t line);
void _exit(int);
// retail globals
int32_t d2win_g_6f9a98a4;          // count
int32_t d2win_g_6f9a8cb0[1];       // 12-byte stride, [i*3]
int32_t d2win_g_6f9a98a8[1];
int32_t d2win_g_6f8ffee0[1];       // 12-byte stride: [i*3], [i*3+1]
extern uint32_t d2win_g_6f9014a0;
}

struct D2WinB67Env {
    uint32_t (*sub_B4F0)(void);               // 0xB4F0, hidden EAX/EDI inputs left to the seam
    uint32_t (*sub_7A60)(void);               // 0x7A60
    void (*sub_7970)(void);                   // 0x7970
    uint32_t (*getTickCount)(void);
};

// 1.13c D2Win+0x0BF60 -- mode = EAX, p = ESI (byte*): remap *p onto a valid slot.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_BF60(int32_t mode, uint8_t* p, const D2WinB67Env* env) {
    const int32_t n = d2win_g_6f9a98a4;
    auto search = [&](bool fallback) -> bool {   // true when slot found and stored
        for (int32_t i = 0; i < n; i++)
            if (d2win_g_6f9a8cb0[i * 3] == d2win_g_6f8ffee0[*p * 3]) { *p = (uint8_t)i; return true; }
        if (fallback) *p = (uint8_t)(*p % 3 + 1);
        return false;
    };
    if (mode >= 1 && mode <= 4 && *p > 3)
        for (int k = 0; k < 4; k++)
            if (env->sub_B4F0() != 0) search(true);
    if (mode == 0 && env->sub_B4F0() == 0) {
        if (env->sub_B4F0() != 0 && search(false)) return;
        *p = 1;
    }
}

// 1.13c D2Win+0x0C180 -- param_1 = mode, rec = EBX (byte record; [5],[6],[7] are slot ids, 0xFF = none).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_C180(int32_t param_1, const uint8_t* rec, const D2WinB67Env* env) {
    uint8_t b5 = rec[5];
    bool both = !(b5 == 0xff || rec[6] == 0xff);
    int32_t a = 0, b = 0;
    if (b5 != 0xff) {
        if (both || ((a = d2win_g_6f9a8cb0[b5 * 3 + 1], rec[6] == 0xff && rec[7] == 0xff) &&
                     d2win_g_6f9a98a8[b5] != d2win_g_6f9a8cb0[b5 * 3 + 1]))
            a = d2win_g_6f9a98a8[b5];
        if (((a == 0xd || a == 0xe) && param_1 != 6) || env->sub_B4F0() != 0)
            a = d2win_g_6f8ffee0[b5 * 3 + 1];
    }
    uint8_t b6 = rec[6];
    if (b6 != 0xff) {
        b = both ? d2win_g_6f9a98a8[b6] : d2win_g_6f9a8cb0[b6 * 3 + 1];
        if (((b == 0xd || b == 0xe) && param_1 != 6) || env->sub_B4F0() != 0)
            b = d2win_g_6f8ffee0[rec[6] * 3 + 1];
    }
    if ((a == 0xd || a == 0xe) && param_1 != 6) a = 0;
    if ((b == 0xd || b == 0xe) && param_1 != 6) b = 0;
    if (a == 0) return b == 0 ? 1 : b;
    if (a == b && (a == 6 || a == 7)) return a;
    if (a == 8) return 8;
    if (b == 0) return a;
    if (a == 4) {
        if (b == 4) return 0xb;
        if (b == 2) return 9;
    } else if (a != 2) {
        if (a == 5 && b == 2) return 0xb;
    } else if (b == 4) {
        return 0xc;
    }
    if (a == 2 && b != 4 && b != 5) return b == 2 ? 10 : 0;
    if (b == 4 || b == 5) { if (a == 2) return 0xb; }
    if (a == 5) return (b != 4 && b != 5) ? 0 : 0xb;
    if (a == 4) return b != 5 ? 0 : 0xb;
    if (a == 0xd) return b == 0xd ? 0xd : 0;
    if (a == 0xe) return b == 0xe ? 0xd : 0;
    return (a == 1 && b == 1) ? 1 : 0;
}

// 1.13c D2Win+0x07C60 -- param_2 receives the 0x7A60 result; returns the Ordinal_10042(0xcf) block.
// unaff_retaddr in retail is a stack slot compared to 0 after 0x7970; passed here as `check`.
extern "C" EMSCRIPTEN_KEEPALIVE
void* d2win_sub_7C60(uint32_t* param_2, uint32_t check, const D2WinB67Env* env) {
    if (Ordinal_10102() == 0) { Ordinal_10029("Error opening file: %s", ""); return nullptr; }
    uint32_t u = env->sub_7A60();
    void* r = Ordinal_10042(0xcf, 0);
    if (!r) return nullptr;
    env->sub_7970();
    if (check == 0) { Ordinal_10024("", Ordinal_10265(0x49)); _exit(-1); }
     Ordinal_10103();
    if (param_2) *param_2 = u;
    return r;
}

// 1.13c D2Win+0x164D0 -- ESI = esi, caller return address = retaddr; links a 0x48-byte node at list tail.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t* d2win_sub_164D0(uint32_t esi, uint32_t retaddr, uint32_t fn63e0, uint32_t lab6310,
                          const uint32_t* bitMasks, uint32_t** head, const D2WinB67Env* env) {
    uint32_t* n = (uint32_t*)Ordinal_10042(0x1c, 0);
    for (int i = 0; i < 0x12; i++) n[i] = 0;
    n[0] = 8;
    n[2] = bitMasks[0] | bitMasks[2];
    n[0x10] = esi;
    n[0x11] = env->getTickCount();
    n[0xd] = retaddr;
    n[8] = fn63e0;
    n[0xf] = 0;
    n[0xc] = lab6310;
    if (*head) {
        uint32_t* t = *head;
        while (t[0xf]) t = (uint32_t*)(uintptr_t)t[0xf];   // +0x3c = next
        t[0xf] = (uint32_t)(uintptr_t)n;
    } else *head = n;
    return n;
}
