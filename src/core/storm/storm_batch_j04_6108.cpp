// src/core/storm/storm_batch_j04_6108.cpp -- 1.13c Storm.dll CRT/inflate/STL-node leaves transcribed
// from build-wasm/ghidra-storm/decomp/0x<RVA>.c. Batch lane: implementation + marker only.
// Callees not yet ours are seams declared in retail argument order; Win32/CRT globals are seams too.
#include <cstdint>
#include <cstring>
#include <cstdio>

extern "C" {
void Storm_ArrayUnwind(void* ptr, uint32_t size, int32_t count, void (*dtor)(void*));  // 0x6000
void Storm_Sub902A(int32_t lock);                       // __unlock
void Storm_Lock(int32_t lock);                          // __lock
void Storm_InitTerm(void (**b)(void), void (**e)(void)); // __initterm
int32_t Storm_Lseeki64_lk(int32_t fh, int64_t off, int32_t whence); // __lseeki64_lk
void Storm_DosMapErr(uint32_t winerr);
int32_t* Storm_Errno();                                  // 0xA062
uint32_t* Storm_DosErrno();                              // 0xA06B
int32_t Storm_WriteFile(void* h, const void* p, uint32_t n, uint32_t* wrote, uint32_t* err);
void Storm_CrtExitProcess(uint32_t code);
void Storm_Exit(int32_t code);
void Storm_TerminateSelf(uint32_t code);
void Storm_CrtMessageBox(const char* text, const char* caption, uint32_t flags);
uint32_t Storm_GetModuleFileName(char* buf, uint32_t n);
int32_t Storm_InflateRefill(uint8_t* st, uint32_t nbits);   // 0xF0D0, nonzero = out of input
void Storm_StlNodeInit(void* node);                      // 0x2BC0
}

// lowio: ioinfo table of 32-entry pages, 0x24 bytes per entry (handle @0, flags @4)
extern uint8_t* Storm_pioinfo[];
extern uint32_t Storm_Cookie_unused;
extern void (**Storm_onexitBegin)(void);
extern void (**Storm_onexitEnd)(void);
extern int32_t Storm_exitingFlag, Storm_doexitDone;
extern uint8_t Storm_exitSkip;
extern void (*Storm_secFailHandler)(void);
extern void (*Storm_xpTable[])(void);   // 0x6FC400A0.. / 0x6FC400AC.. initterm ranges

// 1.13c Storm+0x06108
// Funclet of a vector-dtor frame: if parent's [ebp-0x20] flag is zero, unwind the array.
extern "C" void Storm_Sub6108(uint8_t* ebp) {
    if (*(int32_t*)(ebp - 0x20) != 0) return;
    Storm_ArrayUnwind(*(void**)(ebp + 8), *(uint32_t*)(ebp + 0xC),
                      *(int32_t*)(ebp - 0x1C), *(void (**)(void*))(ebp + 0x18));
}

// 1.13c Storm+0x0C666
// CRT _write_nolock: text-mode LF->CRLF expansion in 1024-byte chunks, error mapping.
extern "C" int32_t Storm_WriteNolockC666(uint32_t fh, const char* buf, uint32_t cnt) {
    char tmp[0x400 + 4];
    uint32_t written = 0, err = 0; int32_t lfs = 0;
    if (cnt == 0) return 0;
    uint8_t* e = Storm_pioinfo[(int32_t)fh >> 5] + (fh & 0x1f) * 0x24;
    if (e[4] & 0x20) Storm_Lseeki64_lk((int32_t)fh, 0, 2);
    if (!(e[4] & 0x80)) {
        uint32_t w;
        if (Storm_WriteFile(*(void**)e, buf, cnt, &w, &err)) { err = 0; written = w; }
    } else {
        const char* p = buf; uint32_t done = 0;
        do {
            int32_t n = 0; char* o = tmp;
            while (n < 0x400 && done < cnt) {
                char c = *p++; done++;
                if (c == '\n') { lfs++; *o++ = '\r'; n++; }
                *o++ = c; n++;
            }
            uint32_t w;
            if (!Storm_WriteFile(*(void**)e, tmp, (uint32_t)(o - tmp), &w, &err)) break;
            written += w;
            if ((int32_t)w < (int32_t)(o - tmp)) break;
        } while (done < cnt);
    }
    if (written) return (int32_t)(written - lfs);
    if (err) {
        if (err == 5) { *Storm_Errno() = 9; *Storm_DosErrno() = 5; return -1; }
        Storm_DosMapErr(err); return -1;
    }
    if ((e[4] & 0x40) && *buf == 0x1a) return 0;
    *Storm_Errno() = 0x1c; *Storm_DosErrno() = 0; return -1;
}

// 1.13c Storm+0x0EEA0
// PKWARE explode: decode one literal (<0x100) or length symbol (+0x100); 0x306 = input exhausted.
extern "C" uint32_t Storm_ExplodeLitEEA0(uint8_t* st) {
    auto R = [&](uint32_t n) { return Storm_InflateRefill(st, n) != 0; };
    const uint32_t bb = *(uint32_t*)(st + 0x14);
    uint32_t s;
    if (!(bb & 1)) {
        if (R(1)) return 0x306;
        if (*(int32_t*)(st + 4) == 0) {
            s = bb & 0xff;
            if (R(8)) return 0x306;
        } else {
            s = bb & 0xff;
            if (s == 0) {
                if (R(8)) return 0x306;
                s = st[(*(uint32_t*)(st + 0x14) & 0xff) + 0x2eb4];
            } else {
                s = st[0x2c34 + s];
                if (s == 0xff) {
                    if ((bb & 0x3f) == 0) {
                        if (R(6)) return 0x306;
                        s = st[(*(uint32_t*)(st + 0x14) & 0x7f) + 0x2e34];
                    } else {
                        if (R(4)) return 0x306;
                        s = st[(*(uint32_t*)(st + 0x14) & 0xff) + 0x2d34];
                    }
                }
            }
            if (R(st[0x2fb4 + s])) return 0x306;
        }
        return s;
    }
    if (R(1)) return 0x306;
    s = st[(*(uint32_t*)(st + 0x14) & 0xff) + 0x2b34];
    if (R(st[0x30f4 + s])) return 0x306;
    uint8_t b = st[s + 0x3104];
    if (b) {
        uint32_t x = ((1u << (b & 0x1f)) - 1u) & *(uint32_t*)(st + 0x14);
        if (R(b) && s + x != 0x10e) return 0x306;
        s = *(uint16_t*)(st + 0x3114 + s * 2) + x;
    }
    return s + 0x100;
}

// 1.13c Storm+0x0F050
// PKWARE explode: decode a distance; returns distance+1, 0 on exhausted input. len==2 uses 2 low bits.
extern "C" int32_t Storm_ExplodeDistF050(uint8_t* st, int32_t len) {
    uint32_t d = st[(*(uint32_t*)(st + 0x14) & 0xff) + 0x2a34];
    if (Storm_InflateRefill(st, st[0x30b4 + d])) return 0;
    if (len == 2) {
        d = d << 2 | (*(uint32_t*)(st + 0x14) & 3);
        if (Storm_InflateRefill(st, 2)) return 0;
    } else {
        d = d << (*(uint32_t*)(st + 0xC) & 0x1f) | (*(uint32_t*)(st + 0x10) & *(uint32_t*)(st + 0x14));
        if (Storm_InflateRefill(st, *(uint32_t*)(st + 0xC))) return 0;
    }
    return (int32_t)(d + 1);
}

// 1.13c Storm+0x08E33
// CRT _doexit(code, quick, retcaller).
extern "C" void Storm_Doexit(uint32_t code, int32_t quick, int32_t retcaller) {
    Storm_Lock(8);
    if (Storm_doexitDone == 1) Storm_TerminateSelf(code);
    Storm_exitingFlag = 1;
    Storm_exitSkip = (uint8_t)retcaller;
    if (!quick) {
        if (Storm_onexitEnd) {
            while (--Storm_onexitBegin, Storm_onexitEnd <= Storm_onexitBegin)  // matches retail's pre-decrement walk
                if (*Storm_onexitBegin) (*Storm_onexitBegin)();
        }
        Storm_InitTerm(&Storm_xpTable[0], &Storm_xpTable[3]);   // 0x6FC400A0
    }
    Storm_InitTerm(&Storm_xpTable[3], &Storm_xpTable[6]);       // 0x6FC400AC
    if (retcaller == 0) { Storm_doexitDone = 1; Storm_CrtExitProcess(code); return; }
    Storm_Sub902A(8);
}

// 1.13c Storm+0x0DD5F
// CRT __security_error_handler: custom handler if set, else MessageBox "Buffer overrun detected!".
extern "C" void Storm_SecurityFailureDD5F(int32_t kind) {
    if (Storm_secFailHandler) { Storm_secFailHandler(); Storm_Exit(3); return; }
    const char* msg = kind == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
    char prog[0x104 + 1]; char out[0x400];
    if (!Storm_GetModuleFileName(prog, 0x104)) strcpy(prog, "<program name unknown>");
    const char* name = prog;
    if (strlen(name) + 0xb > 0x3c) { name = prog + strlen(prog) - 0x3c + 0xb + 3; }   // ponytail: "..." prefix omitted
    snprintf(out, sizeof out, "%s\n\nProgram: %s\n\n%s", msg, name, "");
    Storm_CrtMessageBox(out, "Microsoft Visual C++ Runtime Library", 0x12010);
    Storm_Exit(3);
}

// 1.13c Storm+0x0851A
extern "C" void Storm_CrtInit4_851A(void) { Storm_Sub902A(4); }

// 1.13c Storm+0x0E221
extern "C" void Storm_CrtInit10_E221(void) { Storm_Sub902A(10); }

// 1.13c Storm+0x08EE2
// Tail of _doexit's SEH epilogue: unlock 8 when the frame's retcaller flag differs from EDI.
extern "C" void Storm_Sub8EE2(uint8_t* ebp, int32_t edi) { if (*(int32_t*)(ebp + 0x10) != edi) Storm_Sub902A(8); }

// 1.13c Storm+0x02BC0
// Debug-checked list sentinel: {tag=0xDDDDDDDD, self, ~self}. Guest pointers are 32-bit.
extern "C" void Storm_Sub2BC0(uint32_t* n) {
    uint32_t self = (uint32_t)(uintptr_t)(n + 1);
    n[1] = 0; n[2] = 0; n[0] = 0;
    n[1] = self; n[2] = ~self; n[0] = 0xdddddddd;
}

// 1.13c Storm+0x03370
// SEH-guarded wrapper: null-check then init the node.
extern "C" void Storm_Sub3370(uint32_t* n) { if (n) Storm_Sub2BC0(n); }
