// 1.13c D2Launch+0x13D60 (sub_13D60). No args, plain ret. The gateway caption:
//  gw := (*DAT_6fa656c4)->vfn[0x98/4](); name := BNGatewayAccess::Name(gw, gw[3]) (thunk 0x9B4A, stdcall this + index);
//  Storm 501 SStrCopy(local, name, 0x100) (thunk 0x9790); Unicode::toUnicode(wname, local, 0x100);
//  Unicode::strcpy(wfmt, D2Lang 10003 (thunk 0x9B32, ECX = 0x2B29)); out zeroed for 0x40 dwords;
//  Unicode::sprintf(0x100, out, wfmt, wname); every unit below 0x100 goes through _toUpperTable (strlen re-read per step);
//  thunk 0x990A (ECX = DAT_6fa6551c, EDX = 10, stack out, 0).
#include <cstdint>

struct D2l13d60Seams {
    uint32_t (*vfn98)(uint32_t obj);
    uint32_t (*bnName)(uint32_t gw, int32_t idx);
    void (*sstrcopy)(char* d, const char* s, uint32_t max);
    void (*toUnicode)(uint16_t* d, const char* s, int32_t n);
    uint32_t (*str10003)(uint32_t id);
    void (*ustrcpy)(uint16_t* d, const uint16_t* s);
    void (*usprintf)(int32_t cap, uint16_t* d, const uint16_t* fmt, const uint16_t* a);
    int32_t (*ustrlen)(const uint16_t* s);
    const uint16_t* upper;
    void (*imp990a)(uint32_t h, uint32_t n, const uint16_t* text, uint32_t z);
};

extern "C" void d2l_13d60(uint32_t g256c4, uint32_t g6551c, const D2l13d60Seams* s) {
    uint32_t gw = s->vfn98(g256c4);
    uint32_t name = s->bnName(gw, *(const int32_t*)(uintptr_t)(gw + 0xc));
    char a[0x100];
    s->sstrcopy(a, (const char*)(uintptr_t)name, 0x100);
    uint16_t wname[0x100], wfmt[0x100], out[0x100];
    s->toUnicode(wname, a, 0x100);
    s->ustrcpy(wfmt, (const uint16_t*)(uintptr_t)s->str10003(0x2b29));
    for (int i = 0; i < 0x80; i++) out[i] = 0;
    s->usprintf(0x100, out, wfmt, wname);
    for (int32_t i = 0; i < s->ustrlen(out); i++) {
        uint16_t c = out[i];
        if (c < 0x100) c = s->upper[c];
        out[i] = c;
    }
    s->imp990a(g6551c, 10, out, 0);
}
