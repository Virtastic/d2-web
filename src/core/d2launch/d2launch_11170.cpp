// 1.13c D2Launch+0x11170  sub_11170: load the character-create frontend images once. Each image is wsprintfA(buf, fmt, "DATA\\GLOBAL")
// (user32, IAT 0x1C44C) then D2Win 10111 (thunk 0x9946, fastcall ECX = buf, EDX = 0) into a module dword. Fog 10227 (thunk 0x980E) picks the
// expansion screen and gates the assassin/druid set. Literals are image RVAs; g = the module range [0x25C10, 0x25DB4).
#include <cstdint>

struct D2l11170Seams {
    void (*wsprintf_)(char* buf, uint32_t fmtRva, uint32_t argRva);   // cdecl, 3 words
    uint32_t (*o9946)(char* ecx, uint32_t edx);                       // D2Win 10111
    uint32_t (*o980e)();                                              // Fog 10227
};

static constexpr uint32_t kBase = 0x25C10, kArg = 0x1E968;
// (format string RVA - 0x10000, destination dword RVA - 0x20000)
static const uint16_t kPre[][2] = { {0xDB90, 0x5C14}, {0xDB78, 0x5CA8}, {0xDB60, 0x5CAC} };
static const uint16_t kExp[][2] = { {0xDB3C, 0x5CB0}, {0xDB1C, 0x5CCC}, {0xDAFC, 0x5CD0}, {0xDADC, 0x5CD4}, {0xDABC, 0x5CD8}, {0xDA9C, 0x5CDC},
                                    {0xDA80, 0x5CE0}, {0xDA64, 0x5CE4}, {0xDA48, 0x5CE8}, {0xDA2C, 0x5CEC}, {0xDA10, 0x5CF0} };
static const uint16_t kAll[][2] = {
    {0xD9F4, 0x5CF4}, {0xD9D8, 0x5CF8}, {0xD9BC, 0x5CFC}, {0xD9A0, 0x5D00}, {0xD984, 0x5D04}, {0xD960, 0x5D08}, {0xD93C, 0x5D0C}, {0xD91C, 0x5D10},
    {0xD8F8, 0x5D14}, {0xD8D8, 0x5D18}, {0xD8B8, 0x5D1C}, {0xD898, 0x5D20}, {0xD878, 0x5D24}, {0xD858, 0x5D28}, {0xD838, 0x5D2C}, {0xD818, 0x5D30},
    {0xD7F8, 0x5D34}, {0xD7D8, 0x5D38}, {0xD7B8, 0x5D3C}, {0xD798, 0x5D40}, {0xD778, 0x5D44}, {0xD758, 0x5D48}, {0xD73C, 0x5D4C}, {0xD71C, 0x5D50},
    {0xD700, 0x5D54}, {0xD6E0, 0x5D88}, {0xD6C0, 0x5D8C}, {0xD6A0, 0x5D90}, {0xD67C, 0x5D74}, {0xD658, 0x5D78}, {0xD634, 0x5D7C}, {0xD614, 0x5D9C},
    {0xD5F4, 0x5DB0} };

extern "C" void d2l_11170(const D2l11170Seams* s, uint8_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - kBase)); };
    if (W(0x25C10)) return;
    char buf[260];
    auto ld = [&](uint32_t fmt, uint32_t dst) { s->wsprintf_(buf, fmt, kArg); W(dst) = s->o9946(buf, 0); };
    ld(0x1DBF0, 0x25C30);
    ld(s->o980e() ? 0x1DBC4 : 0x1DBA4, 0x25C10);
    for (auto& e : kPre) ld(0x10000u | e[0], 0x20000u | e[1]);
    if (s->o980e())
        for (auto& e : kExp) ld(0x10000u | e[0], 0x20000u | e[1]);
    for (auto& e : kAll) ld(0x10000u | e[0], 0x20000u | e[1]);
}
