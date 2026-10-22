#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <initializer_list>

extern "C" __attribute__((weak)) int Ordinal_10104(void);
extern "C" __attribute__((weak)) void Ordinal_276(void);
extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
extern "C" __attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
static const char g_b68_msg[1] = {0};  // DAT_6f9c0684

// 1.13c D2sound+0x6070  (hidden register inputs: ESI=obj, EDI=expected, local_108=caller stack local)
void D2sound_b68_6070(int esi, int edi, int local_108)
{
    uint32_t id;
    if (esi == 0) id = 0x87;
    else if (Ordinal_10104() == 0) { Ordinal_276(); id = 0x8f; }
    else { if (edi == local_108) return; id = 0x92; }
    Ordinal_10024(g_b68_msg, Ordinal_10265(id));
    _exit(-1);
}
