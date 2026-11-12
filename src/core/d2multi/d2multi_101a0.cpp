// 1.13c D2Multi+0x101A0 (55 B): entry of the chat callback table.
// Formats the module string at 0x17698 (the second channel-refusal line) with the channel name (second stack word) into a 500-byte stack buffer through the module's sprintf (0x1000, cdecl) and shows the line with 0xFC90(line, 1). The first stack word is not read. Returns 1; ret 8.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_101a0(const D2mEnv& e, uint32_t a, uint32_t name) {
    (void)a;
    char line[500];
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x17698), name);
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, d2m_ptr(line), 1);
    return 1;
}
