// 1.13c Fog+0x20AF0: slot-release sweep. When the object's flag (+0xC18) is 0, every socket slot (+0xB90, 4 ints) that is not -1
// is released through Fog+0x207F0 (EAX = timeout, ECX = object, stack = slot index). Returns 1.
#include <cstdint>
int fog_20af0_sweep(void (*close707f0)(void* self, uint32_t timeout, int idx), void* self, const int* socks, int f_c18, uint32_t timeout)
{
    if (f_c18 == 0)
        for (int i = 0; i < 4; i++)
            if (socks[i] != -1) close707f0(self, timeout, i);
    return 1;
}
