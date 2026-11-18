// 1.13c D2Launch+0xD160  sub_D160: character-select screen build. Every callee is a seam taking the retail register layout
// (ecx/edx/eax as named); literals are passed as image RVAs. g points at the module range [0x24DB0, 0x25EE8), the globals
// this function reads and writes. The fail path calls the exit seam and, as retail does when the seam returns, continues.
#include <cstdint>
#include <cstring>

struct D2lD160Seams {
    void (*f55d30)(); void (*f52bf0)(uint32_t eaxRva, uint32_t strRva);
    uint32_t (*o9952)(uint32_t strRva);                 // Ordinal 10098: widget by definition table
    void (*o99f4)(uint32_t ecx, uint32_t edx);
    uint32_t (*o9820)(uint32_t id);                     // Ordinal 10265 (cdecl)
    void (*o97f6)(uint32_t fmtRva, uint32_t a, uint32_t b);
    void (*exit_)(uint32_t code);
    void (*o9a12)(uint32_t ecx, uint32_t edx); void (*o9a00)(uint32_t ecx, uint32_t edx);
    uint32_t (*o980e)();
    void (*o9976)(uint32_t ecx, uint32_t edx); void (*o99d0)(uint32_t ecx);
    uint32_t (*fc430)();
    void (*o9862)(uint32_t ecx, uint32_t edx); void (*o996a)(uint32_t ecx);
    void (*o98ce)(uint32_t ecx, uint32_t edx, uint32_t s0);
    void (*fcd70)(); void (*fae60)();
    uint32_t (*o9b32)(uint32_t ecx);
    void (*fbeb0)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void (*fcc50)(); void (*fa8c0)(); void (*fb600)();
    void (*o99ee)(uint32_t ecx, uint32_t edx);
    uint32_t (*o9a4e)(); void (*o9a72)(uint32_t ecx);
};

extern "C" void d2l_d160(const D2lD160Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - 0x24DB0)); };
    int32_t i, cnt;
    uint32_t v, ui = W(0x25ABC);
    s->f55d30();
    s->f52bf0(0x1E99C, 0x1E988);
    *(uint8_t*)(uintptr_t)(ui + 0x20C) = 0;
    cnt = (int32_t)W(0x25E30);
    W(0x25ED0) = 0; W(0x25ED4) = 0; W(0x24DB0) = 0; W(0x25EB0) = 1; W(0x25984) = 0; W(0x25EE0) = 0; W(0x25EDC) = 0; W(0x25ED8) = 0;
    if (cnt >= 0x40) goto fail1;
    {   // widgets 1..6: array slot, count, optional global; each re-tests the count
        static const struct { uint32_t str, glob; } T[6] = { {0x22D50, 0}, {0x22D80, 0x25980}, {0x22DB0, 0x2570C}, {0x22DE0, 0x25AAC}, {0x22E10, 0x25998}, {0x22E40, 0x25708} };
        for (i = 0; i < 6; i++) {
            v = s->o9952(T[i].str);
            cnt = (int32_t)W(0x25E30);
            W(0x252B8 + cnt * 4) = v; cnt++;
            W(0x25E30) = (uint32_t)cnt;
            if (T[i].glob) W(T[i].glob) = v;
            if (cnt >= 0x40) goto fail1;
        }
    }
    v = s->o9952(0x22C60);
    cnt = (int32_t)W(0x25E30); W(0x252B8 + cnt * 4) = v; cnt++; W(0x25E30) = (uint32_t)cnt; W(0x25878) = v;
    s->o99f4(W(0x25AAC), 0x5524); s->o99f4(W(0x25998), 0x58CA); s->o99f4(W(0x25708), 0x5524);
    if ((int32_t)W(0x25E30) < 0x40) goto after_fail1;
fail1:
    v = s->o9820(0x624); s->o97f6(0x1E256, v, 0x624); s->exit_(0xFFFFFFFFu);
after_fail1:
    v = s->o9952(0x22E70);
    cnt = (int32_t)W(0x25E30); W(0x252B8 + cnt * 4) = v; W(0x25E30) = (uint32_t)(cnt + 1); W(0x25AA0) = v;
    s->o9a12(v, 0); s->o9a00(W(0x25AA0), 0xC250);
    for (i = 0; i < 8; i++) W(0x25734 + i * 4) = 0;
    W(0x24DBC) = 8;
    for (i = 0; ; ) {
        if (s->o980e()) {
            if ((int32_t)W(0x25E30) >= 0x40) goto fail2;
            v = s->o9952(0x227E0 + i * 0x30 + 0x180);
            cnt = (int32_t)W(0x25E30); W(0x252B8 + cnt * 4) = v; W(0x25E30) = (uint32_t)(cnt + 1);
            W(0x25858 + i * 4) = v;
            s->o9976(v, 0x1EB14);
        }
        s->o99d0(1);
        if ((int32_t)W(0x25E30) >= 0x40) goto fail2;
        v = s->o9952(0x227E0 + i * 0x30);
        cnt = (int32_t)W(0x25E30); W(0x252B8 + cnt * 4) = v; W(0x25E30) = (uint32_t)(cnt + 1);
        W(0x25734 + i * 4) = v;
        s->o99d0(0);
        W(0x25710 + i * 4) = (uint32_t)((i - (i >> 31)) >> 1) * 0x5D + 0xB2;
        i++;
        if (i >= (int32_t)W(0x24DBC)) break;
    }
    if ((int32_t)W(0x25E30) < 0x40) goto after_fail2;
fail2:
    v = s->o9820(0x624); s->o97f6(0x1E256, v, 0x624); s->exit_(0xFFFFFFFFu);
after_fail2:
    v = s->o9952(0x22B40);
    cnt = (int32_t)W(0x25E30); W(0x252B8 + cnt * 4) = v; W(0x25E30) = (uint32_t)(cnt + 1); W(0x25AB0) = v;
    if (!s->fc430()) return;
    s->o9862(W(0x2570C), 0);
    v = W(0x25AD4);
    W(0x25EE4) = 0;
    if (v == 1) {
        s->o996a(W(0x25AA8)); s->o98ce(W(0x25AA8), 0x25758, 0); s->fcd70();
    } else if (v == 0 || v == 3 || v == 2) {
        s->fae60();
        if (!s->o980e()) {
            uint32_t a = s->o9b32(0x1419);
            uint32_t b = s->o9b32(W(0x25AD4) == 1 ? 0x141B : 0x141A);
            W(0x24DB4) = W(0x25ED0);
            s->fbeb0(a, b, 1, 0xC);
        }
        s->fcc50();
    }
    s->fa8c0(); s->fb600();
    {
        int32_t d = (int32_t)W(0x25ED0) - (int32_t)W(0x24DBC);
        if (d > 0) s->o99ee(W(0x25AA0), (uint32_t)(d / 2 + (d & 1)));
    }
    if (s->o9a4e()) s->o9a72(1);
    s->o9862(W(0x2570C), 1);
}
