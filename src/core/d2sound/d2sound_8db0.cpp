// src/core/d2sound/d2sound_8db0.cpp -- 1.13c D2sound+0x8DB0
// __fastcall(ECX = mode). Zero if the EAX property object (DAT_6f9c64b8) is null. Otherwise IKsPropertySet::Set (vtable +0x10, stdcall:
// this, guid, id, instance 0, instance length 0, &value, 4) on it:
//   mode 1:  guid 0x6F9C01D8, id 2, value 0 ........................ success sets DAT_6f9c659c = 1, returns 1; failure returns 0
//   else:    guid 0x6F9C01F8, id 2 then id 3, value -10000 (0xFFFFD8F0) on both; both must be >= 0 to set the flag and return 1
// The object, the flag and the two GUIDs are the caller's (guest words and addresses), the probe pins them.
#include <cstdint>

typedef int32_t (*D2sndPropSet)(uint32_t self, const void* guid, uint32_t id, uint32_t inst, uint32_t instLen, const void* data, uint32_t len);

static inline uint32_t d2snd_rdw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

// 1.13c D2sound+0x8DB0
uint32_t D2sound_8DB0(uint32_t mode, const uint32_t* prop, uint32_t* flag659c, const void* guid01d8, const void* guid01f8) {
    const uint32_t self = *prop;
    if (!self) return 0;
    int32_t value;
    if (mode == 1) {
        value = 0;
        const D2sndPropSet set = (D2sndPropSet)(uintptr_t)d2snd_rdw(d2snd_rdw(self) + 0x10);
        if (set(self, guid01d8, 2, 0, 0, &value, 4) < 0) return 0;
        *flag659c = 1;
        return 1;
    }
    value = (int32_t)0xFFFFD8F0;
    const uint32_t vt = d2snd_rdw(self);
    if (((D2sndPropSet)(uintptr_t)d2snd_rdw(vt + 0x10))(self, guid01f8, 2, 0, 0, &value, 4) < 0) return 0;
    const uint32_t self2 = *prop;
    if (((D2sndPropSet)(uintptr_t)d2snd_rdw(d2snd_rdw(self2) + 0x10))(self2, guid01f8, 3, 0, 0, &value, 4) < 0) return 0;
    *flag659c = 1;
    return 1;
}
