// 1.13c Fog+0x084CC -- CRT _NMSG_WRITE: emit runtime-error text to stderr or a message box.
#include <cstdint>
#include <cstring>
#include <string>

namespace {
// Host seams (null on wasm: nothing is written). table = the 19 (id,msg) pairs at Fog+0x7E7E8.
struct FogNmsg {
    const struct Ent { int32_t id; const char* msg; }* table;
    int32_t app_type;   // DAT_6ff80258 (1 = console)
    int32_t fallback;   // DAT_6ff8025c
    bool (*moduleName)(char* out, uint32_t n);                 // GetModuleFileNameA(NULL)
    void (*writeStderr)(const char* s, uint32_t n);            // WriteFile(GetStdHandle(-12))
    void (*messageBox)(const char* text, const char* caption, uint32_t flags);
};
FogNmsg g_nmsg = {};
}

void fog_nmsg_write(int32_t id) {
    if (!g_nmsg.table) return;
    uint32_t i = 0;
    while (i < 0x13 && g_nmsg.table[i].id != id) i++;
    if (i == 0x13 || g_nmsg.table[i].id != id) return;
    const char* msg = g_nmsg.table[i].msg;
    if (g_nmsg.app_type == 1 || (g_nmsg.app_type == 0 && g_nmsg.fallback == 1)) {
        g_nmsg.writeStderr(msg, (uint32_t)strlen(msg));
    } else if (id != 0xfc) {
        char prog[0x104 + 1] = {};
        if (!g_nmsg.moduleName(prog, 0x104)) strcpy(prog, "<program name unknown>");
        char* p = prog;
        size_t n = strlen(p);
        if (n + 1 > 0x3c) { p = prog + n + 1 - 0x3c; memcpy(p, "...", 3); }
        std::string t = "Runtime Error!\n\nProgram: ";
        t += p; t += "\n\n"; t += msg;
        g_nmsg.messageBox(t.c_str(), "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}
