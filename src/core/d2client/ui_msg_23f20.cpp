// D2Client+0x23F20: per-message dispatcher (fastcall ecx, dl; stack arg p3, ret 4). Callees go through D2cHost in retail order.
// A "load" is the self-RVA call: eax = base, ecx = byte offset, returns the dword at base+offset (caller truncates to retail's width).
#include "client_host.h"
#include <emscripten.h>
static uint32_t cl(D2cHost h, D2cCall c) { return h(&c); }
static uint32_t ld(D2cHost h, uint32_t base, uint32_t off) { return cl(h, { 0x23F20, base, off, 0, 0, 0, { 0, 0, 0, 0 } }); }
static uint32_t ui(D2cHost h, uint32_t edi, uint32_t ebx) { return cl(h, { 0x38A70, 0, 0, 0, ebx, 0, { edi, 0, 0, 0 } }); }
static void tail(D2cHost h, uint32_t edi, uint32_t id) {   // esi = ebx = id, eax = edi
    cl(h, { 0x2C270, edi, 0, 0, id, id, { 0, 0, 0, 0 } });
}
// g[0]=[bbfc], g[1]=[bcc354], g[2]=[bc97f8]; w[0..2] = writes to bcc368 / bcc36c / bcc370 (w[3] set when written)
// 1.13c D2Client+0x23F20
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_23f20(D2cHost h, uint32_t ecx, uint32_t edx, uint32_t p3, const uint32_t* g, uint32_t* w) {
    uint32_t idx = edx & 0xFF;
    w[3] = 0;
    uint32_t edi = cl(h, { 0x54E10, ecx & 0x7F, 0x6FBBA608u + (idx << 9), 0, 0, 0, { idx, 0, 0, 0 } });
    if (!edi) return;
    uint32_t n = (p3 & 0xFFFF) - 10, id;
    if (n <= 0x53) switch (p3 & 0xFFFF) {
    case 10: { uint32_t a = ui(h, edi, 0xA71); cl(h, { 0x37ED0, 0, 0, a, 0, 0, { 0x78, 0, 0, 0 } }); return; }
    case 12: {
        uint32_t u = cl(h, { 0xC1C0, 0, 0, 0, 0, 0, { edi, 0x44, 0, 0 } }); if (!u) return;
        uint32_t s = cl(h, { 0xC4B4, 0, 0, 0, 0, 0, { u, 0x15E, 0, 0 } });
        if (!cl(h, { 0xC838, 0, edi, s, 0, 0, { 0, 0, 0, 0 } })) return;
        uint32_t r = cl(h, { 0x15B0, s, 0, 0, 0, 0, { 0, 0, 0, 0 } }); if (!r) return;
        int16_t v = (int16_t)ld(h, r, 0xFC); if (v <= 0) return;
        ui(h, edi, (uint32_t)(int32_t)v); return; }
    case 13: ui(h, edi, 0xA4A); return;
    case 14: ui(h, edi, 0xA4B); return;
    case 15: if (cl(h, { 0x2490, 0, 0, 0, 0, edi, { 0, 0, 0, 0 } }) != 0x10F) return; ui(h, edi, 0x10C2); return;
    case 16: {
        if (cl(h, { 0x2430, 0, 0, 0, 0, edi, { 0, 0, 0, 0 } }) != 1) return;
        uint32_t r = cl(h, { 0x22A70, edi, 0, 0, 0, 0, { 0, 0, 0, 0 } });
        ui(h, edi, r ? ld(h, r, 0x74) : 0); return; }
    case 17: cl(h, { 0x2C10, edi, 0, 0, 0, 0, { 0, 0, 0, 0 } }); return;
    case 18: { uint32_t b = cl(h, { 0x4CCE0, edi, 0, 0, 0, 0, { 1, g[0], 0, 1 } });
        cl(h, { 0x38A70, 0, 0, 0, b, 0, { g[0], 0, 1, 0 } }); return; }
    case 0x54: case 0x55: case 0x56: case 0x57: {
        static const uint16_t eq[4] = { 0x1207, 0x1204, 0x1205, 0x1206 }, ne[4] = { 0x1211, 0x1208, 0x120B, 0x120E };
        uint32_t k = (p3 & 0xFFFF) - 0x54;
        id = cl(h, { 0x2490, 0, 0, 0, 0, edi, { 0, 0, 0, 0 } }) == 0x10F ? eq[k] : ne[k];
        ui(h, edi, id); tail(h, edi, id);
        goto dflt; }
    case 0x5A: ui(h, edi, 0x111B); return;
    case 0x5B: cl(h, { 0x38A70, 0, 0, 0, 8, 0, { g[0], 0, 0, 0 } }); return;
    case 0x5C: if (!g[1]) return; w[0] = g[2] + 0x19; w[1] = 1; w[2] = g[2] + 0x4B; w[3] = 1; return;
    case 0x5D: ui(h, edi, 0x9F9); return;
    default: break;
    }
dflt:
    if (ld(h, edi, 0) == 0) cl(h, { 0x23200, 0, 0, 0, 0, 0, { edi, p3, 0, 0 } });
}
