// src/core/storm/storm_3a330.cpp -- 1.13c Storm+0x3A330 (ordinal 331), SCodeCompile(prolog, loop, err, n, flags, out).
// Prior art used: the decompile only (D2MOO has no SCode module).
//
// Builds "W=0 " + prolog and the loop text in 0x100-byte stack buffers (flag 0x40000: prolog replaces the prefix and a
// leading '#' becomes '1' / '4'), upper-cases both, allocates the 0x50 + 4n stream, compiles each text through 0x391E0
// on a 0x1120 context, lays the prolog code out once (or three times into two images under the flag) and the loop code
// n times, links the stream into the module stream list and returns it through `out`. Seams: the compiler, the module
// critical section, and the stream list's three words (link offset, head, tail; DAT_6FC45870..78).
#include <cstdint>
#include <cstring>
#include "storm_batch_25220.h"

extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);
extern "C" uint32_t* Storm_Sub1C580(uint32_t size, const char* file, int32_t line, uint32_t flags);
extern "C" void storm_port_1090(uint32_t* r);   // gfx/storm_batch_1000.cpp: r[0] = the object
extern "C" void storm_port_1120(uint32_t* r);
extern "C" void storm_port_11a0(uint32_t* r);

struct Storm3A330Env {
    uint32_t (*compile)(uint32_t* ctx, const char* text, uint32_t* err);   // 0x391E0
    void (*set_last_error)(uint32_t code);
    void (*lock)(int enter);                                                // DAT_6FC45858
    uint32_t* list;                                                         // DAT_6FC45870: [0] link offset, [1] head, [2] tail
};

namespace {
const char kScode[] = "..\\3rdParty\\STORM\\SOURCE\\SCODE.CPP";
const char kStream[] = ".?AUSTREAM@@";
constexpr uint32_t kCompilerVtable = 0x6FC36840;
uint32_t p32(const void* p) { return (uint32_t)(uintptr_t)p; }
uint32_t* w(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
void upper(char* s) { for (; *s; ++s) if (*s >= 'a' && *s <= 'z') *s = (char)(*s - 0x20); }
storm25220::State& st() { static storm25220::State s; return s; }
void call(void (*f)(uint32_t*), uint32_t* p) { uint32_t r[5] = { p32(p), 0, 0, 0, 0 }; f(r); }
void Storm_Sub1090(uint32_t* p) { call(storm_port_1090, p); }
void Storm_Sub1120(uint32_t* p) { call(storm_port_1120, p); }
void Storm_Sub11A0(uint32_t* p) { call(storm_port_11a0, p); }
}

// 1.13c Storm+0x3A330
extern "C" uint32_t Storm_Sub3A330Real(const char* prolog, const char* loop, uint32_t* err, uint32_t n, uint32_t flags,
                                       uint32_t* out, const Storm3A330Env& env) {
    if (err) *err = 0;
    if (!(loop != nullptr && *loop != 0 && n != 0 && out != nullptr)) { env.set_last_error(0x57); return 0; }
    const uint32_t dbl = (flags >> 0x12) & 1;
    char pro[0x100], lp[0x100];
    storm25220::copy(st(), pro, "W=0 ", 0x7FFFFFFF);
    uint32_t base = p32(pro);
    if (prolog) {
        if (dbl == 0) { base = p32(pro) + storm25220::length(st(), pro); storm25220::append(st(), pro, prolog, 0x100); }
        else storm25220::copy(st(), pro, prolog, 0x7FFFFFFF);
    }
    storm25220::copy(st(), lp, loop, 0x7FFFFFFF);
    if (dbl) { if (pro[0] == '#') pro[0] = '1'; if (lp[0] == '#') lp[0] = '4'; }
    upper(pro);
    upper(lp);
    uint32_t* s = Storm_Sub1C580(n * 4 + 0x50, kStream, -2, 8);
    if (s) { s[0] = 0; s[1] = 0; }
    s[2] = flags;
    s[3] = 0xFFFFFFFFu;
    uint32_t c[8];   // the 0x1120 context: +4/+8 prolog code and size, +0x18/+0x1C loop code and size
    Storm_Sub1120(c);
    c[0] = kCompilerVtable;
    uint32_t ok = env.compile(c, pro, err);
    if (err && *err) *err = (*err - base) + p32(prolog);
    if (ok == 0) {
        env.lock(1); Storm_Sub1090(s); Storm_Sub1B960(s, kStream, -2); env.lock(0);   // 0x38EB0
        Storm_Sub11A0(c);
        return 1;   // 0x38EB0 returns 1 for a non-null stream
    }
    const uint32_t reps = dbl * 2 + 1, s1 = c[2];
    uint32_t sz = s1 * reps + c[7];
    s[6] = sz;
    s[5] = p32(Storm_Sub1C580(sz, kScode, 0x440, 0));
    if (dbl) { sz = s1 * reps + c[7]; s[0xC] = sz; s[0xB] = p32(Storm_Sub1C580(sz, kScode, 0x444, 0)); }
    uint8_t* d2 = (uint8_t*)w(s[0xB]);
    uint8_t* d = (uint8_t*)w(s[5]);
    for (uint32_t i = reps; i; --i) {
        std::memcpy(d, w(c[1]), s1); d += s1;
        if (dbl) { std::memcpy(d2, w(c[1]), s1); d2 += s1; }
    }
    std::memcpy(d, w(c[6]), c[7]);
    if (dbl) {
        std::memcpy(d2, w(c[6]), c[7]);
        for (uint32_t k = 0; k <= reps; ++k) { s[7 + k] = s1 * (reps - k) + s[5]; s[0xD + k] = s1 * (reps - k) + s[0xB]; }
    }
    ok = env.compile(c, lp, err);
    if (err && *err) *err = (*err - p32(lp)) + p32(loop);
    if (ok == 0) {
        env.lock(1); Storm_Sub1090(s); Storm_Sub1B960(s, kStream, -2); env.lock(0);
    } else {
        const uint32_t l1 = c[2];
        sz = l1 * n + c[7];
        s[0x12] = sz;
        d = (uint8_t*)Storm_Sub1C580(sz, kScode, 0x46E, 0);
        s[0x11] = p32(d);
        for (uint32_t i = n; i; --i) { std::memcpy(d, w(c[1]), l1); d += l1; }
        std::memcpy(d, w(c[6]), c[7]);
        c[6] += c[7];   // retail walks the context's own loop-code word and count through the copy
        c[7] = 0;
        for (uint32_t k = 0; k <= n; ++k) s[0x13 + k] = l1 * (n - k) + s[0x11];
        env.lock(1);
        uint32_t* L = env.list;
        if (s[0] != 0) {
            const uint32_t nx = s[0], pv = s[1];
            uint32_t* at = (int32_t)pv < 0 ? w(~pv) : w(p32(s) + (pv - *w(nx + 4)));
            *at = nx;
            *w(nx + 4) = s[1];
            s[0] = 0; s[1] = 0;
        }
        s[0] = p32(&L[1]);
        s[1] = L[2];
        uint32_t* at;
        if ((int32_t)L[2] < 0) at = w(~L[2]);
        else {
            uint32_t off = L[0];
            if ((int32_t)off < 0) off = p32(&L[1]) - *w(L[1] + 4);
            at = w(L[2] + off);
        }
        *at = p32(s);
        L[2] = p32(s);
        env.lock(0);
        *out = p32(s);
    }
    Storm_Sub11A0(c);
    return 1;
}
