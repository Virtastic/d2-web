// src/core/storm/storm_26500.cpp -- 1.13c Storm.dll+0x26500, from the cached decompile.
#include <cstdint>
#include <cstring>
#include <Windows.h>

CRITICAL_SECTION* Storm_Lock45988();
uint32_t Storm_List459CC();
void Storm_SetError45388(uint32_t e);

namespace {
uint32_t rd32(uint32_t p, uint32_t off) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + off), 4); return v; }
void wr32(uint32_t p, uint32_t off, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + off), &v, 4); }
// COM call on the sound-buffer object at node+0x3C: vtable slot `slot`, one argument (IDirectSoundBuffer
// SetVolume is +0x3C, SetPan is +0x40).
void callSlot(uint32_t node, uint32_t slot, int32_t arg) {
    uint32_t obj = rd32(node, 0x3C);
    auto fn = (int32_t (*)(void*, int32_t))(uintptr_t)rd32(rd32(obj, 0), slot);
    fn((void*)(uintptr_t)obj, arg);
}
}  // namespace

// 1.13c Storm+0x26500: under the 0x6FC45988 lock, find the 0x6FC459CC record keyed (+8) on key. A
// setting other than 0x7FFFFFFF that differs from the stored one (+0x34 volume, +0x38 pan) is stored,
// then pushed to the buffer object. Found -> 1. Not found -> error 0x85100072 (0x6FC45388 and
// SetLastError) after the lock is released -> 0.
extern "C" uint32_t Storm_Sub26500(int32_t key, int32_t volume, int32_t pan) {
    EnterCriticalSection(Storm_Lock45988());
    uint32_t p = Storm_List459CC();
    if ((int32_t)p > 0) {
        do {
            if ((int32_t)rd32(p, 8) == key) {
                if (volume != 0x7FFFFFFF && volume != (int32_t)rd32(p, 0x34)) {
                    wr32(p, 0x34, (uint32_t)volume);
                    callSlot(p, 0x3C, volume);
                }
                if (pan != 0x7FFFFFFF && pan != (int32_t)rd32(p, 0x38)) {
                    wr32(p, 0x38, (uint32_t)pan);
                    callSlot(p, 0x40, pan);
                }
                LeaveCriticalSection(Storm_Lock45988());
                return 1;
            }
            p = rd32(p, 4);
        } while ((int32_t)p > 0 && p != 0);
    }
    LeaveCriticalSection(Storm_Lock45988());
    Storm_SetError45388(0x85100072u);
    return 0;
}
