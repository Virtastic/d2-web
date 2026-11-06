// D2Common 0x4A3D0 (#10300): the profiler dump. Written from the 1.13c disassembly: the frequency line takes both
// halves of the QueryPerformanceFrequency answer, each record line takes the record's name (record+0), time (+0x68)
// and count (+0x70); the 23 records are 0x78 bytes from RVA 0x99E60.
#include <cstdint>
struct D2Common4a3d0Env {
	void (*queryFrequency)(uint32_t out[2]);   // kernel32 QueryPerformanceFrequency (IAT 0x89204)
	// 0x12C00 cdecl log: (1, 2, 1, 0x80000, file, line, format, w7, w8, w9); w9 unused by the frequency line
	void (*log)(uint32_t line, uint32_t fmt, uint32_t w7, uint32_t w8, uint32_t w9, uint32_t nArgs);
	uint32_t records;                           // the record table (retail: RVA 0x99E60)
};

// 1.13c D2Common+0x4A3D0
extern "C" void D2Common_4A3D0_With(const D2Common4a3d0Env* e)
{
	uint32_t f[2] = { 0, 0 };
	e->queryFrequency(f);
	e->log(0x4E, 0, f[0], f[1], 0, 9);   // format 0: "PROFILING DUMP FREQUENCY %d:\n"
	for (uint32_t i = 0; i < 23; ++i) {
		const uint32_t rec = e->records + i * 0x78;
		const uint32_t* w = reinterpret_cast<const uint32_t*>(uintptr_t(rec));
		e->log(0x54, 1, rec, w[0x68 / 4], w[0x70 / 4], 10);   // format 1: "FUNCTION: %30s, TIME %12d, COUNT %5d"
	}
}
