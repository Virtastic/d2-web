// 1.13c Fog+0x14880 -- crash-report "Annotated stack dump": hex dump of up to 400 lines of stack,
// annotating each line whose dword points into a loaded module with its symbol.
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {
struct Module { uint8_t pad[0x204]; uint32_t base; uint32_t size; Module* next; };  // +0x204/+0x208/+0x20c
struct FogStackDumpHost {
    void (*log)(const char* fmt, ...);                                   // Fog+0x190A0/+0x18AF0 to the open log
    int (*symLookup)(uint32_t proc, uint32_t addr, int* disp, char* name);  // [0x6FFA6E38]; 0 = unknown
    bool (*badRead)(const void* p, uint32_t n);                          // IsBadReadPtr
    void (*fatal)(uint32_t code);                                        // Fog+0x10F30 channel 6, then _exit(-1)
    void (*readString)(char* dst, uint32_t addr, uint32_t n);            // Fog+0x1F710 (string literal peek)
};
FogStackDumpHost g_host{};
bool inModules(const Module* m, uint32_t v) {
    for (; m; m = m->next) if (m->base <= v && v < m->base + m->size) return true;
    return false;
}
}

// ctx: eip at +0xb8, esp at +0xc4. Returns when 400 lines are written or the stack range is exhausted.
void fog_annotatedStackDump(uint32_t proc, const uint8_t* ctx, const Module* mods) {
    uint32_t eip; std::memcpy(&eip, ctx + 0xb8, 4);
    const uint8_t* sp; { uint32_t v; std::memcpy(&v, ctx + 0xc4, 4); sp = (const uint8_t*)(uintptr_t)v; }
    char buf[256], name[24]; int disp;
    g_host.log("");
    g_host.log("Annotated stack dump");
    g_host.symLookup(proc, eip, &disp, name);
    g_host.log("\n                     // EIP = 0x%08X - %s - %s+%04X\n\n", eip, "", name, disp);
    uint32_t left = 0x1000;
    for (int line = 0; line < 400;) {
        g_host.log("              %08X:", (uint32_t)(uintptr_t)sp);
        uint32_t n = left > 15 ? 16 : left;
        buf[0] = 0;
        bool hit = false;
        if (g_host.badRead(sp, n)) { std::snprintf(buf, sizeof buf, "<unreadable>"); return; }
        uint32_t i = 0;
        for (; (int)i < (int)n;) {
            size_t l = std::strlen(buf);
            std::snprintf(buf + l, sizeof buf - l, " %02X%02X%02X%02X", sp[i], sp[i+1], sp[i+2], sp[i+3]);
            uint32_t d; std::memcpy(&d, sp + i, 4);
            i += 4;
            if (inModules(mods, d)) { hit = true; break; }
        }
        // pad to the ASCII column, then the printable bytes
        size_t l = std::strlen(buf);
        size_t pad = ((0x10 - i) * 9 >> 2) + 2;
        for (size_t k = 0; k < pad && l + 1 < sizeof buf; ++k) buf[l++] = ' ';  // ponytail: retail pads by repeated strcat of one string
        for (uint32_t k = 0; k < i && l + 1 < sizeof buf; ++k) { uint8_t c = sp[k]; buf[l++] = (c & 0x7f) < 0x20 ? '.' : c; }
        buf[l] = 0;
        g_host.log("%s\n", buf);
        if (hit) {
            uint32_t tgt; std::memcpy(&tgt, sp + i - 4, 4);
            if (!inModules(mods, tgt)) { g_host.fatal(tgt); return; }
            int d2 = 0; char nm[24]; char lit[0x50];
            const char* sym = g_host.symLookup(proc, tgt, &d2, nm) ? nm : "unknown";
            if (!strcasecmp(sym, "`string'") && d2 == 0) {
                g_host.readString(lit, tgt, 0x50);
                g_host.log("\n                           // 0x%08X - %s - %s='%s'\n\n", tgt, "", sym, lit);
            } else
                g_host.log("\n                           // 0x%08X - %s - %s+%04X\n\n", tgt, "", sym, d2);
            line += 3;
        }
        left -= i; sp += i; ++line;
        if (!left) return;
    }
}
