// Seams of D2Client 0x84080 (item packet that moves an item on a monster unit's owner). Callees are named by their retail address.
#pragma once
#include <cstdint>
typedef uintptr_t U;
struct D2Client84080Env {
    uint32_t (*rd)(U base, uint32_t off, uint32_t nbytes);   // little-endian read of 1..4 bytes
    U monTbl;                                               // D2Client+0x10A808, 128 chain heads
    U itTbl;                                                // D2Client+0x10AE08, 128 chain heads
    uint32_t id;                                            // dword at D2Client+0x11971D
    uint32_t flag;                                          // dword at D2Client+0x119721
    uint32_t g9663;                                         // dword at D2Client+0x119663
    uint32_t g9854;                                         // dword at D2Client+0x119854
    uint32_t* bc54;                                         // dword at D2Client+0x11BC54 (written)
    void     (*f11105)(U src, uint32_t len, uint32_t z, uint8_t* out);        // thunk 0xCA90: 24-byte header out
    uint32_t (*f10646)(U owner, U item);                    // thunk 0xC6D0
    uint32_t (*f10020)(U item);                             // thunk 0xC92E
    uint32_t (*f10584)(U mon, uint32_t a1, uint32_t a2);    // thunk 0xC8A4 (3 args, stdcall)
    void     (*f10869)(uint32_t line, U owner, uint32_t x, uint32_t y, uint32_t q, uint32_t one, uint32_t c);   // thunk 0xC9FA (8 args, file Item.cpp at +0xD4964)
    void     (*f10391)(U owner, U item);                    // thunk 0xC8B6
    uint32_t (*f10197)(uint32_t a, uint32_t b);             // thunk 0xCA3C
    void     (*f10012)(U item, uint32_t v);                 // thunk 0xCA24
    uint32_t (*f11148)(U owner, U item, uint32_t x, uint32_t y, uint32_t q, uint32_t* o1, uint32_t* o2, uint32_t b10);   // thunk 0xC952 (8 args)
    void     (*f82660)(U item, uint32_t eax, uint32_t w6, uint32_t b10, U mon);   // 0x82660: ECX = item, EAX = the dword at hdr+4
    void     (*f82980)(U item, uint32_t a, uint32_t b, uint32_t c, uint32_t d);  // 0x82980: EAX = item, EBX = 0
    U        (*f835c0)(U pkt, uint8_t* hdr, uint32_t a, uint32_t b);              // 0x835C0, EBX = 0
    void     (*f56ef0)(U tbl, uint32_t idx, uint32_t id, uint32_t type);          // 0xA6EF0: ECX = table, EDX = index
    void     (*f47a70)(uint32_t id);
    void     (*f81aa0)();
    void     (*f6e2b0)();
    void     (*f8bbf0)();
    uint32_t (*die)();                                      // 10265(0x5A), 10024(str, r), _exit(-1); returns EAX if the host returns
};
// 1.13c D2Client+0x84080: EDI = packet.
void d2c_84080(const D2Client84080Env& e, U pkt);
