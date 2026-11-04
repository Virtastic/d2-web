// 1.13c D2Multi+0x14100 (stdcall, one ignored stack word; returns 0/1): resolves the game the lobby named. 7C62(DAT_6fa09ccc) must answer, 7C50 turns
// DAT_6fa09cd4/cd8 into the two wide strings (esi == 0 ends it); unicode2Win narrows them into two 256-byte stack buffers, each lowercased by 7BCC. If 7E2A
// answers, CBD0(0, 0). Otherwise the buffer A string is looked up among the DAT_6fa09dcc 0x3B-byte records at DAT_6f9fa598 (name at +1, id word at +0x33, text id
// dword at +0x37): a hit calls 7BD8(the dword) and CBD0(id, its answer). A miss asks 139B0 (EBX = a 0x3B-byte record, buffer A, buffer B): 0 gives AC10 with the
// 0x1411 text, a record whose name (+1) equals buffer A gives CBD0(dword at +0x33, 7BD8(dword at +0x37)). CBD0 takes EBX and EDI as locals; every CBD0 exit continues
// with 1DA0 and AA60(EBX = 0, 0x147B text) and returns 1.
#include "d2multi_env.h"
#include <cstring>

// u2w = Unicode::unicode2Win; h = the dwords at DAT_6fa09ccc.. (0x10 bytes); cnt = DAT_6fa09dcc; tbl = the bytes at DAT_6f9fa598.
uint32_t d2multi_14100(const D2mEnv& e, void (*u2w)(char*, const uint16_t*, int), const uint32_t* h, const int32_t* cnt, const uint8_t* tbl) {
    char bufA[256], bufB[256];
    uint8_t rec[0x3c];
    if (d2m_call(e, 0x7c62, 0, h[0]) == 0) return 0;
    uint32_t esi = d2m_call(e, 0x7c50, 0, h[2]);
    uint32_t edi = d2m_call(e, 0x7c50, 0, h[3]);
    if (esi == 0) return 0;
    u2w(bufA, (const uint16_t*)(uintptr_t)esi, 0x100);
    u2w(bufB, (const uint16_t*)(uintptr_t)edi, 0x100);
    d2m_call(e, 0x7bcc, 0, 0, 0, 0, 0, d2m_ptr(bufA));
    d2m_call(e, 0x7bcc, 0, 0, 0, 0, 0, d2m_ptr(bufB));
    uint32_t s0, s1;
    if (d2m_call(e, 0x7e2a) != 0) {
        s0 = 0; s1 = 0;
    } else {
        int32_t i = 0;
        const int32_t n = *cnt;
        for (; i < n; i++) if (strcmp((const char*)tbl + 1 + i * 0x3b, bufA) == 0) break;
        if (i < n) {
            const uint8_t* r = tbl + i * 0x3b;
            uint32_t txt; memcpy(&txt, r + 0x37, 4);
            s1 = d2m_call(e, 0x7bd8, 0, txt);
            s0 = (uint32_t)r[0x33] | ((uint32_t)r[0x34] << 8);
        } else {
            uint32_t ok = d2m_call(e, 0x139b0, 0, 0, 0, d2m_ptr(rec), 0, d2m_ptr(bufA), d2m_ptr(bufB));
            if (ok == 0) {
                uint32_t t = d2m_call(e, 0x7ea8, 0, 0x1411);
                d2m_call(e, 0xacb0, 0, 0, 0, 0, 0, t);
                return 0;
            }
            if (strcmp((const char*)rec + 1, bufA) != 0) return 0;
            uint32_t txt; memcpy(&txt, rec + 0x37, 4);
            s1 = d2m_call(e, 0x7bd8, 0, txt);
            memcpy(&s0, rec + 0x33, 4);
        }
    }
    d2m_call(e, 0xcbd0, 0, 0, 0, d2m_ptr(bufB), 0, s0, s1);
    d2m_edi(e, 0xcbd0, d2m_ptr(bufA));
    d2m_call(e, 0x11da0);
    uint32_t t = d2m_call(e, 0x7ea8, 0, 0x147b);
    d2m_call(e, 0xaa60, 0, 0, 0, 0, 0, t);
    return 1;
}
