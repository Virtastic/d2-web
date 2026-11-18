// D2Client sub_47B20: close the unit-interaction dialog for unit id EDI.
// Globals and callees arrive through H; every unit / record word is the retail 32-bit layout.
#include <cstdint>
#include <cstring>
namespace d2client_47b20 {
using U = uint32_t; using I = int32_t; using P = uint8_t*;
struct H {
    U* player;      // [0x11BBFC] player unit (+0 type, +0x14 player data)
    U* unitSlots;   // [0x10A808] 128 unit-hash heads, chained through unit+0xE4
    U* a3aba;       // [0x113ABA] dword
    U* a3b10;       // [0x113B10] dword handle, released through 0xCADE
    U* a9721;       // [0x119721] dword
    void (*f46a60)(U id);                  // 0x46A60 (ECX = id)
    void (*send)(const uint8_t* b, I n);   // 0x143E0 (stack buf, EBX = length)
    void (*f7cef0)();                      // 0x7CEF0
    void (*freeHandle)(U h);               // 0xCADE thunk (1 stack)
    void (*fc2790)(I ecx, I edx, I s0);    // 0xC2790
    I (*o11090)(U unit, I n);              // 0xC43C thunk (Ordinal_11090)
    void (*f6a220)(U unit, I s0);          // 0x6A220 (EAX = unit)
    void (*f2a590)(I ecx, U unit);         // 0x2A590 (ECX, EDX = unit)
    void (*f24490)(U unit, I s0, I s1);    // 0x24490 (EAX = unit)
    void (*fatal)(I line);                 // Ordinal_10265(line) + 10024 + exit(-1); never returns
};
static U rd(U a, U o) { U v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static void wr(U a, U o, U v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }

// 1.13c D2Client+0x47B20
void sub_47B20(const H& h, U id) {
    const U pl = *h.player;
    if (pl && rd(pl, 0) == 0 && rd(pl, 0x14)) {
        wr(rd(pl, 0x14), 0x150, 0); wr(rd(pl, 0x14), 0x154, 0); wr(rd(pl, 0x14), 0x158, 0); wr(rd(pl, 0x14), 0x15C, 0);
    }
    h.f46a60(id);
    U u = h.unitSlots[id & 0x7F];
    while (u && rd(u, 0xC) != id) u = rd(u, 0xE4);
    if (u) {
        if (rd(u, 0) != 1) h.fatal(0x5A);
        wr(u, 0xC4, rd(u, 0xC4) | 2);
    }
    uint8_t pkt[9] = { 0x30, 1, 0, 0, 0 };
    std::memcpy(pkt + 5, &id, 4);
    h.send(pkt, 9);
    h.f7cef0();
    *h.a3aba = 0;
    if (*h.a3b10) { h.freeHandle(*h.a3b10); *h.a3b10 = 0; }
    *h.a9721 = 0;
    h.fc2790(8, 1, 0);
    if (!u) return;
    if (rd(u, 0) == 1 && rd(u, 0x14)) { const U r = rd(u, 0x14); *((uint8_t*)(uintptr_t)r + 0x28) &= 0xFE; }
    wr(u, 0xC4, rd(u, 0xC4) & 0xFFFFFFBFu);
    if (h.o11090(u, 1)) { h.f6a220(u, 0); h.f2a590(1, u); h.f24490(u, 1, 0); }
}
}  // namespace d2client_47b20

extern "C" __attribute__((used, visibility("default")))
void d2_client_47b20(uint32_t id, const d2client_47b20::H* h) { d2client_47b20::sub_47B20(*h, id); }
