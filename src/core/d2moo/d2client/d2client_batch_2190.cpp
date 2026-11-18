// src/core/d2client/d2client_batch_2190.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every RVA
// below is retail's. These are the MSVC debug-STL / GAMEDATA container internals D2Client carries; none of
// them touches a static global, so the whole state is the object the retail code receives in EAX/ECX/EBX/
// EDI/ESI. Ghidra shows that register `this` as in_EAX/unaff_EDI/unaff_EBX/unaff_ESI, or misses it in the
// caller; here it is an explicit first parameter named self, and the retail argument order is kept after it.
//
// Unrecovered internal callees and the two imported ordinals are host seams (the
// src/core/d2client/d2client_batch_1e70.cpp pattern): the function takes a D2ClientBatch2190Env AFTER its
// retail arguments. Ordinal_401 is the debug allocator (size, type, file, line) and Ordinal_403 the debug
// deallocator (pointer, type, file, line) -- the "GAMEDATA"/"delete" literals in the decompiles are their
// type tags.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatch2190Env {
    void* vtable_22c0;                                       // &PTR_FUN_6fb860c0
    void* vtable_2670;                                       // &PTR_FUN_6fb860b0
    void  (*f_1ed0)(void* self);                             // 0x1ED0
    void  (*f_2400)(void* self, uint32_t arg);               // 0x2400
    void  (*f_2700)(void* self);                             // 0x2700
    void  (*f_2750)(int32_t obj);                            // 0x2750
    void  (*f_27d0)(void* self);                             // 0x27D0
    void  (*f_28a0)(int32_t* field);                         // 0x28A0
    void  (*f_2900)(int32_t* field);                         // 0x2900
    void  (*f_2ec0)(void* element);                          // 0x2EC0
    void  (*f_30a0)(void);                                   // 0x30A0
    void  (*vt_call0)(void* vtable, int32_t arg);            // (**(code **)self)(arg)
    void* (*dbg_alloc)(uint32_t size, const char* type, uint32_t file, uint32_t line); // Ordinal_401
    void  (*dbg_free)(void* p, const char* type, uint32_t file, uint32_t line);        // Ordinal_403
};

// D2Client+0x02190 callee, defined below.
extern "C" void d2_client_2540(void* self, const D2ClientBatch2190Env* env);
// D2Client+0x02210 callee, defined below.
extern "C" void d2_client_2210(void* self);
// D2Client+0x02670 callee, defined below.
extern "C" void* d2_client_2670(void* self, const D2ClientBatch2190Env* env);

// The relink block both 0x2210 and 0x2360 open-code: link[0] is the node, link[1] the saved back link,
// either absolute (negative, stored as its complement) or self-relative. Rewrite the node's outward links,
// then clear the slot. Reads nothing but the two words, as retail does.
// ponytail: retail keeps these links in 32-bit words; the uintptr_t hops assume the wasm64 heap stays below
// 4 GiB (true for the browser build's default memory). Widen the fields if that ever stops holding.
static void orphan_relink(int32_t* link) {
    uint32_t node = (uint32_t)*link;
    if (node == 0) return;
    uint32_t encoded = (uint32_t)link[1];
    int32_t* slot = ((int32_t)encoded < 0)
                        ? (int32_t*)(uintptr_t)~encoded
                        : (int32_t*)((uint8_t*)link +
                                     (encoded - *(int32_t*)(uintptr_t)(uint32_t)(node + 4)));
    *slot = (int32_t)node;
    // retail reloads both link words after the slot store (the slot can alias link[1]): 1.13c 0x2254-0x2259
    *(int32_t*)(uintptr_t)(uint32_t)((uint32_t)*link + 4) = link[1];
    *link = 0;
    link[1] = 0;
}

// 1.13c D2Client+0x02190: SEH-framed tail call of 0x2540 with the same object (EAX). The exception frame is
// host state, so the callee is the whole rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2190(void* self, const D2ClientBatch2190Env* env) {
    d2_client_2540(self, env);
}

// 1.13c D2Client+0x021D0: forwards its single argument (the object) to 0x2210; l_4 is the SEH state word.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_21d0(void* self) {
    d2_client_2210(self);
}

// 1.13c D2Client+0x02210: orphan both stored links of the object -- the pair at +0x0C/+0x10, then the pair
// at +0x04/+0x08. Each block runs only when its head is non-NULL and clears both words afterwards.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2210(void* self) {
    uint8_t* s = (uint8_t*)self;
    orphan_relink((int32_t*)(s + 0x0c));
    orphan_relink((int32_t*)(s + 0x04));
}

// 1.13c D2Client+0x022C0: construct through 0x2670, install vtable 0x6FB860C0, then store the encoded
// self word at +0x48 (~self & 0x0FFFFFFF) and zero +0x4C.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_22c0(void* self, const D2ClientBatch2190Env* env) {
    d2_client_2670(self, env);
    uint32_t* s32 = (uint32_t*)self;
    s32[0] = (uint32_t)(uintptr_t)env->vtable_22c0;   // 32-bit slot (retail layout)
    s32[0x12] = ~(uint32_t)(uintptr_t)self & 0x0fffffffu;
    s32[0x13] = 0;
    return self;
}

// 1.13c D2Client+0x02320: while the object's +0x40 head is positive, run 0x2750 on it and free that block;
// then the same for the +0x30 head, released through 0x21D0. Both heads are re-read each iteration (retail's
// loop shape); the callee owns advancing them.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2320(void* self, const D2ClientBatch2190Env* env) {
    uint8_t* s = (uint8_t*)self;
    int32_t iVar1;
    while (iVar1 = *(int32_t*)(s + 0x40), 0 < iVar1) {
        env->f_2750(iVar1);
        env->dbg_free((void*)(intptr_t)iVar1, "__AV__TSHashObjectChunk_USGAMEDA", 0xfffffffeu, 0);
    }
    while (iVar1 = *(int32_t*)(s + 0x30), 0 < iVar1) {
        d2_client_21d0((void*)(intptr_t)iVar1);
        env->dbg_free((void*)(intptr_t)iVar1, "__AUSGAMEDATA__", 0xfffffffeu, 0);
    }
}

// 1.13c D2Client+0x02360: clear this+0x10, run 0x30A0, then walk this+0x18 entries of 0x0C bytes. For each:
// while entry[+2] is positive either call the object's vtable slot 0 on it (argument 0) or, for a non-zero
// argument, orphan the node at entry[0]+entry[+2]. Retail re-reads entry[+2] each pass; kept verbatim.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2360(uint32_t param_1, void* self, const D2ClientBatch2190Env* env) {
    uint32_t* s32 = (uint32_t*)self;
    s32[4] = 0;
    env->f_30a0();
    uint32_t local_4 = 0;
    if (s32[6] != 0) {
        int32_t iVar6 = 0;
        do {
            while (true) {
                int32_t iVar1 = *(int32_t*)((uint8_t*)(uintptr_t)s32[7] + 8 + iVar6);
                if (iVar1 < 1) break;
                if (param_1 == 0) {
                    env->vt_call0(*(void**)self, iVar1);
                } else {
                    int32_t iVar2 = *(int32_t*)((uint8_t*)(uintptr_t)s32[7] + iVar6);
                    orphan_relink((int32_t*)((uint8_t*)(uintptr_t)(uint32_t)iVar2 + iVar1));
                }
            }
            local_4 = local_4 + 1;
            iVar6 = iVar6 + 0xc;
        } while (local_4 < s32[6]);
    }
}

// 1.13c D2Client+0x024B0: debug-allocate param_2 + 0x1DF4 bytes, zero the four words at +4..+0x10 and return
// the block, or NULL. Then, if the object in ESI is live, run 0x2400(self, 0). ECX is unused by retail.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_24b0(uint32_t ecx_unused, int32_t param_2, uint32_t param_3,
                                             void* self, const D2ClientBatch2190Env* env) {
    (void)ecx_unused;
    int32_t block = (int32_t)(intptr_t)env->dbg_alloc((uint32_t)(param_2 + 0x1df4), "__AUSGAMEDATA__",
                                                     0xfffffffeu, param_3 | 8);
    int32_t result;
    if (block == 0) {
        result = 0;
    } else {
        uint8_t* b = (uint8_t*)(intptr_t)block;
        *(uint32_t*)(b + 4) = 0;
        *(uint32_t*)(b + 8) = 0;
        *(uint32_t*)(b + 0x0c) = 0;
        *(uint32_t*)(b + 0x10) = 0;
        result = block;
    }
    if (self != nullptr) env->f_2400(self, 0);
    return result;
}

// 1.13c D2Client+0x02540: release this+0x08's this+0x04 elements of 0x0C bytes through 0x2EC0, then free the
// array itself (debug deallocator, tag __AV__TSExplicitList_USGAMEDATA__).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2540(void* self, const D2ClientBatch2190Env* env) {
    uint32_t* s32 = (uint32_t*)self;
    uint32_t uVar2 = 0;
    if (s32[1] != 0) {
        int32_t iVar1 = 0;
        do {
            env->f_2ec0((void*)((uint8_t*)(uintptr_t)s32[2] + iVar1));
            uVar2 = uVar2 + 1;
            iVar1 = iVar1 + 0xc;
        } while (uVar2 < s32[1]);
    }
    if (s32[2] != 0) {
        env->dbg_free((void*)(uintptr_t)s32[2], "__AV__TSExplicitList_USGAMEDATA__", 0xfffffffeu, 0);
    }
}

// 1.13c D2Client+0x02580: run 0x30A0, then orphan the single link pair at +0x04/+0x08.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2580(void* self, const D2ClientBatch2190Env* env) {
    env->f_30a0();
    orphan_relink((int32_t*)((uint8_t*)self + 4));
}

// 1.13c D2Client+0x02600: destructor. Run 0x1ED0(self); when bit 0 of the argument is set and self is live,
// free self with the "delete" tag. Returns self (ret 4).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2600(void* self, uint8_t param_1, const D2ClientBatch2190Env* env) {
    env->f_1ed0(self);
    if (((param_1 & 1) != 0) && (self != nullptr)) {
        env->dbg_free(self, "delete", 0xffffffffu, 0);
    }
    return self;
}

// 1.13c D2Client+0x02630: SEH-framed tail call of 0x2700 with the same object (EAX).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2630(void* self, const D2ClientBatch2190Env* env) {
    env->f_2700(self);
}

// 1.13c D2Client+0x02700: release this+0x08's this+0x04 elements of 0x1DF4 bytes through 0x21D0, then
// debug-free the array itself with the same GAMEDATA tag as the container frees above. The count and head
// are not cleared, as retail leaves them. Object arrives in EAX (Ghidra in_EAX).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2700(void* self, const D2ClientBatch2190Env* env) {
    uint32_t* s32 = (uint32_t*)self;
    uint32_t uVar2 = 0;
    if (s32[1] != 0) {
        int32_t iVar1 = 0;
        do {
            d2_client_21d0((void*)(uintptr_t)(s32[2] + iVar1));
            uVar2 = uVar2 + 1;
            iVar1 = iVar1 + 0x1df4;
        } while (uVar2 < s32[1]);
    }
    if (s32[2] != 0) {
        env->dbg_free((void*)(uintptr_t)s32[2], "__AUSGAMEDATA__", 0xfffffffeu, 0);
    }
}

// 1.13c D2Client+0x02670: construct through 0x27D0, install vtable 0x6FB860B0, initialise +0x28 through
// 0x28A0 and +0x38 through 0x2900, set +0x34 = 0x10, and -- unless 0x28A0 already left 4 at +0x28 -- run
// 0x30A0 and build the empty circular list: +0x2C = &+0x2C, +0x28 = 4, +0x30 = ~&+0x2C.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_2670(void* self, const D2ClientBatch2190Env* env) {
    uint8_t* s = (uint8_t*)self;
    env->f_27d0(self);
    *(uint32_t*)s = (uint32_t)(uintptr_t)env->vtable_2670;   // 32-bit slot (retail layout)
    env->f_28a0((int32_t*)(s + 0x28));
    env->f_2900((int32_t*)(s + 0x38));
    *(uint32_t*)(s + 0x34) = 0x10;
    if (*(int32_t*)(s + 0x28) != 4) {
        env->f_30a0();
        int32_t* puVar2 = (int32_t*)(s + 0x2c);
        *puVar2 = (int32_t)(intptr_t)puVar2;
        *(int32_t*)(s + 0x28) = 4;
        *(uint32_t*)(s + 0x30) = ~(uint32_t)(uintptr_t)puVar2;
    }
    return self;
}
