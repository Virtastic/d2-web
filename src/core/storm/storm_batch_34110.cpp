// src/core/storm/storm_batch_34110.cpp -- 12 UNMAPPED 1.13c Storm.dll rows (0x34110, 0x341D0,
// 0x34550, 0x34570, 0x34590, 0x345B0, 0x345D0, 0x387F0, 0x3A330, 0x3C370, 0x1B960, 0x1C760)
// transcribed from the cached decompiles in build-wasm/ghidra-storm/decomp/0x<RVA>.c (alpha lane:
// marker only, no spec, no vector).
//
// These are the Storm SEVT command-registration exports (0x34110..0x341D0 wrap the 0x36120 /
// 0x36680 / 0x34460 / 0x35DD0 table bodies; 0x34550..0x345B0 are the four MAPISendMail-shaped
// wrappers around 0x34460), the SMemFree/SMemReAlloc pair (0x1B960, 0x1C760), the SMem registry
// read (0x3C370), the blitter trampoline patcher (0x387F0) and the AUSTREAM open (0x3A330).
//
// The unmapped internal callees and the kernel32 imports are host seams on StormBatch34110Env,
// which every body takes AFTER its retail argument list (the storm_batch_382c0.cpp pattern).
// Register-only arguments (0x34150's EDI, 0x1B170's ECX) are explicit parameters.
//
// Two Ghidra artifacts are resolved from the retail disassembly:
//   * 0x345xx pass their key in ESI and handle in EBX to 0x34460 (ret 8); the seam takes all four
//     words in retail order;
//   * 0x3A330's local_230..local_21c is one SFile image object; the out words Ghidra never assigns
//     are its fields (+4 data, +8 stride, +0x18 palette, +0x1c palette size).
#include <cstdint>
#include <cstring>
#include <Windows.h>

struct StormBatch34110Env {
    // ---- SEVT seams, retail argument order -------------------------------------------------------
    uint32_t (*f_36120)(uint32_t param_1, uint32_t param_2);                        // 0x36120 retire
    uint32_t (*f_36680)(uint32_t param_1, uint32_t param_2);                        // 0x36680 clone
    uint32_t (*f_34460)(uint32_t cmd, uint32_t key, uint32_t param_2, uint32_t h);  // 0x34460
    uint32_t (*f_35dd0)(uint32_t param_1, uint32_t param_2, uint32_t param_3,
                        uint32_t* record, void* state);                             // 0x35DD0
    void     (*f_34150)(uint32_t edi);                                              // 0x34150 (EDI)
    // ---- SMem seams ------------------------------------------------------------------------------
    void     (*f_1acf0)(const char* file, int32_t line);                            // 0x1ACF0 free tag
    void     (*f_1b170)(uint32_t ecx, uint16_t* block, uint32_t bucket, void* user); // 0x1B170 (ECX)
    void     (*f_1f590)(uint32_t code, const char* what, int32_t line, uint8_t* buf,
                        int32_t a, int32_t b);                                      // 0x1F590 fatal
    uint32_t (*f_1c580)(uint32_t size, const char* file, int32_t line, uint32_t flags); // 0x1C580
    uint32_t* (*f_1c660)(uint32_t** heap, uint32_t bucket, uint32_t* block, uint32_t size,
                         uint8_t flags);                                            // 0x1C660 realloc
    // ---- SCode registry / stream seams -----------------------------------------------------------
    uint32_t (*f_3bdd0)(uint32_t* key, const char* name, uint8_t flags, uint32_t* type_out,
                        uint8_t* data, uint32_t size, uint32_t* length);            // 0x3BDD0
    uint32_t (*f_2b080)(char* dst, const char* src, uint32_t size);                 // 0x2B080 copy
    char*    (*f_2afd0)(char* path);                                                // 0x2AFD0 tail
    void     (*f_2ae00)(char* dst, const char* src, uint32_t size);                 // 0x2AE00 append
    char*    (*f_strupr)(char* s);                                                  // 0x056ED strupr
    void     (*f_1120)(uint32_t* obj);                                              // 0x1120 ctor
    void     (*f_11a0)(uint32_t* obj);                                              // 0x11A0 dtor
    uint32_t (*f_291e0)(uint32_t* stream, const char* name, int32_t* out);          // 0x291E0 decode
    uint32_t (*f_28eb0)(uint32_t* stream);                                          // 0x28EB0 release
    void     (*f_1090)(uint32_t* stream);                                           // 0x1090 teardown
    // ---- kernel32 --------------------------------------------------------------------------------
    void     (*set_last_error)(uint32_t e);
    void     (*exit_process)(uint32_t code);
};

namespace {

int32_t  g_lastError;                 // 0x6FC45388  last Storm error
uint32_t g_memReady;                  // 0x6FC45168  non-zero once the SCode pool is up
uint32_t g_memLog;                    // 0x6FC44D58  non-zero to route failures to 0x1F590
alignas(4) uint8_t g_heapLocks[0x100 * 0x18]; // 0x6FC43558 per-id SCode heap locks
CRITICAL_SECTION g_streamLock;        // 0x6FC45858 AUSTREAM list lock
uint32_t g_streamAnchor[2];           // 0x6FC45874/78 doubly linked AUSTREAM sentinel {next, prev}
uint32_t g_displayRoot;               // 0x6FC42FD0  active SEVT display object
void*    g_exceptionList;             // retail FS:[0]; no SEH under wasm, saved/restored

void set_error(uint32_t e) { g_lastError = (int32_t)e; }

uint32_t rd32(const void* p, uintptr_t off) { return *(const uint32_t*)((const uint8_t*)p + off); }
void     wr32(void* p, uintptr_t off, uint32_t v) { *(uint32_t*)((uint8_t*)p + off) = v; }

void stream_list_init() {
    if (g_streamAnchor[0] == 0) {
        g_streamAnchor[0] = (uint32_t)(uintptr_t)g_streamAnchor;
        g_streamAnchor[1] = (uint32_t)(uintptr_t)g_streamAnchor;
    }
}

void copy_strided(uint8_t*& dst, const uint8_t* src, uint32_t stride, uint32_t rows) {
    for (uint32_t r = 0; r < rows; r++) {
        std::memcpy(dst, src, stride);
        dst += stride;
    }
}

// 0x387F0's two self-relative patch targets. Distinct bodies so the linker cannot fold them.
__attribute__((noinline)) void blit_end_plain(void) { asm volatile("" ::: "memory"); }
__attribute__((noinline)) void blit_end_strided(void) { asm volatile("" ::: "memory"); }

// 0x3A330's local image object (retail local_230..local_21c).
struct SFileImage {
    uint32_t vtable;      // +0x00
    uint32_t data;        // +0x04  local_22c
    uint32_t stride;      // +0x08  local_228
    uint32_t width;       // +0x0c
    uint32_t height;      // +0x10
    uint32_t field14;     // +0x14
    uint32_t palette;     // +0x18  local_218
    uint32_t paletteSize; // +0x1c  local_214
};

} // namespace

// 1.13c Storm+0x34110: unregister the four SEVT commands for key param_1; always 1.
extern "C"
uint32_t Storm_Sub34110(uint32_t param_1, const StormBatch34110Env* env) {
    env->f_36120(0x534D5348, param_1);
    env->f_36120(0x534D5349, param_1);
    env->f_36120(0x534D534A, param_1);
    env->f_36120(0x534D5347, param_1);
    return 1;
}

// 1.13c Storm+0x341D0: dispatch one window message through the SEVT table. A 0x111 command also
// sends 0x534D5348 with the control id, 0x100/0x101 send 0x534D5349/0x534D534A. When the message
// came from the active display object it is re-dispatched once with param_1 cleared; 0x82 flushes
// the 0x34150 record. The two out words are record[6] and record[7].
extern "C"
uint32_t Storm_Sub341D0(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                        uint32_t* param_5, uint32_t* param_6, const StormBatch34110Env* env) {
    if (param_5 != nullptr) *param_5 = 0;
    if (param_6 != nullptr) *param_6 = 0;
    uint32_t record[8] = {};
    record[0] = param_1;
    record[1] = param_2;
    record[2] = param_3;
    record[3] = param_4;
    record[4] = (param_2 == 0x111) ? (param_3 >> 0x10) : 0;
    for (;;) {
        env->f_35dd0(0x534D5347, param_1, param_2, record, nullptr);
        if (param_2 == 0x111) {
            env->f_35dd0(0x534D5348, param_1, param_3 & 0xffff, record, nullptr);
        } else if (param_2 == 0x100) {
            env->f_35dd0(0x534D5349, param_1, param_3, record, nullptr);
        } else if (param_2 == 0x101) {
            env->f_35dd0(0x534D534A, param_1, param_3, record, nullptr);
        }
        if (param_1 == 0 || param_1 != g_displayRoot) {
            if (param_2 == 0x82) env->f_34150(0);
            if (param_5 != nullptr) *param_5 = record[6];
            if (param_6 != nullptr) *param_6 = record[7];
            return 1;
        }
        param_1 = 0;
    }
}

// 1.13c Storm+0x34550: send SEVT command 0x534D5347 with key/arg1/arg2 (key in ESI, handle EBX).
extern "C"
uint32_t Storm_Sub34550(uint32_t key, uint32_t param_2, uint32_t handle,
                        const StormBatch34110Env* env) {
    return env->f_34460(0x534D5347, key, param_2, handle);
}

// 1.13c Storm+0x34570: send SEVT command 0x534D534A.
extern "C"
uint32_t Storm_Sub34570(uint32_t key, uint32_t param_2, uint32_t handle,
                        const StormBatch34110Env* env) {
    return env->f_34460(0x534D534A, key, param_2, handle);
}

// 1.13c Storm+0x34590: send SEVT command 0x534D5349.
extern "C"
uint32_t Storm_Sub34590(uint32_t key, uint32_t param_2, uint32_t handle,
                        const StormBatch34110Env* env) {
    return env->f_34460(0x534D5349, key, param_2, handle);
}

// 1.13c Storm+0x345B0: send SEVT command 0x534D5348.
extern "C"
uint32_t Storm_Sub345B0(uint32_t key, uint32_t param_2, uint32_t handle,
                        const StormBatch34110Env* env) {
    return env->f_34460(0x534D5348, key, param_2, handle);
}

// 1.13c Storm+0x345D0: clone the bucket group of each of the four commands for key param_1; 1.
extern "C"
uint32_t Storm_Sub345D0(uint32_t param_1, const StormBatch34110Env* env) {
    env->f_36680(0x534D5348, param_1);
    env->f_36680(0x534D5349, param_1);
    env->f_36680(0x534D534A, param_1);
    env->f_36680(0x534D5347, param_1);
    return 1;
}

// 1.13c Storm+0x1B960: SMemFree. Rejects a cold pool (0x8510007D) and a null block (0x85100081);
// a block whose -2 header word is not 0x6F6D is 0x8510007C. Flag bit 1 frees through 0x1ACF0;
// bit 0 validates the trailer and does the same; bit 3 switches to the alternate segment word.
// The segment's heap lock (+8 in the segment header) is taken around 0x1B170.
extern "C"
uint32_t Storm_Sub1B960Env(void* param_1, uint8_t* param_2, int32_t param_3,
                        const StormBatch34110Env* env) {
    if (g_memReady == 0) {
        set_error(0x8510007D);
        env->set_last_error(0x8510007D);
        if (g_memLog != 0) env->f_1f590(0x8510007D, "SMemFree()", -1, nullptr, 1, 1);
        return 0;
    }
    if (param_1 == nullptr) {
        set_error(0x85100081);
        env->set_last_error(0x85100081);
        if (g_memLog != 0) env->f_1f590(0x85100081, (const char*)param_2, param_3, nullptr, 1, 1);
        return 0;
    }
    uint8_t* block = (uint8_t*)param_1;
    uint16_t* puVar3 = (uint16_t*)(block - 8);
    uint8_t flags = *(block - 5);
    if (*(int16_t*)(block - 2) != 0x6F6D) {
        set_error(0x8510007C);
        env->set_last_error(0x8510007C);
        if (g_memLog != 0) env->f_1f590(0x8510007C, (const char*)param_2, param_3, nullptr, 1, 1);
        return 0;
    }
    if ((flags & 2) != 0) {
        env->f_1acf0((const char*)param_2, param_3);
        return 0;
    }
    if ((flags & 1) != 0 &&
        *(int16_t*)((uint8_t*)puVar3 + (uint32_t)*puVar3 - *(block - 6) - 2) != 0x12B1) {
        env->f_1acf0((const char*)param_2, param_3);
    }
    if ((flags & 8) != 0) puVar3 = (uint16_t*)(uintptr_t)((uint32_t*)param_1)[-3];
    uint32_t iVar2 = (uint32_t)puVar3[2] << 16;
    int32_t slot = *(int32_t*)(uintptr_t)(iVar2 + 8);
    EnterCriticalSection((LPCRITICAL_SECTION)(g_heapLocks + slot * 0x18));
    env->f_1b170(0, puVar3, iVar2, param_1);
    if (slot != -1) LeaveCriticalSection((LPCRITICAL_SECTION)(g_heapLocks + slot * 0x18));
    return 1;
}

// 1.13c Storm+0x1C760: SMemReAlloc. A cold pool is fatal (0x8510007D / ExitProcess(1)). A valid
// block (header 0x6F6D, bit 1 clear) is reallocated through 0x1C660 under its heap lock; free-tag
// bit 1 and a bad header route back to 0x1C580 with the original size, and the trailing tag is
// applied as in SMemFree.
extern "C"
uint32_t* Storm_Sub1C760(uint32_t* param_1, uint32_t param_2, uint8_t* param_3, int32_t param_4,
                         uint32_t param_5, const StormBatch34110Env* env) {
    if (g_memReady == 0) {
        env->f_1f590(0x8510007D, "SMemReAlloc()", -1, nullptr, 0, 1);
        env->exit_process(1);
    }
    int32_t iVar2 = param_4;
    if (param_1 != nullptr) {
        uint8_t* block = (uint8_t*)param_1;
        uint32_t* puVar1 = param_1 - 2;
        uint8_t flags = *(block - 5);
        if (*(int16_t*)(block - 2) == 0x6F6D) {
            if ((flags & 2) == 0) {
                if ((flags & 1) != 0 &&
                    *(int16_t*)((uint8_t*)puVar1 + (uint32_t)*(uint16_t*)puVar1 - *(block - 6) - 2) != 0x12B1) {
                    env->f_1acf0((const char*)param_3, param_4);
                }
                if ((flags & 8) != 0) puVar1 = (uint32_t*)(uintptr_t)param_1[-3];
                iVar2 = (int32_t)((uint32_t)*(uint16_t*)((uint8_t*)puVar1 + 4) << 16);
                int32_t slot = *(int32_t*)(uintptr_t)((uint32_t)iVar2 + 8);
                EnterCriticalSection((LPCRITICAL_SECTION)(g_heapLocks + slot * 0x18));
                uint32_t* heap = (uint32_t*)(uintptr_t)slot;
                uint32_t* out = env->f_1c660(&heap, (uint32_t)iVar2, param_1, param_2,
                                             (uint8_t)param_5);
                if (heap != (uint32_t*)0xFFFFFFFF)
                    LeaveCriticalSection((LPCRITICAL_SECTION)(g_heapLocks + (int32_t)(uintptr_t)heap * 0x18));
                return out;
            }
            env->f_1acf0((const char*)param_3, param_4);
        } else {
            set_error(0x8510007C);
            env->set_last_error(0x8510007C);
            if (g_memLog != 0) env->f_1f590(0x8510007C, (const char*)param_3, iVar2, nullptr, 1, 1);
        }
    }
    return (uint32_t*)(uintptr_t)env->f_1c580(param_2, (const char*)param_3, iVar2, param_5);
}

// 1.13c Storm+0x3C370: SMem registry read. A null/empty key or name is ERROR_INVALID_PARAMETER
// (0x57); otherwise 0x3BDD0 reads the value into param_5, with its length word in param_6.
extern "C"
uint32_t Storm_Sub3C370(uint32_t* key, const char* name, uint32_t flags, uint8_t* param_4,
                        uint32_t param_5, uint32_t param_6, const StormBatch34110Env* env) {
    if (key == nullptr || *(const char*)key == '\0' || name == nullptr || *name == '\0') {
        set_error(0x57);
        env->set_last_error(0x57);
        return 0;
    }
    return env->f_3bdd0(key, name, (uint8_t)flags, (uint32_t*)&name, param_4, param_5,
                        (uint32_t*)(uintptr_t)param_6);
}

// 1.13c Storm+0x387F0: pick and patch the pixel blitter for width desc[8] (and the desc[0x18] & 3
// misalignment when flags bit 0x40000 is set), then tail-call it through self+0x10 with
// (desc[0xc], desc, desc[0x14], desc[0x10], stack). The two patched jump targets live at
// self[0x14]+self[0x18]-4 and, when misaligned, self[0x2c]/self[0x44] as well.
extern "C"
void Storm_Sub387F0(uint8_t* self, uint8_t* desc, const StormBatch34110Env* env) {
    (void)env;
    uint32_t uVar1 = rd32(desc, 8);
    if ((rd32(self, 8) & 0x40000) == 0) {
        if (rd32(self, 0xc) != uVar1) {
            wr32(self, 0xc, uVar1);
            wr32(self, 0x10, rd32(self, 0x14));
            int32_t dst = (int32_t)rd32(self, 0x14) + (int32_t)rd32(self, 0x18) - 4;
            *(int32_t*)(uintptr_t)dst = (int32_t)rd32(self, 0x4c + uVar1 * 4) - dst;
            void* target = ((rd32(self, 8) & 0x4000000) == 0) ? (void*)&blit_end_plain
                                                              : (void*)&blit_end_strided;
            int32_t dst2 = (int32_t)rd32(self, 0x44) + (int32_t)rd32(self, 0x48) - 4;
            *(int32_t*)(uintptr_t)dst2 = (int32_t)(uintptr_t)target - dst2;
        }
    } else {
        uint32_t uVar2 = (uVar1 << 2) ^ (rd32(desc, 0x18) & 3);
        if (rd32(self, 0xc) != uVar2) {
            wr32(self, 0xc, uVar2);
            uint32_t local_24 = (4 - (rd32(desc, 0x18) & 3)) & 3;
            if (uVar1 < local_24) local_24 = uVar1;
            uVar2 = (uVar1 - local_24) >> 2;
            uVar1 = (uVar1 - local_24) & 3;
            if (local_24 == 0 && uVar1 == 0) {
                wr32(self, 0x10, rd32(self, 0x4c + uVar2 * 4));
                void* target = ((rd32(self, 8) & 0x4000000) == 0) ? (void*)&blit_end_plain
                                                                  : (void*)&blit_end_strided;
                int32_t dst = (int32_t)rd32(self, 0x44) + (int32_t)rd32(self, 0x48) - 4;
                *(int32_t*)(uintptr_t)dst = (int32_t)(uintptr_t)target - dst;
            } else {
                wr32(self, 0x10, rd32(self, 0x1c + local_24 * 4));
                int32_t dst1 = (int32_t)rd32(self, 0x14) + (int32_t)rd32(self, 0x18) - 4;
                *(int32_t*)(uintptr_t)dst1 = (int32_t)rd32(self, 0x4c + uVar2 * 4) - dst1;
                int32_t dst2 = (int32_t)rd32(self, 0x44) + (int32_t)rd32(self, 0x48) - 4;
                *(int32_t*)(uintptr_t)dst2 = (int32_t)rd32(self, 0x34 + uVar1 * 4) - dst2;
                void* target = ((rd32(self, 8) & 0x4000000) == 0) ? (void*)&blit_end_plain
                                                                  : (void*)&blit_end_strided;
                int32_t dst3 = (int32_t)rd32(self, 0x2c) + (int32_t)rd32(self, 0x30) - 4;
                *(int32_t*)(uintptr_t)dst3 = (int32_t)(uintptr_t)target - dst3;
            }
        }
    }
    typedef void (*BlitFn)(uint32_t, uint8_t*, uint32_t, uint32_t, void*);
    BlitFn fn = (BlitFn)(uintptr_t)rd32(self, 0x10);
    fn(rd32(desc, 0xc), desc, rd32(desc, 0x14), rd32(desc, 0x10), nullptr);
}

// 1.13c Storm+0x3A330: open an AUSTREAM. The directory part param_1 and the file name param_2 are
// upper-cased (a leading '#' becomes '1'/'4' under the 0x40000 flag); '\\' is inserted when param_1
// has no trailing separator, then the first plane is decoded through 0x291E0, copied into a
// param_4-row buffer (three planes when flag 0x40000 is set), optionally followed by the palette
// block, and the same is done again with '#4' substituted when the flag is set. The finished
// stream is linked into the global AUSTREAM list under 0x6FC45858 and returned via param_6.
extern "C"
uint32_t Storm_Sub3A330(uint32_t* param_1, uint32_t* param_2, int32_t* param_3, int32_t param_4,
                        uint32_t param_5, uint32_t* param_6, const StormBatch34110Env* env) {
    void* savedList = g_exceptionList;
    g_exceptionList = &savedList;
    if (param_3 != nullptr) *param_3 = 0;
    if (param_2 == nullptr || *(const char*)param_2 == '\0' || param_4 == 0 || param_6 == nullptr) {
        set_error(0x57);
        env->set_last_error(0x57);
        g_exceptionList = savedList;
        return 0;
    }
    uint32_t uVar8 = (param_5 >> 0x12) & 1;
    char local_20c[0x100];
    char local_10c[0x100];
    env->f_2b080(local_20c, "s___AUSTREAM___", 0x7FFFFFFF);
    uint8_t* local_238 = (uint8_t*)local_20c;
    local_10c[0] = '\0';
    if (param_1 != nullptr) {
        if (uVar8 == 0) {
            char* pcVar2 = env->f_2afd0(local_20c);
            local_238 = (uint8_t*)local_20c + (uintptr_t)pcVar2;
            env->f_2ae00(local_20c, (const char*)param_1, 0x100);
        } else {
            env->f_2b080(local_20c, (const char*)param_1, 0x7FFFFFFF);
        }
    }
    env->f_2b080(local_10c, (const char*)param_2, 0x7FFFFFFF);
    if (uVar8 != 0) {
        if (local_20c[0] == '#') local_20c[0] = '1';
        if (local_10c[0] == '#') local_10c[0] = '4';
    }
    env->f_strupr(local_20c);
    env->f_strupr(local_10c);
    uint32_t* puVar9 = (uint32_t*)(uintptr_t)env->f_1c580((uint32_t)param_4 * 4 + 0x50,
                                                          "s___AUSTREAM___", -2, 8);
    if (puVar9 != nullptr) {
        puVar9[0] = 0;
        puVar9[1] = 0;
    }
    puVar9[2] = param_5;
    puVar9[3] = 0xFFFFFFFF;
    SFileImage img = {};
    img.vtable = 0x6FC36840;
    env->f_1120((uint32_t*)&img);
    img.vtable = 0x6FC36840;
    int32_t iVar3 = (int32_t)env->f_291e0((uint32_t*)&img, local_20c, param_3);
    if (param_3 != nullptr && *param_3 != 0) {
        *param_3 = (*param_3 - (int32_t)(uintptr_t)local_238) + (int32_t)(uintptr_t)param_1;
    }
    uint32_t uVar1;
    if (iVar3 == 0) {
        uVar1 = env->f_28eb0(puVar9);
        env->f_11a0((uint32_t*)&img);
    } else {
        uint32_t planes = (uVar8 != 0) ? 3u : 1u;
        uint32_t stride = img.stride;
        uint32_t palSize = img.paletteSize;
        uint32_t total = stride * planes + palSize;
        puVar9[6] = total;
        uint8_t* puVar4 = (uint8_t*)(uintptr_t)env->f_1c580(total, "..\\3rdParty\\STORM\\SOURCE\\SCODE.CPP",
                                                           0x440, 0);
        puVar9[5] = (uint32_t)(uintptr_t)puVar4;
        uint8_t* second = nullptr;
        if (uVar8 != 0) {
            uint32_t total2 = stride * planes + palSize;
            puVar9[0xc] = total2;
            second = (uint8_t*)(uintptr_t)env->f_1c580(total2,
                                                       "..\\3rdParty\\STORM\\SOURCE\\SCODE.CPP", 0x444, 0);
            puVar9[0xb] = (uint32_t)(uintptr_t)second;
        }
        local_238 = second;
        const uint8_t* src = (const uint8_t*)(uintptr_t)img.data;
        copy_strided(puVar4, src, stride, planes);
        if (uVar8 != 0) {
            copy_strided(local_238, src, stride, planes);
        }
        std::memcpy(puVar4, (const void*)(uintptr_t)img.palette, palSize);
        if (uVar8 != 0) {
            std::memcpy(local_238, (const void*)(uintptr_t)img.palette, palSize);
            uint32_t* offData = puVar9 + 7;   // +0x1c
            uint32_t* offPal = puVar9 + 0xd;  // +0x34
            for (uint32_t p = planes; ; p--) {
                *offData++ = stride * p + puVar9[5];
                *offPal++ = stride * p + puVar9[0xb];
                if (p == 0) break;
            }
        }
        iVar3 = (int32_t)env->f_291e0((uint32_t*)&img, local_10c, param_3);
        if (param_3 != nullptr && *param_3 != 0) {
            *param_3 = (*param_3 - (int32_t)(uintptr_t)local_10c) + (int32_t)(uintptr_t)param_2;
        }
        if (iVar3 == 0) {
            EnterCriticalSection((LPCRITICAL_SECTION)&g_streamLock);
            env->f_1090(puVar9);
            Storm_Sub1B960Env(puVar9, (uint8_t*)"s___AUSTREAM___", -2, env);
            LeaveCriticalSection((LPCRITICAL_SECTION)&g_streamLock);
        } else {
            uint32_t palTotal = stride * (uint32_t)param_4 + palSize;
            puVar9[0x12] = palTotal;
            uint8_t* palDst = (uint8_t*)(uintptr_t)env->f_1c580(palTotal,
                                                               "..\\3rdParty\\STORM\\SOURCE\\SCODE.CPP",
                                                               0x46e, 0);
            puVar9[0x11] = (uint32_t)(uintptr_t)palDst;
            if (param_4 != 0) {
                int32_t rows = param_4;
                do {
                    std::memcpy(palDst, (const void*)(uintptr_t)img.data, stride);
                    palDst += stride;
                    rows--;
                } while (rows != 0);
            }
            std::memcpy(palDst, (const void*)(uintptr_t)img.palette, palSize);
            uint32_t* offPal = puVar9 + 0x13; // +0x4c
            for (int32_t p = param_4; p >= 0; p--) *offPal++ = stride * (uint32_t)p + puVar9[0x11];
            EnterCriticalSection((LPCRITICAL_SECTION)&g_streamLock);
            stream_list_init();
            uint32_t next = puVar9[0];
            if (next != 0) {
                uint32_t prev = puVar9[1];
                uint32_t* slot;
                if ((int32_t)prev < 0) slot = (uint32_t*)(uintptr_t)(~prev);
                else slot = (uint32_t*)((uint8_t*)puVar9 + (prev - *(int32_t*)(uintptr_t)(next + 4)));
                *slot = next;
                *(uint32_t*)(uintptr_t)(puVar9[0] + 4) = puVar9[1];
                puVar9[0] = 0;
                puVar9[1] = 0;
            }
            puVar9[0] = (uint32_t)(uintptr_t)g_streamAnchor;
            puVar9[1] = g_streamAnchor[1];
            *(uint32_t*)(uintptr_t)g_streamAnchor[1] = (uint32_t)(uintptr_t)puVar9;
            g_streamAnchor[1] = (uint32_t)(uintptr_t)puVar9;
            LeaveCriticalSection((LPCRITICAL_SECTION)&g_streamLock);
            *param_6 = (uint32_t)(uintptr_t)puVar9;
        }
        env->f_11a0((uint32_t*)&img);
        uVar1 = 1;
    }
    g_exceptionList = savedList;
    return uVar1;
}
