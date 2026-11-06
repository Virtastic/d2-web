// 1.13c D2Launch+0xFC40  sub_FC40: build the connecting dialog. start clock := GetTickCount() at 0x25558; D2Win 10050 (0x9922) releases each
// control of the table 0x253C0[0x25E34] (count re-read each pass), the count := 0, 0x25E40 := 1; five D2Win 10098 (0x9952) controls from
// the templates 0x23980/0x239B0/0x239E0/0x23A10/0x23A40 go into the table, each shown through 0x98B0(h, 1) (the first also 0x995E(h, 0)),
// the third gets D2Lang 10003(0x1433) through D2Win 10000, the fourth and fifth are kept at 0x25548/0x2554C. 0x25550 := -1, 0x25554 := 0,
// 0x2552C := the callback 0xE2C0, then tail-calls 0xF800. A full table (0x40) reports Fog 10024(0x1E256, Fog 10265(0x60D)) and _exit(-1);
// an _exit that returned falls into the fifth control. cdecl, no arguments; g = the module range [0x25200, 0x25EC0).
#include <cstdint>

struct D2lFC40Seams {
    uint32_t (*tick)();                                                    // kernel32 GetTickCount (IAT 0x1C364)
    void (*o9922)(uint32_t* slot);                                         // D2Win 10050, stdcall
    uint32_t (*o9952)(uint32_t tplRva);                                    // D2Win 10098, stdcall
    void (*o995e)(uint32_t h, uint32_t v);                                 // stdcall 2
    void (*o98b0)(uint32_t h, uint32_t v);                                 // stdcall 2
    const char16_t* (*o9b32)(uint32_t ecx);                                // D2Lang 10003
    void (*o99be)(uint32_t ecx, const char16_t* edx, uint32_t s0, uint32_t s1, uint32_t s2);   // D2Win 10000
    uint32_t (*modptr)(uint32_t rva);                                      // the image address of a module RVA
    uint32_t (*o9820)(uint32_t id);                                        // Fog 10265
    void (*o97f6)(uint32_t strRva, uint32_t msg);                          // Fog 10024
    void (*exit_)(int32_t code);
    void (*f800)();                                                        // sub_F800
};

static constexpr uint32_t kBase = 0x25200;

extern "C" void d2l_fc40(const D2lFC40Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - kBase)); };
    auto add = [&](uint32_t tpl) -> uint32_t {
        const uint32_t h = s->o9952(tpl);
        const uint32_t n = W(0x25E34);
        W(0x253C0 + n * 4) = h;
        W(0x25E34) = n + 1;
        return h;
    };
    W(0x25558) = s->tick();
    for (int32_t i = 0; i < (int32_t)W(0x25E34); i++) s->o9922(&W(0x253C0 + (uint32_t)i * 4));
    W(0x25E34) = 0;
    W(0x25E40) = 1;
    {
        const uint32_t h = add(0x23980);
        s->o995e(h, 0);
        s->o98b0(h, 1);
    }
    if ((int32_t)W(0x25E34) >= 0x40) goto fatal;
    s->o98b0(add(0x239B0), 1);
    if ((int32_t)W(0x25E34) >= 0x40) goto fatal;
    {
        const uint32_t h = add(0x239E0);
        s->o98b0(h, 1);
        s->o99be(h, s->o9b32(0x1433), 0, 0, 0);
    }
    if ((int32_t)W(0x25E34) >= 0x40) goto fatal;
    {
        const uint32_t h = add(0x23A10);
        W(0x25548) = h;
        s->o98b0(h, 1);
    }
    if ((int32_t)W(0x25E34) < 0x40) goto last;
fatal:
    s->o97f6(0x1E256, s->o9820(0x60D));
    s->exit_(-1);
last:
    {
        const uint32_t h = add(0x23A40);
        W(0x2554C) = h;
        s->o98b0(h, 1);
    }
    W(0x25550) = 0xFFFFFFFFu;
    W(0x25554) = 0;
    W(0x2552C) = s->modptr(0xE2C0);
    s->f800();
}
