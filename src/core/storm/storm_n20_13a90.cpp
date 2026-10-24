// src/core/storm/storm_n20_13a90.cpp -- 12 1.13c Storm.dll rows (SNet leave/get-buffer/patch, SDlg
// timer/dialog/paint, SFile shutdown, small setters) transcribed from the cached decompiles
// (build-wasm/ghidra-storm/decomp/0x<RVA>.c). Implementation plus marker only (batch lane).
//
// Retail structs keep their 32-bit layout; link fields are read as 32-bit retail addresses the same
// way storm_batch_109c0.cpp does. Every Win32 / unmapped-internal call is a function-pointer seam
// passed as an argument (the host binds it; in ours the shell supplies the GetTickCount/PostMessage
// analogues). Register-only arguments are explicit parameters.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <strings.h>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION / GetTickCount / InterlockedIncrement

namespace {

uint16_t r16(const uint8_t* p, uintptr_t o) { uint16_t v; std::memcpy(&v, p + o, 2); return v; }
uint32_t r32(const uint8_t* p, uintptr_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
void w16(uint8_t* p, uintptr_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }
void w32(uint8_t* p, uintptr_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
uint8_t* A(uint32_t a) { return (uint8_t*)(uintptr_t)a; }   // retail 32-bit link -> pointer

// ---- module state (retail address in the comment) ----------------------------------------------
uint32_t g_lastError;        // 0x6FC45388
CRITICAL_SECTION g_snetLock; // 0x6FC45A24
CRITICAL_SECTION g_fileLock; // 0x6FC45988
uintptr_t g_provider;        // 0x6FC45834  provider table (null: not initialised)
uint8_t g_41b4c = 0xFF;      // 0x6FC41B4C
uint32_t g_45830;            // 0x6FC45830  archive index base
uint32_t g_41aa8;            // 0x6FC41AA8  packets handed out
uint32_t g_41b48;            // 0x6FC41B48  Sleep ms after leave
uint32_t g_snetFields[8];    // 0x45240 45238 451b8 452c0 45344 45170 45350 (zeroed by 13A90)
uint8_t* g_45358;            // 0x6FC45358  heap block
uint32_t g_timerHead;        // 0x6FC459FC  SDlg timer list
uint32_t g_42ffc;            // 0x6FC42FFC
uint32_t g_43130;            // 0x6FC43130

void setErr(uint32_t e) { g_lastError = e; }

}  // namespace

// ---- 0x13A90 SNetLeaveGame-style teardown ------------------------------------------------------
struct StormLeaveSeam {
    uint8_t* (*firstArchive)();                       // 0x12690
    void (*providerCb44)();                           // [provider+0x44]
    uint32_t* (*alloc12F40)(int, int, uint32_t*, int);// 0x12F40
    void (*send1D90)(uint8_t*, uint32_t*);            // 0x11D90
    void (*free)(void*, const char*, int);            // 0x1B960
    void (*drain1630)();                              // 0x11630
    uint8_t* listHead;                                // 0x6FC45A6C
    uint8_t* lists[2];                                // 0x6FC45A64, 0x6FC45A3C
    void (*sleep)(uint32_t);
};

// 1.13c Storm+0x13A90
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub13A90(uint32_t param_1, StormLeaveSeam* s) {
    EnterCriticalSection(&g_snetLock);
    if (g_provider == 0) { setErr(0x4B4); LeaveCriticalSection(&g_snetLock); return 0; }
    uint8_t* cur = s->firstArchive();
    if (cur == nullptr) { setErr(8); LeaveCriticalSection(&g_snetLock); return 0; }
    if (r32(cur, 0x200) != 0) s->providerCb44();
    for (uint8_t* n = s->listHead; n; n = A(r32(n, 4))) {
        if (n[0x212] != 0xFF) {
            uint32_t msg[2] = {r16(n, 0x1D0), param_1};
            uint32_t* pk = s->alloc12F40(0, 0x0B, msg, 8);
            if (pk) { s->send1D90(n, (uint32_t*)A(pk[3])); s->send1D90(n, (uint32_t*)A(pk[3])); }
        }
    }
    for (uint8_t* n = s->listHead; n; n = A(r32(n, 4))) {
        if (cur[0x212] != 0xFF) {
            const uint8_t* q = cur + 0x4E * 4;
            for (int i = 0; i < 4; ++i, q += 12)
                for (uint8_t* e = A(r32(q, 0)); e; e = A(r32(e, 4))) s->send1D90(cur, (uint32_t*)A(r32(e, 0xC)));
        }
    }
    if (g_45358) { s->free(g_45358, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x65E); g_45358 = nullptr; g_snetFields[6] = 0; }
    g_snetFields[0] = g_snetFields[2] = g_snetFields[3] = g_snetFields[4] = g_snetFields[1] = 0;
    g_snetFields[5] = 0;
    g_41b4c = 0xFF;
    for (int i = 0; i < 2; ++i)
        while ((int32_t)r32(s->lists[i], 8) > 0) s->drain1630();
    LeaveCriticalSection(&g_snetLock);
    s->sleep(g_41b48);
    return 1;
}

// ---- 0x15740 get a send buffer -----------------------------------------------------------------
struct StormGetBufSeam {
    void (*sub4F20)();                 // 0x4F20
    void (*sub1240)();                 // 0x1240 (called with the lock released)
    uint8_t* (*firstArchive)();        // 0x12690
    void (*sub4F70)(uint8_t*);         // 0x4F70
    void (*sub1320)(uint8_t*, uint8_t*);// 0x1320
    uint8_t* listHead;                 // 0x6FC45A6C
};

// 1.13c Storm+0x15740
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub15740(int32_t* idx, int32_t* outPtr, int32_t* outLen, StormGetBufSeam* s) {
    if (idx) *idx = (int32_t)g_45830 - 1;
    if (outPtr) *outPtr = 0;
    if (outLen) *outLen = 0;
    if (!outPtr || !outLen) { setErr(0x57); return 0; }
    EnterCriticalSection(&g_snetLock);
    if (g_provider == 0) { setErr(0x4B4); LeaveCriticalSection(&g_snetLock); return 0; }
    if (g_41b4c == 0xFF) { setErr(0x85100070); LeaveCriticalSection(&g_snetLock); return 0; }
    s->sub4F20();
    LeaveCriticalSection(&g_snetLock);
    s->sub1240();
    EnterCriticalSection(&g_snetLock);
    if (g_41b4c == 0xFF) { setErr(0x85100069); LeaveCriticalSection(&g_snetLock); return 0; }
    const uint32_t now = GetTickCount();
    uint32_t best = 0;
    uint8_t* pick = nullptr;
    for (int pass = 0; pass < 2; ++pass) {
        uint8_t* p = pass == 0 ? s->listHead : s->firstArchive();
        for (; p; p = A(r32(p, 4))) {
            const uint32_t head = r32(p, 0x5D * 4);
            if (p[0x212] == 0xFF || (p[0x46 * 4] & 4) || head == 0) continue;
            if (r16(A(r32(A(head), 0xC)), 4) != r16(p, 0x1D6)) continue;
            uint32_t t = r32(A(head), 0x18);
            for (uint32_t q = r32(A(head), 4); q; q = r32(A(q), 4)) {
                const uint32_t qt = r32(A(q), 0x18);
                if (t - qt < 0x7FFFFFFFu) t = qt - 1;
            }
            if (best <= now - t) { best = now - t; pick = p; }
        }
    }
    if (!pick) { LeaveCriticalSection(&g_snetLock); setErr(0x8510006B); return 0; }
    uint8_t* head = A(r32(pick, 0x5D * 4));
    s->sub4F70(pick + 0x5B * 4);
    w16(pick, 0x1D6, (uint16_t)(r16(pick, 0x1D6) + 1));
    s->sub1320(pick, head);
    if (idx) *idx = (int32_t)(pick[0x212] + g_45830);
    const uint8_t* pkt = A(r32(head, 0xC));
    *outPtr = (int32_t)(r32(head, 0xC) + 0xC);
    *outLen = (int32_t)r16(pkt, 2) - 0xC;
    g_41aa8++;
    LeaveCriticalSection(&g_snetLock);
    return 1;
}

// ---- 0x16260 apply pending patches -------------------------------------------------------------
struct StormPatchSeam {
    int (*regQuery)(const char*, const char*, int, uint32_t*, char*, int);   // 0x2BDD0
    void (*moduleDir)(char*, int);                                           // GetModuleFileNameA(0)
    int (*openArchive)(const char*, int, int, void**);                       // 0x1A0B0
    void (*archiveInfo)(void*, uint32_t*);                                   // 0x17560
    void (*closeArchive)(void*);                                             // 0x16CB0
    int (*openFile)(void*, const char*, int, void**);                        // 0x18960
    uint32_t (*fileSize)(void*);                                             // 0x1C580 side
    int (*readFile)(void*, void*, uint32_t);                                 // 0x195B0
    void (*closeFile)(void*);                                                // 0x16E20
    void* (*alloc)(uint32_t);                                                // 0x0C580
    void (*free)(void*);                                                     // 0x0B960
    void (*deleteFile)(const char*);
    int (*writeFile)(const char*, const void*, uint32_t);                    // Create+Write+Close
    int (*createProcess)(const char*, const char*);                          // cmd, cwd
};

// 1.13c Storm+0x16260
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub16260(int32_t* param_1, uint32_t* outState, StormPatchSeam* s) {
    if (!param_1) { setErr(0x57); return 0; }
    EnterCriticalSection(&g_snetLock);
    char names[0x400] = {0}, cmds[0x400] = {0}, dir[0x104] = {0};
    uint32_t type;
    if (!s->regQuery("Patch", "Patches", 2, &type, names, 0x3FF)) {
        *param_1 = 0; LeaveCriticalSection(&g_snetLock); return 0;
    }
    names[0x3FE] = names[0x3FF] = 0;
    s->moduleDir(dir, 0x104);
    // truncate at the last backslash (the decompile's strrchr idiom); none -> empty
    { char* b = nullptr; for (char* c = dir; *c; ++c) if (*c == '\\') b = c; dir[b ? b - dir : 0] = 0; }
    char* cmd = cmds;
    for (char* nm = names; *nm; nm += std::strlen(nm) + 1) {
        void* arc;
        if (!s->openArchive(nm, 0, 0, &arc)) { *param_1 = -1; LeaveCriticalSection(&g_snetLock); return 0; }
        uint32_t st;
        s->archiveInfo(arc, &st);
        if (st != 0 && st < 5) { s->closeArchive(arc); *param_1 = -1; LeaveCriticalSection(&g_snetLock); return 0; }
        void* lst;
        if (s->openFile(arc, "Prepatch.lst", 0, &lst)) {
            EnterCriticalSection(&g_fileLock); LeaveCriticalSection(&g_fileLock);
            const uint32_t size = s->fileSize(lst);
            uint8_t* buf = (uint8_t*)s->alloc(size);
            if (s->readFile(lst, buf, size) && size != 0) {
                uint32_t pos = 0;
                do {
                    char line[0x100]; int n = 0;
                    while (pos < size && n < 0xFF && buf[pos] && buf[pos] != '\r' && buf[pos] != '\n') line[n++] = (char)buf[pos++];
                    line[n] = 0;
                    while (pos < size && (buf[pos] == '\r' || buf[pos] == '\n')) ++pos;
                    char inArc[0x104] = {0}, onDisk[0x104] = {0};
                    const char* sp = std::strchr(line, ' ');
                    if (sp) {
                        while (*sp == ' ') ++sp;
                        std::strncpy(inArc, sp, 0x103);
                        std::snprintf(onDisk, sizeof onDisk, "%s\\%s", dir, sp);
                    }
                    if (!strncasecmp(line, "delete ", 7)) s->deleteFile(onDisk);
                    if (!strncasecmp(line, "extract ", 8)) {
                        void* f;
                        if (s->openFile(arc, inArc, 0, &f)) {
                            const uint32_t fs = s->fileSize(f);
                            uint8_t* fb = (uint8_t*)s->alloc(fs);
                            if (s->readFile(f, fb, fs)) s->writeFile(onDisk, fb, fs);
                            s->free(fb);
                            s->closeFile(f);
                        }
                    }
                    if (!strncasecmp(line, "execute ", 8)) {
                        std::strcpy(cmd, onDisk);
                        cmd += std::strlen(onDisk) + 1;
                    }
                } while (pos < size);
            }
            s->free(buf);
            s->closeFile(lst);
        }
        s->closeArchive(arc);
    }
    if (cmd == cmds) {   // no execute lines: run the default updater
        std::snprintf(cmd, 0x100, "%s\\bnupdate.exe", dir);
        cmd += std::strlen(cmd) + 1;
    }
    *cmd = 0;
    for (const char* c = cmds; *c; c += std::strlen(c) + 1)
        if (!s->createProcess(c, dir)) { *param_1 = -1; LeaveCriticalSection(&g_snetLock); return 0; }
    LeaveCriticalSection(&g_snetLock);
    *outState = 2;
    return 1;
}

// ---- 0x1D570 SDlg timer pump -------------------------------------------------------------------
struct StormTimerSeam {
    void (*postMessage)(void* hwnd, uint32_t msg, uint32_t wparam, uint32_t lparam);
};

// 1.13c Storm+0x1D570
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub1D570(StormTimerSeam* s) {
    const uint32_t now = GetTickCount();
    for (uint8_t* t = A(g_timerHead); t; t = A(r32(t, 4))) {
        const uint32_t iv = r32(t, 0x10), el = now - r32(t, 0x18);
        if (iv <= el) {
            w32(t, 0x18, iv * 2 < el ? now : r32(t, 0x18) + iv);
            if (r32(t, 0x14)) {
                typedef void (*Cb)(uint32_t, uint32_t, uint32_t, uint32_t);
                ((Cb)(uintptr_t)r32(t, 0x14))(r32(t, 8), 0x113, r32(t, 0xC), now);
                return 1;
            }
            s->postMessage((void*)(uintptr_t)r32(t, 8), 0x113, r32(t, 0xC), 0);
        }
    }
    return 1;
}

// ---- 0x1CB20 / 0x1CB60 SDlg dialog proc --------------------------------------------------------
struct StormDlgSeam {
    void (*setProp)(void* h, const char* name, uintptr_t v);   // SetPropA
    void (*endDialog)(void* h, uintptr_t result);              // EndDialog
    void (*getClassName)(void* h, char* out, int n);
    void (*setTextColor)(void* dc, uint32_t c);
    void (*setBkColor)(void* dc, uint32_t c);
    void (*setBkMode)(void* dc, int m);
    void* (*getStockObject)(int i);
};

// 1.13c Storm+0x1CB20
extern "C" EMSCRIPTEN_KEEPALIVE
void Storm_Sub1CB20(void* hwnd, uintptr_t result, StormDlgSeam* s) {
    s->setProp(hwnd, "SDlg_EndDialog", 1);
    s->setProp(hwnd, "SDlg_EndResult", result);
    s->endDialog(hwnd, result);
}

// 1.13c Storm+0x1CB60
extern "C" EMSCRIPTEN_KEEPALIVE
void* Storm_Sub1CB60(void* hwnd, uint32_t msg, void* hdc, void* child, StormDlgSeam* s) {
    auto dark = [&]() -> void* {
        s->setTextColor(hdc, 0xFFFFFF); s->setBkColor(hdc, 0); s->setBkMode(hdc, 2);
        return s->getStockObject(4);
    };
    if (msg < 0x135) {
        if (msg >= 0x133) return dark();
        if (msg == 0x10) { Storm_Sub1CB20(hwnd, 0, s); return nullptr; }
        if (msg == 0x110) return (void*)1;
    } else if (msg == 0x138) {
        char cls[256] = {0};
        s->getClassName(child, cls, 0x100);
        if (!strcasecmp(cls, "SDlgStatic")) {
            s->setTextColor(hdc, 0xFFFFFF); s->setBkMode(hdc, 1);
            return s->getStockObject(5);
        }
        return dark();
    }
    return nullptr;
}

// ---- 0x21350 modal dialog ----------------------------------------------------------------------
struct StormModalSeam {
    int (*init11220)();                                  // 0x21220
    void* (*create)(void*, void*, void*, void*, intptr_t);// 0x1DF90
    void* (*desktop)();
    void (*setProp)(void*, const char*, uintptr_t);
    uintptr_t (*getProp)(void*, const char*);
    void (*removeProp)(void*, const char*);
    int (*isIconic)(void*);
    void (*enableWindow)(void*, int);
    void (*pump)();                                      // 0x1D600
    void (*destroy)(void*);
    uint32_t* init453b8;                                 // 0x6FC453B8
};

// 1.13c Storm+0x21350
extern "C" EMSCRIPTEN_KEEPALIVE
uintptr_t Storm_Sub21350(void* inst, void* tmpl, void* parent, void* proc, intptr_t lparam, StormModalSeam* s) {
    if (*s->init453b8 == 0 && !s->init11220()) return (uintptr_t)-1;
    void* dlg = s->create(inst, tmpl, parent, proc, lparam);
    if (!dlg) return (uintptr_t)-1;
    const bool owned = parent && parent != s->desktop();
    if (owned) {
        s->setProp(dlg, "SDlg_Modal", 1);
        if (!s->isIconic(parent)) s->enableWindow(parent, 0);
    }
    uintptr_t r;
    while ((r = s->getProp(dlg, "SDlg_EndDialog")) == 0) s->pump();
    r = s->getProp(dlg, "SDlg_EndResult");
    s->removeProp(dlg, "SDlg_EndDialog");
    s->removeProp(dlg, "SDlg_EndResult");
    s->removeProp(dlg, "SDlg_Modal");
    s->destroy(dlg);
    if (parent && parent != s->desktop()) s->enableWindow(parent, 1);
    return r;
}

// ---- 0x1EFC0 SDlg paint (blit a registered bitmap into a window) -------------------------------
struct StormPaintSeam {
    int (*isWindow)(void*);
    uint32_t (*getWindowLong)(void*, int);
    uint8_t* (*lookup)(uint32_t);                                   // 0x1CD30
    void (*getClientRect)(void*, int32_t*);
    void (*clientToScreen)(void*, int32_t*);                        // one POINT
    int (*getSystemMetrics)(int);
    void* (*createRectRgn)(int, int, int, int);
    void (*combineRgn)(void*, void*, void*, int);
    void (*deleteObject)(void*);
    void* (*getWindow)(void*, int);
    void* (*getParent)(void*);
    void* (*desktop)();
    int (*intersectRect)(int32_t*, const int32_t*, const int32_t*);
    void (*clipSibling)(void*, void*, void*);                       // 0x1D2E0
    void (*blit)(uint8_t*, int32_t*, int, uint8_t*, int32_t*, int, int, int, int, uint32_t, int32_t*, void*, uint32_t, int, uint32_t*); // 0x1E830
    void (*mask282C0)(uint8_t*, uint32_t, int, int, int, int, int, int, uint32_t);                                                // 0x382C0
    void* (*alloc)(uint32_t);
    void (*free)(void*);
    // SDlg globals
    int32_t d41a0c, d41a10, d41a14, d41a20, d41a24; int16_t d41a18;
    uint32_t d453e4, d453ac, d453bc, d453b4, d453b0;
};

// 1.13c Storm+0x1EFC0
static uint32_t n20_Sub1EFC0(void* hwnd, uint32_t id, void* clip, int p4, int p5, const int32_t* p6, uint32_t flags, StormPaintSeam* s) {
    if (id == 0 || !hwnd) { setErr(0x57); return 0; }
    if (!s->isWindow(hwnd)) return 0;
    if (id == 1 && (s->getWindowLong(hwnd, -0x14) & 0x20)) return 1;
    uint8_t* v = s->lookup(id);
    if (!v) return 0;
    auto I = [&](int o) { return (int32_t)r32(v, o); };
    int32_t c[4];
    auto client = [&](int32_t* r) {
        s->getClientRect(hwnd, r); s->clientToScreen(hwnd, r); s->clientToScreen(hwnd, r + 2);
    };
    client(c);
    if (p6) {
        c[0] += p6[0] < 0 ? 0 : p6[0];  c[1] += p6[1] < 0 ? 0 : p6[1];
        c[2] += p6[2] > 0 ? 0 : p6[2];  c[3] += p6[3] > 0 ? 0 : p6[3];
    }
    if (!(flags & 1)) {
        int32_t w = I(0x24) - I(0x1C) + c[0];
        if (w <= c[2]) c[2] = w;
        if (!(flags & 2)) {
            int32_t h = I(0x28) - I(0x20) + c[1];
            if (!(c[3] < h)) c[3] = h;
        } else {
            int32_t h = (((I(0x20) - I(0x28)) - c[1]) + c[3]) / 2;
            if (h >= 1) { c[1] += h; c[3] = I(0x28) - I(0x20) + c[1]; }
        }
    }
    int32_t size[2] = {I(0x2C), I(0x30)};
    const int32_t maxW = s->d453e4 ? s->d41a20 : s->getSystemMetrics(0);
    const int32_t maxH = s->d453e4 ? s->d41a24 : s->getSystemMetrics(1);
    uint32_t mode = 3;
    if (s->d453ac == 0) mode = s->d453bc == 0 ? 0x3003 : 0x1003;
    uint32_t zero = 0;
    uint32_t* pz = s->d453ac == 0 ? &zero : nullptr;
    const int srcStride = I(0x2C);
    uint8_t* srcBits = A(r32(v, 0x18));
    if (mode & 2) {
        int32_t right = c[2], bottom = c[3];
        if (maxW < c[2] - c[0]) right = maxW + c[0];
        if (maxH < c[3] - c[1]) bottom = I(0x28) + maxH;
        if (!(mode & 1)) {
            if (I(0x24) - I(0x1C) < right - c[0]) right = I(0x24) - I(0x1C) + c[0];
            if (I(0x28) - I(0x20) < bottom - c[1]) bottom = I(0x28) - I(0x20) + c[1];
        }
        int32_t cl[4];
        client(cl);
        const int32_t org[2] = {cl[0], cl[1]};
        const int32_t lim[4] = {c[0], c[1], right, bottom};
        s->intersectRect(cl, cl, lim);
        if (cl[0] < cl[2] && cl[1] < cl[3]) {
            int32_t own[4];
            s->getClientRect(hwnd, own);
            void* rgn = s->createRectRgn(own[0], own[1], own[2], own[3]);
            if (rgn) {
                if (clip) s->combineRgn(rgn, rgn, clip, 1);
                s->clipSibling(hwnd, rgn, hwnd);
                void* w = hwnd;
                do {
                    for (void* sb = s->getWindow(w, 3); sb; sb = s->getWindow(sb, 3)) s->clipSibling(hwnd, rgn, sb);
                } while ((s->getWindowLong(w, -0x10) & 0x40000000) && (w = s->getParent(w)) && w != s->desktop());
                const bool two = s->d453b4 && s->d453b0 && s->d41a0c >= 0 && s->d41a10 >= 0;
                void* rgnB = nullptr; void* rgnUse = rgn;
                if (two) {
                    rgnB = s->createRectRgn(s->d41a0c - org[0], s->d41a10 - org[1],
                                            s->d41a14 - org[0] + s->d41a0c, (int)s->d41a18 + (s->d41a10 - org[1]));
                    rgnUse = s->createRectRgn(0, 0, 1, 1);
                    s->combineRgn(rgnUse, rgn, rgnB, 4);
                    s->combineRgn(rgnB, rgn, rgnB, 1);
                    s->deleteObject(rgn);
                }
                s->blit(nullptr, cl, 0, srcBits, size, srcStride, p4, p5, 0, 0xCC0020, (int32_t*)org, rgnUse, mode, 0, pz);
                s->deleteObject(rgnUse);
                if (two) {
                    uint8_t* tmp = (uint8_t*)s->alloc((uint32_t)(s->d41a14 * (int)s->d41a18));
                    int32_t dst[4] = {0, 0, s->d41a14, (int)s->d41a18};
                    int32_t o2[2] = {org[0] - s->d41a0c, org[1] - s->d41a10};
                    s->blit(tmp, dst, s->d41a14, srcBits, size, srcStride, p4, p5, 0, 0xCC0020, o2, rgnB, mode & 1, 0, nullptr);
                    s->mask282C0(tmp, s->d453b4, s->d41a14, (int)s->d41a18, s->d41a14, s->d41a14, 0, 0, 0x8800C6);
                    s->mask282C0(tmp, s->d453b0, s->d41a14, (int)s->d41a18, s->d41a14, s->d41a14, 0, 0, 0xEE0086);
                    int32_t at[2] = {s->d41a0c, s->d41a10};
                    s->blit(nullptr, at, 0, tmp, (int32_t*)&s->d41a14, s->d41a14, 0, 0, 0, 0xCC0020, (int32_t*)org, rgnB, mode & ~1u, 0, pz);
                    s->deleteObject(rgnB);
                    s->free(tmp);
                }
            }
        }
        return 1;
    }
    return 1;
}

// ---- 0x265B0 / 0x25D70 setters, 0x262D0 file size ----------------------------------------------
// 1.13c Storm+0x265B0
static uint32_t n20_Sub265B0(uint32_t v) {
    if (v == 0) { setErr(0x57); return 0; }
    g_42ffc = v;
    return 1;
}

// 1.13c Storm+0x25D70
static uint32_t n20_Sub25D70(uint32_t v) { g_43130 = v; return 1; }

// 1.13c Storm+0x262D0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub262D0(const uint8_t* file, uint32_t* hi, uint32_t (*getFileSize)(uintptr_t, uint32_t*)) {
    if (hi) *hi = 0;
    EnterCriticalSection(&g_fileLock);
    const uint32_t h = r32(file, 0x10C);
    if (h != 0xFFFFFFFFu) {
        LeaveCriticalSection(&g_fileLock);
        return getFileSize(h, hi);
    }
    const uint32_t sz = r32(A(r32(file, 0x114)), 8);
    LeaveCriticalSection(&g_fileLock);
    return sz;
}

// ---- 0x27020 SFile shutdown --------------------------------------------------------------------
struct StormFileShutSeam {
    void (*sub26F30)();                        // 0x26F30
    int* stopFlag;                             // 0x6FC43104
    void (*setEvent)();                        // SetEvent(0x6FC4312C)
    void (*waitThread)(uint32_t ms);           // WaitForSingleObject(0x6FC43110, 1000)
    void (*closeHandles)();                    // CloseHandle x2
    int threadAlive;                           // 0x6FC43110 != 0
    uint8_t* obj43108;                         // 0x6FC43108
    uint32_t* list[4];                         // heads: 45978, 459CC, 4596C, 45984
    void (*dtor[4])(uint8_t*);                 // 0x4040, 0x3FC0, 0x3EF0, 0x3E20
    void (*unlog)(const char*, const char*);   // 0x2FB30
    void (*free)(void*, const char*, int);     // 0x1B960
    void** heap[3];                            // 0x43120, 0x43118, 0x43128
};

// 1.13c Storm+0x27020
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t Storm_Sub27020(StormFileShutSeam* s) {
    s->sub26F30();
    if (s->threadAlive) { *s->stopFlag = 1; s->setEvent(); s->waitThread(1000); s->closeHandles(); s->threadAlive = 0; }
    if (s->obj43108) { s->dtor[0](s->obj43108); s->free(s->obj43108, "delete", -1); s->obj43108 = nullptr; }
    static const char* const kind[4] = {nullptr, "AUDIOSTREAM", "HSFILE", "HSARCHIVE"};
    for (int i = 0; i < 4; ++i) {
        while (*s->list[i]) {
            uint8_t* o = A(*s->list[i]);
            if (i) s->unlog(kind[i], i >= 2 ? (const char*)o + 8 : nullptr);
            s->dtor[i](o);
            s->free(o, "delete", -1);
        }
    }
    static const int line[3] = {0xE94, 0xE9A, 0xEA0};
    for (int i = 0; i < 3; ++i)
        if (*s->heap[i]) { s->free(*s->heap[i], "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP", line[i]); *s->heap[i] = nullptr; }
    return 1;
}
