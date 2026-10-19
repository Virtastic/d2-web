// 1.13c D2Client+0x0B17F (setSBCS). The locale state is module-global in retail, so the
// deterministic model passes its byte-addressed global window explicitly.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2ClientSbcsGlobals {
    uint32_t at_12372c;             // module RVA 0x12372C
    uint8_t  gap_123730[4];
    uint32_t at_123734;             // module RVA 0x123734
    uint8_t  gap_123738[8];
    uint8_t  table_123740[0x100];   // module RVA 0x123740: 0x40 dwords
    uint8_t  byte_123840;           // module RVA 0x123840
    uint8_t  gap_123841[3];
    uint32_t at_123844;             // module RVA 0x123844
    uint8_t  gap_123848[8];
    uint32_t at_123850;             // module RVA 0x123850
    uint32_t at_123854;             // module RVA 0x123854
    uint32_t at_123858;             // module RVA 0x123858
};
static_assert(offsetof(D2ClientSbcsGlobals, at_12372c) == 0x000);
static_assert(offsetof(D2ClientSbcsGlobals, at_123734) == 0x008);
static_assert(offsetof(D2ClientSbcsGlobals, table_123740) == 0x014);
static_assert(offsetof(D2ClientSbcsGlobals, byte_123840) == 0x114);
static_assert(offsetof(D2ClientSbcsGlobals, at_123844) == 0x118);
static_assert(offsetof(D2ClientSbcsGlobals, at_123850) == 0x124);
static_assert(offsetof(D2ClientSbcsGlobals, at_123854) == 0x128);
static_assert(offsetof(D2ClientSbcsGlobals, at_123858) == 0x12C);
static_assert(sizeof(D2ClientSbcsGlobals) == 0x130);

// The CRT's SBCS baseline clears 0x40 dwords, the following byte, and six state dwords.
// Intervening bytes are not written by retail and remain untouched.
extern "C" void D2Client_SetSBCS(uint8_t* global_window) {
    auto* g = reinterpret_cast<D2ClientSbcsGlobals*>(global_window);
    std::memset(g->table_123740, 0, sizeof g->table_123740);
    g->byte_123840 = 0;
    g->at_123844 = 0;
    g->at_123734 = 0;
    g->at_12372c = 0;
    g->at_123850 = 0;
    g->at_123854 = 0;
    g->at_123858 = 0;
}
