#include <cstdint>
#include <cstring>

// 1.13c D2Game+0x2A330 (#10048): stores the host callback table pointer (DAT 0x6FD31C3C) and a
// has-callbacks flag (DAT 0x6FD31C40); non-null also tail-calls D2Common#10352 (ret 4) with the
// dword at table+0x3C as its single argument.
void* gpServerCallbacks2A330;
uint32_t gnServerCallbacksSet2A330;
namespace d2common { void D2Common_10352(std::uint32_t); }

// The body with the D2Common call as a parameter (the FN probe passes a logging stand-in for the import thunk).
void GAME_SetServerCallbackFunctions_Impl(void* p, void (*ord10352)(std::uint32_t))
{
    if (p)
    {
        gpServerCallbacks2A330 = p;
        gnServerCallbacksSet2A330 = 1;
        std::uint32_t arg;
        std::memcpy(&arg, (const std::uint8_t*)p + 0x3C, 4);
        ord10352(arg);
        return;
    }
    gpServerCallbacks2A330 = nullptr;
    gnServerCallbacksSet2A330 = 0;
}

void __stdcall GAME_SetServerCallbackFunctions(void* p)
{
    GAME_SetServerCallbackFunctions_Impl(p, d2common::D2Common_10352);
}
