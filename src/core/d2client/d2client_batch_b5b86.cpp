// src/core/d2client/d2client_batch_b5b86.cpp -- two 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c and the retail disassembly (image base
// 0x6FAB0000). The first is an 8-byte CRT fragment; the experience-bar / hover-text draw (0x26970) moved to
// d2client_26970.cpp. Unrecovered internal callees
// and imported ordinals are host seams after the retail arguments / in the env struct (the
// d2client_batch_bf80.cpp and d2client_batch_1e070.cpp pattern). Retail globals are inputs, because
// the client platform layer owns them.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// 1.13c D2Client+0x05B86 (8 B): `push ebx; call D2Client+0x0A71B; pop ecx; ret`. MSVC 2003 emitted
// this fragment out of line for __unlock_fhandle(_FileHandle) inside __write (D2Client+0x05AFF), where
// the handle is already live in EBX. The callee at 0xA71B does
// LeaveCriticalSection(&g_fhandleTable[_Filehandle >> 5][_Filehandle & 0x1F]); the handle is not a
// stack argument, so it is recovered as the explicit first parameter (unaff_EBX) and the critical
// section is a host seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_5b86(uint32_t fileHandle,
                                              void (*unlockFhandle)(uint32_t)) {
    unlockFhandle(fileHandle);
}
