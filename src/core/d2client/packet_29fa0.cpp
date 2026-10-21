// D2Client+0x29FA0: op-keyed packet dispatch. EAX=op, ECX=p1, EDX=p2. Callees go through D2cHost.
// 0x149E0 gets (eax=op, ecx=p1, edx=p2&0xFFFF); 0x143E0 gets the 9-byte local {op, b32, c32} as s[0..2] (EBX=len unrecovered).
#include "client_host.h"
#include <emscripten.h>
// 1.13c D2Client+0x29FA0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_29fa0(D2cHost h, uint32_t op, uint32_t p1, uint32_t p2) {
    if (op == 0x13) return;
    D2cCall k = { 0, op, p1, p2 & 0xFFFF, 0, 0, { 0, 0, 0, 0 } };
    switch (op) {
    case 1: case 3: k.rva = 0x149E0; h(&k); return;
    case 2: case 4: case 6: case 7: case 9: case 10: case 0xd: case 0xe: case 0x10: case 0x11:
        k.rva = 0x143E0; k.s[0] = op & 0xFF; k.s[1] = p1; k.s[2] = p2; h(&k); return;
    default:   // local_7 is uninitialised in retail; passed as 0
        k.rva = 0x143E0; k.s[0] = op & 0xFF; k.s[1] = (p2 << 16) | (p1 & 0xFFFF); h(&k);
    }
}
