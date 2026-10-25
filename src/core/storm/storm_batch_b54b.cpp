// src/core/storm/storm_batch_b54b.cpp -- 1.13c Storm.dll functions transcribed from the cached decompiles.
// Implicit register arguments (ESI/EBX/EAX) become explicit trailing/leading parameters. Win32 and
// unmapped callees are seams with retail argument order. Batch lane: marker only.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "storm_state.h"

namespace {
uint32_t g_lastError;        // 0x6FC45388
int32_t g_errReport;         // 0x6FC44D58
int32_t g_debugFill;         // 0x6FC45164
int32_t g_listHead;          // 0x6FC459F0
const char* const g_defaultStrings[1] = {nullptr};  // 0x6FC35CB0 table

void Storm_SetLastError(uint32_t);
void Storm_Sub1F590(uint32_t, uint32_t*, int32_t, uint8_t*, int32_t, int32_t);
void Storm_Sub1ABF0();
void Storm_Sub1A9E0(int32_t, uint32_t, uint32_t);
int32_t Storm_Sub1AA70(uint16_t* hdr, uint32_t oldSize, uint32_t newSize);
uint32_t* Storm_Sub1C390(int32_t, uint32_t);
void Storm_Memcpy12C80(void*, const void*, uint32_t);
int32_t Storm_InterlockedIncrement(int32_t*);
int32_t Storm_InterlockedDecrement(int32_t*);
void Storm_InitCriticalSection();
void Storm_EnterCriticalSection();
int32_t Storm_LoadString(void* mod, uint32_t id, char* buf, int32_t n);
void Storm_Sub3AE00(char* dst, const char* src, uint32_t n);
void Storm_Sub3B080(char* dst, const char* src, uint32_t n);
uint32_t Storm_RegCreateKeyEx(uintptr_t root, const char* sub, void** outKey);
uint32_t Storm_RegSetValueEx(void* key, const char* name, uint32_t type, const uint8_t* data, uint32_t n);
uint32_t Storm_RegFlushKey(void* key);
uint32_t Storm_RegCloseKey(void* key);
uint32_t Storm_RegOpenKeyEx(uintptr_t root, const char* sub, void** outKey);
uint32_t Storm_RegDeleteValue(void* key, const char* name);
}  // namespace

// 1.13c Storm+0x6851: strtoul via the CRT worker (unsigned flag).
unsigned long Storm_strtoul_6851(const char* s, char** end, int radix) {
    // retail's 32-bit worker clamps to 0xFFFFFFFF past 32 bits (host strtoul is 64-bit here)
    const char* q = s; while (*q == ' ' || (*q >= 9 && *q <= 13)) ++q;
    bool neg = *q == '-';
    unsigned long long v = strtoull(s, end, radix);
    unsigned long long m = neg ? 0 - v : v;
    if (m > 0xFFFFFFFFull) return neg ? 1ul : 0xFFFFFFFFul;   // CRT: overflow -> ULONG_MAX, then negated
    return (uint32_t)v;
}

// 1.13c Storm+0x1ACF0: record the error code (ESI implicit) and report it when reporting is on.
void Storm_Sub1ACF0(uint32_t* p1, int32_t p2, uint32_t err) {
    g_lastError = err;
    Storm_SetLastError(err);
    if (g_errReport != 0) Storm_Sub1F590(err, p1, p2, nullptr, 1, 1);
}

// 1.13c Storm+0x1B170: debug-fill the freed block payload with 0xDD, then release.
void Storm_Sub1B170(uint32_t, uint16_t* hdr, uint32_t, uint32_t* p) {
    if (g_debugFill != 0 && (((uint8_t*)hdr)[3] & 4) == 0) {
        uint32_t n = (((uint32_t)hdr[0] + (((uint8_t*)hdr)[3] & 1) * -2) - (uint8_t)hdr[1]) - 8;
        for (uint32_t i = n >> 2; i; --i) *p++ = 0xDDDDDDDD;
        uint8_t* b = (uint8_t*)p;
        for (uint32_t i = n & 3; i; --i) *b++ = 0xDD;
    }
    Storm_Sub1ABF0();
}

// 1.13c Storm+0x1C660: realloc a heap block (EAX = header).
uint32_t* Storm_Sub1C660(uint16_t* hdr, uint32_t a1, int32_t a2, uint32_t* oldp, uint32_t newSize, uint8_t flags) {
    (void)a1;
    uint32_t oldSize;
    uint8_t ext = ((uint8_t*)hdr)[3] & 4;
    uint32_t* np;
    if (ext == 0) {
        int32_t ov = (uint8_t)hdr[1] + 8;
        if (((uint8_t*)hdr)[3] & 1) ov = (uint8_t)hdr[1] + 10;
        oldSize = (uint32_t)hdr[0] - ov;
    } else {
        oldSize = oldp[-4];
    }
    bool done = false;
    if (ext == 0) {
        if (newSize < oldSize) {
            Storm_Sub1A9E0(a2, oldSize, newSize);
        } else if (newSize > oldSize && Storm_Sub1AA70(hdr, oldSize, newSize) == 0) {
            goto relocate;
        }
        np = oldp;
        if (np != nullptr) done = true;
    }
relocate:
    if (!done) {
        if (flags & 0x10) return nullptr;
        np = Storm_Sub1C390(0, newSize);
        if (np && oldSize && newSize) Storm_Memcpy12C80(np, oldp, oldSize < newSize ? oldSize : newSize);
        Storm_Sub1B170(a2, hdr, a2, oldp);
        if (!np) return nullptr;
    }
    if (oldSize < newSize) {
        uint32_t fill;
        if (flags & 8) fill = 0;
        else if (g_debugFill == 0) return np;
        else fill = 0xEEEEEEEE;
        uint32_t* q = (uint32_t*)((uint8_t*)np + oldSize);
        for (uint32_t i = (newSize - oldSize) >> 2; i; --i) *q++ = fill;
        uint8_t* b = (uint8_t*)q;
        for (uint32_t i = (newSize - oldSize) & 3; i; --i) *b++ = (uint8_t)fill;
    }
    return np;
}

// 1.13c Storm+0x22FA0: is `node` a member of the list headed at 0x6FC459EC (head pointer at +4).
uint32_t Storm_Sub22FA0(uint8_t* node) {
    int32_t head = g_listHead;
    uint8_t* p = (uint8_t*)(intptr_t)(head & ((head < 1) - 1));
    for (;;) {
        if ((intptr_t)p < 1) return 0;
        if (p == node) return 1;
        p = *(uint8_t**)(p + 4);
    }
}

// also Storm 0x2F180 / 0x2F1C0: real bodies in storm_serr.cpp.
void Storm_Sub2F180() { Storm_SErrLock_2F180(); }
const char* Storm_Sub2F1C0(int32_t idx) { return Storm_SErrString_2F1C0(idx); }

static void Storm_RegPath(char* path, uint32_t flags, const char* sub) {
    path[0] = 0;
    if ((flags & 0x10) == 0)
        Storm_Sub3B080(path, (flags & 2) ? "Software\\Battle.net\\" : "Software\\Blizzard Entertainment\\", 0x104);
    Storm_Sub3AE00(path, sub, 0x104);
}

// 1.13c Storm+0x3BCB0: write a registry value (EBX = flags).
uint32_t Storm_Sub3BCB0(const char* sub, const char* name, uint32_t type, const uint8_t* data, uint32_t n, uint32_t flags) {
    char path[0x104];
    void* key;
    Storm_RegPath(path, flags, sub);
    uint32_t err = Storm_RegCreateKeyEx(((flags & 4) != 0) + 0x80000001u, path, &key);
    if (err != 0) { Storm_SetLastError(err); return 0; }
    err = Storm_RegSetValueEx(key, name, type, data, n);
    if (err == 0) {
        if ((flags & 8) != 0) {
            err = Storm_RegFlushKey(key);
            if (err != 0) goto fail;
        }
        err = Storm_RegCloseKey(key);
        if (err == 0) return 1;
    }
fail:
    Storm_RegCloseKey(key);
    Storm_SetLastError(err);
    return 0;
}

// 1.13c Storm+0x3BF30: delete a registry value from HKCU and/or HKLM (EAX = flags).
int32_t Storm_Sub3BF30(const char* sub, const char* name, uint32_t flags) {
    char path[0x104];
    void* key;
    uint32_t err = 0, e1 = 0, r;
    int32_t ok = 0;
    Storm_RegPath(path, flags, sub);
    if ((flags & 5) != 4) {
        r = Storm_RegOpenKeyEx(0x80000001u, path, &key);
        if (r == 0) {
            r = Storm_RegDeleteValue(key, name);
            if (r != 0) e1 = r;
            r = Storm_RegCloseKey(key);
            if (r != 0) e1 = r;
        } else {
            e1 = r;
        }
        if (e1 == 0) ok = 1;
    }
    if ((flags & 5) != 1) {
        r = Storm_RegOpenKeyEx(0x80000002u, path, &key);
        if (r == 0) {
            r = Storm_RegDeleteValue(key, name);
            if (r != 0) err = r;
            r = Storm_RegCloseKey(key);
            if (r != 0) err = r;
        } else {
            err = r;
        }
        if (err == 0) return 1;
    }
    if (ok == 0) {
        if (e1 != 0) err = e1;
        Storm_SetLastError(err);
    }
    return ok;
}
