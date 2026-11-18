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
// 1.13c Fog+0x10E30 (EDI = optional thread context): per-thread dump. cnt is the module counter Fog+0x32AC8. The kernel32 imports go
// through the seam by their IAT slot (0x250F0 GetCurrentThreadId, 0x250BC GetCurrentProcess, 0x25034 GetCurrentThread, 0x250CC
// DuplicateHandle, 0x250A4 GetLastError, 0x25020 CloseHandle). With EDI = 0 retail first captures its own EIP/ESP/EBP into the on-stack
// context (values no callee reads back here); either way 0x155E0 gets (thread copy, context).
void fog_10e30(const FogEnv* e, uint32_t edi, uint32_t* cnt) {
    uint32_t dup = 0;
    uint32_t local[0xB7];   // the on-stack context retail hands 0x155E0 when EDI = 0
    ++*cnt;
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x28558u);
    const uint32_t tid = fe_call(e, 0x250F0);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x28548u, tid);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x2853Cu);
    const uint32_t proc = fe_call(e, 0x250BC);
    const uint32_t thr = fe_call(e, 0x25034);
    if (!fe_call(e, 0x250CC, 0, 0, 0, 0, 0, proc, thr, proc, addr(&dup), 0, 0)) {
        const uint32_t err = fe_call(e, 0x250A4);
        fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x28530u, err);
        return;
    }
    memset(local, 0, sizeof local);
    fe_call(e, 0x155E0, 0, 0, 0, 0, 0, dup, edi ? edi : addr(local));
    fe_call(e, 0x25020, 0, 0, 0, 0, 0, dup);
}
// 1.13c Fog+0x109A0 (ECX = line, EDX = file name or 0, EBX = format, then the category index and the va_list words): the fatal-error report. Retail
// formats with its own vsnprintf and wsprintf (the port mirrors them with the host's); the seam sees the rule lines (0x190A0), the handler install
// (0x10660, ECX = 0, EDX = Fog+0xFD70), the name trim (0xFC70), the report writers (0x18B50 with EAX = the context, 0x18AF0), the flush and the
// MessageBoxA import (slot 0x2523C; the text has no trailing newline, the logged copy has one). A null format makes retail log an internal error
// and call _exit; that path does not return and is not mirrored.
void fog_109a0(const FogEnv* e, uint32_t line, uint32_t file, const char* fmt, uint32_t cat, const uint32_t* va) {
    static const char* const CAT[12] = { "No Error", "Unknown Error", "Out of Memory Error", "File Read Error", "File Write Error", "Assertion Failure",
                                         "Halt", "Warning", "Missing Dll", "Registry Error", "Deadlock", "???" };
    char a[0x1000], b[0x1000], c[0x1000], cap[0x100];
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, CTX);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, CTX);
    fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, MOD | 0x28A14u);
    fe_call(e, 0x10660, 0, 0, MOD | 0xFD70u, 0, 0);
    if (file) {
        const char* name = (const char*)(uintptr_t)fe_call(e, 0xFC70, 0, file);
        snprintf(a, sizeof a, "--------  FILE: %s    LINE: %d  --------\n\n", name, (int)line);
        fe_call(e, 0x18B50, CTX, 0, 0, 0, 0, MOD | 0x26DC4u, addr(a));
    }
    if (!fmt) return;
    fmtTo(a, fmt, va);
    a[4095] = 0;
    if (!file) strcpy(b, a);
    else {
        const char* name = (const char*)(uintptr_t)fe_call(e, 0xFC70, 0, file);
        snprintf(b, sizeof b, "%s\nLocation : %s, line #%d\nExpression : %s", CAT[cat], name, (int)line, a);
    }
    snprintf(cap, sizeof cap, "%s Error", "Diablo II");
    strcpy(c, b);
    strcat(b, "\n");
    fe_call(e, 0x18AF0, 0, 0, 0, 0, 0, CTX, MOD | 0x26DC4u, addr(b));
    fe_call(e, 0x184A0);
    fe_call(e, 0x2523C, 0, 0, 0, 0, 0, 0, addr(c), addr(cap), 0x30);
}
// 1.13c Fog+0x10BF0 (flags byte, format, va words): flag-gated trace line. Bit 4 prefixes the local time (GetLocalTime, kernel32 slot 0x25088, fills a
// SYSTEMTIME through the seam), bit 2 suppresses OutputDebugStringA (slot 0x250B0), bit 1 logs through 0x18AF0, bits 3 and 4 poll GetKeyState(0x14)
// (slot 0x25234). Retail formats with its own snprintf and vsnprintf; the port mirrors them with the host's.
void fog_10bf0(const FogEnv* e, uint32_t flagsWord, const char* fmt, const uint32_t* va) {
    const uint32_t flags = flagsWord & 0xFFu;
    char buf[0x1000];
    buf[0] = 0;
    if (flags & 4) {
        uint16_t st[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        fe_call(e, 0x25088, 0, 0, 0, 0, 0, addr(st));
        snprintf(buf, sizeof buf, "%02d:%02d:%02d.%03d    ", (int)st[4], (int)st[5], (int)st[6], (int)st[7]);
    }
    const size_t used = strlen(buf);
    fmtTo(buf + used, fmt, va);
    buf[0xFFF] = 0;
    if (!(flags & 2)) fe_call(e, 0x250B0, 0, 0, 0, 0, 0, addr(buf));
    if (flags & 1) fe_call(e, 0x18AF0, 0, 0, 0, 0, 0, MOD | 0x28A4Cu, MOD | 0x26DC4u, addr(buf));
    if (flags & 0x18) fe_call(e, 0x25234, 0, 0, 0, 0, 0, 0x14);
}
// 1.13c Fog+0x10F30 (cdecl: category, file name or thread, line, format, then the format's own words): the error reporter. Category 10 is the
// thread-dump report: arg1 is the thread handle and the file becomes the ErrorManager path. The re-entrancy check Fog+0xFCF0 returns non-zero
// while the guard (Fog+0x56E70) is latched; otherwise the guard latches, 0x109A0 reports (ECX = line, EDX = file, EBX = format or the empty default,
// the category and a pointer to the format's words on the stack), the crash context global Fog+0x32C28 is cleared (flags dword 0x10007) and filled by
// GetThreadContext for a thread handle, or from this frame (EIP = Fog+0x10FC7, ESP) otherwise. Categories 2, 5, 6, 7 and 10 dump the thread (0x10E30
// with EDI = the context) and the stack/code (0x10B80 with the context's ESP and EIP: both read back as 0 after GetThreadContext, which the
// descriptor leaves unwritten). The optional hook pointer Fog+0x56E6C runs, the log is written (0x18AF0) and flushed (0x184A0); the guard clears
// again unless the category is 2, 5 or 6. guard is the module word the descriptor pins; hookPtr the value of the hook pointer.
void fog_10f30(const FogEnv* e, uint32_t cat, uint32_t arg1, uint32_t line, uint32_t fmt, uint32_t* guard, uint32_t hookPtr) {
    uint32_t thread = 0, file = arg1;
    if (cat == 10) { thread = arg1; file = MOD | 0x28414u; }
    if (fe_call(e, 0xFCF0, 0, line, 0, 0, 0, file, fmt)) return;
    *guard = 1;
    const uint32_t f = fmt ? fmt : (MOD | 0x268C4u);
    uint32_t vl[2] = { 0, 0 };   // the format's words: retail hands 0x109A0 a pointer into its own argument area
    fe_call(e, 0x109A0, 0, line, file, f, 0, cat, addr(vl));
    uint32_t esp = 0, eip = 0;
    if (thread) fe_call(e, 0x250B4, 0, 0, 0, 0, 0, thread, MOD | 0x32C28u);
    else {
        eip = MOD | 0x10FC7u;
        esp = addr(vl);
        fe_call(e, 0x25034);
    }
    if (cat == 5 || cat == 6 || cat == 7 || cat == 10 || cat == 2) {
        fe_call(e, 0x10E30);
        fe_call(e, 0x10B80, 0, 0, 0, 0, 0, esp, eip);
    }
    if (hookPtr) fe_call(e, hookPtr - 0x6FF50000u);
    fe_call(e, 0x18AF0, 0, 0, 0, 0, 0, CTX, MOD | 0x26DC4u, MOD | 0x283F4u);
    fe_call(e, 0x184A0);
    if (cat != 5 && cat != 6 && cat != 2) *guard = 0;
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
// 1.13c Fog+0x142D0 (ECX = code address, EAX = buffer size, stack: buffer, &section, &offset; ret 0xC): crash-report helper. The module name goes into
// the buffer (the Storm string copy 0xCDDC, "unknown" first), VirtualQuery (slot 0x250B8) names the allocation base, a null base asks GetModuleHandleA(NULL)
// (slot 0x250C4), GetModuleFileNameA (slot 0x250A8) writes the path (a failure copies "unknown" again and returns), then the PE section table of the
// image at that base is walked: a hit is VA <= rel <= VA + max(VirtualSize, SizeOfRawData); *section = 1-based index, *offset = rel - VA.
void fog_142d0(const FogEnv* e, uint32_t code, uint32_t size, uint32_t buf, uint32_t* section, uint32_t* offset) {
    const uint32_t unk = MOD | 0x26790u;
    fe_call(e, 0xCDDC, 0, 0, 0, 0, 0, buf, unk, size);
    *section = 0; *offset = 0;
    uint32_t mbi[7] = { 0, 0, 0, 0, 0, 0, 0 };
    if (!fe_call(e, 0x250B8, 0, 0, 0, 0, 0, code, addr(mbi), 0x1C)) return;
    uint32_t base = mbi[1];
    if (!base) base = fe_call(e, 0x250C4, 0, 0, 0, 0, 0, 0);
    if (!fe_call(e, 0x250A8, 0, 0, 0, 0, 0, base, buf, size)) {
        fe_call(e, 0xCDDC, 0, 0, 0, 0, 0, buf, unk, size);
        return;
    }
    if (!base || fe_r16(base) != 0x5A4Du) return;
    const uint32_t nt = fe_r32(base + 0x3C);
    if (!nt || fe_r32(base + nt) != 0x4550u) return;
    const uint32_t n = fe_r16(base + nt + 6), rel = code - base;
    uint32_t sh = base + nt + 0x18 + fe_r16(base + nt + 0x14);
    for (uint32_t i = 0; i < n; ++i, sh += 40) {
        const uint32_t vsz = fe_r32(sh + 8), va = fe_r32(sh + 12), raw = fe_r32(sh + 16), sz = raw > vsz ? raw : vsz;
        if (rel >= va && rel <= sz + va) { *section = i + 1; *offset = rel - va; return; }
    }
}
// 1.13c Fog+0x11160 (stdcall, EXCEPTION_POINTERS*): the unhandled-exception filter. Fog+0x2FFF0 counts the entries (InterlockedIncrement, kernel32 slot
// 0x250E0); only the first entry reports. It latches the guard Fog+0x56E70, writes the rule lines (0x190A0), installs the handler (0x10660 with
// ECX = 0, EDX = Fog+0xFD70), names the exception (0xFEB0: EAX = code, EBX = a 256-byte buffer, EDI = 0x100), logs the name, the fault address
// (0xFDB0: ECX = address, EAX = 0x104, stack: path buffer, &section, &offset) and the three register-dump lines from the CONTEXT, dumps the thread
// (0x10E30 with EDI = 0) and the stack/code (0x10B80 with the context's ESP and EIP), runs 0x14540, the optional hook pointer Fog+0x56E6C, and raises
// MessageBoxA (flags 0x30; the caption is "<Fog+0x2FCD8> Exception", the text "UNHANDLED EXCEPTION:\n<name> (<code %08x>)"; Storm's string formatter
// 0xCDD0 is mirrored with the host's snprintf). InterlockedDecrement (slot 0x250E4) runs on every path and the function returns 1.
// ep is the EXCEPTION_POINTERS address (record, context), guard the module word Fog+0x56E70 the descriptor pins, product the string at Fog+0x2FCD8.
uint32_t fog_11160(const FogEnv* e, uint32_t ep, uint32_t* guard, uint32_t hookPtr, const char* product) {
    const uint32_t cnt = MOD | 0x2FFF0u;
    if (fe_call(e, 0x250E0, 0, 0, 0, 0, 0, cnt) == 0) {
        char name[256], path[260], cap[0x100], text[0x1000];
        auto log = [&](uint32_t n, uint32_t fmt, uint32_t a2 = 0, uint32_t a3 = 0, uint32_t a4 = 0, uint32_t a5 = 0, uint32_t a6 = 0, uint32_t a7 = 0) {
            fe_wide(n, a6, a7);
            fe_call(e, 0x190A0, 0, 0, 0, 0, 0, CTX, fmt, a2, a3, a4, a5);
        };
        *guard = 1;
        log(2, CTX);
        log(2, CTX);
        log(2, MOD | 0x28A14u);
        fe_call(e, 0x10660, 0, 0, MOD | 0xFD70u, 0, 0);
        const uint32_t rec = fe_r32(ep), ctx = fe_r32(ep + 4);
        const uint32_t code = fe_r32(rec), at = fe_r32(rec + 0xC);
        name[0] = 0;
        fe_call(e, 0xFEB0, code, 0, 0, addr(name), 0);
        log(4, MOD | 0x28664u, addr(name), code);
        uint32_t section = 0, offset = 0;
        path[0] = 0;
        fe_call(e, 0xFDB0, 0x104, at, 0, 0, 0, addr(path), addr(&section), addr(&offset));
        log(6, MOD | 0x28640u, at, section, offset, addr(path));
        log(7, MOD | 0x28610u, fe_r32(ctx + 0xB0), fe_r32(ctx + 0xA4), fe_r32(ctx + 0xAC), fe_r32(ctx + 0xA8), fe_r32(ctx + 0xA0));
        log(7, MOD | 0x285E0u, fe_r32(ctx + 0x9C), fe_r32(ctx + 0xB4), fe_r32(ctx + 0xC4), fe_r32(ctx + 0xB8), fe_r32(ctx + 0xC0));
        log(8, MOD | 0x285B0u, fe_r32(ctx + 0xBC), fe_r32(ctx + 0x98), fe_r32(ctx + 0x94), fe_r32(ctx + 0xC8), fe_r32(ctx + 0x90), fe_r32(ctx + 0x8C));
        fe_call(e, 0x10E30);
        fe_call(e, 0x10B80, 0, 0, 0, 0, 0, fe_r32(ctx + 0xC4), fe_r32(ctx + 0xB8));
        uint32_t local[4] = { 0, 0, 0, 0 };
        fe_call(e, 0x14540, 0, 0, 0, 0, 0, 0x10D003Cu, code, at, addr(local));
        if (hookPtr) fe_call(e, hookPtr - 0x6FF50000u);
        snprintf(cap, sizeof cap, "%s Exception", product);
        snprintf(text, sizeof text, "UNHANDLED EXCEPTION:\n%s (%08x)", name, code);
        fe_call(e, 0x2523C, 0, 0, 0, 0, 0, 0, addr(text), addr(cap), 0x30);
    }
    fe_call(e, 0x250E4, 0, 0, 0, 0, 0, cnt);
    return 1;
}
// 1.13c Fog+0x113B0 (cdecl, no arguments): the start-up banner. Every line goes through the log writer 0x19100 (cdecl varargs): the rule line, the local time
// ("%04d-%02d-%02d %02d:%02d:%02d" from GetLocalTime, kernel32 slot 0x25088), "<product> running under <OS> (Version %.3f)" with the OS name chosen by the
// platform id Fog+0x32F00 (1 Windows 95, 2 98, 3 ME, 4 NT, 5 2000, 6 XP, 7 .Net, 8 MacOS, 9 MacOS X, else unknown) and the version float Fog+0x32F04
// (both pushed as doubles), "Total Physical Memory: %.2fMB" (0x1C830 fills Fog+0x32F08; the signed dword is made unsigned and scaled by 2^-20), the
// application path when 0x143D0 (EAX = 0x104, ESI = a 260-byte buffer) succeeds, the local IP string 0x16A10 returns, "Computer name: %s"
// (GetComputerNameA, slot 0x25070, size 0x40; a failure empties the buffer) and two closing lines. glob = the three pinned words at Fog+0x32F00,
// product = the string at Fog+0x2FCD8.
static void fe_dbl(double d, uint32_t* lo, uint32_t* hi) { uint64_t u; memcpy(&u, &d, 8); *lo = (uint32_t)u; *hi = (uint32_t)(u >> 32); }
void fog_113b0(const FogEnv* e, const uint32_t* glob) {
    auto log = [&](uint32_t n, uint32_t fmt, uint32_t a2 = 0, uint32_t a3 = 0, uint32_t a4 = 0, uint32_t a5 = 0, uint32_t a6 = 0, uint32_t a7 = 0) {
        fe_wide(n, a7);   // words 0..5 are fmt, a2..a6; word 6 is a7
        fe_call(e, 0x19100, 0, 0, 0, 0, 0, fmt, a2, a3, a4, a5, a6);
    };
    log(1, MOD | 0x28A14u);
    uint32_t st[4] = { 0, 0, 0, 0 };
    fe_call(e, 0x25088, 0, 0, 0, 0, 0, addr(st));
    log(7, MOD | 0x289F4u, st[0] & 0xFFFF, st[0] >> 16, st[1] >> 16, st[2] & 0xFFFF, st[2] >> 16, st[3] & 0xFFFF);
    static const char* const names[] = { "unknown", "Windows 95", "Windows 98", "Windows ME", "Windows NT", "Windows 2000", "Windows XP", "Windows .Net", "MacOS", "MacOS X" };
    char os[16];
    const uint32_t id = glob[0];
    const char* nm = names[id >= 1 && id <= 9 ? id : 0];
    memset(os, 0, sizeof os);
    memcpy(os, nm, strlen(nm) + 1);
    float ver; memcpy(&ver, &glob[1], 4);
    uint32_t lo, hi;
    fe_dbl((double)ver, &lo, &hi);
    fe_wide(5, 0, 0);
    fe_call(e, 0x19100, 0, 0, 0, 0, 0, MOD | 0x2895Cu, MOD | 0x2FCD8u, addr(os), lo, hi);
    fe_call(e, 0x1C830);
    const int32_t mem = (int32_t)glob[2];
    double mb = (double)mem;
    if (mem < 0) mb += 4294967296.0;
    mb *= 9.5367431640625e-07;
    fe_dbl(mb, &lo, &hi);
    fe_wide(3, 0, 0);
    fe_call(e, 0x19100, 0, 0, 0, 0, 0, MOD | 0x2893Cu, lo, hi);
    char path[260];
    path[0] = 0;
    if (fe_call(e, 0x143D0, 0x104, 0, 0, 0, addr(path))) log(2, MOD | 0x28924u, addr(path));
    const uint32_t ip = fe_call(e, 0x16A10);
    log(2, MOD | 0x28914u, ip);
    char comp[64];
    uint32_t sz = 0x40;
    comp[0] = 0;
    if (fe_call(e, 0x25070, 0, 0, 0, 0, 0, addr(comp), addr(&sz)) == 0) comp[0] = 0;
    log(2, MOD | 0x28900u, addr(comp));
    log(1, MOD | 0x28A14u);
    log(1, CTX);
}
// 1.13c Fog+0x11860 (fastcall: ECX = program name, EDX = exception callback, stack: version string or 0, log-to-file flag; ret 8): the error-manager
// initialiser. It copies the program name (and the version string when given) into the module buffers with the Storm string copy 0xCDDC (0x104 bytes),
// then, on the first call only (init flag Fog+0x56E78 clear): GetVersionExA (slot 0x25054; a zeroed 0x94-byte OSVERSIONINFOA, platform id at +0x10; the
// Win9x flag Fog+0x56E74 := platform id == 2, 0 when the call fails), two InitializeCriticalSection calls (Fog+0x32AEC, Fog+0x32C0C; slot 0x25038), the init
// flag := 1, 0x1CB50, Storm's value lookup 0xCE66 ("Diablo II", "TraceUnfreedMemory", 0, &out; the out dword is stored at Fog+0x4AF74), the memory log 0x1AE90
// (0, Fog+0x26BEC, 0x40000, 0x8000), SetUnhandledExceptionFilter (slot 0x250DC; the filter is Fog+0x11160), 0xFBC0, the self-check of the 32-entry bit mask table
// Fog+0x26798 and its complement table Fog+0x26818 (a bad entry: 0xFC20, 0xCD8E, the 0x10F30 report with line 0x3B3 / 0x3B4, 0x184A0 and _exit(-1) at Fog+0x1BA9,
// which does not return), then 0x102C0, 0x19530, the banner 0x113B0 when logToFile, _atexit of Fog+0x117E0 (0x2525), the callback word Fog+0x56E6C := EDX and
// 0x164A0. Later calls only run the banner when logToFile is set. glob = the pinned words Fog+0x56E6C..0x56E7C (callback, guard, Win9x flag, init flag),
// tu = Fog+0x4AF74, masks = the 64 table words (mask table, then the complement table).
void fog_11860(const FogEnv* e, uint32_t progName, uint32_t cb, uint32_t version, uint32_t logToFile, uint32_t* glob, uint32_t* tu, const uint32_t* masks) {
    fe_call(e, 0xCDDC, 0, 0, 0, 0, 0, MOD | 0x2FCD8u, progName, 0x104);
    if (version) fe_call(e, 0xCDDC, 0, 0, 0, 0, 0, MOD | 0x32B08u, version, 0x104);
    if (glob[3] != 0) {
        if (logToFile) fe_call(e, 0x113B0);
        return;
    }
    uint32_t vi[37];
    memset(vi, 0, sizeof vi);
    vi[0] = 0x94;
    glob[2] = 0;
    if (fe_call(e, 0x25054, 0, 0, 0, 0, 0, addr(vi))) glob[2] = vi[4] == 2;
    fe_call(e, 0x25038, 0, 0, 0, 0, 0, MOD | 0x32AECu);
    fe_call(e, 0x25038, 0, 0, 0, 0, 0, MOD | 0x32C0Cu);
    glob[3] = 1;
    fe_call(e, 0x1CB50);
    uint32_t out = 0;
    fe_call(e, 0xCE66, 0, 0, 0, 0, 0, MOD | 0x26FD0u, MOD | 0x288ECu, 0, addr(&out));
    *tu = out;
    fe_call(e, 0x1AE90, 0, 0, 0, 0, 0, 0, MOD | 0x26BECu, 0x40000, 0x8000);
    fe_call(e, 0x250DC, 0, 0, 0, 0, 0, MOD | 0x11160u);
    fe_call(e, 0xFBC0);
    for (uint32_t i = 0; i < 32; i++) {
        const uint32_t bit = 1u << i;
        const bool bad1 = masks[i] != bit, bad2 = !bad1 && masks[32 + i] != ~bit;
        if (bad1 || bad2) {
            const uint32_t r = fe_call(e, 0xFC20);
            fe_call(e, 0xCD8E);
            fe_call(e, 0x10F30, 0, 0, 0, 0, 0, 6, CTX, bad1 ? 0x3B3u : 0x3B4u, MOD | 0x2850Cu, r);
            fe_call(e, 0x184A0);
            fe_call(e, 0x1BA9, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
            return;
        }
    }
    fe_call(e, 0x102C0);
    fe_call(e, 0x19530);
    if (logToFile) fe_call(e, 0x113B0);
    fe_call(e, 0x2525, 0, 0, 0, 0, 0, MOD | 0x117E0u);
    glob[0] = cb;
    fe_call(e, 0x164A0);
}
}
