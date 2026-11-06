// 1.13c Storm+0x27020 (SFile teardown, ordinal 262). Module words and callees are injected so the probe can
// pin the globals and record the calls; the engine wires the real imports and the real module functions.
#pragma once
#include <cstdint>

// The module words this function reads and writes (retail RVA in the comment). A 32-bit word per entry.
struct Storm27020Mod {
    uint32_t stopFlag;        // 0x53104
    uint32_t obj;             // 0x53108: the object destroyed through 0x4040
    uint32_t thread;          // 0x53110: the stream thread handle
    uint32_t heap1;           // 0x53118: SFILE.CPP buffer, line 0xE9A
    uint32_t heap0;           // 0x53120: SFILE.CPP buffer, line 0xE94
    uint32_t heap2;           // 0x53128: SFILE.CPP buffer, line 0xEA0
    uint32_t event;           // 0x5312C: the stop event
    uint32_t headDtor4040;    // 0x55978: drained through 0x4040 (no name log)
    uint32_t headAudio;       // 0x559CC: AUDIOSTREAM, through 0x3FC0
    uint32_t headHsFile;      // 0x5596C: HSFILE, through 0x3EF0
    uint32_t headHsArchive;   // 0x55984: HSARCHIVE, through 0x3E20
};

// The callees and imports. A list head is drained while it reads as a positive signed dword.
struct Storm27020Seam {
    void (*sub26F30)();                                           // 0x26F30
    void (*setEvent)(uint32_t handle);                            // SetEvent
    void (*waitThread)(uint32_t handle, uint32_t ms);             // WaitForSingleObject
    void (*closeHandle)(uint32_t handle);                         // CloseHandle
    void (*dtor4040)(uint32_t obj);                               // 0x4040
    void (*dtor3FC0)(uint32_t obj);                               // 0x3FC0
    void (*dtor3EF0)(uint32_t obj);                               // 0x3EF0
    void (*dtor3E20)(uint32_t obj);                               // 0x3E20
    void (*unlog)(const char* kind, uint32_t p);                  // 0x2FB30 (the list name, the object plus 8 or 0)
    void (*smemFree)(uint32_t p, const char* tag, uint32_t line, uint32_t flags);   // 0x1B960 (SMemFree)
};

extern "C" uint32_t Storm13c_27020(Storm27020Mod* m, const Storm27020Seam* s);
