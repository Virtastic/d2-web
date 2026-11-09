// D2Client packet-handler seam shared by the 0xAF090..0xAF750 handlers (ECX/EDX = unit or packet, callees 0xA4E20, 0x2A1B0, 0x2A750, 0xc9030).
#pragma once
#include <cstdint>
struct D2cPktEnv {
    uintptr_t (*lookup)(uint32_t id, uintptr_t row, uint32_t type);                         // D2Client+0xA4E20 (EAX = id & 0x7F, ECX = id, EDX = row, stack type)
    uintptr_t unitTable;                                                                     // 0x6FBBA608 (row = unitTable + type * 0x80 pointers)
    // D2Client+0x2A750: ECX = kind byte, EDX = unit, stack (l = request block, 1); returns through EAX
    void (*f2A750)(uint32_t ecx, uintptr_t edx, const uint32_t* l);
    // D2Client+0x2A1B0: EAX = word, ESI = unit, stack 4 words (ret 0x10)
    void (*f2A1B0)(uint32_t eax, uintptr_t esi, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    // D2Client+0xc9030: EAX = 0, stack 5 words
    void (*f79030)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
};
extern "C" {
void d2c_af090(const D2cPktEnv* e, const uint8_t* p);
void d2c_af0f0(const D2cPktEnv* e, const uint8_t* p);
void d2c_af160(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af1d0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af340(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af390(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af3f0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af460(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af4c0(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af510(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
void d2c_af750(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p);
}
