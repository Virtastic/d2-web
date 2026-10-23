// src/core/d2client/state_add_add70.cpp -- D2Client.dll 1.13c, base 0x6FAB0000.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// 1.13c D2Client+0xADD70 (320 B, __fastcall ecx = packet p): packet handler. p+1 u8 unit type, p+2 u32 unit id,
// p+7 u8 state id, p+8.. = bit-stream of (stat id:9 bits, value) pairs ended by id >= 0x1ff.
// Env seams (host rva): 0xA4E20 unit lookup (eax=type, ecx=id, edx=&list[type]), 0x73760 state-on (ecx=state, eax=unit),
// 0x72530 state-off tail (ecx=state, eax=unit), 0x72610 apply stat (ecx=p+8, edx=local, s0=prev, s1=val, eax=stat),
// ordinals unverified (names unknown): 10126 = bitreader init, 10130 = read unsigned n bits, 10129 = read signed n bits;
// bit reader state is the host's. Rows of the 0x144-stride table (sgptDataTables+0xBCC, count at +0xBD4) are passed as
// tbl/tblN; gdwBitMasks[4] = mask4 (D2 bit 4 test against row+4).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_add70(uint8_t* p, uint8_t* list, uint8_t* tbl, int32_t tblN,
                                                     uint32_t mask4, D2cHost h) {
    D2cCall c = {}; c.rva = 0xA4E20; c.eax = p[1]; c.ecx = *(uint32_t*)(p + 2);
    c.edx = (uint32_t)(uintptr_t)(list + p[1] * 0x80); c.s[0] = p[1];
    if (!h(&c)) return;
    uint32_t unit = c.out[0] ? c.out[0] : 0, st = p[7];
    D2cCall a = {}; a.rva = 0x73760; a.ecx = st; a.eax = unit; h(&a);
    D2cCall i = {}; i.rva = 10126; h(&i);
    D2cCall r = {}; r.rva = 10130; r.s[0] = 9; h(&r);
    int32_t id = (int32_t)r.out[0], prev = 0;
    for (; id < 0x1ff;) {
        if (id < 0 || tblN <= id) return;
        uint8_t* row = tbl + id * 0x144;
        if (!row || !row[8]) return;
        if (row[9]) { D2cCall s = {}; s.rva = 10129; s.s[0] = row[9]; h(&s); }
        D2cCall v = {};
        v.rva = (row[8] < 0x20 && (mask4 & row[4])) ? 10129 : 10130; v.s[0] = row[8]; h(&v);
        D2cCall m = {}; m.rva = 0x72610; m.ecx = (uint32_t)(uintptr_t)(p + 8); m.edx = (uint32_t)(uintptr_t)c.out;
        m.s[0] = prev; m.s[1] = v.out[0]; m.eax = id; h(&m);
        prev = (int32_t)m.out[0];
        D2cCall n = {}; n.rva = 10130; n.s[0] = 9; h(&n); id = (int32_t)n.out[0];
    }
    D2cCall t = {}; t.rva = 0x72530; t.ecx = st; t.eax = unit; h(&t);
}
