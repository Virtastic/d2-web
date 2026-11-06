// 1.13c D2Launch+0xF800  sub_F800: per-frame tick of the connecting dialog. With the dialog control at 0x25548 present: the BNET object's
// slot +0x80 (ECX = &status) gives a status, 0..6 map to string ids 0x1433..0x1439 (else -1) into 0x25550; D2Win 0x996A (ECX) on the dialog,
// the string drawn through D2Win 10000 when the id is valid, then the dots label: _sprintf(buf, "%.*s", (now - start)/250 % 20 + 1,
// "....................") with start at 0x25558, D2Win 0x996A and 0x9976 (ECX = label 0x2554C, EDX = buf). cdecl, no arguments.
// g = the module range [0x25200, 0x25EC0); literals are image RVAs.
#include <cstdint>

struct D2lF800Seams {
    void (*bnet80)(uint32_t bnet, uint32_t* ecx);                          // call [bnet + 0x80] (EAX = bnet)
    void (*o996a)(uint32_t ecx);
    const char16_t* (*o9b32)(uint32_t ecx);                                // D2Lang 10003
    void (*o99be)(uint32_t ecx, const char16_t* edx, uint32_t s0, uint32_t s1, uint32_t s2);   // D2Win 10000
    uint32_t (*tick)();                                                    // kernel32 GetTickCount (IAT 0x1C364)
    void (*sprintf_)(char* buf, uint32_t fmtRva, uint32_t n, uint32_t strRva);   // _sprintf 0x101B, cdecl
    void (*o9976)(uint32_t ecx, const char* edx);
};

static constexpr uint32_t kBase = 0x25200;

extern "C" void d2l_f800(const D2lF800Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - kBase)); };
    if (!W(0x25548)) return;
    uint32_t st;
    char buf[0x100];
    s->bnet80(W(0x256C4), &st);
    W(0x25550) = st <= 6 ? 0x1433 + st : 0xFFFFFFFFu;
    s->o996a(W(0x25548));
    if ((int32_t)W(0x25550) >= 0) s->o99be(W(0x25548), s->o9b32(W(0x25550)), 0, 0, 0);
    const uint32_t q = (s->tick() - W(0x25558)) / 250u % 20u + 1u;
    s->sprintf_(buf, 0x1E9E0, q, 0x1E9E8);
    s->o996a(W(0x2554C));
    s->o9976(W(0x2554C), buf);
}
