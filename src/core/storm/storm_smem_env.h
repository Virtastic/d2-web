// 1.13c Storm SMem entry points (SMemAlloc 0x1C580, SMemFree 0x1B960, SMemReAlloc 0x1C760, SMemFindNextBlock 0x1BAE0):
// the module state and callees they reach, as a seam. Addresses are the retail 32-bit words.
//
// Block header (8 bytes before the user pointer): u16 size, u8 pad, u8 flags (1 guard word 0x12B1 at the end, 2 free,
// 4 large: the user pointer is stored at +8, 8 redirect: the real header pointer is the dword before), u16 heap (the heap
// record's high address word), u16 magic 0x6F6D.
#pragma once
#include <cstdint>
#include <cstring>

struct StormSMemEnv {
    uint32_t modBase;                 // module base: the error-site strings are modBase + rva
    uint32_t* memReady;               // Storm+0x55168
    uint32_t* memLog;                 // Storm+0x54D58
    uint32_t* lastError;              // Storm+0x55388
    uint32_t* heapTrack;              // Storm+0x55160
    uint32_t* heapLists;              // Storm+0x53150, 256 list heads
    uint32_t (*heapLockSlot)(uint32_t heap);        // [heap+8]: the heap's lock index (0xFFFFFFFF: none)
    void (*setLastError)(uint32_t code);
    void (*enterLock)(uint32_t slot);               // EnterCriticalSection(&locks[slot]), locks at Storm+0x53558, 0x18 each
    void (*leaveLock)(uint32_t slot);
    void (*exitProcess)(uint32_t code);
    void (*report2F590)(uint32_t code, uint32_t where, uint32_t line, uint32_t a3, uint32_t a4, uint32_t a5);
    void (*badBlock1ACF0)(uint32_t esiCode, uint32_t tag, uint32_t line);
    void (*free1B170)(uint32_t edxHdr, uint32_t heap, uint32_t ptr);
    uint32_t (*alloc1C580)(uint32_t size, uint32_t tag, uint32_t line, uint32_t flags);
    uint32_t (*realloc1C660)(uint32_t eaxHdr, uint32_t* slot, uint32_t heap, uint32_t ptr, uint32_t size, uint32_t flags);
    uint32_t (*heapId1AD80)(uint32_t esiTag, uint32_t ebxLine);
    uint32_t (*newHeap1BDD0)(uint32_t eaxTag, uint32_t line, uint32_t id, uint32_t idx, uint32_t a4, uint32_t a5, uint32_t a6);
    uint32_t (*alloc1C390)(uint32_t eaxHeap, uint32_t* ebxSlot, uint32_t flags, uint32_t size);
    void (*check1A600)();
};

namespace storm_smem {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline uint32_t rd16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
inline uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
// the guard word of a flag-1 block: u16 at hdr + size - pad - 2 (32-bit wrap as retail)
inline bool guardBad(uint32_t hdr) { return rd16(hdr + (rd16(hdr) - rd8(hdr + 2)) - 2) != 0x12B1; }
}
