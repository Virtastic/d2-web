// 1.13c D2Gfx+0x053C8 (setSBCS). The locale state is module-global in retail, so the
// deterministic model passes its byte-addressed global window explicitly.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2GfxSbcsGlobals {
    uint32_t at_1d6ac;             // module RVA 0x1D6AC
    uint8_t  gap_1d6b0[4];
    uint32_t at_1d6b4;             // module RVA 0x1D6B4
    uint8_t  gap_1d6b8[8];
    uint8_t  table_1d6c0[0x100];   // module RVA 0x1D6C0: 0x40 dwords
    uint8_t  byte_1d7c0;           // module RVA 0x1D7C0
    uint8_t  gap_1d7c1[3];
    uint32_t at_1d7c4;             // module RVA 0x1D7C4
    uint8_t  gap_1d7c8[8];
    uint32_t at_1d7d0;             // module RVA 0x1D7D0
    uint32_t at_1d7d4;             // module RVA 0x1D7D4
    uint32_t at_1d7d8;             // module RVA 0x1D7D8
};
static_assert(offsetof(D2GfxSbcsGlobals, at_1d6ac) == 0x000);
static_assert(offsetof(D2GfxSbcsGlobals, at_1d6b4) == 0x008);
static_assert(offsetof(D2GfxSbcsGlobals, table_1d6c0) == 0x014);
static_assert(offsetof(D2GfxSbcsGlobals, byte_1d7c0) == 0x114);
static_assert(offsetof(D2GfxSbcsGlobals, at_1d7c4) == 0x118);
static_assert(offsetof(D2GfxSbcsGlobals, at_1d7d0) == 0x124);
static_assert(offsetof(D2GfxSbcsGlobals, at_1d7d4) == 0x128);
static_assert(offsetof(D2GfxSbcsGlobals, at_1d7d8) == 0x12C);
static_assert(sizeof(D2GfxSbcsGlobals) == 0x130);

// The CRT's SBCS baseline clears 0x40 dwords, the following byte, and six state dwords.
// Intervening bytes are not written by retail and remain untouched.
extern "C" void D2Gfx_SetSBCS(uint8_t* global_window) {
    auto* g = reinterpret_cast<D2GfxSbcsGlobals*>(global_window);
    std::memset(g->table_1d6c0, 0, sizeof g->table_1d6c0);
    g->byte_1d7c0 = 0;
    g->at_1d7c4 = 0;
    g->at_1d6b4 = 0;
    g->at_1d6ac = 0;
    g->at_1d7d0 = 0;
    g->at_1d7d4 = 0;
    g->at_1d7d8 = 0;
}
