// src/core/realm/bnclient_setargv_450a.cpp -- UNMAPPED 1.13c BNClient.dll row s3687 (implementation only),
// from build-wasm/ghidra-bnclient/decomp/0x450A.c. MSVC 2003 CRT __setargv.
#include <cstdint>
#include <emscripten.h>

// parse_cmdline(argv, &state): state is {argc, charBytes}; the first call (argv null) only counts.
typedef void (*bn_parse_fn)(uint32_t *argv, int32_t *state);
typedef uint32_t *(*bn_malloc_fn)(uint32_t n);

// 1.13c BNClient+0x0450A (162 B, caller 0x1FDD): __setargv. Count pass, malloc(chars + argc*4), fill pass;
// on success *argc_out = argc-1, *argv_out = block, return 0; malloc failure returns -1. The mbctable init,
// GetModuleFileNameA and the argv0 global are the caller's (host CRT) side.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_setargv(int32_t chars, bn_parse_fn parse, bn_malloc_fn alloc,
                                        int32_t *argc_out, uint32_t **argv_out) {
    int32_t st[2] = {chars, 0};
    parse(nullptr, st);
    uint32_t *p = alloc((uint32_t)(st[0] + st[1] * 4));
    if (!p) return -1;
    parse(p, st);
    *argc_out = st[1] - 1;
    *argv_out = p;
    return 0;
}
