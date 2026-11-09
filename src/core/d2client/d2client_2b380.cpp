// 1.13c D2Client+0x2B380 (159 B): EAX = path; stdcall (outSize, rootArg), ret 8. Archive file open.
// buf[260] = path; strcat(buf, import 0xC0BE(rootArg)); h = 0xD810(EAX = buf; import 0xD408(outSize) (cdecl, its
// argument left on the stack becomes 0xD810's second word), outSize); local = 0;
// import 0xC184(h, &local, "..\Source\D2Client\CORE\ARCHIVE.CPP", 0x6B, -1, buf) (six words); return local.
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_2b380(uint32_t path, uint32_t outSize, uint32_t rootArg, const cl::Env& e) {
    volatile uint8_t buf[260] = {};
    uint32_t n = 0;
    for (uint8_t c; (c = (uint8_t)e.R8(path + n)) != 0; ++n) buf[n] = c;
    uint32_t root = e.H(0xC0BE, rootArg);
    for (uint32_t k = 0;; ++k) { uint8_t c = (uint8_t)e.R8(root + k); buf[n + k] = c; if (!c) break; }
    uint32_t pb = (uint32_t)(uintptr_t)buf;
    uint32_t sz = e.H(0xD408, outSize);
    uint32_t h = e.H(0xD810, sz, outSize, 0, 0, 0, pb);
    if (e.blk) { uint32_t len = 0; while (buf[len]) ++len; e.blk(e.ctx, 0xD810, pb, (len + 4) / 4); }
    volatile uint32_t local = 0;
    uint32_t pl = (uint32_t)(uintptr_t)&local;
    e.Xs(pb);
    e.H(0xC184, h, pl, 0xB20D6AB0u, 0x6b, 0xffffffffu);
    volatile uint32_t w[6] = { h, pl, 0xB20D6AB0u, 0x6b, 0xffffffffu, pb };
    if (e.blk) e.blk(e.ctx, 0xC184, (uint32_t)(uintptr_t)w, 6);
    return local;
}
