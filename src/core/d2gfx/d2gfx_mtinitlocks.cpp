// D2Gfx's VC7.1 CRT lock setup. Retail image base 0x6FA80000.
// The analogous D2MOO 1.10f startup and sibling DLL implementations establish the
// CRT table layout; the 1.13c D2Gfx decompile fixes the table and arena addresses.
#include <cstdint>

struct D2GfxLockSlot {
    void* section;
    uint32_t static_init;
};

// Retail +0x10048: 0x24 pointer/flag pairs; +0x10E00: preallocated sections.
static D2GfxLockSlot g_locks[0x24];
static uint8_t g_sections[0x24 * 0x18];

struct D2GfxMtInitLocksHost {
    int32_t (*init_critical_section)(void* section, uint32_t spin);
};

// 1.13c D2Gfx+0x0191E (__mtinitlocks): assign consecutive 24-byte sections
// only to slots flagged 1, initialize each with spin count 4000, and stop at
// the first failure after clearing that slot. Other entries remain untouched.
extern "C" int32_t d2_fn_d2gfx_191e(const D2GfxMtInitLocksHost* host) {
    uint8_t* next = g_sections;
    for (uint32_t i = 0; i < 0x24; ++i) {
        if (g_locks[i].static_init != 1) continue;
        g_locks[i].section = next;
        next += 0x18;
        if (host->init_critical_section(g_locks[i].section, 4000) == 0) {
            g_locks[i].section = nullptr;
            return 0;
        }
    }
    return 1;
}
