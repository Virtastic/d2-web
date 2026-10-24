// Named Fog-side exports for the D2Common/D2Client importers that otherwise bind the weak stubs in compat_link_weak.cpp.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
extern "C" uint32_t fog_FreeClientMemory();
// 1.13c Fog+0x0FC20 ordinal 10265: returns its own return address (the caller's next instruction); args ignored.
extern "C" uint32_t Fog_Ordinal_10265(uint32_t) { return (uint32_t)(uintptr_t)__builtin_return_address(0); }
// 1.13c Fog+0x11100 ordinal 10024 (p1, p2, code): Ordinal_116, logs "Unrecoverable internal error %08x" (type 6, code=p3),
// closes the logs (+0x184A0) and RETURNS; the caller does the _exit(-1). Log files are Win32 state; stderr stands in.
extern "C" void Fog_Ordinal_10024(const void*, const void*, uint32_t code) {
	std::fprintf(stderr, "Fog error 6: Unrecoverable internal error %08x\n", code);
	std::fflush(stderr);
}
// 1.13c Fog+0x1CCF0 ordinal 10043 FreeClientMemory: constant 1, result ignored by callers.
extern "C" void Fog_Ordinal_10043(int32_t, int32_t) { (void)fog_FreeClientMemory(); }
