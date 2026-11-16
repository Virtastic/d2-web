// Env of the 1.13c D2Common+0x3A650 port (#10343, empty a stat list's own array): the Fog realloc thunk and two announcers as inputs.
#pragma once
#include <cstdint>
struct D2Common3A650Env {
    uint32_t (*realloc)(uint32_t pool, uint32_t arr, uint32_t size, uint32_t line);   // 0x9252 (Fog 10047: ECX pool, EDX arr, stack size, file, line, 0)
    void (*announce)(uint32_t statList, uint32_t key, int32_t value);               // 0x39F70 (EAX list, EDI key, stack value, 0)
    uint32_t (*notify)(uint32_t key, uint32_t statList);                            // 0x38550 (EAX key, EBX list); returns its EAX
};
// The function is void (stdcall, ret 4); the result is the EAX it leaves, which a capture without `retmask 0` records: after the last
// removal the list flags (+0x10) when non-negative, else the owner kind (+8) when non-zero, else 0x38550's EAX. A null or empty list
// leaves the caller's EAX (`entry`).
extern "C" uint32_t D2Common_3A650_ClearStats(uint32_t pStatList, uint32_t entry, const D2Common3A650Env* e);
