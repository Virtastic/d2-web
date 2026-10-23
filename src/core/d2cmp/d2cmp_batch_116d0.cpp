// src/core/d2cmp/d2cmp_batch_116d0.cpp -- twelve 1.13c D2CMP.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2cmp/decomp/0x<RVA>.c) and checked against the retail bytes
// (retail/D2/D2SE/CORES/1.13c/D2CMP.dll, image base 0x6FE10000). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// The fatal path all these share is Fog Ordinal 10265(id) -> Ordinal 10024(&0x6FE2D30F) -> _exit(-1);
// only the exit is observable and is d2cmp_fatal. The internal callees the decompile still names
// (0x9720, 0x1BC00, Fog #10043 FOG_Free, Storm #423 SRegLoadValue) are seams on
// D2CMPBatch116d0Env, passed after the retail arguments, which keep their order.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2CMP (export/ordinal map only; the 1.10f modules
// are stubs), and D2MOO's D2TileLibraryEntryStrc layout for the entry getters.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops under wasm

static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

[[noreturn]] static void d2cmp_fatal(uint32_t) { _exit(-1); }

// "..\\Source\\D2CMP\\SRC\\Tilecmp.cpp", the file literal every FOG_Free call in 0x16600 passes.
static const char kTilecmpCpp[] = "..\\Source\\D2CMP\\SRC\\Tilecmp.cpp";

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2CMPBatch116d0Env {
    void (*setSlot)(void** slot, void* value);                                  // 0x9720, stores value in a free slot
    void (*prepare)(void* this_, void* record);                                 // 0x1BC00, ECX = this
    int32_t (*regLoadValue)(const char* key, const char* value, int32_t flags,
                            uint32_t* out);                                     // 0x8D72, Storm #423 SRegLoadValue
    void (*freeMemory)(void* p, const char* file, int32_t line, int32_t n0);    // Fog #10043 FOG_Free (ECX=p, EDX=file)
};

// 1.13c D2CMP+0x116D0 (export #10002, 45 bytes): return the dword at obj+0x0C.
// Object getter in the Cel/Tilecmp family; the decompile shows the load at 0xC and nothing else.
// 1.13c D2CMP+0x116D0
extern "C"
uint32_t d2cmp_116d0(const void* obj) {
    if (obj == nullptr) d2cmp_fatal(0x597);
    return rd32(obj, 0x0C);
}

// 1.13c D2CMP+0x151E0 (export #10011, 45 bytes): the tile entry's +0x28 byte array (D2MOO
// D2TileLibraryEntryStrc dwTileFlags[4] / GetTileFlagArray). Returns the address, not the contents.
// 1.13c D2CMP+0x151E0
extern "C"
void* d2cmp_151e0(void* obj) {
    if (obj == nullptr) d2cmp_fatal(0x81);
    return (uint8_t*)obj + 0x28;
}

// 1.13c D2CMP+0x15480 (export #10007, 112 bytes): put a value in the first empty slot of a 32-dword
// array through 0x9720. Null array is fatal 0x1C, null value fatal 0x1D, a full array fatal 0x2A.
// Retail's post-call `if (i < 0x20) return` is dead (the search never reaches 0x20 with a hit).
// 1.13c D2CMP+0x15480
extern "C"
void d2cmp_15480(void* slots, void* value, const D2CMPBatch116d0Env* env) {
    if (slots == nullptr) d2cmp_fatal(0x1C);
    if (value == nullptr) d2cmp_fatal(0x1D);
    for (int32_t i = 0; i < 0x20; ++i) {
        if (rd32(slots, (uint32_t)i * 4) == 0) {
            env->setSlot((void**)((uint8_t*)slots + (size_t)i * 4), value);
            return;
        }
    }
    d2cmp_fatal(0x2A);
}

// 1.13c D2CMP+0x16600 (export #10008, 329 bytes): clear and release a tile record's block arrays.
// thiscall (ECX = this), two stack arguments: the record and a flag that must be zero to proceed.
// The record's +0x10C count and +0x110 array of 0x60-byte rows are walked; each row's +0x50 count
// and +0x54 array of 0x14-byte blocks are cleared (block +0xA word, +0x10 dword), any nonzero
// source pointers handed to FOG_Free at the retail call sites (0x274, 0x27B, 0x281, 0x285), then
// the rows and the arrays are zeroed. Flags & 4 frees just the record and returns; flags & 8 is
// fatal 0x267. Null record fatal 0x25A.
// 1.13c D2CMP+0x16600
extern "C"
void d2cmp_16600(void* this_, void* record, int32_t flag, const D2CMPBatch116d0Env* env) {
    if (flag != 0) return;
    if (record == nullptr) d2cmp_fatal(0x25A);
    env->prepare(this_, record);
    const uint32_t recordFlags = rd32(record, 0x04);
    if (recordFlags & 0x04) {
        env->freeMemory(record, kTilecmpCpp, 0x262, 0);
        return;
    }
    if (recordFlags & 0x08) d2cmp_fatal(0x267);
    const int32_t rowCount = (int32_t)rd32(record, 0x10C);
    int32_t rowOffset = 0;
    for (int32_t r = 0; r < rowCount; ++r) {
        uint8_t* row = (uint8_t*)(uintptr_t)rd32(record, 0x110) + rowOffset;
        int32_t j = 0;
        int32_t blockOffset = 0;
        while (j < (int32_t)rd32(row, 0x50)) {
            uint32_t block = rd32(row, 0x54) + (uint32_t)blockOffset;
            const uint32_t blockPtr = rd32((void*)(uintptr_t)block, 0x10);
            if (blockPtr != 0) env->freeMemory((void*)(uintptr_t)blockPtr, kTilecmpCpp, 0x274, 0);
            wr16((void*)(uintptr_t)block, 0x0A, 0);
            wr32((void*)(uintptr_t)block, 0x10, 0);
            ++j;
            blockOffset += 0x14;
        }
        const uint32_t blocks = rd32(row, 0x54);
        if (blocks != 0) env->freeMemory((void*)(uintptr_t)blocks, kTilecmpCpp, 0x27B, 0);
        wr32(row, 0x54, 0);
        wr32(row, 0x50, 0);
        rowOffset += 0x60;
    }
    const uint32_t rows = rd32(record, 0x110);
    env->freeMemory((void*)(uintptr_t)rows, kTilecmpCpp, 0x281, 0);
    wr32(record, 0x110, 0);
    wr32(record, 0x10C, 0);
    env->freeMemory(record, kTilecmpCpp, 0x285, 0);
}

// Retail module state: these dwords live in .data (0x6FE32CC0, 0x6FE32CC8) whose file image is only
// 0x3000 bytes, so both come up as the file's initialiser says: 1.
static uint32_t g_d2cmp_32cc0 = 1;   // 0x6FE32CC0: draw/compression mode, 0x14F00 clamps it to 1..5
static uint32_t g_d2cmp_32cc8 = 1;   // 0x6FE32CC8: "CompressedData" registry value, 0xC9A0 refreshes it

// 1.13c D2CMP+0xC9A0 (export #10019, 43 bytes): read "CompressedData" from the "Diablo II" registry
// section (Storm SRegLoadValue, default flag 1) into 0x6FE32CC8. The one stack argument is checked
// after the call: when it is zero the value is cleared instead (the decompile's `pcVar1` check is
// Ghidra folding the string argument into the return register).
// 1.13c D2CMP+0xC9A0
extern "C"
void d2cmp_c9a0(int32_t flag, const D2CMPBatch116d0Env* env) {
    env->regLoadValue("Diablo II", "CompressedData", 1, &g_d2cmp_32cc8);
    if (flag == 0) g_d2cmp_32cc8 = 0;
}

// 1.13c D2CMP+0x15360 (export #10022, 42 bytes): return the dword at obj+0x14; null is fatal 0x79.
// 1.13c D2CMP+0x15360
extern "C"
uint32_t d2cmp_15360(const void* obj) {
    if (obj == nullptr) d2cmp_fatal(0x79);
    return rd32(obj, 0x14);
}

// 1.13c D2CMP+0x14F00 (export #10024, 21 bytes): store the mode at 0x6FE32CC0 when it is in 1..5;
// any other value is ignored (including the initial 1).
// 1.13c D2CMP+0x14F00
extern "C"
void d2cmp_14f00(int32_t mode) {
    if (mode > 0 && mode < 6) g_d2cmp_32cc0 = (uint32_t)mode;
}

// Palette reduction tables, zero in the file image and filled by D2CMP's own setup (not a row here).
static int32_t  g_d2cmp_dist_6fe879f0[511];     // 0x6FE879F0 is table entry 255, the zero offset
static uint8_t  g_d2cmp_excl_6fe87df0[256];     // 0x6FE87DF0: nonzero = entry participates
static uint8_t  g_d2cmp_pal_6fe87f18[256 * 4];  // 0x6FE87F18: B, G, R, X per palette entry
static uint32_t g_d2cmp_palStart_6fe87ef0;      // 0x6FE87EF0: first entry to scan
static uint32_t g_d2cmp_palEnd_6fe875ec;        // 0x6FE875EC: last entry to scan (inclusive)
static uint32_t g_d2cmp_skipSame_6fe875e8;      // 0x6FE875E8: nonzero = reject a repeat of lastColor
static uint32_t g_d2cmp_lastColor_6fe8831c;     // 0x6FE8831C: 0xRRGGBB of the previous result
static uint32_t g_d2cmp_useExcl_6fe88324;       // 0x6FE88324: nonzero = honour the exclusion table

// 1.13c D2CMP+0x9A10 (export #10026, 215 bytes): nearest palette entry for an (R,G,B) triple.
// Scans palStart..palEnd inclusive; with the exclusion table in force an entry with a zero exclusion
// byte is skipped, and with skipSameColor set a repeat of 0x6FE8831C returns 0. The distance is the
// sum of the three per-channel entries of the centred 511-entry table (index 255 + difference), kept
// in unsigned arithmetic and compared strictly less than. Returns the lowest byte of the best index,
// or 0 when the range is empty.
// 1.13c D2CMP+0x09A10
extern "C"
uint32_t d2cmp_9a10(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t bestDistance = 0x7FFFFFFFu;
    uint32_t bestIndex = 0;
    uint32_t i = g_d2cmp_palStart_6fe87ef0;
    if (g_d2cmp_palEnd_6fe875ec < i) return 0;
    do {
        if (g_d2cmp_useExcl_6fe88324 == 0 || g_d2cmp_excl_6fe87df0[i] != 0) {
            if (g_d2cmp_skipSame_6fe875e8 != 0 &&
                g_d2cmp_lastColor_6fe8831c == (((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b)) {
                return 0;
            }
            const uint8_t* entry = &g_d2cmp_pal_6fe87f18[i * 4];
            uint32_t distance = (uint32_t)g_d2cmp_dist_6fe879f0[255 + ((int32_t)r - (int32_t)entry[2])]
                              + (uint32_t)g_d2cmp_dist_6fe879f0[255 + ((int32_t)g - (int32_t)entry[1])]
                              + (uint32_t)g_d2cmp_dist_6fe879f0[255 + ((int32_t)b - (int32_t)entry[0])];
            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndex = i;
            }
        }
        ++i;
        if (g_d2cmp_palEnd_6fe875ec < i) return bestIndex & 0xFF;
    } while (true);
}


// Retail module state for 0xFC20 and 0xC280, all in .data's zero-filled tail.
static int32_t g_d2cmp_edgeLo_6fe6fbc0[80];      // 0x6FE6FBC0: span start offsets, 0..39 then 79..40
static int32_t g_d2cmp_edgeHi_6fe6fd00[80];      // 0x6FE6FD00: span end offsets, same order
static int32_t g_d2cmp_offsets_6fe6fe40[0x1900]; // 0x6FE6FE40: per-pixel source offsets, 0x50 per row
static CRITICAL_SECTION* g_d2cmp_cs_6fe77270;    // 0x6FE77270: lock guarding the value below
static uint32_t g_d2cmp_value_6fe77248;          // 0x6FE77248: value 0xC280 reads under the lock

// One span of the 0xFC20 edge pass: forward from edgeLo to edgeHi (capped at 0x29) and backward from
// edgeHi to edgeLo (floored at 0x28), each copying through the offset row that starts at the span's
// own table entry. offRow points at g_offsets[blockBase + lo].
static void d2cmp_fc20_span(uint8_t* row, int32_t lo, int32_t hi, const int32_t* offRow) {
    uint8_t* p = row + lo;
    int32_t forwardEnd = hi > 0x28 ? 0x29 : hi;
    uint8_t* end = row + forwardEnd;
    if (p < end) {
        const int32_t* off = offRow;
        do {
            *p = p[*off];
            ++p;
            ++off;
        } while (p < end);
    }
    int32_t backwardStop = lo < 0x28 ? 0x28 : lo;
    p = row + hi;
    uint8_t* stop = row + backwardStop;
    if (stop < p) {
        const int32_t* off = offRow + (hi - lo);
        do {
            *p = p[*off];
            --p;
            --off;
        } while (stop < p);
    }
}

// 1.13c D2CMP+0xFC20 (export #10031, 326 bytes): complete the 40-pixel borders of an 80x80 block.
// image is the block base, stride the row pitch. The first pass walks rows 0..39 with the lo/hi edge
// tables at entries 0..39 and offset rows 0, 0x50, ...; the second walks rows 79..40 (the mirrored
// half) with edge entries 79..40 and offset rows 0x18B0, 0x1860, ... Each row fills forward from
// edgeLo to edgeHi (capped 0x29) and backward from edgeHi to edgeLo (floored 0x28).
// 1.13c D2CMP+0xFC20
extern "C"
void d2cmp_fc20(uint8_t* image, int32_t stride) {
    uint8_t* row = image;
    for (int32_t j = 0; j < 40; ++j) {
        const int32_t lo = g_d2cmp_edgeLo_6fe6fbc0[j];
        const int32_t hi = g_d2cmp_edgeHi_6fe6fd00[j];
        d2cmp_fc20_span(row, lo, hi, &g_d2cmp_offsets_6fe6fe40[lo + 0x50 * j]);
        row += stride;
    }
    row = image + 0x4F * (ptrdiff_t)stride;
    for (int32_t j = 0; j < 40; ++j) {
        const int32_t t = 79 - j;
        const int32_t lo = g_d2cmp_edgeLo_6fe6fbc0[t];
        const int32_t hi = g_d2cmp_edgeHi_6fe6fd00[t];
        d2cmp_fc20_span(row, lo, hi, &g_d2cmp_offsets_6fe6fe40[lo + 0x18B0 - 0x50 * j]);
        row -= stride;
    }
}

// 1.13c D2CMP+0xC280 (export #10032, 70 bytes): return 0x6FE77248 under the 0x6FE77270 critical
// section; a null section is fatal 0x7F5. The section itself is a host no-op under wasm.
// 1.13c D2CMP+0xC280
extern "C"
uint32_t d2cmp_c280(void) {
    if (g_d2cmp_cs_6fe77270 == nullptr) d2cmp_fatal(0x7F5);
    EnterCriticalSection(g_d2cmp_cs_6fe77270);
    const uint32_t value = g_d2cmp_value_6fe77248;
    LeaveCriticalSection(g_d2cmp_cs_6fe77270);
    return value;
}

// 1.13c D2CMP+0x15270 (export #10036, 42 bytes): return the dword at obj+0x1C; null is fatal 0x7E.
// 1.13c D2CMP+0x15270
extern "C"
uint32_t d2cmp_15270(const void* obj) {
    if (obj == nullptr) d2cmp_fatal(0x7E);
    return rd32(obj, 0x1C);
}

// 1.13c D2CMP+0x15330 (export #10045, 42 bytes): return the dword at obj+0x18; null is fatal 0x7A.
// 1.13c D2CMP+0x15330
extern "C"
uint32_t d2cmp_15330(const void* obj) {
    if (obj == nullptr) d2cmp_fatal(0x7A);
    return rd32(obj, 0x18);
}

// 1.13c D2CMP+0x15660 (198 bytes): relocate a loaded tile blob in place. EAX = name, ECX = blob, arg = out.
// Version dword must be 7 (fatal 0x37F). If flag 2 at +4: +0x110 += blob, name copied to +8, each 0x60-byte
// row's +0x58 = blob+8 (its +0x54 and +0x5C must be 0, fatal 0x3A2/0x3A3), flags = (flags & ~2) | 4, *out = blob.
// 1.13c D2CMP+0x15660
extern "C"
void d2cmp_15660(const char* name, uint8_t* blob, void** out) {
    if (*(int32_t*)blob != 7) d2cmp_fatal(0x37F);
    if (!(blob[4] & 2)) return;
    uint8_t* rows = blob + *(int32_t*)(blob + 0x110);
    *(uint8_t**)(blob + 0x110) = rows;
    strcpy((char*)blob + 8, name);
    for (int32_t i = 0; i < *(int32_t*)(blob + 0x10C); ++i) {
        uint8_t* r = rows + (size_t)i * 0x60;
        *(uint8_t**)(r + 0x58) = blob + 8;
        if (*(int32_t*)(r + 0x54)) d2cmp_fatal(0x3A2);
        if (*(int32_t*)(r + 0x5C)) d2cmp_fatal(0x3A3);
    }
    *(uint32_t*)(blob + 4) = (*(uint32_t*)(blob + 4) & ~2u) | 4;
    *out = blob;
}
