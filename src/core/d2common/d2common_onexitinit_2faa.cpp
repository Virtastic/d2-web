#include <cstdint>

// 1.13c D2Common+0x02FAA ___onexitinit(): malloc 0x80 bytes for the atexit table; on failure return
// 0x18 (_RT_SPACEARG); else first slot = 0, both table pointers (DAT 0x6fdf2854 / 0x6fdf2850) = block; return 0.
struct D2CommonOnexitInitEnv {
    uint32_t* tableBase;    // DAT 0x6fdf2854
    uint32_t* tableEnd;  // DAT 0x6fdf2850
    uint32_t* (*malloc_)(uint32_t);
};

uint32_t d2common_onexitinit_2faa(D2CommonOnexitInitEnv& e)
{
    e.tableBase = e.malloc_(0x80);
    if (!e.tableBase) return 0x18;
    *e.tableBase = 0;
    e.tableEnd = e.tableBase;
    return 0;
}
