// Alpha batch: D2Game+0xE310 (unit in EAX -> per-difficulty table entry). Written from the cached 1.13c decompile; no test runner run.
#include <cstdint>
#include <cstdlib>

#define IMP(ret, name, ...) namespace ordns_e310 { __attribute__((weak, import_module("env"), import_name(#name))) ret name(__VA_ARGS__); } using namespace ordns_e310
IMP(void*, Ordinal_10331, void*);
IMP(int, Ordinal_10826, void*);
IMP(int, Ordinal_10459, int);
IMP(int, Ordinal_10265, int);
IMP(void, Ordinal_10024, const char*, int);

// DAT_6fd2f86c: count; DAT_6fd2f870..: entries; DAT_6fd2f87c: fallback entry.
struct E310Tbl { uint32_t count; uint32_t entry[4]; };
extern E310Tbl g_d2game_6fd2f86c;
E310Tbl g_d2game_6fd2f86c;

// 1.13c D2Game+0x0E310
uint32_t D2GAME_E310(void* unit)
{
    int line;
    void* g = Ordinal_10331(nullptr);
    if (!g) line = 0xd60;
    else {
        int lvl = Ordinal_10826(g);
        uint8_t b = (uint8_t)Ordinal_10459(lvl);
        if (unit) {
            void* p = *(void**)((uint8_t*)unit + 0x80);
            if (p && *((uint8_t*)p + 0x6d)) b += 2;
            if (g_d2game_6fd2f86c.count <= b) return g_d2game_6fd2f86c.entry[3];
            return g_d2game_6fd2f86c.entry[b];
        }
        line = 0xa2a;
    }
    Ordinal_10024("D2Game", Ordinal_10265(line));
    exit(-1);
}
