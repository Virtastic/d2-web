// 1.13c Fog+0x1CCF0 ordinal 10043 FreeClientMemory: releases the client pool (Fog+0x1B0C0) with an uninitialised local, ignores the outcome, returns 1.
// Our allocator has no per-pool critical section to drive, so the observable contract is the constant return.
#include <cstdint>
extern "C" uint32_t fog_FreeClientMemory() { return 1; }
