// 1.13c D2Client+0x41780 -- fastcall(ECX = unit, EDX = skill index) + 1 stack word p3 (ret 4): returns 1 or 0.
// Row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. v = #40550(EAX = index, unit) < 0 returns 0; n = #40450(p3, EDI = index);
// n <= 0: n = #C316(unit, row dword +0x64, index, p3). #2A0B0(EAX = unit). Unit path (+0x2C) null: the 0xBF92 / 0xBF80 / 0x3664(-1) fatal (3664 does not
// return in retail; the port carries on with its answer as the path). t = #C2DA(path); t != 0: edi = #1630(ECX = t), eax = #1660(ECX = t); else
// #A1340(EBX = unit, EDI = &X, stack &Y) == 0 returns 0, edi = X, eax = Y (n is not replaced by either). Then #41100(EAX = unit, EDX = edi,
// stack v, edi, eax, n, index, p3; ret 0x18) and return 1.
#include "cl_env.h"
extern "C" uint32_t d2c_41780(uint32_t unit, int32_t idx, uint32_t p3, const cl::Env& e) {
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!row) return 0;
    const uint32_t v = e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)idx);
    if ((int32_t)v < 0) return 0;
    uint32_t n = e.H(0x40450, p3);
    e.Ej(0x40450, (uint32_t)idx);
    if ((int32_t)n <= 0) n = e.H(0xc316, unit, e.R(row + 0x64), (uint32_t)idx, p3);
    e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
    uint32_t path = e.R(unit + 0x2c);
    if (!path) {
        const uint32_t msg = e.H(0xbf92, 0x946);
        e.H(0xbf80, 0xB2000000u | 0xD359F, msg, 0x946);
        path = e.H(0x3664, 0xffffffffu);
    }
    const uint32_t t = e.H(0xc2da, path);
    uint32_t edi, eax;
    if (t) {
        edi = e.H(0x1630, 0, 0, 0, 0, 0, 0, t);
        eax = e.H(0x1660, 0, 0, 0, 0, 0, 0, t);
    } else {
        uint32_t loc[2] = {};   // [0] = X (EDI), [1] = Y (stack word 0)
        e.Ed((uint32_t)(uintptr_t)&loc[0]);
        if (!e.H(0xa1340, (uint32_t)(uintptr_t)&loc[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
        edi = loc[0]; eax = loc[1];
    }
    e.Xs(p3);
    e.H(0x41100, v, edi, eax, n, (uint32_t)idx, unit, 0, edi);
    return 1;
}
