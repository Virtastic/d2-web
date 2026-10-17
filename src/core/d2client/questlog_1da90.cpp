// 1.13c D2Client+0x1DA90: per-act quest-log icon chooser. Loads the act's quest-icon cels once, then walks the
// 41 quest rows of that act and writes the chosen icon frame to the global at +0x1123F9. stdcall(act, reset), ret 8.
#include <cstdint>
#include <cstddef>
#include <cstring>

struct QuestLogGlobals { uint32_t rec1, rec2; int32_t g23e4; uint32_t g23ec; uint32_t g394; const uint8_t* init; uint8_t* state; };
struct QuestLogOps {
    void* c;
    int (*qs)(void*, uint32_t, uint32_t, uint32_t);
    uint32_t (*lang)(void*, uint32_t);
    void (*ncpy)(void*, uint8_t*, uint32_t, uint32_t);
    void (*ctor)(void*, uint8_t*);
    void (*csprintf)(void*, uint8_t*, int32_t);
    void (*touni)(void*, uint8_t*, uint8_t*, uint32_t);
    void (*ucpy)(void*, uint8_t*, uint32_t);
    int (*ulen)(void*, uint32_t);
    void (*ucat)(void*, uint8_t*, uint8_t*);
    void (*usprintf)(void*, uint32_t, uint8_t*, uint32_t, uint32_t);
    void (*fatal)(void*);
};
void d2client_sub_1CE00(uint8_t*, const QuestLogGlobals*, const QuestLogOps*, uint8_t*);

struct QuestIconGlobals {
    uint32_t result;       // +0x1123F9 (-1 = none)
    uint32_t g9854;        // +0x109854
    uint32_t cached[5];    // +0x10B7E8 per act, -1 = not decided
    uint32_t fallback[5];  // +0x1123FD per act
    uint32_t loaded[27];   // +0x112421 loaded icon cels (0 = not loaded)
};
struct QuestIconOps {
    void* c;
    int (*o10227)(void*);               // D2Client thunk +0xBF8C
    void (*path)(void*, uint32_t i);    // _sprintf(buf, "%s\\ui\\menu\\%s", "DATA\\GLOBAL", name[i]) (+0x3146)
    uint32_t (*load)(void*, uint32_t);  // +0x2B420 (buf in EAX, stack arg 0)
};
// act -> inclusive range of icon names (0x6FBA8160); rows (0x6FBA8188, 16 bytes): flag, icon index, act, quest id at +12
static const int32_t kRange[5][2] = {{0, 5}, {6, 11}, {12, 17}, {18, 20}, {21, 26}};
static const uint8_t kF[41] = {0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1};
static const uint8_t kI[41] = {0,0,1,2,3,4,5,0,0,6,7,8,9,10,11,0,0,12,13,14,15,16,17,0,0,18,20,19,0,0,0,0,0,0,0,21,22,23,24,25,26};
static const uint8_t kA[41] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,9,9,9,9,9,4,4,4,4,4,4,4};

void d2client_sub_1DA90(uint32_t act, uint32_t reset, QuestIconGlobals* g, const QuestLogGlobals* qg, const QuestLogOps* o, const QuestIconOps* x) {
    int32_t count = 0, first = -1;
    int32_t ebp = 0;
    uint8_t S[0x26C]; memset(S, 0, sizeof S);
    uint8_t scratch[32] = {};
    g->result = 0xFFFFFFFFu;
    if (!((x->o10227(x->c) && g->g9854) || act != 4)) return;
    for (int32_t i = kRange[act][0]; i <= kRange[act][1]; ++i)
        if (!g->loaded[i]) { x->path(x->c, (uint32_t)i); g->loaded[i] = x->load(x->c, 0); }
    if (g->cached[act] != 0xFFFFFFFFu) {
        g->result = g->cached[act];
    } else {
        for (;;) {
            if (kA[ebp] == act && kF[ebp] && kI[ebp] < 0x1B && g->loaded[kI[ebp]]) {
                for (int k = 0; k < 300; ++k) o->ctor(o->c, S + 7 + 2 * k);
                uint32_t id = (uint32_t)ebp + 0;   // row id column equals the row number (table dump: ids 0..40)
                memcpy(S + 1, &id, 4);
                d2client_sub_1CE00(S, qg, o, scratch);
                uint32_t mode; memcpy(&mode, S + 0x266, 4);
                const uint8_t c261 = S[0x261];
                if (S[0x263] || mode == 0) { g->result = c261; ebp = 0x29; }
                if (mode == 1 || mode == 2) ++count;
                else if (mode == 3) { if (first == -1) first = c261; ++count; }
            }
            if (++ebp >= 0x29) break;
        }
        if (count && g->result == 0xFFFFFFFFu) {
            uint32_t v = g->fallback[act];
            if (v == 0xFFFFFFFFu) v = (uint32_t)first;
            if (v != 0xFFFFFFFFu) g->result = v;
        }
    }
    if (reset) for (int k = 0; k < 5; ++k) g->cached[k] = 0xFFFFFFFFu;
}
