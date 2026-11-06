// src/core/d2ddraw/d2ddraw_8880.cpp -- 1.13c D2DDraw.dll device detect: read the DirectDraw device name, flags and GUID from
// the "Diablo II\VideoConfig" registry key through Storm, falling back to the default device. Decompile-derived, checked
// against the disassembly. The module globals arrive as pointers; Fog's system info, the three Storm registry loaders and the
// fatal path are seams.
#include <cstdint>
#include <cstring>

struct D2DDrawDetectEnv {
    uint32_t* windowed;          // DAT 0x101FC: set when the system info's mode word is 4
    uint32_t* flags;             // DAT 0xFCB0: the device flags word (dwFlags)
    uint8_t* guid;               // DAT 0xFCB4: 16 bytes
    uint8_t* name;               // DAT 0xFCC4: 256 bytes
    const uint32_t* bitMasks;    // Fog's gdwBitMasks (the data import at 0xB054)
    uint32_t nameValue, flagsValue, guidValue;   // module strings 0xCE40 "DeviceName", 0xCE38 "dwFlags", 0xCE30 (the GUID value)
    uint32_t fatalFmt;                           // module string 0xCE62
    const uint32_t* (*systemInfo)();                                                            // Fog 10022
    uint32_t (*loadString)(const char* key, uint32_t value, uint32_t flags, uint8_t* buf, uint32_t size);              // Storm 422
    uint32_t (*loadValue)(const char* key, uint32_t value, uint32_t flags, uint32_t* out);                             // Storm 423
    uint32_t (*loadData)(const char* key, uint32_t value, uint32_t flags, uint8_t* buf, uint32_t size, uint32_t* got); // Storm 421
    uint32_t (*langString)(uint32_t id);     // D2Lang lookup
    void (*fatal)(uint32_t fmt, uint32_t text);   // Fog 10024
    void (*exitProcess)(int32_t code);            // D2DDraw 0x1279
};

// 1.13c D2DDraw+0x8880 (no arguments; always 1). No system info is fatal (string 0x6C, then _exit(-1)). Mode word 4 sets
// 0x101FC. Then, keyed by "Diablo II\VideoConfig" (a 22-byte stack copy of 0xCE4C): DeviceName into 0xFCC4 (256), dwFlags
// into 0xFCB0; a flags word sharing a bit with gdwBitMasks[0] is accepted as is, otherwise the 16-byte GUID into 0xFCB4 must
// read back exactly 16 bytes. Any failed load writes "Fallback DirectDraw Device" (27 bytes with the NUL) over the name and
// sets the flags to gdwBitMasks[0].
extern "C" uint32_t d2ddraw_detect_8880(const D2DDrawDetectEnv* e) {
    const uint32_t* info = e->systemInfo();
    if (!info) {
        e->fatal(e->fatalFmt, e->langString(0x6C));
        e->exitProcess(-1);
        return 0;
    }
    if (info[1] == 4) *e->windowed = 1;
    char key[24];
    memcpy(key, "Diablo II\\VideoConfig", 22);
    if (e->loadString(key, e->nameValue, 0, e->name, 0x100) && e->loadValue(key, e->flagsValue, 0, e->flags)) {
        if (e->bitMasks[0] & *e->flags) return 1;
        uint32_t got = 0x10;
        if (e->loadData(key, e->guidValue, 0, e->guid, 0x10, &got) && got == 0x10) return 1;
    }
    memcpy(e->name, "Fallback DirectDraw Device", 27);
    *e->flags = e->bitMasks[0];
    return 1;
}
