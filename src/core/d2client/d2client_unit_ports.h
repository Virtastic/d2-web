// D2Client server->client handlers 0x0E, 0x17, 0x51, 0x0A: seams the integrator wires to the live unit table, roster and object creator.
#pragma once
#include <cstdint>
#include "d2client_pkt_env.h"
struct D2cPlayerEnv {
    const D2cPktEnv* pkt;
    uintptr_t localPlayer;                              // global at D2Client+0x1BBFC (0 = none)
    uint16_t (*rd16)(uint32_t addr);                    // a 16-bit load from an arbitrary 32-bit address (retail dereferences packet words)
    void (*fatal)(void);                                // error text 0x841 then exit(-1): called for a null unit that is not the local player
};
struct D2cRemoveEnv {
    const D2cPktEnv* pkt;
    uintptr_t localPlayer;                              // global at D2Client+0x1BBFC
    uintptr_t rosterHead;                               // global at D2Client+0x1C4D4: roster list head (nodes: +4 kind, +8 id, +0xC link key, +0x30 next)
    uint32_t (*rd32)(uintptr_t addr);                   // 32-bit load for the roster walk and local player key at +0xC
    // D2Client+0xA6EF0: ECX = table row base (unitTable + type * 0x200), EDX = guid & 0x7F, stack (guid, type)
    void (*fA6EF0)(uintptr_t row, uint32_t bucket, uint32_t guid, uint32_t type);
};
struct D2cObjectEnv {
    // D2Client+0xA75C0: ESI = kind byte, stack 7 words (ret 0x1C): guid, w6, w8, wA, 0, b0C, b0D
    void (*fA75C0)(uint32_t esi, uint32_t guid, uint32_t w6, uint32_t w8, uint32_t wA, uint32_t zero, uint32_t b0c, uint32_t b0d);
};
extern "C" {
void d2c_af560(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);                 // 0x0E
void d2c_af240(const D2cPlayerEnv* e, uintptr_t unit, const uint8_t* p);              // 0x17
void d2c_adf40(const D2cObjectEnv* e, const uint8_t* p);                              // 0x51
void d2c_aca40(const D2cRemoveEnv* e, const uint8_t* p);                              // 0x0A
}
