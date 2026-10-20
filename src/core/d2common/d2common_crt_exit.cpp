#include "d2common_crt_exit.h"
#include <cstdlib>
#include <mutex>
#include <new>
namespace {
std::recursive_mutex exit_lock;
d2common_crt::ExitState state;
void walk(d2common_crt::Callback* first, d2common_crt::Callback* last) {
    for (; first != last; ++first) if (*first) (*first)();
}
}
namespace d2common_crt {
ExitState& exit_state() { return state; }
bool register_exit(Callback callback) {
    std::lock_guard<std::recursive_mutex> lock(exit_lock);
    try { state.onexit.push_back(callback); return true; }
    catch (const std::bad_alloc&) { return false; }
}
}
// 1.13c D2Common+0x01986
// Retail asks an already-loaded mscoree.dll to CorExitProcess, then calls ExitProcess. No managed
// runtime exists in the wasm host, so only the ExitProcess leg remains; it never returns.
[[noreturn]] void D2Common_1986_CrtExitProcess(int32_t code) { std::_Exit(code); }

// 1.13c D2Common+0x01A4A
// Lock 8 is recursive: callbacks can reenter returning shutdown or register another callback.
// Pop before calling, matching retail's decrement of the shared end pointer. Keeping an empty
// native vector replaces the retail one-before-begin sentinel without forming an invalid pointer.
void D2Common_1A4A_DoExit(uint32_t code, int32_t quick, int32_t returning) {
    {
        std::lock_guard<std::recursive_mutex> lock(exit_lock);
        if (state.exit_done == 1) std::_Exit(static_cast<int32_t>(code));
        state.termination_done = 1;
        state.exitflag = static_cast<uint8_t>(returning);
        if (quick == 0) {
            while (!state.onexit.empty()) {
                auto callback = state.onexit.back();
                state.onexit.pop_back();
                if (callback) callback();
            }
            walk(state.preterminators, state.preterminators + 2);
        }
        walk(state.terminators, state.terminators + 2);
    }
    if (returning != 0) return;
    state.exit_done = 1;
    D2Common_1986_CrtExitProcess(static_cast<int32_t>(code));
}
