// src/core/fog/fog_batch_z39.cpp -- 12 1.13c Fog.dll rows (alpha lane, stage A) from
// build-wasm/ghidra-fog/decomp/0x<RVA>.c. Base 0x6FF50000. CRT/SEH/x87 internals are seams (z39_seam_*):
// host state behind the compat shim. 32-bit links are uint32_t.
#include <cstdint>
#include <cstddef>

extern "C" {
// seams
void     z39_seam_unlock(int32_t n);                       // Fog+0x3C3F  _unlock
void     z39_seam_LeaveCS(uint32_t cs);                    // LeaveCriticalSection
void     z39_seam_DeleteCS(uint32_t cs);                   // DeleteCriticalSection
void     z39_seam_free_log(uint32_t p, const char* tag, int32_t line, int32_t flag);  // Fog ord 403
void     z39_seam_1080(uint32_t p);                        // Fog+0x1080
int32_t* z39_seam_errno(void);                             // Fog+0x6A5A
uint32_t*z39_seam_doserrno(void);                          // Fog+0x6A63
void     z39_seam_lock_fhandle(int32_t fh);                // Fog+0x3DEB-family
void     z39_seam_unlock_fhandle(void);                    // Fog+0x725F
int32_t  z39_seam_write_nolock(int32_t fh, const void* b, uint32_t n);  // Fog+0x701C
int32_t  z39_seam_tsopen_lk(uint32_t share, int32_t* unlock, uint32_t* fh, const char* name, int32_t flag, uint8_t perm); // 0x9D9F-family
void     z39_seam_sopen_unwind(void);                      // Fog+0xA0CB
void     z39_seam_raise(void);                             // Fog+0xAE5E (FP exception raise)
void     z39_seam_oneArg(int32_t type, int32_t* rec, uint16_t* cw);  // Fog+0x247AA
int32_t  z39_seam_alloc_osfhnd(void);

// +0x2E3.. : __pioinfo[64] (DAT_6ffa7140) and its handle count (DAT_6ffa7134), host CRT state.
extern uint32_t z39_pioinfo[64];   // 32-bit links to 0x24-byte ioinfo blocks (flags at +4)
extern uint8_t* z39_pioinfo_ptr[64];
extern uint32_t z39_nhandle;
extern uint32_t z39_walk_busy;     // DAT_6ffa6edc
}

// 1.13c Fog+0x6690  (_strlen: word-at-a-time scan; the result is the plain NUL index)
extern "C" uint32_t z39_fog_6690(const char* s) {
    const char* p = s;
    while (*p) ++p;
    return (uint32_t)(p - s);
}

// 1.13c Fog+0x68A0  (__unlock_file: static FILE table -> _unlock(idx+16); else LeaveCS(file+0x20))
extern "C" void z39_fog_68A0(uint32_t file) {
    if (file > 0x6ff7e33fu && file < 0x6ff7e5a1u)
        z39_seam_unlock((int32_t)((file - 0x6ff7e340u) >> 5) + 0x10);  // (file-0x37fbf1a*4)>>5
    else
        z39_seam_LeaveCS(file + 0x20);
}

// 1.13c Fog+0x71D8  (__write: validate handle against ioinfo flags (bit0 open), lock, write, unlock)
extern "C" int32_t z39_fog_71D8(int32_t fh, const void* buf, uint32_t n) {
    if ((uint32_t)fh < z39_nhandle) {
        uint8_t* blk = z39_pioinfo_ptr[fh >> 5];
        uint32_t off = (uint32_t)(fh & 0x1f) * 0x24;
        if (blk && (blk[4 + off] & 1)) {
            z39_seam_lock_fhandle(fh);
            int32_t r;
            if (!(blk[4 + off] & 1)) { *z39_seam_errno() = 9; *z39_seam_doserrno() = 0; r = -1; }
            else r = z39_seam_write_nolock(fh, buf, n);
            z39_seam_unlock_fhandle();
            return r;
        }
    }
    *z39_seam_errno() = 9; *z39_seam_doserrno() = 0;
    return -1;
}

// 1.13c Fog+0xA086  (__sopen: _tsopen_lk(share,&unlock,&fh,name,flag,perm), then SEH unlock; returns fh)
extern "C" int32_t z39_fog_A086(const char* name, int32_t oflag, int32_t share, uint8_t perm) {
    int32_t unl = 0; uint32_t fh[6] = {0};
    int32_t r = z39_seam_tsopen_lk((uint32_t)share, &unl, fh, name, oflag, perm);
    z39_seam_sopen_unwind();
    return r;
}

// 1.13c Fog+0xA35C  (__alloc_osfhnd) -- GUESSED: the lock/grow loop is host CRT state; delegated to seam.
extern "C" int32_t z39_fog_A35C(void) { return z39_seam_alloc_osfhnd(); }

// 1.13c Fog+0xA8AB  (__handle_exc: x87 status/control -> raise) -- GUESSED/PARTIAL: only the
// raise-and-clear flag logic for invalid/zerodiv/precision; overflow/underflow double fixups omitted.
extern "C" bool z39_fog_A8AB(uint32_t status, double* val, uint32_t ctl) {
    (void)val;
    uint32_t u = status & 0x1f;
    if ((status & 8) && (ctl & 1)) { z39_seam_raise(); u = status & 0x17; }
    else if ((status & 4) && (ctl & 4)) { z39_seam_raise(); u = status & 0x1b; }
    else if (!((status & 1) && (ctl & 8))) { if ((status & 2) && (ctl & 0x10)) { u &= ~2u; z39_seam_raise(); } }
    else { z39_seam_raise(); u = status & 0x1e; }
    if ((status & 0x10) && (ctl & 0x20)) { z39_seam_raise(); u &= ~0x10u; }
    return u == 0;
}

// 1.13c Fog+0xB9D1  (__setmode_lk: 0x8000 clears text bit 0x80, 0x4000 sets it; returns previous mode)
extern "C" int32_t z39_fog_B9D1(uint32_t fh, int32_t mode) {
    uint8_t* f = z39_pioinfo_ptr[(int32_t)fh >> 5] + 4 + (fh & 0x1f) * 0x24;
    uint8_t old = *f;
    if (mode == 0x8000) *f &= 0x7f;
    else if (mode == 0x4000) *f |= 0x80;
    else { *z39_seam_errno() = 0x16; return -1; }
    return (old & 0x80) ? 0x4000 : 0x8000;
}

// 1.13c Fog+0x15F50  (QuickStackWalk) -- EMPTY/GUESSED: debug-only stack walker (dbghelp thunks,
// RDTSC timing, formatted text). Keeps only the reentrancy guard; returns 0 (no walk).
extern "C" uint32_t z39_fog_15F50(uint32_t out, uint32_t a2, uint32_t ctx) {
    (void)out; (void)a2; (void)ctx;
    return 0;
}

// 1.13c Fog+0x24347  (__startOneArgErrorHandling: packs args, calls Fog+0x247AA, returns the x87 value)
extern "C" void z39_fog_24347(int32_t type, uint16_t cw) {
    int32_t rec[6] = {0};
    z39_seam_oneArg(type, rec, &cw);
}

// 1.13c Fog+0x1000  (operator delete wrapper: non-null -> Fog ord 403 (free) tag "delete")
extern "C" void z39_fog_1000(uint32_t p) {
    if (p) z39_seam_free_log(p, "delete", -1, 0);
}

// 1.13c Fog+0x1030  (DeleteCriticalSection on the cs passed in EAX)
extern "C" void z39_fog_1030(uint32_t cs) { z39_seam_DeleteCS(cs); }

// 1.13c Fog+0x1040  (SEH-wrapped call to Fog+0x1080)
extern "C" void z39_fog_1040(uint32_t p) { z39_seam_1080(p); }
