// src/core/d2client/d2client_unwind_tiny_1018.cpp -- seven VC7.1 exception-unwind funclets and one
// __stdcall member-call thunk of 1.13c D2Client.dll, reimplemented from the decompiles in
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c and the exact bytes in
// build-wasm/ghidra-client113c/d2client-local-20261009.asm (alpha lane).
// Base 0x6FAB0000. The funclets do not establish a frame: they read the establisher's EBP, so the
// caller passes that frame. fp[1] is [ebp+4], fp[3] is [ebp+0xC], fp[-4] is [ebp-0x10].
#include <cstdint>
#include <emscripten.h>

void D2Client_RelinkNode(uint8_t* link);            // 0x3060, pure_leaves_b23.cpp
extern "C" void d2_client_678e0(uint32_t* obj);     // 0x678E0, d2client_batch_108c0.cpp

// 1.13c D2Client+0xCD450 (8 B): 8B 45 04 / E9 <rel32> -- mov eax,[ebp+4]; jmp 0x6FAB3060
// (D2Client_RelinkNode). Tail-jump, so EAX is the callee's link argument.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd450(const uint32_t* fp) { D2Client_RelinkNode((uint8_t*)(uintptr_t)fp[1]); }

// 1.13c D2Client+0xCD475 (8 B): 8B 45 0C / E9 <rel32> -- mov eax,[ebp+0xC]; jmp 0x6FAB3060.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd475(const uint32_t* fp) { D2Client_RelinkNode((uint8_t*)(uintptr_t)fp[3]); }

// 1.13c D2Client+0xCD490 (8 B): 8B 4D F0 / E9 <rel32> -- mov ecx,[ebp-0x10]; jmp 0x6FB178E0
// (vtable reset to 0xD5048).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd490(const uint32_t* fp) { d2_client_678e0((uint32_t*)(uintptr_t)fp[-4]); }

// 1.13c D2Client+0xCD4A2 (8 B): 8B 4D 04 / E9 <rel32> -- mov ecx,[ebp+4]; jmp 0x6FB178E0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd4a2(const uint32_t* fp) { d2_client_678e0((uint32_t*)(uintptr_t)fp[1]); }

// 1.13c D2Client+0xCD4B4 (8 B): 8B 4D 04 / E9 <rel32> -- mov ecx,[ebp+4]; jmp 0x6FB178E0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd4b4(const uint32_t* fp) { d2_client_678e0((uint32_t*)(uintptr_t)fp[1]); }

// 1.13c D2Client+0xCD4C6 (8 B): 8B 4D F0 / E9 <rel32> -- mov ecx,[ebp-0x10]; jmp 0x6FB178E0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd4c6(const uint32_t* fp) { d2_client_678e0((uint32_t*)(uintptr_t)fp[-4]); }

// 1.13c D2Client+0xCD4D8 (8 B): 8B 4D 04 / E9 <rel32> -- mov ecx,[ebp+4]; jmp 0x6FB178E0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cd4d8(const uint32_t* fp) { d2_client_678e0((uint32_t*)(uintptr_t)fp[1]); }

// 1.13c D2Client+0xCAE1A (7 B): 58 / 59 / 87 04 24 / FF E0 -- pop eax; pop ecx; xchg [esp],eax; jmp eax.
// __stdcall _CallMemberFunction1(pthis, pmemfunc, arg): pthis -> ECX, the return address is left under
// the third stack word, and control transfers to pmemfunc with arg as its single stack argument.
// pthis is a register (thiscall ECX) value, a host calling-convention concern, not a parameter here.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_cae1a(uint32_t pthis, void (*pmemfunc)(uint32_t arg), uint32_t arg) {
    (void)pthis;
    pmemfunc(arg);
}
