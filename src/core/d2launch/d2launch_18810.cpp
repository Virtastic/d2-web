#include <cstdint>
// 1.13c D2Launch+0x18810 -- stdcall(unused) ret 4, always returns 1. The gateway object comes from the bnet table's slot +0x98 (a call);
// D2Win#10027 (ECX = DAT 25500) answers the chosen index (-1 = none); otherwise the gateway is set to index+1 and its realm name stored
// by Storm#425("Diablo II", "Preferred Realm", 0, realm). Then the screen list is rebuilt (0x15D30) and 0x17DF0 refreshes the menu.
struct D2l18810Seams {
    uint32_t (*gw)();                                   // [bnet + 0x98]()
    int32_t (*win10027)(uint32_t ecx);
    void (*setGateway)(uint32_t gw, uint32_t idx);      // BNGatewayAccess::SetCurGateway
    uint32_t (*realm)(uint32_t gw, uint32_t idx);       // BNGatewayAccess::Realm
    void (*store425)(uint32_t app, uint32_t key, uint32_t def, uint32_t val);
    void (*f15d30)();
    void (*f17df0)();
};
extern "C" uint32_t d2l_18810_core(uint32_t g25500, uint32_t sApp, uint32_t sKey, const D2l18810Seams* s) {
    uint32_t gw = s->gw();
    int32_t idx = s->win10027(g25500);
    if (idx != -1) {
        uint32_t i1 = (uint32_t)idx + 1;
        s->setGateway(gw, i1);
        s->store425(sApp, sKey, 0, s->realm(gw, i1));
    }
    s->f15d30();
    s->f17df0();
    return 1;
}
