// src/core/storm/storm_batch_44.cpp -- Storm+0x17B30, 0x11760, 0x13930, 0x116A0, 0x16D70
// from build-wasm/ghidra-storm/decomp/0x<RVA>.c. Globals keep retail addresses in comments; unmapped
// callees are seams with retail argument order. ponytail: native pointers, not 32-bit retail layout.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <Windows.h>

namespace {
int32_t g_lastError;          // 0x6FC45388
CRITICAL_SECTION g_lock;      // 0x6FC45A24
uint8_t* g_snet;              // 0x6FC45834  provider vtable object, +0x54 callback
uintptr_t g_45238;            // 0x6FC45238
uint32_t g_45a58;             // 0x6FC45A58  slot count
uint8_t* g_45a5c;             // 0x6FC45A5C  slot array base (0x80 each)
int8_t g_41b4c;               // 0x6FC41B4C
int32_t g_45830;              // 0x6FC45830
intptr_t g_45a6c;             // 0x6FC45A6C  archive list head
volatile int32_t g_41a98;        // 0x6FC41A98
void* g_4584c;               // 0x6FC4584C
void* g_45824;                // 0x6FC45824
int32_t g_45844;              // 0x6FC45844
void* g_45390;              // 0x6FC45390
uint8_t g_45a3c[12], g_45a64[12]; // 0x6FC45A3C / 0x6FC45A64 lists, count at +8
void* g_45a50; uint8_t g_45a4c[8], g_45a48[12]; // 0x6FC45A50/4C/48 provider list

void* Storm_Sub1C580(uint32_t n, const char* f, int line, int fl) { (void)f; (void)line; (void)fl; return calloc(1, n); } // SMemAlloc
void Storm_Sub1B960(void* p, const char* f, int line) { (void)f; (void)line; free(p); }                                   // SMemFree
void Storm_Sub4DA0(void* list);                                   // list clear
void Storm_Sub11630();                                            // pop one entry of the list in EDX/ESI
int32_t Storm_SubFEB0(int32_t idx);                               // slot lookup
void* Storm_Sub12690();
int32_t Storm_Sub12F40(int32_t, char, uint32_t*, uint32_t);       // send to peer
void* Storm_Sub168B0(uint32_t* path);                             // snp record alloc
void* Storm_Sub4FC0(void* list, int, int);                        // new list node
int32_t Storm_SubFFE0(void* node);                                // compare with new entry
size_t Storm_Sub3AFD0(const char* s);                             // strlen
void Storm_Sub3B080(void* dst, const void* src, uint32_t n);      // strncpy-like

// Win32 module/file APIs absent from the compat shim; seams (wasm has no .snp plug-ins, so these find nothing).
struct FindData { char cFileName[260]; };
void Storm_SetEvent(void*) {}
uint32_t Storm_GetModuleFileNameA(void*, char* b, uint32_t) { b[0] = 0; return 0; }
void* Storm_FindFirstFileA(const char*, FindData*) { return (void*)(intptr_t)-1; }
int Storm_FindNextFileA(void*, FindData*) { return 0; }
void Storm_FindClose(void*) {}
void* Storm_LoadLibraryA(const char*) { return nullptr; }
void* Storm_GetProcAddress(void*, const char*) { return nullptr; }
void Storm_FreeLibrary(void*) {}
void setErr(uint32_t e) { g_lastError = (int32_t)e; SetLastError(e); }
}

// 1.13c Storm+0x17B30: validate a 'MO' (0x4F4D) object with subtype 0; drop it from the current-global, free +0xC, clear list.
uint32_t Storm_Sub17B30(uint8_t* o) {
    if (o && *(int16_t*)(o + 8) == 0x4F4D && *(int16_t*)(o + 10) == 0) {
        if (g_45824 == o) g_45824 = nullptr;
        if (*(void**)(o + 0xC)) { Storm_Sub1B960(*(void**)(o + 0xC), "..\\3rdParty\\STORM\\SOURCE\\SGDI.CPP", 0x117); *(void**)(o + 0xC) = nullptr; }
        Storm_Sub4DA0(nullptr);
        return 1;
    }
    return 0;
}

// 1.13c Storm+0x11760: build array of `count` slot pointers (start..start+count) and hand to provider +0x54.
uint32_t Storm_Sub11760(uint32_t start, int32_t count, int32_t a3, uint32_t a4, uint32_t a5) {
    if (!count || !a3) { setErr(0x57); return 0; }
    EnterCriticalSection(&g_lock);
    if (!g_snet) { setErr(0x4B4); LeaveCriticalSection(&g_lock); return 0; }
    uintptr_t* arr = (uintptr_t*)Storm_Sub1C580(count * sizeof(uintptr_t), "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x14B7, 8);
    uint32_t end = start + count, off = start << 7; uintptr_t* p = arr;
    for (uint32_t i = start; i < end; i++, p++, off += 0x80) if (i < g_45a58) *p = (uintptr_t)g_45a5c + off;
    typedef void (*Cb)(uintptr_t, int32_t, uintptr_t*, int32_t, uint32_t, uint32_t);
    if (*(Cb*)(g_snet + 0x54)) (*(Cb*)(g_snet + 0x54))(g_45238, count, arr, a3, a4, a5);
    Storm_Sub1B960(arr, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x14C8);
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x13930: send buffer to one peer (or all: -1/-2) under the lock; signals the pump event.
uint32_t Storm_Sub13930(int32_t who, uint32_t* buf, uint32_t len) {
    if (!buf || !len) { setErr(0x57); return 0; }
    EnterCriticalSection(&g_lock);
    if (!g_snet) { setErr(0x4B4); LeaveCriticalSection(&g_lock); return 0; }
    if (g_41b4c == -1) { setErr(0x85100070); LeaveCriticalSection(&g_lock); return 0; }
    bool all = (who == -2);
    if (!all) {
        if (who != -1) who -= g_45830;
        all = (who == -2 || who == -1);
        if (!all && !Storm_SubFEB0(who)) { setErr(0x8510006A); LeaveCriticalSection(&g_lock); return 0; }
    }
    if (all) {
        for (intptr_t n = g_45a6c; n > 0; n = *(intptr_t*)(n + 4)) if (*(int8_t*)(n + 0x212) != -1) Storm_Sub12F40(1, 0, buf, len);
        if (who == -1 && Storm_Sub12690()) Storm_Sub12F40(1, 0, buf, len); // else straight to the counter bump
    } else Storm_Sub12F40(1, 0, buf, len);
    __sync_fetch_and_add(&g_41a98, 1);
    LeaveCriticalSection(&g_lock);
    Storm_SetEvent(g_4584c);
    return 1;
}

// 1.13c Storm+0x116A0: drain both lists (0x6FC45A64 then 0x6FC45A3C) until their count (+8) is 0.
void Storm_Sub116A0() {
    for (int i = 0; i < 2; i++) { uint8_t* l = i ? g_45a3c : g_45a64; while (*(int32_t*)(l + 8) > 0) Storm_Sub11630(); }
}

// 1.13c Storm+0x16D70: once, scan <exe dir>*.snp, load each, query SnpQuery(i,...) and append records to the provider list.
void Storm_Sub16D70() {
    if (g_45844) return;
    g_45844 = 1;
    char dir[260] = {0}; Storm_GetModuleFileNameA(g_45390, dir, 0x104);
    char* sl = strrchr(dir, '\\');
    if (!sl) { setErr(0x57); sl = dir + strlen(dir); }
    *sl = 0; // retail terminates at the char after last '\\' kept: it writes at the located slash+? -- see spec
    char pat[260]; wsprintfA(pat, "%s*.snp", dir);
    FindData fd; void* h = Storm_FindFirstFileA(pat, &fd);
    if (h == (void*)(intptr_t)-1) return;
    do {
        char path[260 * 4]; wsprintfA(path, "%s%s", dir, fd.cFileName);
        void* rec = Storm_Sub168B0((uint32_t*)path);
        if (!rec) continue;
        void* m = Storm_LoadLibraryA(path);
        if (!m) { Storm_Sub1B960(rec, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x22D); continue; }
        typedef int (*Query)(uint32_t, uint32_t*, const char**, uint32_t*, uint32_t**);
        Query q = (Query)Storm_GetProcAddress(m, "SnpQuery");
        if (!q) { Storm_Sub1B960(rec, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x235); Storm_FreeLibrary(m); continue; }
        uint32_t idx = 0, id = 0, u = 0; const char* name = nullptr; uint32_t* caps = nullptr;
        // ponytail: per-record packing/validation (retail 0x6FC06B00-0x6FC06CE0) stays in the seams Storm_Sub4FC0/FFE0.
        while (q(idx, &id, &name, &u, &caps) && caps && *caps >= 0x24 && !(caps[1] & 0x10000000)) {
            uint8_t* n = (uint8_t*)Storm_Sub4FC0(g_45a48, 0, 0);
            Storm_Sub3B080(n + 8, path, 0x104);
            *(uint32_t*)(n + 0x10C) = idx; *(uint32_t*)(n + 0x110) = id;
            if (name) Storm_Sub3B080(n + 0x114, name, 0x80);
            memcpy(n + 0x214, caps, 36); *(uint32_t*)(n + 0x21C) -= 0x10;
            idx++; id = u = 0; name = nullptr; caps = nullptr;
        }
        Storm_FreeLibrary(m);
        Storm_Sub1B960(rec, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x281);
    } while (Storm_FindNextFileA(h, &fd));
    Storm_FindClose(h);
}
