// Seams of D2Client 0x854E0 (item packet: owner equip confirmation). Callees are named by their retail address.
#pragma once
#include <cstdint>
typedef uintptr_t U;
struct D2Client854e0Env {
    uint32_t (*rd)(U base, uint32_t off, uint32_t nbytes);   // little-endian read of 1..4 bytes
    U itTbl;                                                // D2Client+0x10AE08, 128 chain heads
    uint32_t self;                                          // dword at D2Client+0x11BBFC
    uint32_t g9854;                                         // dword at D2Client+0x119854
    void     (*f11105)(U src, uint32_t len, uint32_t z, uint8_t* out);        // thunk 0xCA90: 24-byte header out
    U        (*fa4e20)(uint32_t id, uint32_t type);         // 0xA4E20: ECX = id, EAX = id&0x7F, EDX = table 0x10A608+type*0x200, stack type
    uint32_t (*f10646)(uint32_t owner, U item);             // thunk 0xC6D0
    uint32_t (*f10584)(U mon, uint32_t a1, uint32_t a2);    // thunk 0xC8A4
    void     (*f10869)(uint32_t line, uint32_t owner, uint32_t x, uint32_t y, uint32_t q, uint32_t one, uint32_t z);   // thunk 0xC9FA (8 args, Item.cpp at +0xD4964)
    void     (*f8bbf0)();
    void     (*f6e2b0)();
    uint32_t (*f10197)(uint32_t a, uint32_t b);             // thunk 0xCA3C
    void     (*f56ef0)(U tbl, uint32_t idx, uint32_t id, uint32_t type);      // 0xA6EF0: ECX = table, EDX = index
    U        (*f833f0)(U pkt, uint8_t* hdr, uint32_t a, uint32_t b);          // 0x833F0, EBX = 0
    void     (*f82980)(U item, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t ebx);   // 0x82980: EAX = item
    void     (*f10058)(U item, uint32_t v);                 // thunk 0xC9DC
    uint32_t (*f10405)(uint32_t owner, U item, uint32_t mode);   // thunk 0xCA6C
    uint32_t (*f81b40)(U item, uint32_t esi, uint32_t owner);    // 0x81B40: EBX = item, ESI, EDI = owner
    void     (*f693c0)(U mon);                              // 0x693C0: EDX
    void     (*f10138)(uint32_t owner, U item);             // thunk 0xCA18
    void     (*f6a220)(U mon);                              // 0x6A220: EAX, stack 0
    void     (*f81ff0)(U mon, U item, U ebx);               // 0x81FF0: EDI = mon, ESI = item, EBX, stack 0
    void     (*f82fe0)(U mon);                              // 0x82FE0
    uint32_t (*f81aa0)();
    void     (*die)(uint32_t line);                         // 10265(line), 10024(str, r), _exit(-1); falls through if the host returns
};
// 1.13c D2Client+0x854E0: EAX = packet.
// Returns the EAX retail leaves on each exit.
uint32_t d2c_854e0(const D2Client854e0Env& e, U pkt);
