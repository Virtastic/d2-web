// src/core/d2launch/d2launch_batch_z31.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (batch z31).
// VS2003 CRT. Retail 32-bit layouts; globals and callees are seams (uint32_t links).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>

// Seams: Win32 / CRT callees
static uint32_t (*z31_HeapCreate)(uint32_t flags, uint32_t init, uint32_t max) = nullptr;
static void (*z31_HeapDestroy)(uint32_t) = nullptr;
static void (*z31_HeapFree)(uint32_t, uint32_t, uint32_t) = nullptr;
static void (*z31_VirtualFree)(uint32_t, uint32_t, uint32_t) = nullptr;
static int (*z31_sbh_heap_init)(uint32_t) = nullptr;            // 0x6CA3
static uint32_t (*z31_get_osfhandle)(uint32_t) = nullptr;       // 0x3B0C
static uint32_t (*z31_SetFilePointer)(uint32_t, int32_t, void*, uint32_t) = nullptr;
static uint32_t (*z31_GetLastError)() = nullptr;
static int (*z31_WriteFile)(uint32_t, const void*, uint32_t, uint32_t*, void*) = nullptr;
static uint32_t (*z31_GetStdHandle)(uint32_t) = nullptr;
static uint32_t (*z31_GetModuleFileNameA)(uint32_t, char*, uint32_t) = nullptr;
static void (*z31_dosmaperr)(uint32_t) = nullptr;               // 0x3B0C-area __dosmaperr
static int32_t* (*z31_errno_ptr)() = nullptr;                   // 0x3AFA
static uint32_t* (*z31_doserrno_ptr)() = nullptr;               // 0x3B03
static void (*z31_lock_fhandle)(int) = nullptr;                 // 0x6A65
static void (*z31_unlock_fhandle_lseek)() = nullptr;            // 0x5658
static void (*z31_unlock_fhandle_write)() = nullptr;            // 0x58BF
static int32_t (*z31_lseeki64_lk)(uint32_t, uint32_t, uint32_t, uint32_t) = nullptr;  // 0x68A6
static void (*z31_crtMessageBoxA)(const char*, const char*, uint32_t) = nullptr;      // 0x5AC2
static void* (*z31_nh_malloc)(size_t, int32_t) = nullptr;       // 0x59CC
static void* (*z31_getptd)() = nullptr;                         // 0x4714
static void* (*z31_updatetlocinfo)() = nullptr;                 // 0x7A13
static int (*z31_wctomb_mt)(void*, char*, uint16_t) = nullptr;  // 0x5A0A

// Seams: globals
static int32_t z31_osver_mode = 0, z31_winmajor = 0;            // DAT_6fa64de4 / 6fa64df0
static uint32_t z31_heap = 0;                                   // DAT_6fa66f7c
static int32_t z31_heap_type = 0;                               // DAT_6fa66f80
static int32_t z31_sbh_count = 0;                               // DAT_6fa66f64
static uint8_t* z31_sbh_regions = nullptr;                      // DAT_6fa66f68
static int32_t z31_app_type = 0, z31_app_type2 = 0;             // DAT_6fa64e34 / e38
static void (*z31_user_banner)() = nullptr;                     // DAT_6fa650b4
static uint32_t z31_nhandle = 0;                                // DAT_6fa66f84
static uint8_t* z31_pioinfo[64] = {};                           // DAT_6fa66fa0 (entries 0x24 bytes)
static int32_t z31_newmode = 0;                                 // DAT_6fa65124
static void* z31_ptloci_cur = nullptr;                          // PTR_DAT_6fa6075c
struct Z31Ptd { uint8_t pad[0x4C]; void* locinfo; };            // _tfpecode slot, guessed offset
// DAT_6fa60618: 19 {code, msg} pairs
struct Z31Msg { int32_t code; const char* msg; };
static Z31Msg z31_rterrs[19] = {};

// 1.13c D2Launch+0x5102
uint32_t z31_heap_select() {
    if (z31_osver_mode == 2 && 4 < z31_winmajor) return 1;
    return 3;
}

// 1.13c D2Launch+0x511C
int z31_heap_init(int isMT /* stack arg */) {
    z31_heap = z31_HeapCreate(isMT == 0, 0x1000, 0);
    if (!z31_heap) return 0;
    z31_heap_type = (int32_t)z31_heap_select();
    if (z31_heap_type == 3 && z31_sbh_heap_init(0x3f8) == 0) { z31_HeapDestroy(z31_heap); return 0; }
    return 1;
}

// 1.13c D2Launch+0x516D
void z31_heap_term() {
    if (z31_heap_type == 3) {
        uint32_t* p = (uint32_t*)(z31_sbh_regions + 0xc);
        for (int i = 0; i < z31_sbh_count; i++, p += 5) {
            z31_VirtualFree(p[0], 0x100000, 0x4000);
            z31_VirtualFree(p[0], 0, 0x8000);
            z31_HeapFree(z31_heap, 0, p[1]);
        }
        z31_HeapFree(z31_heap, 0, (uint32_t)(uintptr_t)z31_sbh_regions);
    }
    z31_HeapDestroy(z31_heap);
}

// 1.13c D2Launch+0x53AC  (__NMSG_WRITE)
void z31_nmsg_write(int rterrnum) {
    uint32_t i = 0;
    do { if (rterrnum == z31_rterrs[i].code) break; i++; } while (i < 0x13);
    if (i >= 0x13 || rterrnum != z31_rterrs[i].code) return;
    const char* msg = z31_rterrs[i].msg;
    if (z31_app_type == 1 || (z31_app_type == 0 && z31_app_type2 == 1)) {
        uint32_t n;
        z31_WriteFile(z31_GetStdHandle(0xfffffff4), msg, (uint32_t)strlen(msg), &n, nullptr);
    } else if (rterrnum != 0xfc) {
        char prog[0x104 + 8] = {};
        if (z31_GetModuleFileNameA(0, prog, 0x104) == 0) strcpy(prog, "<program name unknown>");
        const char* name = prog;
        char tail[64];
        if (strlen(prog) + 1 > 0x3c) {                  // keep last 0x3c-4 chars after "..."
            memcpy(tail, "...", 3);
            strncpy(tail + 3, prog + strlen(prog) - 0x3c + 4, sizeof(tail) - 4);
            tail[sizeof(tail) - 1] = 0;
            name = tail;
        }
        char buf[0x400];
        snprintf(buf, sizeof buf, "Runtime Error!\n\nProgram: %s\n\n%s", name, msg);  // DAT_6fa5ca48 = "\n\n" (guessed)
        z31_crtMessageBoxA(buf, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}

// 1.13c D2Launch+0x5524  (__FF_MSGBANNER)
void z31_ff_msgbanner() {
    if (z31_app_type == 1 || (z31_app_type == 0 && z31_app_type2 == 1)) {
        z31_nmsg_write(0xfc);
        if (z31_user_banner) z31_user_banner();
        z31_nmsg_write(0xff);
    }
}

static inline uint8_t* z31_pio(uint32_t fh) { return z31_pioinfo[(int32_t)fh >> 5] + (fh & 0x1f) * 0x24; }

// 1.13c D2Launch+0x555D  (__lseek_lk)
uint32_t z31_lseek_lk(uint32_t fh, int32_t off, uint32_t origin) {
    uint32_t h = z31_get_osfhandle(fh);
    if (h == 0xffffffffu) { *z31_errno_ptr() = 9; return 0xffffffffu; }
    uint32_t r = z31_SetFilePointer(h, off, nullptr, origin);
    uint32_t err = (r == 0xffffffffu) ? z31_GetLastError() : 0;
    if (err == 0) { z31_pio(fh)[4] &= 0xfd; return r; }
    z31_dosmaperr(err);
    return 0xffffffffu;
}

// 1.13c D2Launch+0x55D1  (_lseek)
long z31_lseek(int fh, long off, int origin) {
    if ((uint32_t)fh < z31_nhandle && (z31_pio(fh)[4] & 1)) {
        z31_lock_fhandle(fh);
        long r;
        if (!(z31_pio(fh)[4] & 1)) { *z31_errno_ptr() = 9; *z31_doserrno_ptr() = 0; r = -1; }
        else r = (long)z31_lseek_lk(fh, off, origin);
        z31_unlock_fhandle_lseek();
        return r;
    }
    *z31_errno_ptr() = 9; *z31_doserrno_ptr() = 0;
    return -1;
}

// 1.13c D2Launch+0x567C  (_write_lk)
int z31_write_lk(uint32_t fh, const char* buf, uint32_t count) {
    if (count == 0) return 0;
    int crs = 0; uint32_t written = 0, werr = 0, n;
    uint8_t* pio = z31_pio(fh);
    if (pio[4] & 0x20) z31_lseeki64_lk(fh, 0, 0, 2);
    uint32_t h = *(uint32_t*)pio;
    if (!(pio[4] & 0x80)) {
        if (z31_WriteFile(h, buf, count, &n, nullptr)) written = n; else werr = z31_GetLastError();
    } else {
        const char* p = buf;
        char tmp[100 * 0 + 0x400 + 2];
        do {
            int k = 0; char* o = tmp;
            while ((uint32_t)(p - buf) < count && k < 0x400) {
                char c = *p++;
                if (c == '\n') { crs++; *o++ = '\r'; k++; }
                *o++ = c; k++;
            }
            if (!z31_WriteFile(h, tmp, (uint32_t)(o - tmp), &n, nullptr)) { werr = z31_GetLastError(); break; }
            written += n;
            if ((int)(o - tmp) > (int)n) break;
        } while ((uint32_t)(p - buf) < count);
    }
    if (written) return (int)(written - crs);
    if (werr) {
        if (werr == 5) { *z31_errno_ptr() = 9; *z31_doserrno_ptr() = 5; return -1; }
        z31_dosmaperr(werr); return -1;
    }
    if ((pio[4] & 0x40) && *buf == 0x1a) return 0;
    *z31_errno_ptr() = 0x1c; *z31_doserrno_ptr() = 0;
    return -1;
}

// 1.13c D2Launch+0x5838  (_write)
int z31_write(int fh, void* buf, uint32_t count) {
    if ((uint32_t)fh < z31_nhandle && (z31_pio(fh)[4] & 1)) {
        z31_lock_fhandle(fh);
        int r;
        if (!(z31_pio(fh)[4] & 1)) { *z31_errno_ptr() = 9; *z31_doserrno_ptr() = 0; r = -1; }
        else r = z31_write_lk(fh, (const char*)buf, count);
        z31_unlock_fhandle_write();
        return r;
    }
    *z31_errno_ptr() = 9; *z31_doserrno_ptr() = 0;
    return -1;
}

// 1.13c D2Launch+0x5927  (_isatty)
int z31_isatty(int fh) {
    if (z31_nhandle <= (uint32_t)fh) return 0;
    return (int)(int8_t)z31_pio(fh)[4] & 0x40;
}

// 1.13c D2Launch+0x59F8  (_malloc)
void* z31_malloc(size_t n) { return z31_nh_malloc(n, z31_newmode); }

// 1.13c D2Launch+0x5A6A  (_wctomb)
int z31_wctomb(char* mb, uint16_t wc) {
    Z31Ptd* ptd = (Z31Ptd*)z31_getptd();
    void* loc = ptd->locinfo;
    if (loc != z31_ptloci_cur) loc = z31_updatetlocinfo();
    return z31_wctomb_mt(loc, mb, wc);
}
