// Fog lock shims for the ported QServer and rate code, which call the C-linkage names directly.
// The engine runs single-threaded in wasm, so taking and releasing a critical section changes
// nothing a record can see; the lock tracker only feeds a debug counter. All four are no-ops,
// unless a test installs an observer (fog_lock_observer), which then sees each call in order:
// 0 tracker, 1 enter, 2 leave, 3 delete, with the section's address.
#include <cstdint>
extern "C" {
void (*fog_lock_observer)(int op, void* cs) = nullptr;
void FUN_6ff61690(void* cs) { if (fog_lock_observer) fog_lock_observer(0, cs); }
void EnterCriticalSection(void* cs) { if (fog_lock_observer) fog_lock_observer(1, cs); }
void LeaveCriticalSection(void* cs) { if (fog_lock_observer) fog_lock_observer(2, cs); }
void DeleteCriticalSection(void* cs) { if (fog_lock_observer) fog_lock_observer(3, cs); }
}
