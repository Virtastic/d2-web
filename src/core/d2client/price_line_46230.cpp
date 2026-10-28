// 1.13c D2Client+0x46230: the item tooltip's price line ("Sell value: N").
// eax=&amount, ecx=item, stack (carried, wbuf, cap). Returns 1 when the line was built. Every callee is a callback.
#include <emscripten.h>
#include <cstdint>
namespace {
struct Cb {
    int32_t (*itemType)(void* item);                        // 0xC220 (4 -> no line)
    int32_t (*itemId)(uint32_t id);                         // 0xC1BA
    bool (*vendor)(int32_t* cls);                           // 0x46150: false = no vendor unit
    int32_t (*sellable)(void* item);                        // 0xC970
    int32_t (*cost)(void* item, int32_t npc, int32_t type); // 0xC844 (player/difficulty/quest are the globals)
    const uint16_t* (*str)(uint32_t id);                    // 0xD1EC
    int32_t (*repairFlag)();                                // 0xC58C
    int32_t (*gambleOk)(void* item);                        // 0xC1A8
    int32_t (*b5b40)(void* item);                           // 0x45B40
    int32_t (*b5bd0)(void* item);                           // 0x45BD0
    void (*done)(int32_t);                                  // 0xBF5F0
};
int32_t wlen(const uint16_t* s) { int32_t n = 0; while (s[n]) n++; return n; }
}
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_46230(int32_t* amt, void* item, int32_t carried, uint16_t* buf, int32_t cap,
                                                         int32_t uiState, int32_t vstock, const Cb* cb) {
    *amt = 0;
    if (!item || cb->itemType(item) == 4) return 0;
    uint32_t sid;
    int32_t type, cls = -1;
    if (!carried) {
        type = vstock ? 2 : 0;
        sid = 0xd01;
    } else {
        int32_t id = *(int32_t*)((uint8_t*)item + 4);
        if (!cb->vendor(&cls)) return 0;
        cb->vendor(&cls);
        cb->itemId((uint32_t)id);
        enum { SELL, GAMBLE, REPAIR, NONE } k = NONE;
        switch ((uint32_t)cls) {
        case 0x93: case 0x94: case 0xb1: case 0xc7: case 0xca: case 0xf6: case 0xfc: case 0xfe: case 0xff:
        case 0x195: case 0x200: case 0x201: case 0x202: k = SELL; break;
        case 0x9a: case 0xb2: case 0xfd: case 0x101: case 0x1ff: k = uiState == 4 ? GAMBLE : SELL; break;
        case 0xf4: case 0xf5: case 0x109: k = REPAIR; break;
        }
        if (k == NONE) return 0;
        if (k == REPAIR) {
            if (cb->b5bd0(item) == 1) { *(int32_t*)buf = 0; return 1; }   // the retail writes a dword
            *amt = cb->repairFlag() ? 0 : 100; sid = 0xd04; type = -1;
        } else if (k == GAMBLE) {
            if (!cb->gambleOk(item)) {
                const uint16_t* s = cb->str(0xfb6); int32_t n = wlen(buf);
                for (int32_t i = 0; (buf[n + i] = s[i]); i++) {}
                cb->done(1); return 1;
            }
            if (!cb->b5b40(item)) return 0;
            type = 3; sid = 0xd02;
        } else {
            if (!cb->sellable(item)) return 0;
            type = 1; sid = 0xd03;
        }
    }
    if (type != -1) { cb->vendor(&cls); *amt = cb->cost(item, cls, type); }
    const uint16_t* s = cb->str(sid);
    int32_t n = wlen(s);
    if (cap < n + 10) return 0;
    int32_t i = 0;
    for (; s[i]; i++) buf[i] = s[i];
    char d[12]; int32_t k = 0; int64_t v = *amt; bool neg = v < 0; if (neg) v = -v;
    do { d[k++] = (char)('0' + v % 10); v /= 10; } while (v);
    if (neg) buf[i++] = '-';
    while (k) buf[i++] = (uint16_t)d[--k];
    buf[i] = 0;
    return 1;
}
