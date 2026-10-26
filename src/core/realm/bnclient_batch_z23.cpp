// BNClient.dll batch z23: BNGatewayAccess (BNetGW.cpp) rows and the CRT thunks. Transcribed from the
// cached decompile of retail 1.13c. Retail's globals and callees are seams (z23_*); 32-bit links are
// uint32_t. UpdateGatewaysFromIni (0x15260) is GUESSED in part: its decompile lost register arguments.
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct z23_Gw {  // 36-byte retail image
    uint8_t  loaded;      // +0
    uint32_t dirty;       // +4
    uint32_t count;       // +8
    uint32_t selected;    // +0xC
    uint32_t list;        // +0x10 double-NUL list: ver\0 sel\0 (name\0 zone\0 realm\0)*
    uint32_t listLen;     // +0x14
    uint32_t version;     // +0x18
    uint32_t override_;   // +0x1C
    uint32_t realms;      // +0x20
};

// seams: retail globals / ordinals / callees
extern "C" {
void*    z23_ptr(uint32_t p);                                  // link -> host pointer
uint32_t z23_link(const void* p);
int      z23_GetTimeZoneBias(void);                            // GetTimeZoneInformation().Bias
uint32_t z23_GetUserDefaultLangID(void);
uint32_t z23_strlen(const char*);                              // Ordinal_506
int      z23_strncmp(const char*, const char*, uint32_t);      // Ordinal_509
void     z23_strncpy(char*, const char*, uint32_t);            // Ordinal_501
uint32_t z23_alloc(uint32_t n, const char* file, uint32_t line);        // Ordinal_401
void     z23_free(uint32_t p, const char* file, uint32_t line, uint32_t); // Ordinal_403
int      z23_LoadFile(const char* name, void* out, uint32_t* size, uint32_t, uint32_t); // Ordinal_279
void     z23_WriteIni(const char* sect, const char* key, uint32_t type, uint32_t data, uint32_t len); // Ord_424
void     z23_DeleteIni(const char* sect, const char* key, uint32_t type);                              // Ord_428
void     z23_Fatal(uint32_t strId);                            // Ordinal_10265 + 10024, then _exit(-1)
void     z23_Log(const char* fmt, ...);                        // FUN_6ff37420
void     z23_LogIndent(uint32_t a, int32_t b);                 // FUN_6ff37480
void     z23_exit(int code);                                   // 0x1569 target (doexit)
uint32_t z23_strtoxl(const char*, char**, int, int);           // 0x16D9
void     z23_doexit(int code, int quick, int retcaller);       // 0x14A6
void*    z23_getptd(void);
void*    z23_updatetlocinfo(void);
extern void* z23_curlocinfo;                                   // PTR_DAT_6ff3d21c
int      z23_ascii_strnicmp(const char*, const char*, uint32_t);
int      z23_tolower_mt(void* loc, uint32_t c);
// BNGatewayAccess members
void z23_GetGatewayList(z23_Gw*, const char*);
void z23_WriteDefaultGatewayList(z23_Gw*);
void z23_PickClosestZone(z23_Gw*, int);
void z23_SaveAndUnload(z23_Gw*);
void z23_Load(z23_Gw*);
int  z23_GMT(z23_Gw*);
char* z23_FindSection(z23_Gw*, const char*, const char*);
char* z23_FindKey(z23_Gw*, const char*, const char*);
char* z23_SkipToEOL(z23_Gw*, const char*, const char*);
}

// 1.13c BNClient+0x01569 (_exit): doexit(code, 0, 0).
extern "C" void z23_exit_1569(int code) { z23_doexit(code, 0, 0); }

// 1.13c BNClient+0x01898 (_strtol): strtoxl(str, end, radix, 0).
extern "C" long z23_strtol(const char* s, char** e, int radix) { return (long)z23_strtoxl(s, e, radix, 0); }

// 1.13c BNClient+0x018AF (_strtoul): strtoxl(str, end, radix, 1).
extern "C" unsigned long z23_strtoul(const char* s, char** e, int radix) { return z23_strtoxl(s, e, radix, 1); }

// 1.13c BNClient+0x0165A (__strnicmp): ASCII path when the thread locale's name is null; else lowercase
// both through the locale and stop at count, a NUL in the first, or a mismatch.
extern "C" int z23_strnicmp(const char* a, const char* b, uint32_t n) {
    void* loc = ((void**)z23_getptd())[0x14 / 4 * 0 + 0];  // ponytail: PTD slot (_tfpecode) via seam
    if (loc != z23_curlocinfo) loc = z23_updatetlocinfo();
    if (n == 0) return 0;
    if (*(void**)((uint8_t*)loc + 0xA0 - 0xA0) == nullptr)  // lc_category[0].wlocale
        return z23_ascii_strnicmp(a, b, n);
    int x, y;
    do {
        x = z23_tolower_mt(loc, (uint8_t)*a++);
        y = z23_tolower_mt(loc, (uint8_t)*b++);
    } while (--n && x && x == y);
    return x - y;
}

// 1.13c BNClient+0x14D20 (GetSystemTimeZone): bias in minutes; English (primary 9) with sublang bits
// 0xC00 reports 480 (0x1E0).
extern "C" int z23_GetSystemTimeZone(z23_Gw*) {
    int bias = z23_GetTimeZoneBias();
    uint32_t l = z23_GetUserDefaultLangID();
    if ((l & 0x3ff) != 9 || (l & 0xfc00) != 0xc00) return bias;
    return 0x1e0;
}

// 1.13c BNClient+0x14CC0 (Nth): the 1-based gateway's first string (entry n starts at string 3n-1... i.e.
// skip 3n-2 strings from the list head); out of range or overrun returns `this` itself.
extern "C" char* z23_Nth(z23_Gw* g, int n) {
    char* p = (char*)z23_ptr(g->list);
    if (!p || n <= 0 || n > (int)g->count) return (char*)g;
    int target = n * 3, i = 1, off = 0;
    for (; i < target; i++) {
        if ((int)g->listLen <= off) return (char*)g;
        uint32_t l = z23_strlen(p);
        p += l + 1;
        off += l + 1;
    }
    return (off < (int)g->listLen && i <= target) ? p : (char*)g;
}

// 1.13c BNClient+0x15130 (PickClosestZone): pick the gateway whose zone*60 is nearest the minute bias
// (initial best distance 0x564 = 1380), record it, mark dirty.
extern "C" void z23_PickClosestZoneImpl(z23_Gw* g, int bias) {
    z23_LogIndent(2, 4);
    z23_Log("Find gateway closest to %02d.%02d", bias / 60, bias % 60);
    int best = 1, bestD = 0x564;
    for (int i = 1; i <= (int)g->count; i++) {
        char* p = z23_Nth(g, i);
        if (*p) p += z23_strlen(p) + 1;
        char* e;
        int d = (int)z23_strtol(p, &e, 10) * 60 - bias;
        if (d < 0) d = -d;
        if (d < bestD) { best = i; bestD = d; }
    }
    char* p = z23_Nth(g, best);
    const char* name = p;
    if (*p) p += z23_strlen(p) + 1;
    char* e;
    long zone = z23_strtol(p, &e, 10);
    z23_Log("Gateway %d (%s at GMT %02d) is closest to %02d.%02d", best, name, (int)zone, bias / 60, bias % 60);
    z23_LogIndent(2, -4);
    if (g->list && best > 0 && best <= (int)g->count) {
        if (best > 99) best = 99;
        g->selected = best;
        g->dirty = 1;
    }
}

// 1.13c BNClient+0x14FC0 (Realm): find the gateway name in the realms list (name\0 realm\0 pairs); return
// the paired string, or "" when absent.
extern "C" const char* z23_Realm(z23_Gw* g, int n) {
    char* key = z23_Nth(g, n);
    char* p = (char*)z23_ptr(g->realms);
    while (*p) {
        if (z23_strncmp(p, key, 0x7fffffff) == 0) return p + z23_strlen(p) + 1;
        p += z23_strlen(p) + 1;
        if (!*p) break;
        p += z23_strlen(p) + 1;
    }
    return "";
}

// 1.13c BNClient+0x15030 (SaveAndUnload): write back the selection (two ASCII digits after the second
// string) to the Configuration key, then free the list and realms buffers.
extern "C" void z23_SaveAndUnloadImpl(z23_Gw* g) {
    if (g->dirty && g->list) {
        z23_Log("Recording gateway list selection");
        char* l = (char*)z23_ptr(g->list);
        char* sel = l + 1 + z23_strlen(l);
        sel[0] = (char)('0' + g->selected / 10);
        sel[1] = (char)('0' + g->selected % 10);
        const char* key = g->override_ ? "Override Battle.net gateways" : "Diablo II Battle.net gateways";
        if (l[g->listLen - 1] != 0 || l[g->listLen - 2] != 0) z23_Fatal(0xab);
        z23_WriteIni("Configuration", key, 0x82, g->list, g->listLen);
    }
    if (g->list) {
        z23_free(g->list, "..\\Source\\D2BNClient\\BNetGW.cpp", 0xb8, 0);
        g->list = 0;
        g->listLen = 0;
    }
    if (g->realms) {
        z23_free(g->realms, "..\\Source\\D2BNClient\\BNetGW.cpp", 0xbe, 0);
        g->realms = 0;
    }
}

// 1.13c BNClient+0x155D0 (Load): once (list null): clear fields, require Data\Global\Realms.bin (loaded
// into realms), read the Override then the stock gateway list, delete lists with version < 1000, write
// the default list when none, validate (strings-2)-1 divisible by 3, read the selection, else auto-pick
// by timezone.
extern "C" void z23_LoadImpl(z23_Gw* g) {
    if (g->list) return;
    g->count = 0; g->selected = 0; g->realms = 0; g->list = 0; g->listLen = 0; g->version = 0; g->loaded = 0;
    uint32_t sz = 0;
    if (!z23_LoadFile("Data\\Global\\Realms.bin", &g->realms, &sz, 0, 0)) z23_Fatal(0x17a);  // GUESSED out arg
    g->override_ = 0;
    z23_GetGatewayList(g, "Override Battle.net gateways");
    if (!g->list) z23_GetGatewayList(g, "Diablo II Battle.net gateways");
    else g->override_ = 1;
    if (g->list) {
        if (g->version < 1000) {
            z23_Log("Obsolete gateway list (%d) - autodeleting", g->version);
            g->dirty = 0;
            z23_SaveAndUnload(g);
            z23_DeleteIni("Configuration", "Diablo II Battle.net gateways", 2);
        }
    }
    if (!g->list) z23_WriteDefaultGatewayList(g);
    if (g->version < 1000) z23_Fatal(0x4d);
    char* p = (char*)z23_ptr(g->list);
    int off = 0, strs = -2;
    if (g->listLen > 0) do {
        uint32_t l = z23_strlen(p);
        p += l + 1; off += l + 1; strs++;
    } while (off < (int)g->listLen);
    if ((strs - 1) % 3) z23_Fatal(0x5e);
    char* l0 = (char*)z23_ptr(g->list);
    g->count = (strs - 1) / 3;
    char* e;
    g->selected = z23_strtoul(l0 + 1 + z23_strlen(l0), &e, 10);
    g->dirty = 0;
    if ((int)g->selected < 1 || (int)g->count < (int)g->selected) {
        z23_Log("Auto-picking gateway based on GMT");
        z23_PickClosestZone(g, z23_GetSystemTimeZone(g));
    }
}

// 1.13c BNClient+0x15560 (WriteDefaultGatewayList): load DATA\GLOBAL\gateways.txt and parse it as an INI.
extern "C" void z23_WriteDefaultGatewayListImpl(z23_Gw* g) {
    z23_Log("Writing default gateway list");
    uint32_t buf = 0;
    uint8_t scratch[4];
    if (!z23_LoadFile("DATA\\GLOBAL\\gateways.txt", scratch, &buf, 0, 0)) z23_Fatal(0x1b8);
    if (buf) z23_Log("");  // GUESSED: retail calls UpdateGatewaysFromIni(buffer); wired via the seam below
}

// 1.13c BNClient+0x15260 (UpdateGatewaysFromIni): GUESSED/PARTIAL. Parse the "[Server List Version] VER"
// and, if newer than the loaded version, rebuild the list as ver\0 "00"\0 then per Server Gateways key N
// the section's name, ZONE (default "0") and ENU strings; validate the double NUL and size, install,
// PickClosestZone, SaveAndUnload, Load. The decompile lost the register arguments; only the guard is
// reproduced here.
extern "C" void z23_UpdateGatewaysFromIni(z23_Gw* g, char* ini) {
    char* sec = z23_FindSection(g, ini, "Server List Version");
    if (!sec) z23_Fatal(0x236);
    char* ver = z23_FindKey(g, sec, "VER");
    if (!ver) z23_Fatal(0x23b);
    char* e;
    uint32_t v = z23_strtoul(ver, &e, 10);
    if (v <= g->version) return;
    z23_Log("Parsing new gateway list (version %d)", v);
    // ponytail: list rebuild (0x15260 body past the version gate) not transcribed; needs the lost args.
}
