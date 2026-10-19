// 1.13c D2Game+0x0790D (setSBCS). Model the module's CRT locale globals as
// an explicit byte window so the reset has no process-global side effects.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2GameSbcsGlobals {
    uint32_t at_19fd0;             // module RVA 0x19FD0
    uint8_t  gap_19fd4[4];
    uint32_t at_19fd8;             // module RVA 0x19FD8
    uint8_t  gap_19fdc[4];
    uint8_t  table_19fe0[0x100];   // module RVA 0x19FE0: 0x40 dwords
    uint8_t  byte_1a0e0;           // module RVA 0x1A0E0
    uint8_t  gap_1a0e1[3];
    uint32_t at_1a0e4;             // module RVA 0x1A0E4
    uint8_t  gap_1a0e8[8];
    uint32_t at_1a0f0;             // module RVA 0x1A0F0
    uint32_t at_1a0f4;             // module RVA 0x1A0F4
    uint32_t at_1a0f8;             // module RVA 0x1A0F8
};
static_assert(offsetof(D2GameSbcsGlobals, at_19fd0) == 0x000);
static_assert(offsetof(D2GameSbcsGlobals, at_19fd8) == 0x008);
static_assert(offsetof(D2GameSbcsGlobals, table_19fe0) == 0x010);
static_assert(offsetof(D2GameSbcsGlobals, byte_1a0e0) == 0x110);
static_assert(offsetof(D2GameSbcsGlobals, at_1a0e4) == 0x114);
static_assert(offsetof(D2GameSbcsGlobals, at_1a0f0) == 0x120);
static_assert(offsetof(D2GameSbcsGlobals, at_1a0f4) == 0x124);
static_assert(offsetof(D2GameSbcsGlobals, at_1a0f8) == 0x128);
static_assert(sizeof(D2GameSbcsGlobals) == 0x12C);

// Retail clears the 0x40-dword table, its following byte, and six locale
// state dwords. Gaps between these globals are not written.
extern "C" void D2Game_SetSBCS(uint8_t* global_window) {
    auto* g = reinterpret_cast<D2GameSbcsGlobals*>(global_window);
    std::memset(g->table_19fe0, 0, sizeof g->table_19fe0);
    g->byte_1a0e0 = 0;
    g->at_1a0e4 = 0;
    g->at_19fd8 = 0;
    g->at_19fd0 = 0;
    g->at_1a0f0 = 0;
    g->at_1a0f4 = 0;
    g->at_1a0f8 = 0;
}
