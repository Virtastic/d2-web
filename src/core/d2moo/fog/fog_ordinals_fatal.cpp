// Named Fog-side exports for the D2Common/D2Client importers that otherwise bind the weak stubs in compat_link_weak.cpp.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
extern "C" uint32_t fog_FreeClientMemory();
// 1.13c Fog+0x0FC20 ordinal 10265 D2GetInstructionPointer: retail is `mov eax,[esp]; ret`, the caller's return address;
// args ignored. wasm has no code addresses (__builtin_return_address walks a JS stack trace and returns a build-dependent
// offset), so the port returns the fixed call-site token FOG_RETSITE, the stand-in for "the return address of the call
// into this function". Callers only pass it to the Fog 10024
// fatal report as the caller address; there it reads 0xB7000000 on every site and every build.
static constexpr uint32_t FOG_RETSITE = 0xB7000000u;
extern "C" uint32_t Fog_Ordinal_10265(uint32_t) { return FOG_RETSITE; }
// older port of Fog+0x11100 ordinal 10024 (p1, p2, code): Ordinal_116, logs "Unrecoverable internal error %08x" (type 6, code=p3),
// closes the logs (+0x184A0) and RETURNS; the caller does the _exit(-1). Log files are Win32 state; stderr stands in.
extern "C" void Fog_Ordinal_10024(const void*, const void*, uint32_t code) {
	std::fprintf(stderr, "Fog error 6: Unrecoverable internal error %08x\n", code);
	std::fflush(stderr);
}
// 1.13c Fog+0x1CCF0 ordinal 10043 FreeClientMemory: constant 1, result ignored by callers.
extern "C" void Fog_Ordinal_10043(int32_t, int32_t) { (void)fog_FreeClientMemory(); }
