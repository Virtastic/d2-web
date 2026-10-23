// src/core/d2win/d2win_sub_18750.cpp -- 1.13c D2Win+0x18750 (sub_18750, 139 B), from the cached decompile.
// Walks the global list (head DAT_6f9a9e30, next at +0xC4) three times: EA30 per node; E5F0(-1) per node;
// then D4D0 + CFC0(node) per node (next is read AFTER CFC0, as retail). Null node mid-walk is unreachable
// (inlined assert): Ordinal_10265(0xd41) text, Ordinal_10024, _exit(-1).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2WinSub18750Env {
    uint8_t* head;                       // DAT_6f9a9e30
    void (*ea30)();                      // sub_EA30
    void (*e5f0)(int32_t);               // sub_E5F0
    void (*d4d0)();                      // sub_D4D0
    void (*cfc0)(uint8_t*);              // sub_CFC0
    void (*fatal)(uint32_t code);        // 0xD41
    uint8_t* (*next)(const uint8_t*);    // *(node+0xC4), read at call time
};

// 1.13c D2Win+0x18750
void D2Win_sub_18750(const D2WinSub18750Env& e) {
    if (!e.head) return;
    for (uint8_t* n = e.head; n; n = e.next(n)) e.ea30();
    for (uint8_t* n = e.head; n; n = e.next(n)) e.e5f0(-1);
    for (uint8_t* n = e.head; n;) { e.d4d0(); e.cfc0(n); n = e.next(n); }
}
