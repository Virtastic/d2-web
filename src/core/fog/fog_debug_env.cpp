// Fog debug-output family over the FogEnv seam: every callee retail makes (the 0xCD8E thunk, the error reporter 0x10F30, the log
// writer 0x18AF0, the hex-dump helpers 0x190A0/0x18950, the flush 0x184A0, the OutputDebugStringA import at slot 0x250B0) goes through
// fe_call with the words retail pushes. Module addresses are passed as 0xB2000000|rva, as the capture logs them. The older ports in
// fog_batch_105d0.cpp / fog_batch_19040.cpp / fog_batch_102C0.cpp / fog_ordinals_fatal.cpp keep serving production callers.
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "fog_env.h"

namespace {
constexpr uint32_t MOD = 0xB2000000u;
constexpr uint32_t CTX = MOD | 0x268ECu;     // the error context global
constexpr uint32_t FMT = MOD | 0x2850Cu;     // report format for the file/line variants
inline uint32_t addr(const void* p) { return (uint32_t)(uintptr_t)p; }

// Fog+0x11070 / 0x110D0 / 0x11100 / 0x11130 share one body: Ordinal_116 thunk, 0x10F30(category, ctx, line, fmt, file), 0x184A0.
void report(const FogEnv* e, uint32_t cat, uint32_t file, uint32_t line) {
    fe_call(e, 0xCD8E);
    fe_call(e, 0x10F30, 0, 0, 0, 0, 0, cat, CTX, line, FMT, file);
    fe_call(e, 0x184A0);
}
// the shared formatter of Fog+0x10DD0 / 0x10CD0: buf[0] = 0, vsnprintf(buf, 0x1000, fmt, va). Retail's va list is 32-bit stack words; the
// host's is laid out per conversion (a pointer takes sizeof(void*) at its own alignment, every other conversion a 4-byte int), so the
// words are repacked by walking the format. The host's 0x1000-byte buffer keeps its residue after the terminator, as retail's does.
void fmtTo(char* buf, const char* fmt, const uint32_t* va) {
    alignas(8) unsigned char vb[128];
    size_t off = 0;
    uint32_t n = 0;
    for (const char* f = fmt; *f && n < 6; f++) {
        if (*f != '%') continue;
        f++;
        while (*f && strchr("-+ #0123456789.", *f)) f++;
        if (*f == 0) break;
        if (*f == '%') continue;
        uint32_t w = va[n++];
        if (*f == 's' || *f == 'p') {
            off = (off + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
            void* p = (void*)(uintptr_t)w;
            memcpy(vb + off, &p, sizeof p); off += sizeof p;
        } else {
            off = (off + 3) & ~(size_t)3;
            memcpy(vb + off, &w, 4); off += 4;
        }
    }
    va_list ap;
    *(void**)&ap = vb;
    buf[0] = 0;
    vsnprintf(buf, 0x1000, fmt, ap);
}
}

extern "C" {
// 1.13c Fog+0x11070 (cat, msg, file, line); msg is never read
void fog_11070(const FogEnv* e, uint32_t cat, uint32_t, uint32_t file, uint32_t line) { report(e, cat, file, line); }
// 1.13c Fog+0x110A0 (thread, ticks)
void fog_110a0(const FogEnv* e, uint32_t thread, uint32_t ticks) {
    fe_call(e, 0xCD8E);
    fe_call(e, 0x10F30, 0, 0, 0, 0, 0, 10, thread, 0x780, MOD | 0x284E4u, thread, ticks);
    fe_call(e, 0x184A0);
}
// 1.13c Fog+0x110D0 (msg, file, line)
void fog_110d0(const FogEnv* e, uint32_t, uint32_t file, uint32_t line) { report(e, 7, file, line); }
// 1.13c Fog+0x11100 (msg, file, line)
void fog_11100(const FogEnv* e, uint32_t, uint32_t file, uint32_t line) { report(e, 6, file, line); }
// 1.13c Fog+0x11130 (msg, file, line)
void fog_11130(const FogEnv* e, uint32_t, uint32_t file, uint32_t line) { report(e, 5, file, line); }
// 1.13c Fog+0x10B80 (stack bytes, code bytes): crash-report hex dump
void fog_10b80(const FogEnv* e, uint32_t stackBytes, uint32_t codeBytes) {
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x28720u);
    fe_call(e, 0x18950, 0, 0, 0, 0, 0, CTX, CTX, stackBytes, 0x1000, 0, 1);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x28714u);
    fe_call(e, 0x18950, 0, 0, 0, 0, 0, CTX, CTX, codeBytes, 0x20, 0, 1);
    fe_call(e, 0x184A0);
}
// 1.13c Fog+0x10DD0 (fmt, va...)
void fog_10dd0(const FogEnv* e, const char* fmt, const uint32_t* va) {
    char buf[0x1000];
    fmtTo(buf, fmt, va);
    fe_call(e, 0x250B0, 0, 0, 0, 0, 0, addr(buf));
}
// 1.13c Fog+0x10CD0 (fmt, va...)
void fog_10cd0(const FogEnv* e, const char* fmt, const uint32_t* va) {
    char buf[0x1000];
    fmtTo(buf, fmt, va);
    fe_call(e, 0x250B0, 0, 0, 0, 0, 0, addr(buf));
    fe_call(e, 0x18AF0, 0, 0, 0, 0, 0, MOD | 0x28A4Cu, MOD | 0x26DC4u, addr(buf));
}
}
