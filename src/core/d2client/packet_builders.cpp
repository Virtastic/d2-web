#include <cstdint>
#include <emscripten.h>

// Packet builders that fill a small stack buffer and hand it to the send path (D2Client+0x143E0, EBX=size).
// Register convention: AL = opcode, ECX/EDX = payload. Here the packet bytes are returned instead of sent.
static void put(uint8_t* o, int at, uint32_t v, int n) { for (int i = 0; i < n; ++i) o[at + i] = (uint8_t)(v >> (8 * i)); }
static uint32_t pk(uint32_t op, uint32_t c, uint32_t d, int size, int cw, int dw, int dat, uint8_t* o) {
    o[0] = (uint8_t)op; put(o, 1, c, cw); if (dw) put(o, dat, d, dw); return (uint32_t)size;
}
// 1.13c D2Client+0x14910: [op, ecx32, edx32], 9 bytes
extern "C" uint32_t d2_client_14910(uint32_t op, uint32_t c, uint32_t d, uint8_t* o) { return pk(op, c, d, 9, 4, 4, 5, o); }
// 1.13c D2Client+0x149A0: [op, cx16, dx16, stack16], 7 bytes
extern "C" uint32_t d2_client_149a0(uint32_t op, uint32_t c, uint32_t d, uint32_t s, uint8_t* o) { pk(op, c, d, 7, 2, 2, 3, o); put(o, 5, s, 2); return 7; }
// 1.13c D2Client+0x149E0: [op, cx16, dx16], 5 bytes
extern "C" uint32_t d2_client_149e0(uint32_t op, uint32_t c, uint32_t d, uint8_t* o) { return pk(op, c, d, 5, 2, 2, 3, o); }
// 1.13c D2Client+0x14A10: [op, cx16], 3 bytes
extern "C" uint32_t d2_client_14a10(uint32_t op, uint32_t c, uint32_t d, uint8_t* o) { return pk(op, c, d, 3, 2, 0, 0, o); }
// 1.13c D2Client+0x14A30: [op], 1 byte
extern "C" uint32_t d2_client_14a30(uint32_t op, uint32_t c, uint32_t d, uint8_t* o) { return pk(op, c, d, 1, 0, 0, 0, o); }

static uint32_t fold(uint32_t n, const uint8_t* b) {
    uint32_t h = 0x811C9DC5u; h = (h ^ n) * 0x01000193u;
    for (uint32_t i = 0; i < n; ++i) h = (h ^ b[i]) * 0x01000193u;
    return h;
}
#define PROBE(R) extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_##R(uint32_t* r) { uint8_t b[16] = {}; r[4] = fold(d2_client_##R(r[0], r[1], r[2], b), b); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_149a0(uint32_t* r) { uint8_t b[16] = {}; r[4] = fold(d2_client_149a0(r[0], r[1], r[2], r[3], b), b); }
PROBE(149e0) PROBE(14a10) PROBE(14a30)
// 0x14910 has two descriptor shapes: no blob (the result is the fold of the packet), or a `lg` blob in r[3] holding the send hook's
// log (0x143E0: EBX = size, stack word 0 = the packet's stack address) at word 0 and its hookwlog (count, length, bytes) at word 16.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_14910(uint32_t* r) {
    uint8_t b[16] = {}; const uint32_t n = d2_client_14910(r[0], r[1], r[2], b);
    if (!r[13]) { r[4] = fold(n, b); return; }
    uint32_t* L = (uint32_t*)(uintptr_t)r[3];
    L[0] = 1; L[1] = 0x143e0; L[5] = n; L[7] = 0xB3000000u;
    L[16] = 1; L[17] = n; for (uint32_t i = 0; i < n; ++i) ((uint8_t*)(L + 18))[i] = b[i];
    r[4] = 0;
}
extern "C" uint32_t d2_client_148a0_build(uint8_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
// 1.13c D2Client+0x148A0: [op, ecx32, edx32, c32, d32], 17 bytes (r[3]=c, r[5]=d)
// Two vectors share each of the two probes below: the FNV fold (no blob) and the wire capture, whose one blob is the log of the
// 0x143E0 send (`hook 0x143e0 1 bE`: hooklog at +0 with EBX = byte count and stack word 0 = the stack buffer, hookwlog at +0x40)
// and whose result is masked (retmask 0). r[13] is the blob count.
static uint32_t wire(uint32_t lg, uint32_t n, const uint8_t* b) {
    uint32_t* L = (uint32_t*)(uintptr_t)lg; uint8_t* w = (uint8_t*)L + 0x40;
    for (int i = 0; i < 11; ++i) L[i] = 0;
    L[0] = 1; L[1] = 0x143E0; L[5] = n; L[7] = 0xB3000000u;
    put(w, 0, 1, 4); put(w, 4, n, 4);
    for (uint32_t i = 0; i < ((n + 3) & ~3u); ++i) w[8 + i] = i < n ? b[i] : 0;
    return 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_148a0(uint32_t* r) { uint8_t b[17] = {}; uint32_t n = d2_client_148a0_build(b, r[0], r[1], r[2], r[3], r[5]); r[4] = r[13] ? wire(r[6], n, b) : fold(n, b); }
extern "C" uint32_t d2_client_148e0_build(uint8_t*, uint32_t, uint32_t, uint32_t, uint32_t);
// 1.13c D2Client+0x148E0: [op, ecx32, edx32, c32], 13 bytes (r[3]=c)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_148e0(uint32_t* r) { uint8_t b[13] = {}; uint32_t n = d2_client_148e0_build(b, r[0], r[1], r[2], r[3]); r[4] = r[13] ? wire(r[5], n, b) : fold(n, b); }

// 1.13c D2Client+0x45FB0: clears the dword at 0x11978E, sends [0x38, 0x00000000, key = [0x11971D] (32), ECX (32)] (13 bytes, EBX = 0xD).
extern "C" uint32_t d2_client_45fb0(uint32_t* g978e, uint32_t key, uint32_t ecx, uint8_t* o) { *g978e = 0; pk(0x38, 0, key, 13, 4, 4, 5, o); put(o, 9, ecx, 4); return 13; }
