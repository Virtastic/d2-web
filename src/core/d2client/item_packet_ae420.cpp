// src/core/d2client/item_packet_ae420.cpp -- D2Client.dll 1.13c, base 0x6FAB0000. Mapping lane, env-seam style.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// host rva: < 0x10000 = D2Client RVA, 0x10000|n = import ordinal n. UNVERIFIED ordinals (names/conventions guessed):
// 10126 bitreader init(rd,buf,len), 10130 read(rd,nbits), 10129 skip(rd,nbits), 10887 set unit stat(u,stat,val,0),
// 10069 set (u,0,val), 10930/11013/10785/10807 inventory/socket-item create+attach, 10188 add magic prop,
// 11104 is-player-unit(u). Globals: DAT_6fbcbbfc=+0x10BBFC (player), DAT_6fbcc4d4=+0x1104D4 (client-unit list),
// sgptDataTables rows 0x144 @+0xBCC / count +0xBD4 (magic property rows).
struct AE420G { const uint8_t* player; uint32_t listHead; const uint8_t* dataTables; };
static uint32_t H(D2cHost h, uint32_t rva, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
    D2cCall k = {}; k.rva = rva; k.s[0] = a; k.s[1] = b; k.s[2] = c; k.s[3] = d; return h(&k);
}
// 1.13c D2Client+0xAE420 (1093 B; no static callers -- packet dispatch table): parse the item-drop/extended-item packet
// pkt (bytes: +1 id32, +5 u16, +7 u16, +9 u16, +0xB owner-mode, +0xC len, +0xD bitstream), build the item spec, call
// FUN_6fb57780 (A7780 create), link/flag the unit and read the property list. Bitstream field order below is
// the decompiler's call order; stack-aliased locals (spec words 0..6) are UNVERIFIED.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_ae420(const uint8_t* pkt, const AE420G* g, D2cHost h) {
    uint8_t rd[16], rd2[16];
    auto rb = [&](uint8_t* r, uint32_t n) { return H(h, 0x10000 | 10130, (uint32_t)(uintptr_t)r, n); };
    H(h, 0x10000 | 10126, (uint32_t)(uintptr_t)rd, (uint32_t)(uintptr_t)(pkt + 0xD), pkt[0xC] - 0xD);
    uint32_t type = rb(rd, 4);                      // 4-bit item type/action
    uint32_t hasData = rb(rd, 1);
    if (hasData) {                                  // 16 x per-slot nibble widths from record 0x15..: bits = clz-style width of (b-1), b<3 -> 1
        uint32_t rec = H(h, 0x1260);                // FUN_6fab1260 (A1260 record lookup)
        if (!rec) return 0;
        for (int i = 0; i < 16; i++) {
            uint32_t b = ((const uint8_t*)(uintptr_t)rec)[0x15 + i], w;
            if (b < 3) w = 1; else { b--; w = 32 - __builtin_clz(b); }
            rb(rd2, w);
        }
    }
    uint32_t flags = 0, name = 0, extra = 0;
    if (rb(rd2, 1)) {                               // extended item
        uint32_t f4 = rb(rd2, 1) ? 4 : 0, f8 = rb(rd2, 1) ? 8 : 0, f2 = rb(rd2, 1) ? 2 : 0, f10 = rb(rd2, 1) ? 0x10 : 0, f40 = rb(rd2, 1) ? 0x40 : 0;
        flags = f4 | f8 | f2 | f10 | f40;
        if (f2) H(h, 0x10000 | 10129, (uint32_t)(uintptr_t)rd2, 0x10);
        while (rb(rd2, 8)) {}                       // NUL-terminated 8-bit name chars (into a stack buffer)
        rb(rd2, 16);
        if (rb(rd2, 1)) extra = rb(rd2, 0x20);
    }
    H(h, 0x53C30, type);                            // FUN_6fb03c30
    int32_t u = (int32_t)H(h, 0xA7780, 0, 0, 0, 0); // FUN_6fb57780(&spec,..) spec = {id,txt,seed,..flags,extra}; 0 -> bail
    if (!u) return 0;
    H(h, 0x6CB30);
    uint32_t isP = H(h, 0x10000 | 11104, u), same = 0;
    if (isP && g->player) {
        same = 0xFFFFFFFFu;
        for (uint32_t n = g->listHead; n; ) {       // list node: type +4 (7), id +8, key +0xC, next +0x30
            D2cCall k = {}; k.rva = 0; k.eax = n; h(&k);
            if (k.out[0] == 7 && k.out[2] == *(const uint32_t*)(g->player + 0xC)) { same = k.out[1]; break; }
            n = k.out[3];
        }
    }
    if (!isP || !same) { H(h, 0x10000 | 10887, u, 7, 0x8000); H(h, 0x10000 | 10887, u, 6, (uint32_t)pkt[0xB] << 8); }
    if (isP && type == 1) *(uint32_t*)(uintptr_t)(u + 0xC8) &= 0xFFFBFFFFu;
    H(h, 0x10000 | 10887, u, 0x148, 0);             // arg = (spec.id + spec.size) & 0xFFFF
    if (rb(rd2, 1)) H(h, 0x10000 | 10069, u, 0, rb(rd2, 0x1F));
    if (rb(rd2, 1)) {                               // magic property list: 9-bit ids until >= 0x1FF
        uint32_t it = 0;
        for (int32_t id = (int32_t)rb(rd2, 9); id < 0x1FF && id >= 0 && id < *(const int32_t*)(g->dataTables + 0xBD4); id = (int32_t)rb(rd2, 9)) {
            D2cCall k = {}; k.rva = 1; k.eax = id; h(&k);   // host: row = rows+id*0x144; out[0]=bits(+8) out[1]=skip(+9) out[2]=signed-flag(+4&bitmask[4])
            if (!k.out[0]) break;
            if (k.out[1]) H(h, 0x10000 | 10129, (uint32_t)(uintptr_t)rd2, k.out[1]);
            uint32_t v = k.out[0] < 0x20 && k.out[2] ? H(h, 0x10000 | 10129, (uint32_t)(uintptr_t)rd2, k.out[0]) : rb(rd2, k.out[0]);
            if (!it) { it = H(h, 0x10000 | 10930, (uint32_t)u, 0, 0x40);
                if (!it) { it = H(h, 0x10000 | 11013, 0, 0x40, 0, *(uint32_t*)(uintptr_t)u); H(h, 0x10000 | 10785, it, 0); H(h, 0x10000 | 10807, (uint32_t)u, it, 1); } }
            if (it) H(h, 0x10000 | 10188, it, (uint32_t)id, v);
        }
    }
    return u;
}
