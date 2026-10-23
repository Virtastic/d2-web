// 1.13c Fog+0x03CEB -- CRT stub: calls sub_3C3F(10) (lock slot 10).
#include <cstdint>

namespace {
// Host seam for Fog sub_3C3F (CRT lock-by-number); null until wired.
using LockFn = void (*)(int num);
LockFn g_lock3C3F = nullptr;
}

void fog_sub3CEB() { if (g_lock3C3F) g_lock3C3F(10); }
