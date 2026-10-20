#pragma once
#include <cstdint>
#include <vector>
namespace d2common_crt {
using Callback = void (*)();
// Module-owned native equivalent of +0xA2854/+0xA2850. Native pointers never use retail dword stride.
struct ExitState {
    std::vector<Callback> onexit;
    // The shipped +0x99028..30 and +0x99030..38 tables each contain two NULL slots.
    Callback preterminators[2]{};
    Callback terminators[2]{};
    uint32_t termination_done = 0, exit_done = 0;
    uint8_t exitflag = 0;
};
ExitState& exit_state();
bool register_exit(Callback callback);
}
void D2Common_1A4A_DoExit(uint32_t code, int32_t quick, int32_t returning);
