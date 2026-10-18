// src/core/storm/storm_batch_109c0.cpp -- 8 1.13c Storm.dll SNet rows transcribed from the cached
// decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c) and checked against the retail disassembly
// (retail/D2/D2SE/CORES/1.13c/Storm.dll). Stage A batch lane (OBJECTIVE v8.3): implementation plus
// marker only, no spec, no vector.
//
// These are the SNet connection/provider-info family. Register-only arguments (Ghidra in_EAX,
// unaff_ESI/EDI/EBX, ECX) are explicit parameters, as storm_batch_alpha.cpp does; the retail
// register each one came from is noted on the function. The module state is modelled in this file
// only (the other Storm batch files keep their own copy), and the provider table at 0x6FC45834 is
// never initialised in ours -- StormLib replaces Storm and SNetInitializeProvider is never called
// -- so the provider-callback paths are guarded and the deterministic outcome is retail's own
// "provider not initialised" error branch.
//
// Ghidra artifacts resolved against the disassembly:
//   * 0x3B080 is a bounded strcpy that returns strlen(src), not an end pointer (the decompile types
//     the result as a pointer); 0x109C0 adds it to the destination, as the LEA does;
//   * 0x10AA0 writes its 12-byte info block through EDI and copies block[0] to the stack argument;
//   * 0x10D20 takes (array, data, length) on the stack plus the record count in EBX;
//   * the two provider callbacks are __stdcall (the retail body does not clean the pushed args).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops, deterministic GetTickCount

namespace {

// ---- retail module state (globals keep their retail addresses in the comments) -----------------
uint32_t g_lastError;             // 0x6FC45388  last Storm error, mirrored to SetLastError
CRITICAL_SECTION g_lock;          // 0x6FC45A24  guards the archive/provider list
uintptr_t* g_smack;               // 0x6FC45834  SNet provider table; never initialised in ours
uint8_t* g_listHead;              // 0x6FC45A6C  archive list head, next at +0x04, index at +0x212
uint8_t* g_list0_3c;              // 0x6FC45A3C+8  first connection list head
uint8_t* g_list0_64;              // 0x6FC45A64+8  second connection list head
uint8_t g_41b4c;                  // 0x6FC41B4C  0xFF = archive set not available
uint32_t g_45170;                 // 0x6FC45170  provider-info word (copied to info[2])
uint8_t g_45240[0x100];           // 0x6FC45240  provider name
uint8_t g_452c0[0x100];           // 0x6FC452C0  provider version
uint8_t g_451b8[0x100];           // 0x6FC451B8  provider host
uint32_t g_45344;                 // 0x6FC45344  connection flags
uint32_t g_45174;                 // 0x6FC45174  provider-info word
uint32_t g_45354;                 // 0x6FC45354  provider-info word
uint32_t g_45238;                 // 0x6FC45238  provider-info word
uint32_t g_45348;                 // 0x6FC45348  provider-info start tick
uint32_t g_4583c;                 // 0x6FC4583C  send pacing timestamp
uint8_t* g_45848;                 // 0x6FC45848  packet staging buffer (0x2000, VirtualAlloc)
uint8_t* g_archive0;              // 0x6FC45840  first archive object; +0x228 = pacing divisor
uint8_t* g_45838;                 // 0x6FC45838  provider module handle
uint32_t g_41ad8;                 // 0x6FC41AD8  bytes sent
intptr_t g_45a50;                 // 0x6FC45A50  provider-info list head (checked > 0)
int32_t g_45844;                  // 0x6FC45844  loop flag cleared by 0x11040

// ---- deterministic Win32/kernel32 seams -------------------------------------------------------
void StormSetLastError(uint32_t e) { g_lastError = e; }   // SetLastError is not in compat/Windows.h
void StormSeamFreeLibrary(void*) {}                        // 0x6FC331D0; module never loaded in ours
void StormSeamVirtualUnlock(void*, uint32_t) {}            // 0x6FC33108
void StormSeamVirtualFree(void*, uint32_t, uint32_t) {}    // 0x6FC3323C

// ---- unmapped internal callees (never reached: the provider/list state is always empty) -------
void StormSeamB960(void*, const char*, int32_t, int32_t) {}  // 0xB960 SMemFree / operator delete
void StormSeam4D60(uint8_t*) {}                              // 0x4D60 list release (0x4B80)

// 0x3B080: bounded strcpy; returns strlen(src) (bytes before the terminator), 0 on the retail
// error/none path. The two sources used here are fixed 0x80-byte NUL-terminated buffers.
int32_t StormStrCpy3B080(uint8_t* dst, const uint8_t* src, int32_t max) {
    if (dst == nullptr || src == nullptr || max == 0) return 0;
    for (int32_t i = 0; i < max; ++i) {
        const uint8_t c = src[i];
        dst[i] = c;
        if (c == 0) return i;
    }
    dst[max - 1] = 0;
    return max - 1;
}

// 0xFF40: Internet checksum. The first (register) argument is ignored by the retail body.
uint16_t StormChecksumFF40(uint32_t /*seed*/, int32_t len, const uint8_t* buf) {
    uint32_t accum = 0;   // running value, folded at 0xFF
    uint32_t total = 0;   // sum of the running values
    const uint8_t* p = buf + len - 1;
    for (; len != 0; --len) {
        accum += *p--;
        if (accum > 0xFE) accum -= 0xFF;
        total += accum;
    }
    return (uint16_t)(((accum & 0xFF) << 8) | (total % 0xFF));
}

uint16_t rd16(const uint8_t* p, uintptr_t off) {
    uint16_t v;
    std::memcpy(&v, p + off, 2);
    return v;
}
uint32_t rd32(const uint8_t* p, uintptr_t off) {
    uint32_t v;
    std::memcpy(&v, p + off, 4);
    return v;
}
void wr16(uint8_t* p, uintptr_t off, uint16_t v) { std::memcpy(p + off, &v, 2); }
void wr32(uint8_t* p, uintptr_t off, uint32_t v) { std::memcpy(p + off, &v, 4); }

// 0x10C0 provider table slots: [0x08] destructor, [0x38] send, [0x40] info.
typedef void (*StormProviderInfoCb)(const void*, const void*, const void*, uint32_t, uint32_t,
                                    uint32_t, uint32_t, uint32_t, void*, uint32_t);
typedef uint32_t (*StormProviderSendCb)(uint32_t, const void*, void*, uint32_t);
typedef void (*StormProviderDtor)(void*);

}  // namespace

// 1.13c Storm+0x109C0: build the archive-info block from a record (EAX) into out (ESI).
// Header at +0x00 is the total byte count; +0x24 and the byte after it are two 0x80-byte strings
// (record +0x08 and +0x88). param_1 is the one stack argument, stored at +0x10.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub109C0(uint8_t* out, const uint8_t* rec, int32_t param_1) {  // 1.13c Storm+0x109C0
    wr32(out, 0x04, rec[0x212]);
    wr32(out, 0x08, rd32(rec, 0x200));
    wr32(out, 0x0C, rd32(rec, 0x118));
    wr32(out, 0x10, (uint32_t)param_1);
    wr32(out, 0x14, rd32(rec, 0x108));
    wr32(out, 0x18, rd32(rec, 0x10C));
    wr32(out, 0x1C, rd32(rec, 0x110));
    wr32(out, 0x20, rd32(rec, 0x114));
    const int32_t len1 = StormStrCpy3B080(out + 0x24, rec + 0x08, 0x80);
    const int32_t len2 = StormStrCpy3B080(out + 0x24 + len1 + 1, rec + 0x88, 0x80);
    const uint32_t total = 0x26u + (uint32_t)len1 + (uint32_t)len2;
    wr32(out, 0x00, total);
    return total;
}

// 1.13c Storm+0x10AA0: provider-info query. info (EDI) receives {size=0xC, count, word}; the stack
// argument receives info[0]. count starts at 1 and counts archive nodes whose index is not 0xFF.
extern "C" EMSCRIPTEN_KEEPALIVE
void Storm_Sub10AA0(uint32_t* info, uint32_t* out) {  // 1.13c Storm+0x10AA0
    info[0] = 0xC;
    EnterCriticalSection(&g_lock);
    info[1] = 0;
    if (g_smack == nullptr) {
        g_lastError = 0x4B4;
        StormSetLastError(0x4B4);
    } else if (g_41b4c == 0xFF) {
        g_lastError = 0x85100070;
        StormSetLastError(0x85100070);
    } else {
        info[1] = 1;
        intptr_t node = (intptr_t)g_listHead;
        while (node > 0) {
            const uint8_t* n = (const uint8_t*)node;
            if (n[0x212] != 0xFF) ++info[1];
            node = (intptr_t)(uintptr_t)rd32(n, 0x04);
        }
    }
    LeaveCriticalSection(&g_lock);
    info[2] = g_45170;
    *out = info[0];
}

// 1.13c Storm+0x10B50: checksum a packet's payload. EAX = packet, ECX = seed (ignored by 0xFF40).
// Length is the packet's own word at +0x02 minus 2; payload starts at +0x02.
extern "C" EMSCRIPTEN_KEEPALIVE
uint16_t Storm_Sub10B50(const uint8_t* pkt, uint32_t seed) {  // 1.13c Storm+0x10B50
    const uint16_t ck = StormChecksumFF40(seed, (int32_t)rd16(pkt, 0x02) - 2, pkt + 0x02);
    const uint32_t lo = ck & 0xFF;
    const uint32_t hi = ck >> 8;
    const uint8_t b1 = (uint8_t)(0xFFu - ((lo + hi) % 0xFFu));
    const uint8_t b2 = (uint8_t)(0xFFu - (((uint32_t)b1 + hi) % 0xFFu));
    return (uint16_t)(((uint32_t)b1 << 8) | b2);
}

// 1.13c Storm+0x10CB0: pass the provider-info block and the elapsed whole seconds to the provider's
// info callback (+0x40). The provider table is never initialised in ours, so the call is guarded.
extern "C" EMSCRIPTEN_KEEPALIVE
void Storm_Sub10CB0(void) {  // 1.13c Storm+0x10CB0
    uint32_t info[0x40];   // retail local_100[256]
    uint32_t size;         // retail local_104
    Storm_Sub10AA0(info, &size);
    const uint32_t seconds = (GetTickCount() - g_45348) / 1000u;
    if (g_smack == nullptr) return;
    reinterpret_cast<StormProviderInfoCb>(g_smack[0x40 / sizeof(uintptr_t)])(
        g_45240, g_452c0, g_451b8, g_45344, seconds, g_45174, g_45354, g_45238,
        (void*)info, size);
}

// 1.13c Storm+0x10D20: stage a send. parts (stack arg 1) is an array of 16-byte record pointers,
// data (arg 2) is copied at +0x140, length is arg 3, and the record count arrives in EBX.
// Returns 0 before staging when the provider is absent or the count/length is out of range.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub10D20(const void* const* parts, const uint8_t* data, uint32_t length, uint32_t count) {  // 1.13c Storm+0x10D20
    if (g_smack == nullptr) return 0;
    const uint32_t now = GetTickCount();
    uint32_t stamp = g_4583c;
    if (now - stamp < 0x7FFFFFFFu) stamp = now;
    if (g_archive0 == nullptr) return 0;   // retail divides by archive0+0x228 and would fault
    const uint32_t divisor = rd32(g_archive0, 0x228);
    if (divisor == 0) return 0;            // retail would fault here too
    uint32_t scaled = (length + 0x40u) * count;
    scaled *= 1000u;
    g_4583c = stamp + scaled / divisor;
    uint8_t* base = g_45848;
    if (base != nullptr) {
        if (count > 0x10 || length > 0x1F00) return 0;
        // The staging buffer is a retail 32-bit structure: its pointer slots are 4 bytes wide.
        // It is only consumed by the provider callback, which ours never has.
        for (uint32_t i = 0; i < count; ++i) {
            wr32(base, i * 4, (uint32_t)(uintptr_t)(base + 0x40 + i * 0x10));
        }
        for (uint32_t i = 0; i < count; ++i) {
            std::memcpy(base + 0x40 + i * 0x10, parts[i], 0x10);
        }
        std::memcpy(base + 0x140, data, length);
    }
    g_41ad8 += length;
    return reinterpret_cast<StormProviderSendCb>(g_smack[0x38 / sizeof(uintptr_t)])(
        count, data, base != nullptr ? base + 0x140 : (uint8_t*)data, length);
}

// 1.13c Storm+0x10E30: queue one packet on a connection. conn (EDI) +0x108 is the 16-byte record,
// packet (ESI) supplies the length at +0x02, the slot index at +0x08 and the word at +0x06.
extern "C" EMSCRIPTEN_KEEPALIVE
void Storm_Sub10E30(uint8_t* conn, const uint8_t* pkt) {  // 1.13c Storm+0x10E30
    const void* part = conn + 0x108;
    Storm_Sub10D20(&part, pkt, rd16(pkt, 0x02), 1);
    const uint32_t index = pkt[0x08];
    wr16(conn, 0x1E6 + index * 2, rd16(pkt, 0x06));
    wr32(conn, 0x1F0 + index * 4, 0);
}

// 1.13c Storm+0x10FC0: set the provider index on a record (EAX) and clear the matching index on
// every node of both connection lists whose +0x213 byte equals it (ECX = provider char).
extern "C" EMSCRIPTEN_KEEPALIVE
void Storm_Sub10FC0(uint8_t* rec, char provider) {  // 1.13c Storm+0x10FC0
    rec[0x213] = 0xFF;
    rec[0x212] = (uint8_t)provider;
    for (int32_t which = 1; which >= 0; --which) {
        uint8_t* head = (which != 0) ? g_list0_3c : g_list0_64;
        intptr_t node = (intptr_t)head;
        while (node > 0) {
            uint8_t* n = (uint8_t*)node;
            if (n[0x213] == (uint8_t)provider) n[0x213] = 0xFF;
            node = (intptr_t)(uintptr_t)rd32(n, 0x04);
        }
    }
}

// 1.13c Storm+0x11040: tear down the provider. Everything below is guarded by state that is never
// set in ours (the module, provider table, packet buffer and provider-info list are all empty).
extern "C" EMSCRIPTEN_KEEPALIVE
void Storm_Sub11040(int32_t param_1) {  // 1.13c Storm+0x11040
    if (g_45838 != nullptr) {
        if (g_smack != nullptr) {
            reinterpret_cast<StormProviderDtor>(g_smack[0x08 / sizeof(uintptr_t)])(g_smack);
        }
        if (g_45838 != nullptr) {
            StormSeamFreeLibrary(g_45838);
            g_45838 = nullptr;
        }
    }
    if (g_smack != nullptr) {
        StormSeamB960(g_smack, "delete", -1, 0);
        g_smack = nullptr;
    }
    if (g_45848 != nullptr) {
        StormSeamVirtualUnlock(g_45848, 0x2000);
        StormSeamVirtualFree(g_45848, 0, 0x8000);
        g_45848 = nullptr;
    }
    if (param_1 != 0) {
        while (g_45a50 > 0) {
            uint8_t* node = (uint8_t*)(uintptr_t)g_45a50;
            StormSeam4D60(node);
            StormSeamB960(node, ".?AUPROVIDERINFO@@", -2, 0);
        }
        g_45844 = 0;
    }
}
