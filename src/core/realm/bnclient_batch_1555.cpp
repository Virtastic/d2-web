// BNClient.dll alpha-lane batch 2 (rows 0x01555, 0x01569, 0x0157A, 0x0158B, 0x015F2, 0x015FF, 0x01621,
// 0x018AF, 0x018C6, 0x01940, 0x019FE, 0x01AC7): the MSVC 2003 Release CRT leaves this DLL statically
// links. Every rule is transcribed from the cached Ghidra decompile of retail 1.13c Bnclient.dll (base
// 0x6FF20000). Host services the CRT asks for (the system clock, process termination, critical
// sections) are the port's seams, not invented behaviour. The probe ABI is the repo's: arguments in
// r[0..], result in r[4].
#include <emscripten.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>

// 0x018AF's parser and 0x015F2/0x015FF's thread block live in bnclient_batch_rowex.cpp.
extern "C" uint32_t bn_strtoxl(const char* s, const char** endptr, uint32_t base, uint32_t flags);
extern "C" void* bn_getptd(void);

// --- shared CRT state and leaf helpers --------------------------------------------------------------
// The PTD is the one block bn_getptd (BNClient+0x03255) hands out; the CRT puts _holdrand at +0x14.
static uint32_t* bn_holdrand(void) { return (uint32_t*)((uint8_t*)bn_getptd() + 0x14); }

static uint8_t bn_fold_lower(uint8_t c) { return (uint8_t)(c - 'A') < 26 ? (uint8_t)(c + 0x20) : c; }

// ___ascii_stricmp (0x038D0): fold A-Z on both bytes at the first mismatch, then -1 / 1 by the folded
// order; equal folded bytes continue, the pair of NULs ends with 0.
static int32_t bn_ascii_stricmp(const char* a, const char* b) {
    for (;;) {
        uint8_t x = (uint8_t)*a++, y = (uint8_t)*b++;
        if (x == y) {
            if (x == 0) return 0;
            continue;
        }
        x = bn_fold_lower(x);
        y = bn_fold_lower(y);
        if (x != y) return x < y ? -1 : 1;
    }
}

// The shipped FILETIME-to-__time32 conversion: low + 0x2AC18000 with the carry into high +
// 0xFE624E21, then an unsigned 64-bit divide by 10,000,000, truncated to the low 32 bits. That sum is
// exactly filetime - 116444736000000000 (the 1601-to-1970 epoch offset).
static int32_t bn_time32_from_filetime(uint64_t ft) {
    const uint64_t seconds = (ft - 116444736000000000ull) / 10000000ull;
    return (int32_t)(uint32_t)seconds;
}

// GetSystemTimeAsFileTime is the host seam: the port's clock in 100 ns ticks since 1601.
static uint64_t bn_host_filetime(void) {
    const int64_t unix_seconds = (int64_t)std::time(nullptr);
    return (uint64_t)unix_seconds * 10000000ull + 116444736000000000ull;
}

// --- row implementations ----------------------------------------------------------------------------

// 0x01555 is a Ghidra-split 14-byte block of _doexit (0x014A6): with EDI = 0 at retail's call site it
// tests the caller's third argument slot [EBP+0x10] (doexit's `retcaller`), and when nonzero calls
// FUN_6FF230AE(8) = LeaveCriticalSection(&DAT_6FF3D090[8*2]) before returning. Our single-threaded CRT
// shim has no lock objects, so the row keeps the branch decision; that predicate is its observable.
static int32_t bn_doexit_unlock_fires(int32_t caller_slot, int32_t edi) {
    return caller_slot != edi;
}

// rand state helpers for 0x015F2 / 0x015FF.
static void bn_srand(uint32_t seed) { *bn_holdrand() = seed; }

static int32_t bn_rand() {
    uint32_t* holdrand = bn_holdrand();
    const uint32_t next = *holdrand * 0x343FDu + 0x269EC3u;
    *holdrand = next;
    return (int32_t)((next >> 16) & 0x7FFF);
}

static int32_t bn_time32(int32_t* out) {
    const int32_t t = bn_time32_from_filetime(bn_host_filetime());
    if (out != nullptr) *out = t;
    return t;
}

// _strtoul (0x018AF) is the unsigned flag (bit 1) of the shared strtoxl parser.
static uint32_t bn_strtoul(const char* s, const char** endptr, int radix) {
    return bn_strtoxl(s, endptr, (uint32_t)radix, 1);
}

// __stricmp (0x018C6): retail takes the thread locale and, when lc_category[0].wlocale is NULL, calls
// ___ascii_stricmp. The port's locale block is the zeroed one from bn_updatetlocinfo, so that ASCII
// path is the only one reachable here; the code-page path needs locale tables the port does not carry.
// ponytail: ASCII-only; add the LCMapString path if a caller ever needs a non-ASCII code page.
static int32_t bn_stricmp(const char* a, const char* b) { return bn_ascii_stricmp(a, b); }

// _strchr (0x01940): first byte equal to the low byte of _Val, the terminator included when _Val is 0.
static const char* bn_strchr(const char* s, int val) {
    const char c = (char)val;
    for (;; ++s) {
        if (*s == c) return s;
        if (*s == '\0') return nullptr;
    }
}

// ___toupper_mt (0x019FE): the shipped fast path (locale's +0x14 unset, or a single-byte code page and
// c < 0x80) folds a-z by -0x20 and returns every other byte unchanged. The LCMapString branch below it
// needs the CRT code-page tables the port does not carry.
// ponytail: ASCII-only, the fast path; add the code-page tables if a caller ever passes >= 0x80.
static uint32_t bn_toupper_mt(const void* locale, uint32_t c) {
    (void)locale;
    if (c > 0x60 && c < 0x7B) return c - 0x20;
    return c;
}

// _toupper (0x01AC7): __getptd, refresh the locale, then ___toupper_mt. Both prefix calls only locate
// the locale object, so the result is the same fast-path fold.
static int32_t bn_toupper(int c) { return (int32_t)bn_toupper_mt(nullptr, (uint32_t)c); }

// --- probes -----------------------------------------------------------------------------------------

// 1.13c BNClient+0x01555 (14 B): _doexit's unlock-on-early-return block; returns 1 when retail would
// call LeaveCriticalSection(lock 8). Inputs: r[0] = [EBP+0x10] (doexit's retcaller), r[1] = EDI.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_1555(uint32_t* r) {
    r[4] = (uint32_t)bn_doexit_unlock_fires((int32_t)r[0], (int32_t)r[1]);
}

// 1.13c BNClient+0x01569 (17 B): _exit(code) = doexit(code, quick=0, retcaller=0), which terminates the
// process after the CRT walks and never returns. The port has no process to end; process termination
// with the same code is the stand-in, as for D2MCPClient's _exit (src/core/realm/mcp_exit.cpp).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_1569(uint32_t* r) {
    std::exit((int)(int32_t)r[0]);
}

// 1.13c BNClient+0x0157A (17 B): __exit(code) = doexit(code, quick=1, retcaller=0): the `quick` branch
// skips the __onexit walk, still runs the terminator table, then terminates and never returns.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_157a(uint32_t* r) {
    std::exit((int)(int32_t)r[0]);
}

// 1.13c BNClient+0x0158B (15 B): __cexit() = doexit(0, quick=0, retcaller=1): run the __onexit list
// backwards and the terminator table, then return instead of terminating. The port registers no
// __onexit entries and its C-terminator table is the shim in bn_doexit, so the call returns.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_158b(uint32_t* r) {
    r[4] = 0;
}

// 1.13c BNClient+0x015F2 (13 B): _srand(seed) -- store the seed in the thread's _holdrand and return.
// r[0] = seed.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_15f2(uint32_t* r) {
    bn_srand(r[0]);
}

// 1.13c BNClient+0x015FF (34 B): _rand -- _holdrand = _holdrand * 0x343FD + 0x269EC3; return bits
// 16..30. r[4] = result.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_15ff(uint32_t* r) {
    r[4] = (uint32_t)bn_rand();
}

// 1.13c BNClient+0x01621 (57 B): __time32/_time -- GetSystemTimeAsFileTime, convert to seconds since
// 1970, and store through the optional out pointer. r[0] = out pointer (0 = NULL), r[4] = result.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_1621(uint32_t* r) {
    int32_t* out = (int32_t*)(uintptr_t)r[0];
    r[4] = (uint32_t)bn_time32(out);
}

// 1.13c BNClient+0x018AF (23 B): _strtoul(s, endptr, radix) = strtoxl(s, endptr, radix, unsigned).
// r[0] = string, r[1] = endptr pointer (0 = NULL), r[2] = radix, r[4] = result.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_18af(uint32_t* r) {
    r[4] = bn_strtoul((const char*)(uintptr_t)r[0], (const char**)(uintptr_t)r[1], (int)r[2]);
}

// 1.13c BNClient+0x018C6 (105 B): __stricmp(a, b) through the thread locale; our locale is the ASCII
// one, so the result is ___ascii_stricmp's -1 / 0 / 1. r[0], r[1] = strings, r[4] = result.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_18c6(uint32_t* r) {
    r[4] = (uint32_t)bn_stricmp((const char*)(uintptr_t)r[0], (const char*)(uintptr_t)r[1]);
}

// 1.13c BNClient+0x01940 (195 B): _strchr(s, c) -- first match or NULL; the scan stops at the NUL.
// r[0] = string, r[1] = character, r[4] = pointer (0 = not found).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_1940(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)bn_strchr((const char*)(uintptr_t)r[0], (int)r[1]);
}

// 1.13c BNClient+0x019FE (201 B): ___toupper_mt(locale, c) -- the fast-path fold. r[0] = locale
// pointer, r[1] = character, r[4] = result.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_19fe(uint32_t* r) {
    r[4] = bn_toupper_mt((const void*)(uintptr_t)r[0], r[1]);
}

// 1.13c BNClient+0x01AC7 (34 B): _toupper(c) -- get the thread locale and fold through ___toupper_mt.
// r[0] = character, r[4] = result.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_1ac7(uint32_t* r) {
    r[4] = (uint32_t)bn_toupper((int)r[0]);
}

struct BnSeedSha1 { uint32_t state[5], bitLow, bitHigh; uint8_t block[64]; };
extern "C" void d2_bnclient_sha1_update(BnSeedSha1*, uint8_t*, uint32_t);
extern "C" void d2_bnclient_sha1_final(BnSeedSha1*, uint8_t*);

// 1.13c BNClient+0x0ABE0 (180 B): reseed -- hash {time32, <uninit>, rand()} (12 B) with SHA-1 into
// s+0xC (5 dwords), then copy those to s+0x20 and zero s+0x34..0x4B. Hidden ESI = s, EAX = time32 result
// (srand seed). r[0] = s. The middle 4 input bytes are uninitialised stack in retail; zero here.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_bnclient_abe0(uint32_t* r) {
    uint32_t* s = (uint32_t*)(uintptr_t)r[0];
    int32_t in[3] = {0, 0, 0};
    uint32_t seed = (uint32_t)bn_time32(&in[0]);
    bn_srand(seed);
    in[2] = bn_rand();
    BnSeedSha1 c = {{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0}, 0, 0, {0}};
    d2_bnclient_sha1_update(&c, (uint8_t*)in, 12);
    d2_bnclient_sha1_final(&c, (uint8_t*)(s + 3));
    for (int i = 0; i < 5; ++i) s[8 + i] = s[3 + i];
    for (int i = 0; i < 6; ++i) s[13 + i] = 0;
}
