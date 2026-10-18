// src/core/d2client/d2client_batch_1cb0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is
// retail's. These are the Storm/MSVC C++ container and string-table constructors/destructors around the
// AUSGAMEDATA/TSExplicitList classes; internal callees are host seams in the src/core/d2client/
// cursor_init_166c0.cpp pattern, passed in an env AFTER the retail arguments.
// The decompiler did not recover several register arguments (a dtor's this in EBX/EAX/EDI); those are
// recorded per function and forwarded explicitly to the seam.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include <Windows.h>       // CRITICAL_SECTION no-ops and the Initialize/Delete shims
#include "D2DataTbls.h"

// Data table layout used by 0x1E10 (retail 32-bit offsets, 4-byte pointers): count at +0xC34 and the
// D2MonTypeTxt array at +0xC3C, record stride 0xC. The count is D2MOO's nMonPropTxtRecordCount; the
// decompile pairs it with the pMonTypeTxt array, and the named fields are that same layout here.
static_assert(sizeof(D2MonTypeTxt) == 0xC, "D2MonTypeTxt stride 0xC");

struct D2ClientBatch1cb0Env {
    void (*unicodeDefaultCtor)(void*);    // Unicode::_default_constructor_closure_ (imported)
    void (*dtor17940)(void*);             // 0x17940, scalar destructor body
    void (*dtor16c40)(void*);             // 0x16C40
    void (*dtor16b80)(void*);             // 0x16B80
    void (*dtor21d0)(void*);              // 0x21D0
    void (*ctor22c0)(void*);              // 0x22C0
    void (*hashDtor2320)(void*);          // 0x2320; this in EDI
    void (*tsClear2360)(void*, int32_t);  // 0x2360; this in EBX, flag on the stack
    void (*listFree2540)(void*);          // 0x2540; this in EAX
    void (*listUnlink2580)(void*);        // 0x2580; argument is the node
    void (*opDelete)(void*);              // Ordinal_403(..., "delete", -1, 0)
};

// Declarations for the in-file forward calls (0x1E80 -> 0x1FB0 -> 0x2050).
extern "C" void d2_fn_d2client_1fb0(void* obj, const D2ClientBatch1cb0Env* env);
extern "C" void d2_fn_d2client_2050(void* obj, const D2ClientBatch1cb0Env* env);

// 1.13c D2Client+0x001CB0 (196 B): default-construct six arrays of 0x400 four-byte Unicode objects
// (blocks every 0x1000 bytes from the base). Returns the base.
extern "C" EMSCRIPTEN_KEEPALIVE
void* d2_fn_d2client_1cb0(void* base, const D2ClientBatch1cb0Env* env) {
    uint8_t* p = (uint8_t*)base;
    for (int block = 0; block < 6; ++block)
        for (int i = 0; i < 0x400; ++i)
            env->unicodeDefaultCtor(p + block * 0x1000 + i * 4);
    return base;
}

// 1.13c D2Client+0x001D80 (40 B): scalar deleting destructor. Run 0x17940(this); when flag bit 0 is
// set and this is non-NULL, operator delete(this). Returns this.
extern "C" EMSCRIPTEN_KEEPALIVE
void* d2_fn_d2client_1d80(void* obj, uint32_t flags, const D2ClientBatch1cb0Env* env) {
    env->dtor17940(obj);
    if ((flags & 1) != 0 && obj != nullptr) env->opDelete(obj);
    return obj;
}

// 1.13c D2Client+0x001DB0 (41 B): same shape, destructor body 0x16C40.
extern "C" EMSCRIPTEN_KEEPALIVE
void* d2_fn_d2client_1db0(void* obj, uint32_t flags, const D2ClientBatch1cb0Env* env) {
    env->dtor16c40(obj);
    if ((flags & 1) != 0 && obj != nullptr) env->opDelete(obj);
    return obj;
}

// 1.13c D2Client+0x001DE0 (40 B): same shape, destructor body 0x16B80.
extern "C" EMSCRIPTEN_KEEPALIVE
void* d2_fn_d2client_1de0(void* obj, uint32_t flags, const D2ClientBatch1cb0Env* env) {
    env->dtor16b80(obj);
    if ((flags & 1) != 0 && obj != nullptr) env->opDelete(obj);
    return obj;
}

// 1.13c D2Client+0x001E10 (42 B): index (EAX) into the data table. A signed index in
// [0, count@+0xC34) returns array@+0xC3C + index * 0xC; anything else returns the array base.
extern "C" EMSCRIPTEN_KEEPALIVE
uintptr_t d2_fn_d2client_1e10(int32_t index) {
    const int32_t count = sgptDataTables->nMonPropTxtRecordCount;
    const uint8_t* array = (const uint8_t*)sgptDataTables->pMonTypeTxt;
    if (index >= 0 && index < count) return (uintptr_t)(array + (size_t)index * 0xC);
    return (uintptr_t)array;
}

// 1.13c D2Client+0x001E80 (76 B): destructor -- DeleteCriticalSection at +0x50, then the 0x1FB0 body.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_1e80(void* obj, const D2ClientBatch1cb0Env* env) {
    DeleteCriticalSection((LPCRITICAL_SECTION)((uint8_t*)obj + 0x50));
    d2_fn_d2client_1fb0(obj, env);
}

// 1.13c D2Client+0x001ED0 (55 B): destructor body 0x1FB0 only.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_1ed0(void* obj, const D2ClientBatch1cb0Env* env) {
    d2_fn_d2client_1fb0(obj, env);
}

// 1.13c D2Client+0x001F10 (84 B): constructor -- 0x22C0(this), vtable RVA 0xD60D0 at +0, then
// InitializeCriticalSection at +0x50. Returns this.
extern "C" EMSCRIPTEN_KEEPALIVE
void* d2_fn_d2client_1f10(void* obj, const D2ClientBatch1cb0Env* env) {
    env->ctor22c0(obj);
    *(uint32_t*)obj = 0xD60D0u;
    InitializeCriticalSection((LPCRITICAL_SECTION)((uint8_t*)obj + 0x50));
    return obj;
}

// 1.13c D2Client+0x001F70 (30 B): destructor -- 0x21D0(this), then operator delete(this).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_1f70(void* obj, const D2ClientBatch1cb0Env* env) {
    env->dtor21d0(obj);
    env->opDelete(obj);
}

// 1.13c D2Client+0x001FB0 (125 B): destructor body. vtable RVA 0xD60B0 at +0; 0x2320(this, EDI);
// unlink the lists at +0x38 then +0x28; then the 0x2050 body.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_1fb0(void* obj, const D2ClientBatch1cb0Env* env) {
    *(uint32_t*)obj = 0xD60B0u;
    env->hashDtor2320(obj);
    env->listUnlink2580((uint8_t*)obj + 0x38);
    env->listUnlink2580((uint8_t*)obj + 0x28);
    d2_fn_d2client_2050(obj, env);
}

// 1.13c D2Client+0x002050 (114 B): destructor body. vtable RVA 0xD60E0 at +0; 0x2360(this, EBX; 1);
// 0x2540(this, EAX); unlink the node at +4.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_2050(void* obj, const D2ClientBatch1cb0Env* env) {
    *(uint32_t*)obj = 0xD60E0u;
    env->tsClear2360(obj, 1);
    env->listFree2540(obj);
    env->listUnlink2580((uint8_t*)obj + 4);
}

// 1.13c D2Client+0x0020D0 (42 B): reset the object. 0x2360(this, EBX; 1); zero +0x10 and +0x24;
// 0x2540(this, EAX); zero +0x14, +0x18 and +0x1C.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_20d0(void* obj, const D2ClientBatch1cb0Env* env) {
    env->tsClear2360(obj, 1);
    *(uint32_t*)((uint8_t*)obj + 0x10) = 0;
    *(uint32_t*)((uint8_t*)obj + 0x24) = 0;
    env->listFree2540(obj);
    *(uint32_t*)((uint8_t*)obj + 0x14) = 0;
    *(uint32_t*)((uint8_t*)obj + 0x18) = 0;
    *(uint32_t*)((uint8_t*)obj + 0x1C) = 0;
}
