// 1.13c D2Client+0xA7D00 (75 B): EDI = argument of 0xC5B0 (stdcall edi, &buf; fills buf[0] = x, buf[1] = y). For every node of the list 0xC652 () with flag bit 0 of
// node[0x1C]: 0xC5B0 (edi, buf), then 0xA7BF0 (ECX = node[4], EDX = node[8] + x, stack node[0x18] + y, node[0x14], node[0]; ret 0xC). Next node = node[0xC].
// Retail's buffer overlaps the saved ESI slot, so ESI is clobbered on return; nothing reads it.
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7d00(uint32_t edi, uint32_t (*fc652)(), void (*fc5b0)(uint32_t edi, int32_t* buf),
                                                     void (*fa7bf0)(uint32_t ecx, int32_t edx, int32_t y, uint32_t s1, uint32_t s2)) {
    for (uintptr_t n = fc652(); n; n = *(const uint32_t*)(n + 0xc)) {
        if (!(*(const uint32_t*)(n + 0x1c) & 1)) continue;
        int32_t buf[2];
        fc5b0(edi, buf);
        fa7bf0(*(const uint32_t*)(n + 4), (int32_t)*(const uint32_t*)(n + 8) + buf[0], (int32_t)*(const uint32_t*)(n + 0x18) + buf[1], *(const uint32_t*)(n + 0x14), *(const uint32_t*)n);
    }
}
