// 1.13c D2Client+0x2CA80 (212 B): append a formatted record value to the wide string in EDI.
//   Register inputs EBX (selector: BX == 0x1506 rejects) and EDI (destination); stack a0..a3 (ret 0x10).
//   Rejected when BX == 0x1506 or a1 == 0: returns 0. Else: local[32 wchar] zeroed; 0x2C7E0(EAX = a1, ECX = a2, &local) fills it;
//   then strcat(EDI, str(id)) for id = EBX, 0xF9D, 0xF9B, a0, 0xF9B; strcat(EDI, local); if a3 strcat(EDI, str(0xFA1));
//   strcat(EDI, str(0xF9E)); returns 1. str = D2Lang Ordinal 10003 (thunk 0xD1EC, ECX = id), strcat = the Unicode import [0x6FB7EABC].
#include <cstdint>
#include <cstring>

struct D2Client2ca80Env {
    uint32_t (*str)(uint32_t id);                                     // 0xD1EC: wide string address
    void (*fmt)(uint32_t eax, uint32_t ecx, uint16_t* local);         // 0x2C7E0, stack = local buffer (ret 4)
};

static void c2ca80_cat(uint32_t dst, const uint16_t* src) {
    uint16_t* d = (uint16_t*)(uintptr_t)dst;
    while (*d) ++d;
    do { *d++ = *src; } while (*src++);
}

extern "C" uint32_t d2client_2ca80_port(uint32_t ebx, uint32_t edi, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                                         const D2Client2ca80Env* env) {
    if ((uint16_t)ebx == 0x1506 || !a1) return 0;
    uint16_t local[32];
    std::memset(local, 0, sizeof local);
    env->fmt(a1, a2, local);
    const uint32_t ids[5] = { ebx, 0xF9D, 0xF9B, a0, 0xF9B };
    for (int i = 0; i < 5; ++i) c2ca80_cat(edi, (const uint16_t*)(uintptr_t)env->str(ids[i]));
    c2ca80_cat(edi, local);
    if (a3) c2ca80_cat(edi, (const uint16_t*)(uintptr_t)env->str(0xFA1));
    c2ca80_cat(edi, (const uint16_t*)(uintptr_t)env->str(0xF9E));
    return 1;
}
