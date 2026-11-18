// 1.13c Fog+0x084CC -- CRT _NMSG_WRITE: emit runtime-error text to stderr or a message box.
#include <cstdint>
#include <cstring>
#include <string>

// Host seams (null on wasm: nothing is written). table = the 19 (id,msg) pairs at Fog+0x7E7E8.
struct FogNmsg {
    const struct Ent { int32_t id; const char* msg; }* table;
    int32_t app_type;   // DAT_6ff80258 (1 = console)
    int32_t fallback;   // DAT_6ff8025c
    uint32_t (*moduleName)(char* out, uint32_t n);             // GetModuleFileNameA(NULL): the length, 0 on failure
    void* (*getStdHandle)(uint32_t which);                     // GetStdHandle(-12)
    void (*writeFile)(void* h, const char* s, uint32_t n);     // WriteFile(h, s, n, &written, NULL)
    void (*messageBox)(const char* text, const char* caption, uint32_t flags);
};

void fog_nmsg_write_with(const FogNmsg* g, int32_t id) {
    if (!g->table) return;
    uint32_t i = 0;
    while (i < 0x13 && g->table[i].id != id) i++;
    if (i == 0x13) return;
    const char* msg = g->table[i].msg;
    if (g->app_type == 1 || (g->app_type == 0 && g->fallback == 1)) {
        uint32_t n = (uint32_t)strlen(msg);
        void* h = g->getStdHandle(0xfffffff4u);
        g->writeFile(h, msg, n);
    } else if (id != 0xfc) {
        char prog[0x104 + 1] = {};
        if (g->moduleName(prog, 0x104) == 0) strcpy(prog, "<program name unknown>");
        char* p = prog;
        size_t n = strlen(p);
        if (n + 1 > 0x3c) { p = prog + n + 1 - 0x3c; memcpy(p, "...", 3); }
        std::string t = "Runtime Error!\n\nProgram: ";
        t += p; t += "\n\n"; t += msg;
        g->messageBox(t.c_str(), "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}

namespace { FogNmsg g_nmsg = {}; }
void fog_nmsg_write(int32_t id) { fog_nmsg_write_with(&g_nmsg, id); }
