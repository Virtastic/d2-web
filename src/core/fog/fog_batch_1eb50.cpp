// src/core/fog/fog_batch_1eb50.cpp -- 1.13c Fog.dll +0x1FB20 (ordinal 10067) and +0x1EB50 (ordinal 10070),
// from build-wasm/ghidra-fog/decomp/0x1FB20.c, 0x1EB50.c and the disassembly. Seams on the env struct.
#include <cstdint>
#include <cstring>

namespace {
#pragma pack(push, 1)
struct FogArr {  // DataArrays handle (see fog_batch_1fa90.cpp)
    char name[0x100]; int32_t count; int32_t stride; int32_t init; int32_t at10c;
    int32_t* index10; int32_t at114; int32_t at118; int32_t at11c; uint8_t at120;
};
#pragma pack(pop)
struct FogEnv1EB50 {
    uint32_t (*capture_ip)(void);                                       // +0xFC20
    void (*fatal)(uint32_t ip, uint32_t line);                          // +0x11100 report + _exit(-1)
    int32_t (*send)(void* h, const void* buf, uint16_t n);              // +0x1E910
};
}  // namespace

// 1.13c Fog+0x1FB20 (ordinal 10067), fastcall(ecx=array, edx=packed index)
// Slot lookup: -1 gives 0; index<0 or (index>>16) >= count is fatal (line 0xDB); an array without
// storage (+0x108==0) is fatal (line 0xDE). Returns the slot pointer, or 0 when its tag word is 0 or
// the low word is nonzero and differs from the tag.
extern "C" int32_t fog_sub_1FB20(uint8_t* a, int32_t idx, const FogEnv1EB50* e, int32_t* data, int32_t count, int32_t stride, int32_t init) {
    if (idx == -1) return 0;
    const int32_t hi = (int16_t)((uint32_t)idx >> 16);
    if (idx < 0 || count <= hi || !init) { e->fatal(e->capture_ip(), idx < 0 || count <= hi ? 0xDB : 0xDE); return 0; }
    uint8_t* slot = (uint8_t*)data + (int64_t)stride * hi;
    const int16_t tag = *(int16_t*)slot;
    return (tag && ((int16_t)idx == 0 || tag == (int16_t)idx)) ? (int32_t)(intptr_t)slot : 0;
}

// 1.13c Fog+0x1EB50 (ordinal 10070), stdcall(array, data, len16)
// Frame and send. array null / +0x110 null / *(+0x110)==0 -> -1. If +0x118==4 && +0x11C==2: data and
// len required, len in 1..0x205F (else fatal 0x5D); frame = {u8 array[0x120], u16 len+3, data}.
// Otherwise len+2 must be < 0x2064 (else fatal 0x71); frame = {u16 len+2, data}, data dropped (len 0)
// if null. Returns send(frame) + (len - frameLen).
extern "C" int32_t fog_sub_1EB50(const FogEnv1EB50* e, uint8_t* arr, const uint8_t* data, uint16_t len) {
    if (!arr || !*(int32_t**)(arr + 0x110) || !**(int32_t**)(arr + 0x110)) return -1;
    static thread_local uint8_t buf[0x2068];
    uint32_t total, n = len;
    if (*(int32_t*)(arr + 0x118) == 4 && *(int32_t*)(arr + 0x11C) == 2) {
        if (!data || !len || len >= 0x2060) { e->fatal(e->capture_ip(), 0x5D); return -1; }
        buf[0] = arr[0x120]; uint16_t t = (uint16_t)(len + 3); memcpy(buf + 1, &t, 2);
        memcpy(buf + 3, data, len); total = n + 3;
    } else {
        uint16_t t = (uint16_t)(len + 2);
        if (t >= 0x2064) { e->fatal(e->capture_ip(), 0x71); return -1; }
        memcpy(buf, &t, 2);
        if (!data) n = 0;
        total = n + 2;
        if (data && total < 0x2064) memcpy(buf + 2, data, n);
    }
    return e->send(arr, buf, (uint16_t)total) + (int32_t)(n - total);
}
