// src/core/d2client/d2client_batch_1de0.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000;
// every RVA below is retail's. Unrecovered callees (0x66B80, 0x1FB0, 0x2050, 0x2540, 0x2670, 0x2750,
// 0x30A0) and Storm ordinal 403 (SMemFree(location, logfile, logline, flags)) are host seams AFTER the
// retail arguments, which keep their order (the d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host (kernel) state, a no-op under wasm

struct D2ClientBatch1de0Env {
    void (*smem_free)(void* location, const char* logfile, int32_t logline, uint32_t flags); // Storm ordinal 403
    void (*sub_66B80)(void* self);        // 0x66B80  destructor body of the 0x1DE0 object
    void (*sub_1FB0)(void* self);         // 0x1FB0  teardown body of the 0x1E80/0x1ED0 objects
    void (*sub_2050)(void* self);         // 0x2050  destructor body of the 0x2160 object
    void (*sub_2540)(void);               // 0x2540  body of the 0x2190 wrapper
    void (*sub_2670)(void* self);         // 0x2670  base constructor of the 0x22C0 object
    void (*sub_2750)(void* self);         // 0x2750  releases one 0x40-list chunk in 0x2320
    void (*sub_30A0)(void);               // 0x30A0  first step of the 0x2360 teardown
    void (*vslot0)(void* self, int32_t value); // (**(code**)*self)(value), __thiscall ECX = self
    void (*sub_22c0)(void* self);         // 0x22C0  optional seam for the 1F10 base-ctor call (the capture hooks it: its output is address-dependent)
};

// Intrusive-list unlink shared by 0x2210 and 0x2360 (the d2client_batch_8xx.cpp pattern). `node` is the
// two-word head at the field itself: [0] successor (0 = not linked), [1] encoded predecessor. A
// non-negative encoding is relative (node + enc - successor[1]); a negative one is ~target.
static void d2c13_unlink_node(uint32_t node) {
    uint32_t succ = *(const uint32_t*)(uintptr_t)node;
    if (succ == 0) return;
    uint32_t enc = *(const uint32_t*)(uintptr_t)(node + 4);
    uint32_t target;
    if ((int32_t)enc < 0) target = ~enc;
    else target = (uint32_t)(node + (enc - *(const uint32_t*)(uintptr_t)(succ + 4)));
    *(uint32_t*)(uintptr_t)target = succ;
    *(uint32_t*)(uintptr_t)(*(const uint32_t*)(uintptr_t)node + 4) = enc;
    *(uint32_t*)(uintptr_t)node = 0;
    *(uint32_t*)(uintptr_t)(node + 4) = 0;
}

static uint32_t d2_client_22c0(void* self, const D2ClientBatch1de0Env* env);
static void d2_client_2210(uint32_t object);
static void d2_client_21d0(uint32_t object);

// 1.13c D2Client+0x01DE0 (40 B): scalar deleting destructor (vtable slot). Runs the destructor body
// 0x66B80, then for flags bit 0 frees the object with SMemFree(this, "delete", -1, 0); returns this.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_1de0(void* self, uint8_t flags,
                                           const D2ClientBatch1de0Env* env) {
    env->sub_66B80(self);
    if ((flags & 1) != 0 && self != nullptr)
        env->smem_free(self, "delete", -1, 0);
    return self;
}

// 1.13c D2Client+0x01E10 (42 B): D2Common data-table accessor. `idx` arrives in EAX. Reads the record
// count at tables+0xC34 and the record array at tables+0xC3C; in-range returns array + idx*0xC,
// out-of-range returns the array base. sgptDataTables is the D2Common import slot at D2Client+0xCE474,
// hence `tables` is an argument here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1e10(int32_t idx, const uint8_t* tables) {
    int32_t count = *(const int32_t*)(tables + 0xC34);
    uint32_t array = *(const uint32_t*)(tables + 0xC3C);
    if (-1 < idx && idx < count)
        return array + (uint32_t)idx * 0xC;
    return array;
}

// 1.13c D2Client+0x01E80 (76 B): destructor of the 0x1F10 object. Deletes the critical section at
// this+0x50, then runs the 0x1FB0 teardown.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1e80(void* self, const D2ClientBatch1de0Env* env) {
    DeleteCriticalSection((LPCRITICAL_SECTION)((uint8_t*)self + 0x50));
    env->sub_1FB0(self);
}

// 1.13c D2Client+0x01ED0 (55 B): destructor body that only runs the 0x1FB0 teardown (the critical
// section, if any, is not touched here).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1ed0(void* self, const D2ClientBatch1de0Env* env) {
    env->sub_1FB0(self);
}

// 1.13c D2Client+0x01F10 (84 B): constructor. Runs the 0x22C0 base constructor, overwrites its vtable
// with RVA 0xD60D0, then initialises the critical section at this+0x50.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_1f10(void* self, const D2ClientBatch1de0Env* env) {
    if (env->sub_22c0) env->sub_22c0(self); else d2_client_22c0(self, env);
    *(uint32_t*)self = 0xD60D0u;
    InitializeCriticalSection((LPCRITICAL_SECTION)((uint8_t*)self + 0x50));
    return self;
}

// 1.13c D2Client+0x02160 (40 B) d2client_batch_1de0.cpp:d2_client_2160: scalar deleting destructor (vtable slot). Runs the 0x2050 destructor
// body, then for flags bit 0 frees the object with SMemFree(this, "delete", -1, 0); returns this.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2160(void* self, uint8_t flags,
                                           const D2ClientBatch1de0Env* env) {
    env->sub_2050(self);
    if ((flags & 1) != 0 && self != nullptr)
        env->smem_free(self, "delete", -1, 0);
    return self;
}

// 1.13c D2Client+0x02190 (54 B): tail-call wrapper for 0x2540.
static void d2_client_2190(const D2ClientBatch1de0Env* env) {
    env->sub_2540();
}

// 1.13c D2Client+0x021D0 (55 B): tail-call wrapper for 0x2210.
static void d2_client_21d0(uint32_t object) {
    d2_client_2210(object);
}

// 1.13c D2Client+0x02210 (170 B): unlink the two intrusive-list heads embedded in the object, at
// object+0xC (encoded predecessor at +0x10) and object+4 (encoded predecessor at +8), in that order.
static void d2_client_2210(uint32_t object) {
    d2c13_unlink_node(object + 0xC);
    d2c13_unlink_node(object + 4);
}

// 1.13c D2Client+0x022C0 (88 B): constructor. Runs the 0x2670 base constructor, sets vtable RVA
// 0xD60C0, then [0x12] = ~this & 0x0FFFFFFF (self-relative hash) and [0x13] = 0.
static uint32_t d2_client_22c0(void* self, const D2ClientBatch1de0Env* env) {
    env->sub_2670(self);
    uint32_t* obj = (uint32_t*)self;
    obj[0] = 0xD60C0u;
    obj[0x12] = ~(uint32_t)(uintptr_t)self & 0x0FFFFFFFu;
    obj[0x13] = 0;
    return (uint32_t)(uintptr_t)self;
}

// 1.13c D2Client+0x02320 (63 B): destructor. Drains the list head at this+0x40, releasing each chunk
// through 0x2750 then SMemFree(chunk, "TSHashObjectChunk", -2, 0); then drains this+0x30, running the
// 0x2210 unlink (via 0x21D0) on each entry and SMemFree(entry, "USGAMEDATA", -2, 0).
static void d2_client_2320(void* self, const D2ClientBatch1de0Env* env) {
    int32_t v;
    while (v = *(const int32_t*)((const uint8_t*)self + 0x40), 0 < v) {
        env->sub_2750((void*)(uintptr_t)(uint32_t)v);
        env->smem_free((void*)(uintptr_t)(uint32_t)v, "TSHashObjectChunk", -2, 0);
    }
    while (v = *(const int32_t*)((const uint8_t*)self + 0x30), 0 < v) {
        d2_client_21d0((uint32_t)v);
        env->smem_free((void*)(uintptr_t)(uint32_t)v, "USGAMEDATA", -2, 0);
    }
}

// (superseded by d2client_batch_2190.cpp:d2_client_2360) teardown of a 12-byte-row table. Clears this+0x10, calls 0x30A0,
// then walks this[6] rows at this[7]; while a row's word at +8 is positive, either calls the object's
// vtable slot 0 with that word (mode 0) or unlinks the node at rowBase + word (mode != 0).
static void d2_client_2360(void* self, int32_t mode,
                                          const D2ClientBatch1de0Env* env) {
    uint32_t* obj = (uint32_t*)self;
    obj[4] = 0;
    env->sub_30A0();
    uint32_t rows = obj[6];
    for (uint32_t row = 0; row < rows; ++row) {
        uint8_t* table = (uint8_t*)(uintptr_t)obj[7];
        for (;;) {
            int32_t v = *(const int32_t*)(table + 8 + (size_t)row * 0xC);
            if (v < 1) break;
            if (mode == 0) {
                env->vslot0(self, v);
            } else {
                uint32_t rowBase = *(const uint32_t*)(table + (size_t)row * 0xC);
                d2c13_unlink_node(rowBase + (uint32_t)v);
            }
        }
    }
}
