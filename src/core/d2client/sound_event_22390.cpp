#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// 1.13c D2Client+0x22390: the sound-event id of a source record,
// forwarded to D2Sound.dll ordinal 10034 through the import thunk at D2Client+0xD276 (IAT slot VA
// 0x6FB7ED60). Retail's convention here is its own: EAX = the object (live-in),
// one stack word = the id, `ret 4`; void, the child's return value is discarded.
//
// The child is __fastcall with two register arguments and no stack word: ECX = the context, EDX = the
// value. The context is the ADDRESS p + 0x78 -- not a field read through it: `lea ecx,[eax+0x78]`.
typedef void (*Cb22390)(uint32_t ecx, uint32_t edx);

// The object as 0x22390 addresses it: nothing of it is dereferenced, only the +0x78 displacement.
struct B22390Object { uint8_t p0[0x78]; uint32_t ctx; };
static_assert(offsetof(B22390Object, ctx) == 0x78, "the +0x78 context handed to D2Sound.#10034");

// retail: `test eax,eax` -> Fog.#10265 (eip) / Fog.#10024 D2FogAssert(msg 0x6FB8359F, eip, 0xC2C) /
// _exit(-1). That path does not return, so a null object cannot be a vector tuple; ours returns
// without calling the child, the only behaviour left to code that cannot terminate the host.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_22390(uint32_t p, uint32_t v, Cb22390 cb) {
    if (p == 0) return;
    cb(p + 0x78, v);   // v is passed through untouched: no mask, no clamp, no sign test
}
