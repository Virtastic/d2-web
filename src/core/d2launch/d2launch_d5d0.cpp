// 1.13c D2Launch+0xD5D0  sub_D5D0: realm / character-list screen build. Every callee is a seam taking the retail register layout (names
// give the register or stack word); literals are image RVAs. g points at the module range [0x24DB0, 0x25EE8). The overflow exit calls the
// exit seam and, as retail does when the seam returns, falls into the tail that follows it.
#include <cstdint>
#include <cstring>

struct D2lD5D0Seams {
    uint32_t (*o980e)(); void (*fd160)();
    void (*f15d30)(); void (*o98e6)(uint32_t s0); void (*f12bf0)(uint32_t eaxRva, uint32_t strRva);
    void (*f15b60)(); void (*fe250)();
    uint32_t (*o9b32)(uint32_t ecx);
    void (*f15090)(uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*f12b10)(uint32_t eax);
    void (*o99ac)(uint32_t ecx); void (*o9904)(uint32_t ecx);
    void (*o9862)(uint32_t ecx, uint32_t edx); void (*o987a)(uint32_t ecx, uint32_t edx); void (*o99d0)(uint32_t ecx);
    uint32_t (*o9952)(uint32_t strRva);
    void (*o9a36)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
    void (*o99be)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*o9754)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    uint32_t (*fbe50)();                         // EDI = 0x25758 at the call
    uint32_t (*gw98)(uint32_t gw);               // call [gw + 0x98]; returns the realm record pointer
    uint32_t (*o9b44)(uint32_t p, uint32_t ecx);
    void (*o9790)(uint32_t bufRva, uint32_t src, uint32_t max);
    void (*o9796)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    uint32_t (*o9820)(uint32_t id); void (*o97f6)(uint32_t fmtRva, uint32_t a, uint32_t b); void (*exit_)(uint32_t code);
    void (*fd050)(uint32_t eaxRva);
    uint32_t (*o9a4e)(); void (*o9a72)(uint32_t ecx);
};

extern "C" void d2l_d5d0(const D2lD5D0Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - 0x24DB0)); };
    auto D = [](uint32_t p) -> uint32_t { return *(uint32_t*)(uintptr_t)p; };
    int32_t i, cnt;
    uint32_t v, id;
    auto push = [&](uint32_t val) { cnt = (int32_t)W(0x25E30); W(0x252B8 + cnt * 4) = val; W(0x25E30) = (uint32_t)(cnt + 1); };
    if (s->o980e()) { s->fd160(); return; }
    s->f15d30();
    s->o98e6(W(0x25C54));
    s->f12bf0(0x1E99C, 0x1E988);
    if (W(0x25AD4) == 1) {
        s->f15b60();
        int32_t e78 = (int32_t)W(0x25E78);
        if (e78 == -1) { s->fe250(); s->f15090(s->o9b32(0x14E3), 0x17DF0, 1); return; }
        if (e78 == 0) { s->fe250(); s->f15090(s->o9b32(0x14AA), 0x17DF0, 1); return; }
    }
    W(0x25EB0) = 1; W(0x25ED0) = 0; W(0x25ED4) = 0; W(0x24DB0) = 0; W(0x25984) = 0; W(0x25EE0) = 0; W(0x25EDC) = 0; W(0x25EB8) = 0;
    v = s->f12b10(0x7A);
    s->o99ac(v); s->o9904(v);
    *(uint8_t*)(uintptr_t)(W(0x25ABC) + 0x20C) = 0;
    W(0x2570C) = s->f12b10(0x7C);
    v = s->f12b10(0x7D); W(0x25708) = v; s->o9862(v, 0);
    W(0x25980) = s->f12b10(0x7E);
    W(0x25730) = s->f12b10(0x99);
    v = s->f12b10(0x9A); W(0x2587C) = v;
    s->o987a(W(0x25730), 0); s->o987a(W(0x2587C), 0);
    W(0x25878) = s->f12b10(0x9B);
    W(0x25704) = s->f12b10(0x9D);
    W(0x25988) = s->f12b10(0x9E);
    v = s->f12b10(0x9F);
    for (i = 0; i < 8; i++) W(0x25734 + i * 4) = 0;
    W(0x25AAC) = v;
    if (W(0x25AD4) == 1) {
        W(0x24DBC) = 4;
        uint32_t ebx = 0xF6, edi = 0x22720;
        i = 0;
        do {
            s->o99d0(1);
            if ((int32_t)W(0x25E30) >= 0x40) goto fail;
            v = s->o9952(edi); push(v); W(0x25734 + i * 4) = v;
            s->o99d0(0);
            W(0x25710 + i * 4) = ebx;
            i++; edi += 0x30; ebx += 0x5D;
        } while (i < (int32_t)W(0x24DBC));
        if ((int32_t)W(0x25E30) >= 0x40) goto fail;
        v = s->o9952(0x22BA0); push(v); W(0x25EDC) = v;
        if (cnt + 1 >= 0x40) goto fail;
        v = s->o9952(0x225D0); push(v); W(0x25EE0) = v;
        s->o9a36(W(0x25730), 0x27A, 0x8F, 0x1D, 0x14);
        if ((int32_t)W(0x25E30) >= 0x40) goto fail;
        v = s->o9952(0x22210); push(v);
        if (cnt + 1 >= 0x40) goto fail;
        v = s->o9952(0x22270); push(v); W(0x25AB4) = v;
        if (cnt + 1 >= 0x40) goto fail;
        v = s->o9952(0x222A0); push(v); W(0x25AA8) = v;
        if (cnt + 1 >= 0x40) goto fail;
        v = s->o9952(0x22240); push(v);
        s->o99be(v, s->o9b32(0x2B32), 4, 0, 0);
        if (s->o9754(0x1E204, 0x1DC80, 0, 0x25758, 0x100) == 0 || s->fbe50() == 0) {
            uint32_t rl = s->gw98(W(0x256C4));
            v = s->o9b44(rl, D(rl + 0xC));
            s->o9790(0x25758, v, 0x7FFFFFFF);
            s->o9796(0x1E204, 0x1DC80, 0, 0x25758);
            if (s->fbe50() == 0) {
                if ((int32_t)W(0x25E78) > 0) {
                    uint32_t p = D(W(0x25E74));
                    if (p == 0) { id = 0x1143; goto fail_id; }
                    v = D(p);
                } else v = 0;
                s->o9790(0x25758, v, 0x7FFFFFFF);
                s->o9796(0x1E204, 0x1DC80, 0, 0x25758);
            }
        }
    } else {
        W(0x24DBC) = 5;
        uint32_t ebx = 0x99, edi = 0x226F0;
        i = 0;
        do {
            s->o99d0(1);
            if ((int32_t)W(0x25E30) >= 0x40) goto fail;
            v = s->o9952(edi); push(v); W(0x25734 + i * 4) = v;
            s->o99d0(0);
            W(0x25710 + i * 4) = ebx;
            i++; edi += 0x30; ebx += 0x5D;
        } while (i < (int32_t)W(0x24DBC));
        s->o9a36(W(0x25730), 0x27A, 0x32, 0x1D, 0x14);
    }
    if ((int32_t)W(0x25E30) >= 0x40) goto fail;
    v = s->o9952(0x22B40); push(v); W(0x25AB0) = v;
    if (cnt + 1 < 0x40) goto tail;
fail:
    id = 0x624;
fail_id:
    v = s->o9820(id); s->o97f6(0x1E256, v, id); s->exit_(0xFFFFFFFFu);
tail:
    v = s->o9952(0x22B70); push(v); W(0x25990) = v;
    s->o987a(v, 0);
    s->o9862(W(0x2570C), 0);
    s->fd050(0x25758);
    if (s->o9a4e()) s->o9a72(1);
    s->o9862(W(0x2570C), 1);
}
