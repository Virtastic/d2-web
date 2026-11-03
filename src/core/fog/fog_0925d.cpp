// 1.13c Fog+0x0925D setSBCS (CRT locale reset). The locale state is module-global in retail, so the
// port takes the byte window that starts at module RVA 0x56EE4 (312 bytes, through RVA 0x5701B).
// Retail writes 0x40 dwords from RVA 0x56F00, one byte at 0x57000, and six state dwords; the bytes
// between them are not written and stay untouched.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace {
struct FogSbcsWindow {
    uint32_t at_56ee4;             // window +0x00
    uint8_t  gap_56ee8[4];
    uint32_t at_56eec;             // window +0x08
    uint8_t  gap_56ef0[0x10];      // window +0x0C..+0x1B
    uint8_t  table_56f00[0x100];   // window +0x1C: 0x40 dwords
    uint8_t  byte_57000;           // window +0x11C
    uint8_t  gap_57001[3];
    uint32_t at_57004;             // window +0x120
    uint8_t  gap_57008[8];
    uint32_t at_57010;             // window +0x12C
    uint32_t at_57014;             // window +0x130
    uint32_t at_57018;             // window +0x134
};
static_assert(offsetof(FogSbcsWindow, table_56f00) == 0x01C);
static_assert(offsetof(FogSbcsWindow, byte_57000) == 0x11C);
static_assert(offsetof(FogSbcsWindow, at_57004) == 0x120);
static_assert(offsetof(FogSbcsWindow, at_57010) == 0x12C);
static_assert(offsetof(FogSbcsWindow, at_57018) == 0x134);
static_assert(sizeof(FogSbcsWindow) == 0x138);
}  // namespace

extern "C" void fog_setSBCS_0925d(uint8_t* window) {
    auto* g = reinterpret_cast<FogSbcsWindow*>(window);
    std::memset(g->table_56f00, 0, sizeof g->table_56f00);
    g->byte_57000 = 0;
    g->at_57004 = 0;
    g->at_56ee4 = 0;
    g->at_56eec = 0;
    g->at_57010 = 0;
    g->at_57014 = 0;
    g->at_57018 = 0;
}
