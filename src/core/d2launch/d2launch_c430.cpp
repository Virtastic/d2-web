// 1.13c D2Launch+0xC430 (sub_C430). No args, plain ret, returns 0 or 1. Realm-list refresh for the BNET screen.
//  DAT_6fa65eb8 := 0; DAT_6fa65ad4 != 1 returns 1. sub_15B60() reloads the list. Count DAT_6fa65e78:
//   -1: gateway DAT_6fa656c4 present: DAT_6fa65eb0 := 1, bytes 0x35d/0x37d of the screen block DAT_6fa65abc := 0, OutputDebugStringA("Disconnect from BNET\n"),
//       thunk 0x9AAE, (*gateway)->vfn[0x10/4](ECX = gateway); then Lang#10003(ECX = 0x14e3) and sub_15090(text, module+0x17df0, 1); returns 0.
//    0: sub_E250(); Lang(ECX = 0x14aa); sub_15090 as above; returns 0.
//   else: handle pushes via sub_12B10(EAX = 0x7a, 0x94, 0x95, 0x69, 0x6b, 0x6c, 0x6a): the first (h) feeds thunks 0x99AC and 0x9904 (ECX = h),
//       0x94 -> DAT_65edc, 0x95 -> DAT_65ee0, 0x6b -> DAT_65ab4, 0x6c -> DAT_65aa8; Lang(ECX = 0x2b32, three stack words popped) is the EDX of thunk 0x99BE
//       (ECX = the 0x6a handle). Then Storm#422 (section module+0x1e204, key module+0x1dc80, default 0, buffer DAT_65758, size 0x100): when it returns 0
//       or sub_BE50(EDI = buffer) returns 0, the gateway block runs: obj := (*gateway)->vfn[0x98/4](); thunk 0x9B44(obj, obj[3]) returns the realm text;
//       Ordinal_501(buffer, text, 0x7fffffff) copies it, Ordinal_425 (section, key, 0, buffer) stores it; if sub_BE50 still returns 0 the entry-0 name
//       sub_B5B0() is copied and stored the same way. Returns 1.
#include <cstdint>

struct D2lC430S {
    void (*f15b60)();
    void (*dbg)(uint32_t text);                           // OutputDebugStringA("Disconnect from BNET\n"), text = module+0x1ea30
    void (*imp9aae)();
    void (*vcall10)(uint32_t gateway);                    // (*gateway)->vfn[0x10/4](), ECX = gateway
    uint32_t (*lang)(uint32_t ecx);                       // thunk 0x9B32 (ECX id)
    void (*f15090)(uint32_t text, uint32_t cb, uint32_t one);
    void (*fe250)();
    uint32_t (*f12b10)(uint32_t idx);
    void (*imp99ac)(uint32_t h);
    void (*imp9904)(uint32_t h);
    void (*imp99be)(uint32_t h, uint32_t text);
    uint32_t (*imp9754)(uint32_t sect, uint32_t key, uint32_t def, uint32_t buf, uint32_t size);
    uint32_t (*fbe50)(uint32_t buf);
    uint32_t (*vcall98)(uint32_t gateway);
    uint32_t (*imp9b44)(uint32_t obj, uint32_t w3);
    void (*imp9790)(uint32_t dst, uint32_t src, uint32_t max);
    void (*imp9796)(uint32_t sect, uint32_t key, uint32_t def, uint32_t buf);
    uint32_t (*fb5b0)(uint32_t prev);
};
struct D2lC430G {
    uint32_t* g25eb8; int32_t g25ad4; int32_t g25e78; uint32_t g256c4; uint8_t* g25abc;
    uint32_t* g25eb0; uint32_t* g25edc; uint32_t* g25ee0; uint32_t* g25ab4; uint32_t* g25aa8;
};

extern "C" int32_t d2l_c430(const D2lC430G* g, uint32_t modbase, const D2lC430S* s) {
    const uint32_t sect = modbase + 0x1e204, key = modbase + 0x1dc80, buf = modbase + 0x25758;
    *g->g25eb8 = 0;
    if (g->g25ad4 != 1) return 1;
    s->f15b60();
    if (g->g25e78 == -1) {
        if (g->g256c4) {
            *g->g25eb0 = 1;
            g->g25abc[0x35d] = 0;
            g->g25abc[0x37d] = 0;
            s->dbg(modbase + 0x1ea30);
            s->imp9aae();
            s->vcall10(g->g256c4);
        }
        s->f15090(s->lang(0x14e3), modbase + 0x17df0, 1);
        return 0;
    }
    if (g->g25e78 == 0) {
        s->fe250();
        s->f15090(s->lang(0x14aa), modbase + 0x17df0, 1);
        return 0;
    }
    uint32_t h = s->f12b10(0x7a);
    s->imp99ac(h);
    s->imp9904(h);
    *g->g25edc = s->f12b10(0x94);
    *g->g25ee0 = s->f12b10(0x95);
    s->f12b10(0x69);
    *g->g25ab4 = s->f12b10(0x6b);
    *g->g25aa8 = s->f12b10(0x6c);
    uint32_t text = s->lang(0x2b32);
    s->imp99be(s->f12b10(0x6a), text);
    uint32_t r = s->imp9754(sect, key, 0, buf, 0x100);
    if (!r || !(r = s->fbe50(buf))) {
        uint32_t obj = s->vcall98(g->g256c4);
        uint32_t w3 = *(const uint32_t*)(uintptr_t)(obj + 0xc);
        s->imp9790(buf, s->imp9b44(obj, w3), 0x7fffffff);
        s->imp9796(sect, key, 0, buf);
        r = s->fbe50(buf);
        if (!r) {
            s->imp9790(buf, s->fb5b0(r), 0x7fffffff);
            s->imp9796(sect, key, 0, buf);
        }
    }
    return 1;
}
