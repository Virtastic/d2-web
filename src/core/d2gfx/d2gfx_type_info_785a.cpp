// Prior art used: D2Gfx entry-point catalog and D2MOO's
// D2Gfx module (no CRT type_info body); sibling deleting-destructor seam
// d2gfx_batch_39ad_tiny.cpp. Rule checked against the 1.13c cached decompile.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

// Serialized x86 RTTI prefix. These are retail addresses/handles, never native
// wasm64 pointers. The allocator seam resolves the cached-name handle.
struct D2GfxTypeInfo32 {
    uint32_t vtable;
    uint32_t cachedName;
};
static_assert(offsetof(D2GfxTypeInfo32, vtable) == 0);
static_assert(offsetof(D2GfxTypeInfo32, cachedName) == 4);
static_assert(sizeof(D2GfxTypeInfo32) == 8);

// 1.13c D2Gfx+0x0785A
// Restore the module's type_info vtable before taking CRT lock 14. Free an
// existing cached demangled name, then unlock (retail thunk +0x7897). Retail
// leaves the cached-name field unchanged and does not free the object here.
// The supplied vtable is the module-data identity; lock/free/unlock are the
// runtime boundary, matching the callback seams of the deleting destructor.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2gfx_type_info_destroy_785a(
    D2GfxTypeInfo32* self, uint32_t typeInfoVtable,
    void (*lock)(uint32_t), void (*freeName)(uint32_t),
    void (*unlock)(uint32_t)) {
    self->vtable = typeInfoVtable;
    lock(14);
    if (self->cachedName != 0) freeName(self->cachedName);
    unlock(14);
}
