// src/core/d2launch/d2launch_batch_n48.cpp -- UNMAPPED 1.13c D2Launch.dll rows (implementation + marker
// only), from build-wasm/ghidra-d2launch/decomp/0x<RVA>.c (image base 0x6FA40000). Host seams: every
// imported ordinal goes through H.ord(ordinal,a,b,c); every retail .data address (0x6FA6xxxx) through
// H.data(addr); module globals live in L.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef uintptr_t u;
struct LaunchHost {
    u     (*ord)(uint32_t ordinal, u a, u b, u c);
    void* (*data)(uint32_t addr);
    void  (*exit_)(int);
};
struct LaunchState {
    u ctl[64]; uint32_t nCtl;      // DAT_6fa652b8 / DAT_6fa65e30
    u ctlB[64]; uint32_t nCtlB;    // DAT_6fa653c0 / DAT_6fa65e34
    uint32_t feChar, isFeCharPal;  // DAT_6fa65e5c / DAT_6fa65ad8
    u gw;                          // DAT_6fa656c4 (gateway vtable ptr)
    u ui;                          // DAT_6fa65abc
    uint32_t f65e40, f65eb0;
    u c65e68, c65e64, c652a0, c65514, c65510, c65ac8, c65acc, c652ac, c654f8;
};
static LaunchState L;

[[noreturn]] static void fatal(const LaunchHost& H) {
    u s = H.ord(10265, 0x624, 0, 0);
    H.ord(10024, (u)"", s, 0);
    H.exit_(-1);
    abort();
}
// the repeated "if (n < 0x40) ctl[n++] = Ordinal_10098(desc) else fatal" step
static u push(const LaunchHost& H, uint32_t addr) {
    if (L.nCtl > 0x3f) fatal(H);
    u c = H.ord(10098, (u)H.data(addr), 0, 0);
    L.ctl[L.nCtl++] = c;
    return c;
}
typedef bool (*fn_t)();

// 1.13c D2Launch+0x12A70
extern "C" uint32_t d2launch_sub_12A70(const LaunchHost& H, const uint16_t* text, u param3) {
    uint16_t buf[1024] = {0};
    for (int i = 0; i < 1024 && text[i]; ++i) buf[i] = text[i];
    uint16_t* start = buf; uint16_t* p = buf;
    if (buf[0] != 0) {
        do {
            if (*p == 10) { *p = 0; H.ord(10000, param3, 0, 0); start = p + 1; }
            ++p;
        } while (*p != 0);
        if (start < p) H.ord(10000, param3, 0, 0);
    }
    return 1;
}

// 1.13c D2Launch+0x12B10 (EAX = descriptor index)
extern "C" void d2launch_sub_12B10(const LaunchHost& H, int idx) {
    if (L.nCtl > 0x3f) fatal(H);
    L.ctl[L.nCtl++] = H.ord(10098, (u)H.data(0x6fa60f20) + idx * 0x30, 0, 0);
}

// 1.13c D2Launch+0x12BA0 (EAX = descriptor index)
extern "C" void d2launch_sub_12BA0(const LaunchHost& H, int idx) {
    if (L.nCtlB > 0x3f) { u s = H.ord(10265, 0x60d, 0, 0); H.ord(10024, (u)"", s, 0); H.exit_(-1); abort(); }
    L.ctlB[L.nCtlB++] = H.ord(10098, (u)H.data(0x6fa60f20) + idx * 0x30, 0, 0);
}


// 1.13c D2Launch+0x07F60 -- msvcrt strcpy (word-at-a-time null scan in retail; same result bytewise)
extern "C" char* d2launch_strcpy(char* dst, const char* src) {
    char* d = dst;
    while ((*d++ = *src++) != 0) {}
    return dst;
}

// 1.13c D2Launch+0x07F70 -- msvcrt strcat
extern "C" char* d2launch_strcat(char* dst, const char* src) {
    char* d = dst;
    while (*d) ++d;
    while ((*d++ = *src++) != 0) {}
    return dst;
}

// 1.13c D2Launch+0x0CC50 -- scan the save directory for *.d2s and register each valid name.
// H.ord(10115) fills the save path; find* are host seams over FindFirstFileA/FindNextFileA/FindClose;
// sub_F190 loads a header (nonzero = ok); sub_C720 files the character.
struct LaunchFind {
    void* (*first)(const char* pattern, char* nameOut /*260*/);   // returns null on failure
    bool  (*next)(void* h, char* nameOut);
    void  (*close)(void* h);
};
static uint32_t g_charCount;           // DAT_6fa65eec
static char g_charName[16];            // DAT_6fa65880
extern "C" uint32_t d2launch_sub_CC50(const LaunchHost& H, const LaunchFind& F, char* savePath,
                                      int (*sub_F190)(uint8_t* hdr, const char* name),
                                      void (*sub_C720)(const char* name, uint8_t* hdr, int)) {
    uint8_t hdr[1280]; char pat[1024], fn[260];
    g_charCount = 0;
    H.ord(10115, (u)savePath, 0, 0);
    snprintf(pat, sizeof pat, "%s*.d2s", savePath);
    void* h = F.first(pat, fn);
    if (h) {
        do {
            char* p = g_charName; const char* c = fn;
            while (*c && *c != '.') *p++ = *c++;
            *p = 0;
            int len = (int)(p - g_charName);
            if (len > 1 && len < 0x10 && p[-1] != '-' && g_charName[0] != '_' && sub_F190(hdr, g_charName) != 0) {
                sub_C720(g_charName, hdr, 0);
                ++g_charCount;
            }
        } while (F.next(h, fn));
        F.close(h);
    }
    return 1;
}

// 1.13c D2Launch+0x0F3A0 -- delete every non-directory file "<savepath><name>.*" (fastcall: ECX = name,
// EAX = optional subdirectory). attr/del are host seams over GetFileAttributesA/DeleteFileA.
extern "C" void d2launch_sub_F3A0(const LaunchHost& H, const LaunchFind& F, const char* name, const char* sub,
                                  bool (*isDir)(const char*), bool (*del)(const char*)) {
    char dir[260], tmp[260], pat[500], path[500], fn[260];
    if (*name == 0) fatal(H);   // string 0xbf
    H.ord(10115, (u)dir, 0, 0);
    if (sub && *sub) {
        snprintf(tmp, sizeof tmp, "%s%s\\", dir, sub);
        H.ord(10114, (u)tmp, 0, 0);
        H.ord(501, (u)dir, (u)tmp, 0x104);
    }
    snprintf(pat, sizeof pat, "%s%s.*", dir, name);
    void* h = F.first(pat, fn);
    for (;;) {
        for (;;) {
            if (!h) return;
            snprintf(path, sizeof path, "%s%s", dir, fn);
            if (!isDir(path)) break;
            if (!F.next(h, fn)) return;
        }
        if (!del(path)) return;
        h = F.first(pat, fn);   // retail restarts the search after each delete
    }
}

// CRT per-thread data (VS2003). Host seams: tls slot fns, calloc, thread id.
struct LaunchPtd { u initaddr; uint32_t holdrand, tid; int32_t thandle; };
struct LaunchTls {
    uint32_t slot;                       // DAT_6fa60560, -1 = unallocated
    u (*get)(uint32_t);                  // DAT_6fa64f98
    int (*set)(uint32_t, u);             // DAT_6fa64f9c
    void (*freeSlot)(uint32_t);          // DAT_6fa64fa0
    u (*calloc_)(uint32_t, uint32_t);    // FUN_6fa449ea
    void (*amsgExit)(int);
    uint32_t (*tid)();
};

// 1.13c D2Launch+0x04714 (GetLastError/SetLastError bracket omitted: wasm has no last-error)
extern "C" LaunchPtd* d2launch___getptd(LaunchTls& T, u initaddr) {
    LaunchPtd* p = (LaunchPtd*)T.get(T.slot);
    if (!p) {
        p = (LaunchPtd*)T.calloc_(1, 0x8c);
        if (!p || !T.set(T.slot, (u)p)) T.amsgExit(0x10);
        p->initaddr = initaddr; p->holdrand = 1;
        p->thandle = -1; p->tid = T.tid();
    }
    return p;
}

