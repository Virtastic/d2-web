// 1.13c D2Client+0x14660: send the 0x69 packet, bump the two counters, reset the UI
// flags unless the state is 0,1,6,7,8,9, then call the screen object's slot 0x90 in states 6 and 7.
#include <cstdint>
struct Client14660State { uint32_t s, pad, f39c, f3a0, f3a4, c4e0, c4e4, c4e8; };   // 11c394, 11c398, 11c39c, 11c3a0, 11c3a4, 11c4e0, 11c4e4, 11c4e8 (as laid out by the probe)
static_assert(sizeof(Client14660State) == 32, "state");
extern "C" void d2_client_14660(Client14660State* g, uint32_t obj,
                                void (*send)(uint32_t, uint32_t, uint32_t), void (*call90)(uint32_t)) {
    send(1, 0, 0xB3000000u);   // D2Net #10024 stdcall (1, 0, &buf): buf = the stack byte 0x69 (ECX low 3 bytes are never sent)
    ++g->c4e0; ++g->c4e8;
    uint32_t s = g->s;
    if (s != 0 && s != 1 && s != 6 && s != 7 && s != 8 && s != 9) { g->f3a4 = 1; g->f3a0 = 0; g->f39c = 0; }
    if ((s == 6 || s == 7) && obj) call90(obj);
}
