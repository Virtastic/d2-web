// 1.13c D2Launch+0x17DF0  sub_17DF0: leave BNET (when connected) and build the main-menu control set. Core d2l_17df0 takes every callee as a
// seam in the retail register layout (names give the thunk RVA; literals are image RVAs) and g = the module range [0x25200, 0x25EC0).
// The legacy entry d2launch_sub_17DF0 copies the extern globals into that range around the core.
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2l17DF0Seams {
    void (*dbg)(uint32_t strRva);                       // OutputDebugStringA (IAT 0x1C2D4)
    void (*o9aae)();                                    // D2MCPClient 10018
    void (*bnet10)(uint32_t bnet);                      // call [bnet + 0x10]
    void (*f1b310)();
    uint32_t (*o980e)();                                // Fog 10227
    uint32_t (*o9952)(uint32_t tplRva);                 // D2Win 10098: control from template
    void (*f13d60)();
    uint32_t (*o98aa)();                                // D2Win 10165
    void (*o9862)(uint32_t ecx, uint32_t edx);          // D2Win 10147
    uint32_t (*bnet5c)(uint32_t bnet);                  // call [bnet + 0x5c]: version number
    uint32_t (*bnetb0)(uint32_t bnet);                  // call [bnet + 0xb0]: AL is read signed
    void (*o9778)(char* buf, uint32_t max, uint32_t fmtRva, int32_t a, int32_t b, int32_t c, uint32_t nargs);   // Storm 578, cdecl
    void (*o98ce)(uint32_t ecx, const char* edx, uint32_t s0);   // D2Win 10022
    uint32_t (*o9a4e)();                                // D2sound 10035
    void (*o9a72)(uint32_t ecx);                        // D2sound 10045
    uint32_t (*o9820)(uint32_t id);                     // Fog 10265
    void (*o97f6)(uint32_t strRva, uint32_t msg);       // Fog 10024
    void (*exit_)(int32_t code);                        // _exit
};

static constexpr uint32_t kBase = 0x25200;

extern "C" void d2l_17df0(const D2l17DF0Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - kBase)); };
    auto B = [](uint32_t p, uint32_t off) -> uint8_t& { return *(uint8_t*)(uintptr_t)(p + off); };
    auto fatal = [&](uint32_t id) { s->o97f6(0x1E256, s->o9820(id)); s->exit_(-1); };
    // a push past the 64-entry table is fatal 0x624; returns false then
    auto push = [&](uint32_t tpl, uint32_t& out) -> bool {
        if ((int32_t)W(0x25E30) >= 0x40) { fatal(0x624); return false; }
        out = s->o9952(tpl);
        int32_t n = (int32_t)W(0x25E30);
        W(0x252B8 + (uint32_t)n * 4) = out;
        W(0x25E30) = (uint32_t)(n + 1);
        return true;
    };
    uint32_t edi = 0, esi = 0, v = 0;
    if (W(0x256C4)) {
        uint32_t st = W(0x25ABC);
        W(0x25EB0) = 1;
        B(st, 0x35D) = 0;
        B(st, 0x37D) = 0;
        s->dbg(0x1EA30);
        s->o9aae();
        s->bnet10(W(0x256C4));
    }
    B(W(0x25ABC), 0x33) = 0;
    s->f1b310();
    if (s->o980e()) {
        if (!push(0x21280, v)) return;
        W(0x2551C) = v;
        s->f13d60();
        if (!push(0x21250, edi)) return;
        if (!push(0x212B0, esi)) return;
        if (!push(0x21220, v)) return;
    } else {
        if (!push(0x21100, edi)) return;
        if (!push(0x21130, v)) return;
        W(0x2551C) = v;
        s->f13d60();
        if (!push(0x210D0, v)) return;
        if (!push(0x21160, esi)) return;
    }
    if (s->o98aa()) {
        s->o9862(edi, 0);
        s->o9862(esi, 0);
        s->o9862(W(0x2551C), 0);
    }
    if (!push(0x21190, v)) return;
    if (!push(0x211C0, v)) return;
    if (!push(0x211F0, v)) return;
    if (!push(0x24310, edi)) return;
    if (!W(0x256C4)) { fatal(0x9FC); return; }
    char ver[32];
    int32_t n = (int32_t)s->bnet5c(W(0x256C4));
    if (B(W(0x25ABC), 0x21F)) {
        int32_t c = (int8_t)s->bnetb0(W(0x256C4));
        s->o9778(ver, 0x20, 0x1E0CC, n / 100 + 1, n % 100, c, 6);
    } else {
        s->o9778(ver, 0x20, 0x1E0C0, n / 100 + 1, n % 100, 0, 5);
    }
    s->o98ce(edi, ver, 0);
    if (s->o9a4e()) s->o9a72(1);
}

// ---- legacy entry: the play-path externs ----
extern "C" {
int32_t d2launch_ord_10098(const void* tpl);
int32_t d2launch_ord_10227(void);
int32_t d2launch_ord_10165(void);
void    d2launch_ord_10147(uint32_t ctrl, uint32_t flag);
void    d2launch_ord_10018(void);
void    d2launch_ord_10022(uint32_t ctrl, const char* text, uint32_t flag);
int32_t d2launch_ord_10035(void);
void    d2launch_ord_10045(uint32_t on);
int32_t d2launch_ord_10265(int32_t);
void    d2launch_ord_10024(const char*, int32_t);
int     d2launch_ord_578(char*, int, const char*, ...);
void    d2launch_sub_5B310(void);   // 0x1B310
void    d2launch_sub_53D60(void);   // 0x13D60
void    d2launch_dbg(const char*);  // OutputDebugStringA
extern uint8_t* g_d2launch_state;   // DAT_6fa65abc
extern int32_t* g_d2launch_bnet;    // DAT_6fa656c4 (vtable object)
extern int32_t  g_d2launch_bnetOff; // DAT_6fa65eb0
extern int32_t  g_d2launch_resCnt;  // DAT_6fa65e30
extern int32_t  g_d2launch_resTab[64]; // DAT_6fa652b8
extern int32_t  g_d2launch_res551c; // DAT_6fa6551c
}

namespace {
const char* lstr(uint32_t rva) {
    switch (rva) {
    case 0x1EA30: return "Disconnect from BNET\n";
    case 0x1E0C0: return "v %d.%02d";
    case 0x1E0CC: return "v %d.%02d %d";
    default: return "";
    }
}
uint32_t vcall(uint32_t bnet, uint32_t off) { return (uint32_t)(*(int32_t (**)(void))(uintptr_t)(bnet + off))(); }
void l_dbg(uint32_t r) { d2launch_dbg(lstr(r)); }
void l_9aae() { d2launch_ord_10018(); }
void l_b10(uint32_t b) { vcall(b, 0x10); }
uint32_t l_980e() { return (uint32_t)d2launch_ord_10227(); }
uint32_t l_9952(uint32_t tpl) { return (uint32_t)d2launch_ord_10098((const void*)(uintptr_t)tpl); }   // template RVA; the play path resolves it
uint32_t l_98aa() { return (uint32_t)d2launch_ord_10165(); }
void l_9862(uint32_t c, uint32_t d) { d2launch_ord_10147(c, d); }
uint32_t l_b5c(uint32_t b) { return vcall(b, 0x5c); }
uint32_t l_bb0(uint32_t b) { return vcall(b, 0xb0); }
void l_9778(char* buf, uint32_t max, uint32_t f, int32_t a, int32_t b, int32_t c, uint32_t) { d2launch_ord_578(buf, (int)max, lstr(f), a, b, c); }
void l_98ce(uint32_t c, const char* t, uint32_t s0) { d2launch_ord_10022(c, t, s0); }
uint32_t l_9a4e() { return (uint32_t)d2launch_ord_10035(); }
void l_9a72(uint32_t c) { d2launch_ord_10045(c); }
uint32_t l_9820(uint32_t id) { return (uint32_t)d2launch_ord_10265((int32_t)id); }
void l_97f6(uint32_t, uint32_t m) { d2launch_ord_10024("", (int32_t)m); }
void l_exit(int32_t c) { _exit(c); }
}

extern "C" void d2launch_sub_17DF0(void) {
    static const D2l17DF0Seams S = { l_dbg, l_9aae, l_b10, d2launch_sub_5B310, l_980e, l_9952, d2launch_sub_53D60, l_98aa, l_9862, l_b5c, l_bb0,
                                     l_9778, l_98ce, l_9a4e, l_9a72, l_9820, l_97f6, l_exit };
    static uint8_t img[0xCC0];
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(img + (rva - kBase)); };
    W(0x25ABC) = (uint32_t)(uintptr_t)g_d2launch_state;
    W(0x256C4) = (uint32_t)(uintptr_t)g_d2launch_bnet;
    W(0x25EB0) = (uint32_t)g_d2launch_bnetOff;
    W(0x25E30) = (uint32_t)g_d2launch_resCnt;
    W(0x2551C) = (uint32_t)g_d2launch_res551c;
    memcpy(&W(0x252B8), g_d2launch_resTab, sizeof g_d2launch_resTab);
    d2l_17df0(&S, img);
    g_d2launch_bnetOff = (int32_t)W(0x25EB0);
    g_d2launch_resCnt = (int32_t)W(0x25E30);
    g_d2launch_res551c = (int32_t)W(0x2551C);
    memcpy(g_d2launch_resTab, &W(0x252B8), sizeof g_d2launch_resTab);
}
