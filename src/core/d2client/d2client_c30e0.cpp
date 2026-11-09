// 1.13c D2Client+0xC30E0: open the quest / waypoint style message panel (stdcall, one stack argument `mode`, ret 4).
// With a player ([0x11BBFC]) that D2Common 11017 ([player+0x60]) does not veto and no panel open ([0xFAD60] == 0): on the first
// opening ([0xFAD74] == 0) clear [0x11C434] and, if [0x12349C] == 1, hide 0xF51BC (0x3CC60, EAX = 4) and show 0xF51EC once
// (0x3CCC0, EAX = 2, latch [0x123498]). Modes 0..4 pick a string id (0xFC1 / 0xFC1 / 0xFCE / 0xFD1 / 0xFD2) and a stat
// (0xE, mode 4 0xF) whose D2Common 10973 value sizes the panel; modes 1..4 first call 0x987F0, mode 0 sets [0x11BBBC]. The panel
// window (0xA4040) is filled with the word-wrapped string (0xD378 lines, 0xA36F0 each, both Fog-freed) and four child controls
// built on Storm 401 blocks (0x3F500, 0x38A20, 0x66EB0 twice, each attached by 0xA3400). Any other mode, no player or a veto:
// 0xBF720. The `mode == 5` text branch after the switch is unreachable (the switch rejects mode 5) and is not ported.
#include <cstdint>
namespace d2c_c30e0 {
using u32 = uint32_t;
struct Env {
    u32 (*g)(u32 rva); void (*sg)(u32 rva, u32 v);       // module globals by RVA
    u32 (*rd32)(u32 a);
    u32 mod;                                            // address tag of module RVA 0
    u32 (*c11017)(u32 s0);                              // 0xC202 (D2Common 11017), stdcall 1
    void (*fbf720)();                                   // 0xBF720
    u32 (*gfx10048)();                                  // 0xD0B4
    void (*f3cc60)(u32 eax, u32 s0, u32 s1);            // ret 8
    void (*f3ccc0)(u32 eax, u32 s0, u32 s1);            // ret 8
    void (*f987f0)();
    u32 (*c10973)(u32 unit, u32 stat, u32 layer);       // 0xC1C6, stdcall 3
    u32 (*fa4040)(u32 w, u32 h, u32 cb, u32 a, u32 b, u32 c);   // stdcall 6
    void (*fa3340)(u32 s0);                             // stdcall 1
    void (*d3a8)(u32 ecx);                              // 0xD3A8
    void (*uctor)(uint16_t* p);                         // D2Lang Unicode default constructor (IAT 0xCEB00, ECX = this)
    u32 (*d1ec)(u32 id);                                // 0xD1EC (ECX = string id): the Unicode text
    void (*ustrcpy)(uint16_t* dst, u32 src);            // D2Lang Unicode::strcpy (IAT 0xCEAB8, ECX = dst, EDX = src)
    u32 (*d378)(uint16_t* text, u32* out, u32 width);   // 0xD378 (ECX, EDX, one stack word): the wrapped line list
    void (*fa36f0)(u32 eax, u32 ecx, u32 s0, u32 s1, u32 s2, u32 s3, u32 s4, u32 s5);   // stdcall 6
    void (*fog10043)(u32 ptr, u32 file, u32 line, u32 z);    // 0xBF86 (ECX, EDX, two stack words)
    u32 (*storm401)(u32 size, u32 file, u32 line, u32 flags);   // 0xBE66, stdcall 4
    u32 (*f3f500)(u32 p, u32 w, u32 h, u32 z, u32 cb);  // stdcall 5
    u32 (*f38a20)(u32 p, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 cb, u32 g);   // stdcall 9
    u32 (*f66eb0)(u32 p, u32 w, u32 h, u32 z, u32 cb);  // stdcall 5
    void (*fa3400)(u32 esi, u32 s0);                    // ESI = window, stdcall 1
    void (*f595c0)();
    void (*vcall)(u32 obj, u32 off, u32 arg);           // thiscall [[obj]+off](obj, arg), ret 4
};
void c30e0(const Env& e, u32 mode) {
    const u32 pl = e.g(0x11bbfc);
    if (!pl || e.c11017(e.rd32(pl + 0x60))) { e.fbf720(); return; }
    if (e.g(0xfad60)) return;
    if (!e.g(0xfad74)) {
        e.sg(0x11c434, 0);
        if (e.g(0x12349c) == 1) {
            e.sg(0x12349c, 0);
            e.f3cc60(4, e.gfx10048(), e.mod + 0xf51bc);
            if (!e.g(0x123498)) { e.f3ccc0(2, e.gfx10048(), e.mod + 0xf51ec); e.sg(0x123498, 1); }
        }
        e.sg(0xfad74, 1);
    }
    e.sg(0x11bbb0, 0);
    if (mode > 4) { e.fbf720(); return; }
    static const u32 kStr[5] = { 0xfc1, 0xfc1, 0xfce, 0xfd1, 0xfd2 };
    const u32 sid = kStr[mode];
    if (mode == 0) e.sg(0x11bbbc, 1); else e.f987f0();
    const u32 sv = e.c10973(e.g(0x11bbfc), mode == 4 ? 0xf : 0xe, 0);
    e.sg(0xfad5c, mode);
    e.sg(0xfad60, e.fa4040(0xd7, 0x8c, e.mod + 0xbfeb0, 0, 0, 1));
    e.fa3340(1);
    e.d3a8(1);
    uint16_t buf[0x100]; u32 out = 0;
    for (u32 i = 0; i < 0x100; ++i) e.uctor(buf + i);
    for (u32 i = 0; i < 0x100; ++i) buf[i] = 0;
    e.ustrcpy(buf, e.d1ec(sid));
    for (u32 n = e.d378(buf, &out, 0xc8); n;) {
        e.fa36f0(4, e.g(0xfad60), e.rd32(n), 0x18, 0, 1, 0, 0);
        const u32 nx = e.rd32(n + 4);
        e.fog10043(e.rd32(n), e.mod + 0xd34c8, 0x182b, 0);
        e.fog10043(n, e.mod + 0xd34c8, 0x182c, 0);
        n = nx;
    }
    u32 p = e.storm401(0xc, e.mod + 0xd04f0, 0xffffffff, 0);
    u32 o = p ? e.f3f500(p, 0xdf, 0xdb, 0, e.mod + 0xbe190) : 0;
    e.sg(0xfad64, o); e.fa3400(e.g(0xfad60), o);
    p = e.storm401(0xc, e.mod + 0xd04f0, 0xffffffff, 0);
    o = p ? e.f38a20(p, 0x102, 0xe4, 0x64, 1, sv, 0xa, e.mod + 0xbe230, 1) : 0;
    e.sg(0xfad68, o); e.fa3400(e.g(0xfad60), o);
    if (e.g(0xfad68)) e.vcall(e.g(0xfad68), 0x44, 1);
    p = e.storm401(0xc, e.mod + 0xd04f0, 0xffffffff, 0);
    o = p ? e.f66eb0(p, 0xfa, 0x11f, 0, e.mod + 0xbfdf0) : 0;
    e.sg(0xfad6c, o); e.fa3400(e.g(0xfad60), o);
    e.vcall(e.g(0xfad6c), 0x30, 1);
    p = e.storm401(0xc, e.mod + 0xd04f0, 0xffffffff, 0);
    o = p ? e.f66eb0(p, 0x163, 0x11f, 1, e.mod + 0xbfde0) : 0;
    e.sg(0xfad70, o); e.fa3400(e.g(0xfad60), o);
    if (mode == 2) { e.f595c0(); return; }
    if (mode == 3 && e.g(0xfad68)) e.vcall(e.g(0xfad68), 0x38, sv);
}
} // namespace d2c_c30e0
