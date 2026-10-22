// D2Client batch b71: seam-based, no Win32.
#include <cstdint>
#include <initializer_list>

struct B71Env {
    int32_t** listHead;                   // &DAT_6fbcbf30 (node: [0]=entry*, [1]=next)
    int32_t (*unitCheck)();               // Ordinal_10053
    int32_t* dat6fbc9854;                 // expansion/hires flag
    int32_t* rectA;                       // DAT_6fbbb39c..3a8 {x0,r,y0,y1}-> [0]=DAT_..39c [1]=3a0 [2]=3a4 [3]=3a8
    int32_t* rectB;                       // DAT_6fbbb2d4..2e0, same order
    uintptr_t* bankCel;                   // DAT_6fbcbc74
    uintptr_t* tradeStashCel;             // DAT_6fbcbc84
    uintptr_t* buySellTabsCel;            // DAT_6fbcbc6c
    uintptr_t* miniConvertCel;            // DAT_6fbcbc70
    uintptr_t* invChar6TabCel;            // DAT_6fbcbc80
    int32_t* dat6fbbb348;                 // _DAT_6fbbb348
    uintptr_t (*loadCel)(const char* path); // FUN_6fadb420(0) after Ordinal_578 formatting
    int32_t (*expansion)();               // Ordinal_10227
};
static B71Env b71_env;
extern "C" void b71_set_env(const B71Env* e) { b71_env = *e; }

// 1.13c D2Client+0x87BD0
extern "C" int32_t* b71_sub_87BD0(int32_t* self, int32_t flag) {
    for (int32_t* n = *b71_env.listHead; n; n = (int32_t*)(intptr_t)n[1]) {
        int32_t* e = (int32_t*)(intptr_t)n[0];
        if (e != self && e[0x30 / 4] == 1 && (flag == 0 || b71_env.unitCheck() != 0)
            && e[1] > 0xb75 && e[1] < 0x1231) return e;
    }
    return nullptr;
}

// 1.13c D2Client+0x8C4C0
extern "C" int32_t b71_sub_8C4C0(int32_t x, int32_t y) {
    const int32_t* r = *b71_env.dat6fbc9854 ? b71_env.rectB : b71_env.rectA;
    return r[0] < x && x <= r[1] && r[2] <= y && y <= r[3];
}

// 1.13c D2Client+0x8CBC0
extern "C" void b71_sub_8CBC0() {
    *b71_env.dat6fbbb348 = 0;
    if (*b71_env.bankCel) return;
    *b71_env.bankCel = b71_env.loadCel("DATA\\GLOBAL\\ui\\panel\\bank");
    if (b71_env.expansion()) *b71_env.tradeStashCel = b71_env.loadCel("DATA\\GLOBAL\\ui\\panel\\TradeStash");
}

// 1.13c D2Client+0x8CE10
extern "C" void b71_sub_8CE10() {
    if (!*b71_env.buySellTabsCel) *b71_env.buySellTabsCel = b71_env.loadCel("DATA\\GLOBAL\\ui\\panel\\buyselltabs");
    if (!*b71_env.miniConvertCel) *b71_env.miniConvertCel = b71_env.loadCel("DATA\\GLOBAL\\ui\\panel\\miniconvert");
    if (!*b71_env.invChar6TabCel && b71_env.expansion() && *b71_env.dat6fbc9854)
        *b71_env.invChar6TabCel = b71_env.loadCel("DATA\\GLOBAL\\ui\\panel\\invchar6tab");
}
