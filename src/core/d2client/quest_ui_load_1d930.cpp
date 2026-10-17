#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// The six quest-panel cel handles, laid out as D2Client keeps them (0x122411..0x122494).
struct Client1d930Cels {
    uint32_t sockets;      // +0x00 0x122411
    uint32_t background;   // +0x04 0x122415
    uint32_t tabs;         // +0x08 0x122419
    uint32_t done;         // +0x0C 0x12241D
    uint8_t  pad[0x6C];    // +0x10 untouched
    uint32_t last;         // +0x7C 0x12248D
    uint32_t invps;        // +0x80 0x122491
};
static_assert(offsetof(Client1d930Cels, last) == 0x7C && sizeof(Client1d930Cels) == 0x84, "1d930 layout");

enum { Q1D930_BG = 0xD6E7C, Q1D930_TABS = 0xD6E4C, Q1D930_EXPTABS = 0xD6E64, Q1D930_SOCKETS = 0xD6E34,
       Q1D930_DONE = 0xD6E1C, Q1D930_INVPS = 0xD6E08, Q1D930_LAST = 0xD6DF0 };   // format-string RVAs, "%s\\ui\\menu\\<name>"

struct Client1d930Env {
    uint32_t (*isExpansion)();            // Fog#10227 (thunk 0xBF8C)
    void (*format)(uint32_t fmtRva);      // _sprintf(buf, fmt, "DATA\\GLOBAL") (0x3146)
    uint32_t (*loadCel)();                // 0x2B420(stdcall 0) on the formatted path
};

// 1.13c D2Client+0x1D930: load the quest-panel cels once. skip = byte 0x10F2F3; expFlag = dword 0x119854.
extern "C" void d2_client_1d930(const uint8_t* skip, uint32_t expFlag, Client1d930Cels* c, const Client1d930Env* e) {
    if (*skip) return;
    if (!c->background) { e->format(Q1D930_BG); c->background = e->loadCel(); }
    if (!c->tabs) {
        bool exp = e->isExpansion() != 0 && expFlag != 0;
        e->format(exp ? Q1D930_EXPTABS : Q1D930_TABS); c->tabs = e->loadCel();
    }
    if (!c->sockets) { e->format(Q1D930_SOCKETS); c->sockets = e->loadCel(); }
    if (!c->done) { e->format(Q1D930_DONE); c->done = e->loadCel(); }
    if (!c->invps) { e->format(Q1D930_INVPS); c->invps = e->loadCel(); }
    if (!c->last) { e->format(Q1D930_LAST); c->last = e->loadCel(); }
}
