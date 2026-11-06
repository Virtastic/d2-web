// 1.13c BNClient+0x12A70 (441 B, cdecl, no args): the BNNews table load. g = the six globals 0x1F8CC..0x1F8E0 {cap, count, table, step, flag, header}
// (the 0x11C0/0x1180 growable-array header starts at cap). Rule:
//   data = 0; ok = 0; 0x14050(ECX = the name global 0x1E0F0, EDX = &stamp, &data, &size) == 0 -> fail.
//   header = data[0]; n = data[1]; n > 0x100 -> fail. count = 0; n > cap -> 0x11C0(EAX = n, EBX = &cap). flag = 0; prev = 0.
//   for k < n: want = count + 1; want > cap -> { s = step ? step : 0x1180(EAX = want, &cap); want % s -> want += s - want % s; 0x11C0(want, &cap) }
//     e = table + count++ * 0x408; e[0] = 0; e[1] = stamp word; the line (strlen >= 0x400 -> fail) copied with its NUL to e+8;
//     prev > e[1] -> stop (ok = 0); prev = e[1]. ok = (k == n); ok -> skip the reset.
//   fail: table -> 0x801E(table, ".?AUItem@NewsTable@@", -2, 0); cap = count = table = flag = header = 0 (step kept).
//   data -> 0x801E(data, "..\Source\D2BNClient\BNNews.cpp", 0x12B, 0). Returns ok.
#include <cstdint>
#include "core/bnclient/bnclient_12a70.h"

extern "C" uint32_t bn_news_load_12a70(uint32_t* g, const BnNews12a70Env* e) {
    uint32_t stamp[2] = {0, 0}, size = 0, ok = 0;
    uint32_t* data = nullptr;
    if (e->load(stamp, &data, &size)) {
        const uint32_t* p = data;
        g[5] = *p++;
        const uint32_t n = *p++;
        if (n <= 0x100) {
            g[1] = 0;
            if (n > g[0]) e->grow(n, g);
            g[4] = 0;
            uint32_t prev = 0, k = 0;
            bool stop = false;
            while (k < n) {
                uint32_t want = g[1] + 1;
                if (want > g[0]) {
                    const uint32_t s = g[3] ? g[3] : e->step(want, g);
                    const uint32_t rem = want % s;
                    if (rem) want = s - rem + want;
                    e->grow(want, g);
                }
                uint32_t* ent = (uint32_t*)(uintptr_t)(g[2] + g[1] * 0x408u);
                ++g[1];
                ent[0] = 0;
                ent[1] = *p++;
                const char* s = (const char*)p;
                uint32_t len = 0;
                while (s[len]) ++len;
                if (len >= 0x400) { stop = true; break; }
                char* d = (char*)(ent + 2);
                for (uint32_t i = 0; i <= len; ++i) d[i] = s[i];
                p = (const uint32_t*)(s + len + 1);
                if (prev > ent[1]) { stop = true; break; }
                prev = ent[1];
                ++k;
            }
            ok = (!stop && k == n) ? 1u : 0u;
        }
    }
    if (!ok) {
        if (g[2]) e->free((void*)(uintptr_t)g[2], 0);
        g[0] = g[1] = g[2] = g[4] = g[5] = 0;
    }
    if (data) e->free(data, 1);
    return ok;
}
