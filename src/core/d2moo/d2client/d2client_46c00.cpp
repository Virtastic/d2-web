// D2Client 0x46C00: send the 9-byte '6' packet, tear down the open popup, open the wait box (no static callers).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
static uint32_t rd46c00(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static void wr46c00(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
// 1.13c D2Client+0x46C00 ; a = 0x119654.. (+0 index, +4 0x119658, +0xF 0x119663 := 10 as a dword), t = the 0x10-byte entries at 0x11967D (the
// word at index*0x10 rides zero-extended), f = 0x11974B.. (+0 deadline 0x11974B, +4 0x11974F, +0x14 box 0x11975F, +0x24 flag 0x11976F,
// +0x30 object 0x11977B). Host: 0x143E0 (stack packet, EBX = 9), 0xA3D20, rva 0 = the object's vtable slot 0 (ECX = object, 1 word = 1),
// 0x46880 (ESI = the 8-byte out pair, zeroed first), 0xA3E40 (11 words: pair[0], pair[1], callback 0x45BF0, 0, 0, 0, 1, 0, 0, 0, 1),
// 0xD1EC (ECX = 0xD19, the string), 0xA36F0 (ECX = box, EAX = 0; string, 0xF, 0, 1, 0, 0). A null box is fatal (string 0x529, exit -1).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_46c00(uint8_t* a, const uint8_t* t, uint8_t* f, uint32_t tick, uint32_t* (*at)(uint32_t),
                                                    void (*fatal)(), D2cHost host) {
    uint8_t pk[9]; uint16_t w; std::memcpy(&w, t + rd46c00(a, 0) * 0x10, 2);
    pk[0] = '6'; wr46c00(pk, 1, rd46c00(a, 4)); wr46c00(pk, 5, w);
    D2cCall c = {}; c.rva = 0x143E0; c.ebx = 9; c.s[0] = (uint32_t)(uintptr_t)pk; host(&c);
    if (rd46c00(f, 0x24)) {
        c = {}; c.rva = 0xA3D20; host(&c);
        if (uint32_t o = rd46c00(f, 0x30)) { c = {}; c.rva = 0; c.ecx = o; c.s[0] = 1; host(&c); }
        wr46c00(f, 0x30, 0); wr46c00(f, 0x24, 0);
    }
    wr46c00(a, 0xF, 10);
    uint32_t pr[2] = { 0, 0 };
    c = {}; c.rva = 0x46880; c.esi = (uint32_t)(uintptr_t)pr; host(&c);
    c = {}; c.rva = 0xA3E40; c.s[0] = pr[0]; c.s[1] = pr[1]; c.s[2] = 0x45BF0; uint32_t box = host(&c);
    wr46c00(f, 0x14, box); wr46c00(f, 0, tick + 60000); wr46c00(f, 4, 0);
    if (!box) { fatal(); return; }
    at(box)[0x17] = 0;
    c = {}; c.rva = 0xD1EC; c.ecx = 0xD19; uint32_t s = host(&c);
    c = {}; c.rva = 0xA36F0; c.ecx = box; c.s[0] = s; c.s[1] = 0xF; c.s[2] = 0; c.s[3] = 1; host(&c);
}
