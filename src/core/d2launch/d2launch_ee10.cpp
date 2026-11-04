// 1.13c D2Launch+0xEE10 (sub_EE10, Connect to Battle.net) as raw seams for the FN replay (src/core/fn/d2launch_ee10.cpp).
// The logic is d2launch_n31::FUN_6fa4ee10 (d2launch_n31.cpp); the retail EDI test is the out word slot 0x48 fills (EDI is zero at
// entry), so the core passes edi 0 and the port reads that out word through ConnSeams::out48.
#include <cstdint>
#include <string>
#include <functional>
#include "d2launch_n31.h"

struct D2lEe10Seams {
    void (*dbg)();                                  // OutputDebugStringA (the banner)
    void (*o10018)();
    void (*fc40)();
    void (*o10014)(uint32_t cb);                    // the registered callback's RVA
    void (*f582d0)();
    void (*f549e0)();
    void (*f57df0)();
    int (*vt)(int off);                             // slot call; the probe fills outs[0] for 0x48 and outs[1] for 0x28
    int (*e510)(const uint32_t* m);
    void (*o426)(const char* key, uint32_t val);    // Storm 426 profile write: key "UseCmdLine" or "Skip To Open"
    void (*o10032)();
    void (*gateway)(uint32_t slot98);               // BNGatewayAccess::SaveAndUnload(slot 0x98 result)
    void (*terminate)();                            // GetCurrentProcess + TerminateProcess
};

// gea = DAT_6fa65ea0..eb0 (5 dwords: ea0 ea4 ea8 eac eb0), g294 = DAT_6fa65294, gbe0 = DAT_6fa65be0
extern "C" int32_t d2l_ee10_core(const uint32_t* ifc, uint32_t* gea, uint32_t* g294, uint32_t* gbe0, uint32_t ad4, int32_t* outs,
                                 const D2lEe10Seams* k) {
    using namespace d2launch_n31;
    static BnClient bn;
    State s; Host h; ConnSeams c;
    uint32_t last98 = 0;
    bn.vt = [&](int off) {
        int r = k->vt(off);
        if (off == 0x48) c.out48 = outs[0];
        if (off == 0x28) c.errCode = outs[1];
        if (off == 0x98) last98 = (uint32_t)r;
        return r;
    };
    s.bn = *ifc ? &bn : nullptr;
    c.a0 = (int)gea[0]; c.a4 = (int)gea[1]; s.ea8 = (int)gea[2]; s.eb0 = (int)gea[4]; s.a294 = (int)*g294; c.ad4 = (int)ad4;
    h.debugOut = [&](const char*) { k->dbg(); };
    h.o10018 = [&] { k->o10018(); };
    h.fatal = [](int) {};
    c.fc40 = [&] { k->fc40(); };
    c.o10014a = [&] { k->o10014(0xF950); };
    c.o10014b = [&] { k->o10014(0xE4E0); };
    c.f582d0 = [&] { k->f582d0(); };
    c.f549e0 = [&] { k->f549e0(); };
    c.f57df0 = [&] { k->f57df0(); };
    c.o426 = [&](const char*, const char* key, int, int v) { k->o426(key, (uint32_t)v); };
    c.o10032 = [&] { k->o10032(); };
    c.saveGateway = [&] { k->gateway(last98); };
    c.terminate = [&] { k->terminate(); };
    c.e510 = [&](const uint32_t* m) { return k->e510(m); };
    const int32_t r = FUN_6fa4ee10(s, h, c, [](const char*) { return std::u16string(); }, 0);
    gea[1] = (uint32_t)c.a4; gea[2] = (uint32_t)s.ea8; gea[4] = (uint32_t)s.eb0; *g294 = (uint32_t)s.a294;
    if (c.be0) *gbe0 = 1;
    return r;
}
