// Fog lock shims for the ported QServer and rate code, which call the C-linkage names directly.
// The engine runs single-threaded in wasm, so taking and releasing a critical section changes
// nothing a record can see; the lock tracker only feeds a debug counter. All three are no-ops.
extern "C" {
void FUN_6ff61690(void*) {}
void EnterCriticalSection(void*) {}
void LeaveCriticalSection(void*) {}
}
