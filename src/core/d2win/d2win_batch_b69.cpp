// src/core/d2win/d2win_batch_b69.cpp -- 1.13c D2Win rows 0xDFF0 0xB250 0x7970 0x12280 0xD790 0xC810 0xCBA0
// 0xCFC0 0xD4D0 0xE5F0 0xEA30 from cached decompiles.
// Skipped: CRT/chkstk (0x1B3C 0x35E0), 0x17260 (alloca'd 20 KB screenshot frame, unresolved layout).
// Hidden register inputs are explicit leading params. Callees are seams; retail globals are externs.
#include <cstdint>
#include <cstring>
#include <strings.h>
#include <initializer_list>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const char*, uint32_t);
__attribute__((weak)) void Ordinal_10029(const char*, const char*);
__attribute__((weak)) uint32_t Ordinal_10102(void);
__attribute__((weak)) void Ordinal_10103(void);
__attribute__((weak)) uint32_t Ordinal_10104(void);
__attribute__((weak)) void Ordinal_276(void);
__attribute__((weak)) intptr_t Ordinal_10045(...);  // variadic like d2game_batch_b63 (same wasm signature)
__attribute__((weak)) void Ordinal_10041(void*, int32_t, int32_t, uint32_t, uint32_t, void*);
__attribute__((weak)) void Ordinal_10011(void*, int32_t, int32_t);
__attribute__((weak)) void Ordinal_10014(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
__attribute__((weak)) void* Ordinal_10003(uint32_t) __asm__("d2win_Ordinal_10003");  // distinct import: the flat Ordinal_10003 name is declared with other signatures by D2Multi/D2Launch
__attribute__((weak)) void* Ordinal_10211(const char*, int32_t);
__attribute__((weak)) void Ordinal_10215(void*, uint32_t);
__attribute__((weak)) void* Ordinal_10208(void*, void*);
__attribute__((weak)) int32_t Ordinal_10210(void*);
__attribute__((weak)) void Ordinal_10207(void*, uint32_t, void*, int32_t, uint32_t);
__attribute__((weak)) void Ordinal_10209(void*);
__attribute__((weak)) int wsprintfA(char*, const char*, ...);
void _exit(int);
extern const uint32_t* gdwBitMasks_exref;
extern uint32_t d2win_g_6f8ffdac, d2win_g_6f8ffe00, d2win_g_6f8ffe98;
extern char d2win_g_6f9a9d12;
extern int32_t d2win_g_6f900c2c;
extern uint32_t d2win_g_6f9a9e48, d2win_g_6f9a9e50, d2win_g_6f9a9e38, d2win_g_6f9a9e44,
    d2win_g_6f9a9e40, d2win_g_6f9a9e4c, d2win_g_6f9a9e3c;
extern int32_t d2win_g_6f901488, d2win_g_6f90148c;   // cursor x,y
extern int32_t d2win_g_6f9a8c68, d2win_g_6f9a8c74, d2win_g_6f9a8c7c, d2win_g_6f9a8c84;
extern uint8_t* d2win_g_6f9a8c6c;
extern uint8_t* d2win_g_6f9a8c70;
extern uint8_t* d2win_g_6f9a8c78;
extern uint8_t* d2win_g_6f9a8c80;
extern uint16_t* d2win_g_6f9a8c88;
extern void* d2win_g_6f9a8ca0;
extern uint32_t d2win_g_6f9a8ca4;
extern const char d2win_g_6f8fcc00[4];
extern const char d2win_g_6f8fcdb0[];   // table-file extension
__attribute__((weak)) void Ordinal_10046_p(const char*, int32_t, int32_t) __asm__("Ordinal_10046"); // link sig matches d2common b5e870; hidden-reg ptr dropped
__attribute__((weak)) void Unicode_toUnicode(char16_t*, const char*, int32_t);
__attribute__((weak)) void Unicode_strcat(char16_t*, const char16_t*);
__attribute__((weak)) void Unicode_strcpy(char16_t*, const char16_t*);
__attribute__((weak)) void Unicode_sprintf(int32_t, char16_t*, const char16_t*, ...);
__attribute__((weak)) void Unicode_win2Unicode(char16_t*, const char*, int32_t);
__attribute__((weak)) int32_t Unicode_strlen(const char16_t*);
extern char d2win_g_6f8fcc04;
extern const char* d2win_g_fontNames[];       // 6f8fc588 table
extern const char* d2win_g_6f9014f4;
extern const char* d2win_g_6f8fc884;
extern const char* d2win_g_6f8fc8a8;
extern const char* d2win_g_FontInGameChat;
}


// Definitions for the b69 globals nothing else defines (weak: a real definition elsewhere wins).
extern "C" {
#define G(T, n) __attribute__((weak)) T n = {}
G(char, d2win_g_6f8fcc04); G(char, d2win_g_6f9a9d12); G(int32_t, d2win_g_6f900c2c);
G(uint32_t, d2win_g_6f8ffdac); G(uint32_t, d2win_g_6f8ffe00); G(uint32_t, d2win_g_6f8ffe98);
G(uint32_t, d2win_g_6f9a9e38); G(uint32_t, d2win_g_6f9a9e40); G(uint32_t, d2win_g_6f9a9e4c); G(uint32_t, d2win_g_6f9a9e3c); G(uint32_t, d2win_g_6f9a9e44); G(uint32_t, d2win_g_6f9a9e48); G(uint32_t, d2win_g_6f9a9e50);
G(int32_t, d2win_g_6f9a8c68); G(int32_t, d2win_g_6f9a8c74); G(int32_t, d2win_g_6f9a8c7c); G(int32_t, d2win_g_6f9a8c84);
G(uint8_t*, d2win_g_6f9a8c6c); G(uint8_t*, d2win_g_6f9a8c70); G(uint8_t*, d2win_g_6f9a8c78); G(uint8_t*, d2win_g_6f9a8c80);
G(uint16_t*, d2win_g_6f9a8c88); G(void*, d2win_g_6f9a8ca0); G(uint32_t, d2win_g_6f9a8ca4);
G(const char*, d2win_g_6f8fc884); G(const char*, d2win_g_6f8fc8a8); G(const char*, d2win_g_6f9014f4); G(const char*, d2win_g_FontInGameChat);
__attribute__((weak)) const char* d2win_g_fontNames[32] = {};
__attribute__((weak)) extern const char d2win_g_6f8fcc00[4] = {'.', 't', 'x', 't'};
__attribute__((weak)) extern const char d2win_g_6f8fcdb0[8] = ".txt";
}
struct D2WinB69Env {
    int32_t (*sub_D790)(void* a, int32_t* tbl, int32_t b, void* c);    // 0xD790
    int32_t* (*sub_B860)(uint32_t a, uint32_t b);                      // 0xB860
    uint8_t* (*sub_BAD0)(void);                                        // 0xBAD0
    uint8_t* (*sub_C680)(uint32_t a, int32_t b);                       // 0xC680
    uint32_t (*sub_B7B0)(uint32_t a, uint32_t b);                      // 0xB7B0
    void* (*sub_B810)(uint32_t);                                       // 0xB810
    void (*sub_E0D0)(void);                                            // 0xE0D0
    int32_t (*sub_E920)(uint8_t*, uint8_t*, int32_t*, uint8_t, int32_t);
    void (*sub_E5C0)(uint32_t, int32_t, void*, uint32_t*, uint8_t);
    void* (*sub_7C60)(void* thisp, int32_t** out);                     // 0x7C60
    void (*sub_B8B0)(void* p, uint32_t eax, uint8_t di);
    void (*sub_7970)(void);
    void (*sub_1000)(void);
    void (*sub_2FE0)(int32_t);                                         // 0x12FE0
    void (*sub_C980)(void* ecx, uint8_t* dst, void* str, int32_t* y);
    void (*sub_C590)(void* str, int32_t w);
    void (*sub_1FB0)(void);                                            // 0x11FB0
    void (*sub_1E80)(void);                                            // 0x11E80
    void (*sub_2940)(void* str, int32_t x, int32_t y, int32_t color, int32_t z);
    const char* (*sub_BB50)(uint32_t, uint32_t, uint32_t);
};

static void d2win_fatal(uint32_t id) { Ordinal_10024("", Ordinal_10265(id)); _exit(-1); }

// 1.13c D2Win+0x0DFF0 -- rec = ESI. Clamp the three indices and verify the table lookup (retry with 7,5,1).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_DFF0(uint32_t* rec, const D2WinB69Env* env) {
    if (!rec) d2win_fatal(0x80f);
    if (rec[7] >= d2win_g_6f8ffdac || rec[8] >= d2win_g_6f8ffe00 || rec[9] >= d2win_g_6f8ffe98) {
        rec[7] = 7; rec[8] = 5; rec[9] = 1;
    }
    int32_t* tbl = (int32_t*)(uintptr_t)rec[0x2e];
    if (!tbl) d2win_fatal(0x81c);
    if (env->sub_D790((void*)(uintptr_t)rec[8], tbl, (int32_t)rec[7], (void*)(uintptr_t)rec[8]) == 0) {
        rec[7] = 7; rec[8] = 5; rec[9] = 1;
        if (env->sub_D790((void*)(uintptr_t)5, tbl, 7, (void*)(uintptr_t)5) == 0) d2win_fatal(0x82b);
    }
    return 1;
}

// 1.13c D2Win+0x0B250 -- name = EAX: open Data\Global\Items\Palette\<name><ext>, read, close.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_B250(const char* name, const D2WinB69Env* env) {
    char path[256];
    strcpy(path, "Data\\Global\\Items\\Palette\\");
    strcat(path, name);
    memcpy(path + strlen(path), d2win_g_6f8fcc00, 4);
    size_t n = strlen(path);
    path[n] = d2win_g_6f8fcc04; path[n + 1] = 0;
    if (Ordinal_10102() == 0) {
        // GetLastError()==2 (file not found) stays silent; the host seam reports other errors.
         Ordinal_10029("Error opening file: %s", path);
        d2win_fatal(0xdcd);
    }
    env->sub_7970();
     Ordinal_10103();
}

// 1.13c D2Win+0x07970 -- ESI = handle, EDI = expected size; size read through Ordinal_10104.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_7970(int32_t handle, int32_t expectSize, int32_t gotSize) {
    if (!handle) d2win_fatal(0x87);
    if (Ordinal_10104() == 0) { Ordinal_276(); d2win_fatal(0x8f); }
    if (expectSize != gotSize) d2win_fatal(0x92);
}

// 1.13c D2Win+0x12280 -- EBX = font index, EDI = dst, param_1 = dst capacity.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_12280(int32_t cap, int32_t fontIdx, char* dst) {
    char dir[260];
    wsprintfA(dir, "%s\\%s\\", "DATA\\LOCAL", d2win_g_6f8fc8a8);
    if (cap <= (int32_t)strlen(dir) + 0x14) d2win_fatal(0x95);
    const char* mid = d2win_g_6f9014f4;
    const char* name = d2win_g_fontNames[fontIdx];
    if (d2win_g_6f9a9d12 != 0 && fontIdx == 0xd) { mid = d2win_g_6f8fc884; name = d2win_g_FontInGameChat; }
    wsprintfA(dst, "%s%s%s.tbl", dir, mid, name);
}

// 1.13c D2Win+0x0D790 -- this = key a, param_3 = key b; returns the cached object or 0 after a fill.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_D790(uint32_t a, int32_t* tbl, int32_t, uint32_t b, uint32_t eax, uint8_t di,
                       const D2WinB69Env* env) {
    if (!tbl) d2win_fatal(0x4bf);
    int32_t* p = env->sub_B860(a, b);
    if (!p) { env->sub_B8B0((void*)(uintptr_t)b, eax, di); return 0; }
    int32_t r = *(int32_t*)(uintptr_t)(*p + 0x14);
    if (!r) d2win_fatal(0x4d7);
    return r;
}

// 1.13c D2Win+0x0C810 -- name = EAX, this = file arg, stride = retaddr slot; returns table data.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t* d2win_sub_C810(const char* name, void* thisp, int32_t stride, int32_t* outCount,
                        const D2WinB69Env* env) {
    char path[240];
    int32_t* data = nullptr;
    if (d2win_g_6f900c2c == 0 && strcasecmp(name, "leveldefs") == 0)
        wsprintfA(path, "%s\\%s%s", "DATA\\GLOBAL\\EXCEL", "levels", d2win_g_6f8fcdb0);
    else
        wsprintfA(path, "%s\\%s%s", "DATA\\GLOBAL\\EXCEL", name, d2win_g_6f8fcdb0);
    int32_t* raw = (int32_t*)env->sub_7C60(thisp, &data);
    if (!raw) d2win_fatal(0x9d8);
    int32_t count;
    if (d2win_g_6f900c2c == 0) {
        void* h = Ordinal_10208(raw, data);
        count = Ordinal_10210(h);
        void* buf = (void*)Ordinal_10045("..\\Source\\D2Win\\Src\\D2Comp.cpp", 0x9ef, 0);
        memset(buf, 0, (size_t)(count * stride));
        Ordinal_10207(h, 0, buf, count, 0);   // ponytail: stack extras uStack_14/uStack_c left to the seam
        Ordinal_10209(h);
        data = (int32_t*)buf;
    } else { count = *raw; data = raw + 1; }
    if (outCount) *outCount = count;
    return data;
}

// 1.13c D2Win+0x0cba0 -- eax/ebp/esi = the three excel file contexts (hidden inputs); builds the merged
// 0x1A8-byte component table and its lookup arrays.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_CBA0(void* ctxA, void* ctxB, void* ctxC, const D2WinB69Env* env,
                    int32_t* (*sub_C810)(const char*, void*, const void*, int32_t*)) {
    struct Col { const void* name; uint32_t type, a, off; const void* extra; };
    static const char kTerm[] = "";
    // names at 6f8fcd98 / 6f8fcd90 / 6f8fcd2c are retail data strings; seams keep them opaque
    const Col cols[12] = {
        {"version", 3, 0, 0xf6, nullptr}, {d2win_g_6f8fcdb0, 0xf, 0, 0x11e, &d2win_g_6f9a8ca4},
        {kTerm, 9, 0, 0x80, nullptr},     {"alternategfx", 9, 0, 0x90, nullptr},
        {"spawnable", 4, 0, 0x133, nullptr}, {"level", 4, 0, 0xfd, nullptr},
        {"component", 4, 0, 0x115, nullptr}, {"wclass", 9, 0, 0xc0, nullptr},
        {"2handedwclass", 9, 0, 0xc4, nullptr}, {"1or2handed", 6, 0, 0x13d, nullptr},
        {"Transform", 4, 0, 0x141, nullptr},  {kTerm, 0, 0, 0, nullptr}};
    const char* const F = "..\\Source\\D2Win\\Src\\D2Comp.cpp";
    int32_t* a = sub_C810((const char*)ctxA, ctxA, cols, &d2win_g_6f9a8c74);
    int32_t* b = sub_C810((const char*)ctxB, ctxB, cols, &d2win_g_6f9a8c7c);
    int32_t* c = sub_C810((const char*)ctxC, ctxC, cols, &d2win_g_6f9a8c84);
    (void)env;
    d2win_g_6f9a8c68 = d2win_g_6f9a8c84 + d2win_g_6f9a8c7c + d2win_g_6f9a8c74;
    d2win_g_6f9a8c80 = (uint8_t*)Ordinal_10045(F, 0xa29, 0);
    d2win_g_6f9a8c6c = d2win_g_6f9a8c70 = d2win_g_6f9a8c80;
    memcpy(d2win_g_6f9a8c80, a, (size_t)d2win_g_6f9a8c74 * 0x1a8);
    d2win_g_6f9a8c80 += (size_t)d2win_g_6f9a8c74 * 0x1a8;
    d2win_g_6f9a8c78 = d2win_g_6f9a8c80;
    memcpy(d2win_g_6f9a8c80, b, (size_t)d2win_g_6f9a8c7c * 0x1a8);
    d2win_g_6f9a8c80 += (size_t)d2win_g_6f9a8c7c * 0x1a8;
    memcpy(d2win_g_6f9a8c80, c, (size_t)d2win_g_6f9a8c84 * 0x1a8);
    Ordinal_10046_p(F, 0xa39, 0); Ordinal_10046_p(F, 0xa3a, 0); Ordinal_10046_p(F, 0xa3b, 0);
    d2win_g_6f9a8c88 = (uint16_t*)Ordinal_10045(F, 0xa40, 0);
    memset(d2win_g_6f9a8c88, 0, (size_t)d2win_g_6f9a8c68 * 2);
    for (int32_t i = 0, k = 0; i < d2win_g_6f9a8c68; i++)
        if (*(int16_t*)(d2win_g_6f9a8c6c + (size_t)i * 0x1a8 + 0xf6) == 0) d2win_g_6f9a8c88[k++] = (uint16_t)i;
    d2win_g_6f9a8ca0 = Ordinal_10211(F, 0xa4b);
    for (int32_t i = 0; i < d2win_g_6f9a8c68; i++)
        Ordinal_10215(d2win_g_6f9a8ca0, *(uint32_t*)(d2win_g_6f9a8c6c + (size_t)i * 0x1a8 + 0x80));
}

// 1.13c D2Win+0x0CFC0 -- tooltip over a player's portrait; p = player-name record, returns 1 when drawn.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_CFC0(uint32_t* p, const D2WinB69Env* env) {
    char16_t buf[512] = {};
    uint8_t base[3636];
    int32_t h = 0;
    int32_t flag = 0;
    auto line = [&](const char* t) { Unicode_toUnicode(buf, t, 0x100); env->sub_C980(buf, base, buf, &h); };
    if (!p) d2win_fatal(0x73d);
    if (!(p[0] & gdwBitMasks_exref[3])) return 0;
    const int32_t L = (int32_t)p[5] - 0x1e, T = (int32_t)p[6] - 0x50, R = (int32_t)p[5] + 0x1e, B = (int32_t)p[6] + 0x14;
    if (!(d2win_g_6f901488 >= L && d2win_g_6f901488 < R && d2win_g_6f90148c >= T && d2win_g_6f90148c < B)) return 0;
    int32_t x = (int32_t)p[5], y = (int32_t)p[6] - 0x50;
    for (int i = 0; i < 7; i++) env->sub_1000();
    env->sub_2FE0(1);
    if (p[0x8c] == 0x44325850) line("Expansion");
    uint32_t t = p[7];
    bool realm = false;
    if (t == 0x13) { line("Moderator"); realm = true; }
    else if (t == 0x14) { line("Sysop"); flag = 3; }
    else if (t == 0x12) { line("Blizzard Rep"); flag = 3; }
    else if (t == 0x17) line("Speaker");
    else if (t == 0x15) line("Referee");
    else if (t == 0x16) line("Chat");
    else if (t == 0x18) line("Open Character");
    else {
        uint32_t g = p[0x8c];
        const char* n;
        if (g == 0x44324456 || g == 0x44325850 || g == 0x44325354) realm = true;
        else {
            switch (g) {
            case 0x53534852: n = "Starcraft Shareware"; break;
            case 0x44545354: case 0x4452544c: n = "Diablo"; break;
            case 0x44534852: n = "Diablo Shareware"; break;
            case 0x4a535452: case 0x53544152: n = "Starcraft"; break;
            case 0x53455850: n = "Brood War"; break;
            case 0x57335850: n = "Frozen Throne"; break;
            case 0x5732424e: n = "Warcraft II"; break;
            case 0x57415233: n = "Warcraft III"; break;
            default: n = "Unknown";
            }
            line(n);
        }
    }
    if (realm) {
        int32_t sid = (int32_t)p[0x8b];
        if (sid > 0) {
            Unicode_sprintf(0x100, buf, (const char16_t*)Ordinal_10003((uint32_t)sid));
            env->sub_C980(buf, base, buf, &h);
            if (p[0] & gdwBitMasks_exref[5]) env->sub_C980(buf, base, Ordinal_10003(0), &h);
            Unicode_toUnicode(buf, "Realm: ", 0x100);
            Unicode_strcat(buf, (const char16_t*)(p + 0x7b));
            env->sub_C980(buf, base, buf, &h);
        }
    }
    (void)flag;
    Unicode_toUnicode(buf, "Account Name: ", 0x100);
    Unicode_strcat(buf, (const char16_t*)(p + 0x33));
    env->sub_C980(buf, base, buf, &h);
    x = x < 0 ? 0 : (x > 800 ? 800 : x);
    Ordinal_10014(x, y - (h + 6), x, y, 0, 2);
    return 1;
}

// 1.13c D2Win+0x0D4D0 -- EBX = p: draw a player's name label (title prefix, {suffix}).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_D4D0(uint32_t* p, const D2WinB69Env* env) {
    char16_t name[512] = {}, suffix[512] = {}, pre[512] = {};
    if (!p) d2win_fatal(0x6b0);
    const uint32_t* m = gdwBitMasks_exref;
    if (!(p[0] & m[1])) return 0;
    int32_t y = (int32_t)p[6] + 6;
    int32_t color = 0;
    bool two = false;
    if (p[7] == 0x14 || p[7] == 0x12) { two = true; color = 3; }
    else if (p[0] & m[5]) color = 1;
    if (p[0] & m[8]) y = (int32_t)p[6] + 0x15;
    if (!(p[0] & m[6]) && !(p[0] & m[7])) {
        uint32_t g = p[0x8c];
        Unicode_strcpy(name, (const char16_t*)(p + ((g == 0x44324456 || g == 0x44325354 || g == 0x44325850) ? 0x53 : 0x33)));
    } else {
        const char* s = env->sub_BB50(p[7], ((p[0] >> 5) & 1) * 3 + ((p[0] >> 7) & 1) * 2 + ((p[0] >> 6) & 1), (p[0] >> 12) & 1);
        Unicode_win2Unicode(pre, s, 0xff);
        Unicode_strcpy(name, pre);
        Unicode_strcat(name, (const char16_t*)(p + 0x53));
    }
    if (Unicode_strlen((const char16_t*)(p + 0x73)) != 0) {
        Unicode_toUnicode(suffix, " {", 0x100);
        Unicode_strcat(name, suffix);
        Unicode_strcat(name, (const char16_t*)(p + 0x73));
        Unicode_toUnicode(suffix, "}", 0x100);
        Unicode_strcat(name, suffix);
    }
    env->sub_2FE0(0xc);
    env->sub_C590(name, 0x78);
    env->sub_1FB0();
    env->sub_1E80();
    if (two) env->sub_2940(name, y - 1, 0, 6, 0);
    env->sub_2940(name, y, 0, color, 0);
    return 1;
}

// 1.13c D2Win+0x0E5F0 -- EBX = p, param_1 = draw arg: draw a character's animated paper-doll layers.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_E5F0(uint32_t arg, uint32_t* p, const D2WinB69Env* env) {
    uint32_t s[18];
    if (!p) d2win_fatal(0x5e8);
    const uint32_t* m = gdwBitMasks_exref;
    if (!(m[0] & p[0])) return 0;
    if (m[10] & p[0]) {
        if (!d2win_g_6f9a9e48) d2win_fatal(0x5f3);
        memset(s, 0, sizeof s);
        s[13] = d2win_g_6f9a9e48; s[0] = d2win_g_6f9a9e50; s[16] = 0;
        Ordinal_10041(s, (int32_t)p[5] + 5, (int32_t)p[6] - 5, 0xffffffffu, 3, nullptr);
    }
    if (p[0x2f]) {
        uint8_t ex[3] = {0, (uint8_t)p[0x30], *((uint8_t*)p + 0xc1)};
        memset(s, 0, sizeof s);
        s[16] = 0; s[0] = 0; s[14] = 1; s[13] = p[0x2f];
        Ordinal_10041(s, (int32_t)p[5] - 0x14, (int32_t)p[6] - 0x3c, 0xffffffffu, 5, ex);
    }
    if (!p[0x2e]) d2win_fatal(0x617);
    int32_t* e = env->sub_B860(p[9], p[8]);
    if (!e) return 0;
    if (*(int32_t*)(uintptr_t)(*e + 0x14) == 0) d2win_fatal(0x620);
    uint8_t* cnt = env->sub_BAD0();
    uint32_t cur = p[2];
    uint8_t* lay = env->sub_C680((uint8_t)p[1], (int32_t)cur >> 8);
    if (!lay) d2win_fatal(0x625);
    uint32_t mode = 5;
    if (((m[5] & p[0]) && (m[4] & p[0])) || (m[13] & p[0])) mode = 1;
    for (int32_t i = 0; i < *cnt; i++) {
        uint8_t l = lay[i];
        memset(s, 0, sizeof s);
        s[16] = env->sub_B7B0((uint8_t)p[1], *((uint8_t*)p + 5)) & 0xff;
        s[13] = 0; s[0] = (uint32_t)((int32_t)cur >> 8);
        ((uint8_t*)s)[5] = l;
        env->sub_E0D0();
        s[17] |= 2;
        uint32_t idx = *((uint8_t*)p + l + 0x28);
        void* pal = idx != 0xff ? env->sub_B810(idx) : nullptr;
        Ordinal_10041(s, (int32_t)p[5], (int32_t)p[6], arg, mode, pal);
    }
    p[2] = cur + p[4];
    if ((int32_t)(p[3] - 1) <= (int32_t)(cur + p[4]) >> 8) p[2] = 0;
    uint32_t now = (uint32_t)emscripten_get_now();  // GetTickCount, ms
    if (d2win_g_6f9a9e38 < now) {
        d2win_g_6f9a9e38 = now + 0x42;
        if (d2win_g_6f9a9e40 <= ++d2win_g_6f9a9e44) d2win_g_6f9a9e44 = 0;
        if (d2win_g_6f9a9e4c <= ++d2win_g_6f9a9e50) d2win_g_6f9a9e50 = 0;
    }
    if (m[9] & p[0]) {
        if (!d2win_g_6f9a9e3c) d2win_fatal(0x666);
        memset(s, 0, sizeof s);
        s[13] = d2win_g_6f9a9e3c; s[16] = 0; s[0] = d2win_g_6f9a9e44;
        Ordinal_10041(s, (int32_t)p[5], (int32_t)p[6] + 10, 0xffffffffu, 3, nullptr);
    }
    return 1;
}

// 1.13c D2Win+0x0ea30 -- EAX = p: draw a character's equipment-layer overlays (layer ids < 0x10).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_EA30(uint32_t* p, const D2WinB69Env* env) {
    if (!p) d2win_fatal(0x5a8);
    const uint32_t* m = gdwBitMasks_exref;
    if (!(m[0] & p[0]) || !(m[2] & p[0])) return;
    int32_t x = (int32_t)p[5] - 2, y = (int32_t)p[6];
    if (!p[0x2e]) d2win_fatal(0x5b8);
    int32_t* e = env->sub_B860(p[9], p[8]);
    if (!e) return;
    if (*(int32_t*)(uintptr_t)(*e + 0x14) == 0) d2win_fatal(0x5c0);
    uint8_t* cnt = env->sub_BAD0();
    int32_t frame = (int32_t)p[2] >> 8;
    uint8_t* lay = env->sub_C680((uint8_t)p[1], frame);
    if (!lay) d2win_fatal(0x5c5);
    int32_t t = 0;
    if (env->sub_E920(cnt, lay, &t, (uint8_t)p[1], frame) < 1) return;
    for (uint32_t i = 0; i < *cnt; i++) {
        if (lay[i] >= 0x10) d2win_fatal(0x5cf);
        uint32_t s[18] = {};
        env->sub_E5C0((uint8_t)p[1], frame, s, p, lay[i]);
        Ordinal_10011(s, x, y);
    }
}

// 1.13c D2Win+0x0BE00 -- loads the 8 item palettes: ECX = 1..8 (slot), EAX = name; last call is a tail jmp.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_BE00(const D2WinB69Env* env) {
    static const char* const k[8] = {"grey", "grey2", "gold", "brown", "greybrown", "invgrey", "invgrey2", "invgreybrown"};
    for (int i = 0; i < 8; i++) d2win_sub_B250(k[i], env);  // ponytail: slot ECX=i+1 not modelled in sub_B250's seam
}
