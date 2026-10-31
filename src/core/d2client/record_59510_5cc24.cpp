// Two small record helpers of the D2Client panels. Callees through D2cHost (client_host.h).
#include "client_host.h"
#include <emscripten.h>

// 1.13c D2Client+0x59510: ESI = src record. Copies src[1..15] (15 bytes) into a 16-byte stack string, forces its last
// byte to 0, converts it to Unicode into the global string at 0x6FBCFED8 through the import slot 0x6FB7EAC0 (win2Unicode,
// rva 0xCEAC0; ECX = dst, EDX = text, stack 0x10), then DAT_6FBC77E4 = the dword at src + 0x11.
// buf16: caller-owned 16 bytes; gStr/dstTok are the destination (guest address 0x6FBCFED8), outG the word DAT_6FBC77E4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_59510(D2cHost h, const uint8_t* src, uint8_t* buf16, uint32_t bufTok, uint32_t dstTok, uint32_t* outG) {
    for (int i = 0; i < 15; ++i) buf16[i] = src[1 + i];
    buf16[15] = 0;
    D2cCall c = { 0xCEAC0, 0, dstTok, bufTok, 0, 0, { 0x10, 0, 0, 0 } };
    h(&c);
    *outG = (uint32_t)src[0x11] | (uint32_t)src[0x12] << 8 | (uint32_t)src[0x13] << 16 | (uint32_t)src[0x14] << 24;
}

// 1.13c D2Client+0x5CC24: EBP = key (a register argument; ret 4 pops an unused stack word). Walks the 16 entries of the
// 0x20-byte table at 0x6FBC75C0 (entry base = 0x6FBC75C0 + 0x20*i, the pointer slot at base + 8). An entry is a candidate when
// 0xD2BE(ECX = entry[0]) != 0, ptr = entry[2] != 0 and ptr[1] == key; among candidates the lowest ptr[0x34/4] (unsigned) wins,
// the earlier entry on a tie. Returns the winning slot 0..15 or -1 (retail returns the entry address or 0).
// ptrView[i] = the words behind entry i's pointer slot (null when the slot is 0).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_5cc24(D2cHost h, uint32_t key, const uint32_t* entries /* 16 x 8 words */, const uint32_t* const* ptrView) {
    int32_t best = -1;
    for (int i = 0; i < 16; ++i) {
        const uint32_t* e = entries + i * 8;
        D2cCall c = { 0xD2BE, 0, e[0], 0, 0, 0, { 0, 0, 0, 0 } };
        if (!h(&c)) continue;
        const uint32_t* p = ptrView[i];
        if (e[2] == 0 || !p) continue;
        if (p[1] != key) continue;
        if (best >= 0 && !(p[0x34 / 4] < ptrView[best][0x34 / 4])) continue;
        best = i;
    }
    return best;
}
