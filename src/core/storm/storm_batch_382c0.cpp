// src/core/storm/storm_batch_382c0.cpp -- 1.13c Storm.dll rows 0x382C0, 0x385C0, 0x38470, 0x329C0,
// 0x333C0, 0x331F0, 0x38EB0, 0x19C80, 0x198D0 and 0x18F00 reimplemented from the decompiles in
// build-wasm/ghidra-storm/decomp/0x<RVA>.c (alpha/implementation lane: marker only, no spec, no vector).
//
// These are Storm's SDRAW image/blit layer: the blit core and its clipped/tiled callers, the image
// loader family (LoadImage by format and by extension), the surface free, the screen-capture save and
// the shutdown / message-pump pair. Retail globals are file statics below, each tagged with its
// 1.13c address. The unmapped internal callees and the kernel32/user32 imports are host seams on
// StormBatch382c0Env, which every body takes AFTER its retail argument list (the
// src/core/d2client/d2client_batch_2190.cpp pattern).
//
// Two rows are deliberately not here: 0x387F0 (636 bytes) patches self-relative 32-bit machine-code
// trampolines into the blitter dispatch buffer and 0x3A330 (1300 bytes) reads four stack words Ghidra
// never assigns; neither transcribes faithfully in this lane.
#include <cstdint>
#include <cstring>
#include <strings.h>
#include <Windows.h>

struct StormBatch382c0Env {
    // ---- unmapped internal callees, retail argument order --------------------------------------
    void     (*f_1090)(void* block);                              // 0x1090, pre-free of a surface
    void     (*f_b960)(void* p, const char* file, int32_t line);  // 0x0B960 debug free
    void*    (*f_c580)(uint32_t size, const char* file, int32_t line, uint32_t type); // 0x0C580
    void     (*f_37e80)(void* self, uint32_t* param_7);           // 0x37E80
    uint32_t (*f_1360)(uint32_t format);                          // 0x1360 blitter select
    uint32_t (*f_387f0)(int32_t blitter, void* desc);             // 0x387F0 blitter (row skipped)
    // The width-specialised fast path: retail indexes the jump table at 0x6FC28390 with width>>2
    // after FUN_6fbf1350(0x6FC4587C); the table is unrecovered, so the body is this one seam.
    uint32_t (*f_fastblit)(void* self, uint32_t dst, uint32_t src, uint32_t width, int32_t height,
                           int32_t dpitch, int32_t spitch, uint32_t fmt1, uint32_t fmt2);
    const char* (*f_22710)(const char* path);                     // 0x22710 file extension
    uint32_t (*f_22c00)(void* file, int32_t palette, uint32_t dst, int32_t width);        // PCX
    uint32_t (*f_23070)(int32_t palette, void* dst, int32_t width);                       // GIF
    uint32_t (*f_22170)(void* file, int32_t palette, void* dst, int32_t width,
                        int32_t height, uint32_t pitch);                                  // TGA
    uint32_t (*f_22ec0)(void* file, void* dst, int32_t height);                           // raw
    uint32_t (*f_320b0)(uint32_t* w_out, uint32_t* h_out, void* out, uint8_t* src, int32_t size);
    uint32_t (*f_31e80)(int16_t* header, int32_t* h_out, uint32_t size, void* out, uint8_t* src,
                        int32_t param_6, int32_t* w_out);
    uint32_t (*f_328c0)(uint32_t size, void* out, uint8_t* src, int32_t param_6, uint32_t* a,
                        uint32_t* b, uint32_t* c);
    uint32_t (*f_9150)(void);                                     // 0x09150, capture begin
    void     (*f_8f90)(void);                                     // 0x08F90 palette reset
    uint32_t (*f_92a0)(int32_t a, void* b, int32_t* w, void** pixels, int32_t e); // 0x092A0
    void     (*f_9040)(int32_t a, int32_t b);                     // 0x09040 capture end
    uint32_t (*f_2b080)(char* dst, const char* src, uint32_t size); // 0x2B080 bounded strcpy
    uint32_t (*f_d4d0)(void);                                     // 0x0D4D0
    uint32_t (*f_7d60)(void);                                     // 0x07D60
    uint32_t (*f_13250)(void);                                    // 0x13250
    // ---- kernel32 / user32 imports ------------------------------------------------------------
    void*    (*create_file_a)(const char* name, uint32_t access, uint32_t share, void* sa,
                              uint32_t disposition, uint32_t flags, void* templ);
    int32_t  (*close_handle)(void* h);
    void*    (*get_module_handle)(const char* name);
    int32_t  (*get_save_file_name)(void* ofn);
    void     (*delete_object)(void* obj);
    int32_t  (*destroy_cursor)(void* cursor);
    int32_t  (*invalidate_rect)(void* hwnd, void* rect, int32_t erase);
};

namespace {

int32_t g_lastError;              // 0x6FC45388  last Storm error, mirrored to SetLastError
CRITICAL_SECTION g_imgLock;       // 0x6FC45858

// SDRAW module state.
void*    g_453dc;                 // 0x6FC453DC  HGDIOBJ owned by the capture path
int32_t  g_453cc;                 // 0x6FC453CC  non-zero while g_453dc is live
int32_t  g_453c8;                 // 0x6FC453C8  non-zero while the display objects are live
int32_t  g_453d4;                 // 0x6FC453D4  set while the save dialog is up
void*    g_453d0;                 // 0x6FC453D0  HCURSOR
void*    g_453d8;                 // 0x6FC453D8  HWND
void*    g_453e4;                 // 0x6FC453E4  display interface (vtable +0x10 = set palette)
void*    g_453e8;                 // 0x6FC453E8  palette interface
uint32_t g_453f0[0x100];          // 0x6FC453F0  current palette
int32_t  g_41a1c;                 // 0x6FC41A1C  bits per pixel
uint32_t g_41a20;                 // 0x6FC41A20  width
uint32_t g_41a24;                 // 0x6FC41A24  height
int32_t  g_42fd0;                 // 0x6FC42FD0
// 0x6FC45804..0x6FC45810 are walked as one 4-entry vtable array by 0x198D0.
void*    g_45804;                 // 0x6FC45804  primary draw interface
void*    g_45808;                 // 0x6FC45808  (also a count test in 0x18F00)
void*    g_4580c;                 // 0x6FC4580C
void*    g_45810;                 // 0x6FC45810

void StormSetLastError(uint32_t e) { g_lastError = (int32_t)e; }

// (**(code **)(*obj + off))(obj, ...) -- retail calls the object's vtable slot.
template <typename Fn>
Fn vcall(void* obj, uintptr_t off) {
    return (Fn)(*(void***)obj)[off / sizeof(void*)];
}

} // namespace

// 1.13c Storm+0x38EB0: free an image surface. NULL is the only rejected argument; the two locking
// seams and the raw release run under the image lock and the row always reports success afterwards.
extern "C"
uint32_t Storm_Sub38EB0(void* param_1, const StormBatch382c0Env* env) {
    if (param_1 == nullptr) {
        StormSetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_imgLock);
    env->f_1090(param_1);
    env->f_b960(param_1, "__AUSTREAM__", -2);
    LeaveCriticalSection(&g_imgLock);
    return 1;
}

// 0x331F0 is defined below; 0x333C0 is its tail call.
extern "C"
uint32_t Storm_Sub331F0(const char* name, int32_t palette, void* dst, int32_t width, int32_t height,
                        int32_t bpp, uint32_t pitch, const StormBatch382c0Env* env);

// 1.13c Storm+0x333C0: 0x331F0 with a zeroed pitch (the loader recomputes it from the bpp).
extern "C"
uint32_t Storm_Sub333C0(const char* name, int32_t palette, void* dst, int32_t width, int32_t height,
                        int32_t bpp, const StormBatch382c0Env* env) {
    return Storm_Sub331F0(name, palette, dst, width, height, bpp, 0, env);
}

// 1.13c Storm+0x331F0: open an image file and dispatch on its extension (.pcx/.gif/.tga, else raw).
// The pitch is derived from the bpp when the caller passes 0. Only 8bpp requires a palette, and only
// .tga accepts anything other than 8bpp.
extern "C"
uint32_t Storm_Sub331F0(const char* name, int32_t palette, void* dst, int32_t width, int32_t height,
                        int32_t bpp, uint32_t pitch, const StormBatch382c0Env* env) {
    uint32_t ok;
    if (bpp == 8 && palette == 0) ok = 0; else ok = 0xffffffffu;
    if (!((name != nullptr && *name != '\0') && dst != nullptr && ok == 0xffffffffu &&
          width >= 1 && height >= 1)) {
        StormSetLastError(0x57);
        return 0;
    }
    const char* ext = env->f_22710(name);
    if (ext == nullptr || strcasecmp(ext, ".tga") != 0) {
        if (bpp != 8) return 0;
    } else if (bpp != 8 && bpp != 0x18 && bpp != 0x20) {
        return 0;
    }
    if (pitch == 0) {
        int32_t t = bpp + 7;
        pitch = (uint32_t)(((t + ((t >> 31) & 7)) >> 3) * width);
    }
    void* h = env->create_file_a(name, 0x40000000, 0, nullptr, 2, 0x80, nullptr);
    if (h == (void*)(intptr_t)-1) return 0;
    uint32_t r;
    if (ext != nullptr && strcasecmp(ext, ".pcx") == 0) {
        r = env->f_22c00(h, palette, (uint32_t)(uintptr_t)dst, width);
    } else if (ext != nullptr && strcasecmp(ext, ".gif") == 0) {
        r = env->f_23070(palette, dst, width);
    } else if (ext != nullptr && strcasecmp(ext, ".tga") == 0) {
        r = env->f_22170(h, palette, dst, width, height, pitch);
    } else {
        r = env->f_22ec0(h, dst, height);
    }
    env->close_handle(h);
    return r;
}

// 1.13c Storm+0x329C0: LoadImage-style format dispatch. param_1 selects the decoder (0 = sniff from
// the header: 0x050A -> PCX, 'BM' -> BMP, anything else -> TGA); the three out-words are zeroed
// first. A zero size with non-NULL pixel data, or a NULL header/size, is the only rejection.
extern "C"
uint32_t Storm_Sub329C0(int32_t param_1, int16_t* param_2, uint32_t param_3, uint32_t* param_4,
                        uint8_t* param_5, int32_t param_6, uint32_t* param_7, uint32_t* param_8,
                        uint32_t* param_9, const StormBatch382c0Env* env) {
    if (param_7 != nullptr) *param_7 = 0;
    if (param_8 != nullptr) *param_8 = 0;
    if (param_9 != nullptr) *param_9 = 0;
    uint32_t uVar1 = (param_6 == 0 && param_5 != nullptr) ? 0 : 0xffffffffu;
    if (!((param_2 != nullptr && param_3 != 0) && uVar1 == 0xffffffffu)) {
        StormSetLastError(0x57);
        return 0;
    }
    if (param_1 == 0 && param_3 > 1) {
        if (*param_2 == 0x50a) param_1 = 2;
        else param_1 = (uint32_t)(*param_2 != 0x4d42) * 2 + 1;
    }
    if (param_1 == 1) return env->f_320b0(param_8, param_9, param_4, param_5, param_6);
    if (param_1 == 2) {
        return env->f_31e80(param_2, (int32_t*)param_8, param_3, param_4, param_5, param_6,
                            (int32_t*)param_9);
    }
    if (param_1 == 3) {
        return env->f_328c0(param_3, param_4, param_5, param_6, param_7, param_8, param_9);
    }
    return 0;
}

// 1.13c Storm+0x382C0: the blit core. Width/height below 1 is a no-op success. The aligned 32bpp
// fast path is a width-indexed trampoline table retail patches at run time; it is the f_fastblit
// seam. Everything else builds the 0x44-byte descriptor at +0x00..+0x2C and hands it to the blitter
// selected by format.
extern "C"
uint32_t Storm_Sub382C0(void* self, uint32_t param_1, uint32_t param_2, uint32_t param_3,
                        int32_t param_4, int32_t param_5, int32_t param_6, uint32_t param_7,
                        uint32_t param_8, const StormBatch382c0Env* env);

struct BlitDesc {
    uint32_t flags;      // +0x00
    uint32_t zero;       // +0x04
    uint32_t width;      // +0x08
    int32_t height;      // +0x0C
    int32_t ddelta;      // +0x10
    int32_t sdelta;      // +0x14
    uint32_t dst;        // +0x18
    uint32_t src;        // +0x1C
    uint32_t pad0[3];    // +0x20..+0x28
    uint32_t pal;        // +0x2C
    uint32_t pad1[5];    // +0x30..+0x40
};

extern "C"
uint32_t Storm_Sub382C0(void* self, uint32_t param_1, uint32_t param_2, uint32_t param_3,
                        int32_t param_4, int32_t param_5, int32_t param_6, uint32_t param_7,
                        uint32_t param_8, const StormBatch382c0Env* env) {
    if ((int32_t)param_3 < 1 || param_4 < 1) return 1;
    if ((param_8 == 0xcc0020 || param_8 == 0xf00021) && (((param_1 | param_3) & 3) == 0) &&
        param_3 < 0x281) {
        return env->f_fastblit(self, param_1, param_2, param_3, param_4, param_5, param_6,
                               param_7, param_8);
    }
    if (param_8 != 0xcc0020 && param_8 != 0xf00021) env->f_37e80(self, &param_7);
    int32_t blitter = (int32_t)env->f_1360(param_8);
    BlitDesc d;
    d.flags = 0x40;
    d.zero = 0;
    d.width = param_3;
    d.height = param_4;
    d.ddelta = param_5 - (int32_t)param_3;
    d.sdelta = param_6 - (int32_t)param_3;
    d.dst = param_1;
    d.src = param_2;
    d.pal = param_7;
    return env->f_387f0(blitter, &d);
}

// 1.13c Storm+0x385C0: clip two source rectangles against two optional clip rects, intersect the
// two extents, then blit the piece. Negative origins clamp to 0; the destination is
// top*stride + left + base and the source is top*srcStride + left + srcOffset (0 when no offset).
extern "C"
uint32_t Storm_Sub385C0(int32_t param_1, uint32_t* param_2, int32_t* param_3, int32_t param_4,
                        int32_t param_5, uint32_t* param_6, int32_t* param_7, int32_t param_8,
                        uint32_t param_9, uint32_t param_10, const StormBatch382c0Env* env) {
    uint32_t uVar6 = 0, uVar3 = 0x7fffffff, uVar2 = 0, uVar4 = 0x7fffffff;
    uint32_t local_8 = 0, local_c = 0;
    uint32_t local_14 = 0x7fffffff, local_10 = 0x7fffffff;
    if (param_2 != nullptr) {
        uVar3 = param_2[0];
        uVar4 = param_2[1];
        local_8 = ((int32_t)uVar3 < 0) ? 0 : uVar3;
        uVar3 = param_2[2] - uVar3;
        uVar2 = ((int32_t)uVar4 < 0) ? 0 : uVar4;
        uVar4 = param_2[3] - uVar4;
    }
    if (param_3 != nullptr) {
        uint32_t t = (uint32_t)(param_3[0] - (int32_t)local_8);
        uint32_t s = ((int32_t)t < 0) ? 0 : t;
        if ((int32_t)s <= (int32_t)uVar3) uVar3 = s;
        t = (uint32_t)(param_3[1] - (int32_t)uVar2);
        s = ((int32_t)t < 0) ? 0 : t;
        if ((int32_t)s <= (int32_t)uVar4) uVar4 = s;
    }
    if (param_6 != nullptr) {
        local_14 = param_6[0];
        local_10 = param_6[1];
        local_c = ((int32_t)local_14 < 0) ? 0 : local_14;
        uVar6 = ((int32_t)local_10 < 0) ? 0 : local_10;
        local_14 = param_6[2] - local_14;
        local_10 = param_6[3] - local_10;
    }
    if (param_7 != nullptr) {
        uint32_t t = (uint32_t)(param_7[0] - (int32_t)local_c);
        uint32_t s = ((int32_t)t < 0) ? 0 : t;
        if ((int32_t)s <= (int32_t)local_14) local_14 = s;
        t = (uint32_t)(param_7[1] - (int32_t)uVar6);
        s = ((int32_t)t < 0) ? 0 : t;
        if ((int32_t)s <= (int32_t)local_10) local_10 = s;
    }
    if ((int32_t)local_14 <= (int32_t)uVar3) uVar3 = local_14;
    if ((int32_t)local_10 <= (int32_t)uVar4) uVar4 = local_10;
    if ((int32_t)uVar3 > 0 && (int32_t)uVar4 > 0) {
        uint32_t srcOff = (param_5 == 0) ? 0 : uVar6 * (uint32_t)param_8 + local_c +
                                                     (uint32_t)param_5;
        void* dst = (void*)(uintptr_t)(uVar2 * (uint32_t)param_4 + local_8 + (uint32_t)param_1);
        return Storm_Sub382C0(dst, (uint32_t)(uintptr_t)dst, srcOff, uVar3, (int32_t)uVar4,
                              param_4, param_8, param_9, param_10, env);
    }
    return 1;
}

// 1.13c Storm+0x38470: tile the source rect across the destination rect. The first row/column of
// each tile is shifted by the caller's fractional offsets (modulo the source extent); the blit
// covers the smaller of the remaining destination extent and the source extent. Returns 0 only when
// the blit reports failure, 1 when the destination rect is exhausted.
extern "C"
uint32_t Storm_Sub38470(int32_t param_1, int32_t* param_2, int32_t param_3, int32_t param_4,
                        int32_t* param_5, int32_t param_6, int32_t param_7, int32_t param_8,
                        uint32_t param_9, uint32_t param_10, const StormBatch382c0Env* env) {
    int32_t iVar1 = param_5[2];
    int32_t iVar2 = param_5[1];
    int32_t iVar3 = param_5[0];
    int32_t iVar8 = param_2[1];
    int32_t iVar4 = param_5[3];
    int32_t iVar10 = param_2[3];
    if (iVar10 <= iVar8) return 1;
    do {
        int32_t iVar6 = param_5[1];
        if (iVar8 == param_2[1]) iVar6 += ((iVar4 - iVar2) + param_8) % (iVar4 - iVar2);
        int32_t local_14 = param_5[3] - iVar6;
        if (iVar10 - iVar8 <= param_5[3] - iVar6) local_14 = iVar10 - iVar8;
        iVar10 = param_2[0];
        int32_t iVar5 = param_2[2];
        if (iVar10 < iVar5) {
            do {
                int32_t iVar7 = param_5[0];
                if (iVar10 == param_2[0]) iVar7 += (param_7 + (iVar1 - iVar3)) % (iVar1 - iVar3);
                uint32_t uVar9 = (uint32_t)(param_5[2] - iVar7);
                if (iVar5 - iVar10 <= param_5[2] - iVar7) uVar9 = (uint32_t)(iVar5 - iVar10);
                uint32_t iVar5b = Storm_Sub382C0(
                    (void*)(intptr_t)local_14,
                    (uint32_t)(iVar10 + iVar8 * param_3 + param_1),
                    (uint32_t)(iVar7 + iVar6 * param_6 + param_4), uVar9, local_14, param_3,
                    param_6, param_9, param_10, env);
                if (iVar5b == 0) return 0;
                iVar5 = param_2[2];
                iVar10 += (int32_t)uVar9;
            } while (iVar10 < iVar5);
        }
        iVar10 = param_2[3];
        iVar8 += local_14;
        if (iVar10 <= iVar8) return 1;
    } while (true);
}

// 1.13c Storm+0x19C80: save a screen capture. Only runs with the display up at 8bpp. Without a
// caller path it opens the "Save Screen Capture" dialog (field offsets of OPENFILENAMEA below; the
// dialog buffer is 0x150 bytes and its +0x4C holds the 0x104-byte filename). The screen is grabbed
// through 0x92A0, blitted into the capture buffer with the primary blit, then written as PCX.
extern "C"
uint32_t Storm_Sub19C80(uint32_t* param_1, const StormBatch382c0Env* env) {
    if (g_453e4 == nullptr || g_453d8 == nullptr || g_45804 == nullptr || g_41a1c != 8) return 0;
    env->f_9150();
    uint8_t* cap = (uint8_t*)env->f_c580(g_41a24 * g_41a20,
                                         "..\\3rdParty\\STORM\\SOURCE\\SDRAW.CPP", 0x19a, 0);
    if (g_453e8 != nullptr) {
        typedef void (*SetPalFn)(void*, int32_t, int32_t, uint32_t, void*);
        vcall<SetPalFn>(g_453e8, 0x10)(g_453e8, 0, 0, 0x100, g_453f0);
        env->f_8f90();
    }
    int32_t local_158 = 0;
    void* local_154 = nullptr;
    if (env->f_92a0(0, nullptr, &local_158, &local_154, 0) != 0) {
        Storm_Sub382C0(local_154, (uint32_t)(uintptr_t)cap, (uint32_t)local_158, g_41a20, g_41a24,
                       (int32_t)g_41a20, (int32_t)(intptr_t)local_154, 0, 0xcc0020, env);
        env->f_9040(0, local_158);
    }
    uint8_t ofn[0x150];
    if (param_1 == nullptr || *(char*)param_1 == '\0') {
        std::memset(ofn, 0, 0x4c);
        auto put = [&](uintptr_t off, uint32_t v) { std::memcpy(ofn + off, &v, 4); };
        put(0x00, 0x4c);                                    // lStructSize
        put(0x04, (uint32_t)(uintptr_t)g_453d8);            // hwndOwner
        put(0x08, (uint32_t)(uintptr_t)env->get_module_handle(nullptr)); // hInstance
        put(0x1c, (uint32_t)(uintptr_t)(ofn + 0x4c));       // lpstrFile
        put(0x0c, (uint32_t)(uintptr_t)"PC Paintbrush (*.pcx)"); // lpstrFilter
        put(0x18, 1);                                       // nFilterIndex
        put(0x20, 0x104);                                   // nMaxFile
        put(0x30, (uint32_t)(uintptr_t)"Save Screen Capture");   // lpstrTitle
        put(0x34, 0x80e);                                   // Flags
        put(0x3c, (uint32_t)(uintptr_t)".pcx");             // lpstrDefExt
        g_453d4 = 1;
        int32_t ok = env->get_save_file_name(ofn);
        g_453d4 = 0;
        if (ok == 0) {
            env->f_b960(cap, "..\\3rdParty\\STORM\\SOURCE\\SDRAW.CPP", 0x1ca);
            return 0;
        }
    } else {
        env->f_2b080((char*)ofn + 0x4c, (const char*)param_1, 0x104);
    }
    Storm_Sub333C0((const char*)ofn + 0x4c, (int32_t)(uintptr_t)g_453f0, cap, (int32_t)g_41a20,
                   (int32_t)g_41a24, g_41a1c, env);
    env->f_b960(cap, "..\\3rdParty\\STORM\\SOURCE\\SDRAW.CPP", 0x1d7);
    return 1;
}

// 1.13c Storm+0x198D0: shut the display layer down. Runs the three teardown seams, releases the
// capture bitmap, resets the mode to 8bpp 640x480, releases the cursor, calls vtable slot 2 on the
// four draw interfaces and on the two mode interfaces, then clears the module state. The result is
// the bit-wise AND of the three seam results with the low bit of the first.
extern "C"
uint32_t Storm_Sub198D0(const StormBatch382c0Env* env) {
    uint32_t uVar2 = env->f_d4d0();
    uint32_t uVar3 = env->f_7d60();
    uint32_t uVar4 = env->f_13250();
    if (g_453dc != nullptr && g_453cc != 0) {
        g_453cc = 0;
        env->delete_object(g_453dc);
    }
    g_41a1c = 8;
    g_41a20 = 0x280;
    g_41a24 = 0x1e0;
    if (g_453c8 != 0) {
        g_453c8 = 0;
        if (g_453d0 != nullptr) env->destroy_cursor(g_453d0);
        void* arr[4] = {g_45804, g_45808, g_4580c, g_45810};
        for (int i = 3; i >= 0; --i) {
            if (arr[i] != nullptr) {
                typedef void (*ReleaseFn)(void*);
                vcall<ReleaseFn>(arr[i], 8)(arr[i]);
            }
        }
        if (g_453e8 != nullptr) {
            typedef void (*ReleaseFn)(void*);
            vcall<ReleaseFn>(g_453e8, 8)(g_453e8);
        }
        if (g_453e4 != nullptr) {
            typedef void (*ReleaseFn)(void*);
            vcall<ReleaseFn>(g_453e4, 8)(g_453e4);
        }
    }
    g_453dc = nullptr;
    g_453e8 = nullptr;
    g_45810 = nullptr;
    g_4580c = nullptr;
    g_45808 = nullptr;
    g_45804 = nullptr;
    g_453e4 = nullptr;
    g_453d0 = nullptr;
    if (g_453d8 != nullptr) {
        g_42fd0 = 0;
        g_453d8 = nullptr;
    }
    return uVar2 & 1 & uVar3 & uVar4;
}

// 1.13c Storm+0x18F00: pump the message queue while the display is live and the save dialog is not.
// Vtable slot 0x2C (pump) is retried on -0x7789FE3E (a "still busy" HRESULT); on that result slot
// 0x6C drains, and a non-zero drain result aborts with 0. The outer loop repeats on -0x7789FDE4 and
// otherwise reports 1. Without a live display it reports 0.
extern "C"
uint32_t Storm_Sub18F00(const StormBatch382c0Env* env) {
    if (g_453d4 != 0 || g_453e4 == nullptr || g_45804 == nullptr || g_45808 == nullptr) return 0;
    int32_t iVar1;
    do {
        while ((iVar1 = vcall<int32_t (*)(void*, int32_t, int32_t)>(g_45804, 0x2c)(
                    g_45804, 0, 1)) == (int32_t)0x887601c2u) {
            if (g_45804 != nullptr) {
                iVar1 = vcall<int32_t (*)(void*)>(g_45804, 0x6c)(g_45804);
                if (iVar1 != 0) return 0;
                env->invalidate_rect(g_453d8, nullptr, 0);
            }
        }
    } while (iVar1 == (int32_t)0x8876021cu);
    return 1;
}
