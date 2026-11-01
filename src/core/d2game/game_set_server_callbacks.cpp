#include <cstdint>

// 1.13c D2Game+0x2A330 (#10048): stores the host callback table pointer (DAT 0x6FD31C3C) and a
// has-callbacks flag (DAT 0x6FD31C40); non-null also calls D2Common#10352 (sub_70F40, ret 4, one ignored arg).
void* gpServerCallbacks2A330;
uint32_t gnServerCallbacksSet2A330;
namespace d2common { void D2Common_10352(std::uint32_t); }

void __stdcall GAME_SetServerCallbackFunctions(void* p)
{
    if (p)
    {
        gpServerCallbacks2A330 = p;
        gnServerCallbacksSet2A330 = 1;
        d2common::D2Common_10352(0);
        return;
    }
    gpServerCallbacks2A330 = nullptr;
    gnServerCallbacksSet2A330 = 0;
}
