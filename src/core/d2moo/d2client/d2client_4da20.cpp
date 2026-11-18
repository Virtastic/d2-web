// 1.13c D2Client+0x4DA20 (57 B, EAX = id): look the record up (0xA60D0, EAX in) and, when found, release it
// through 0xB1420 (EAX = 0, ECX = record, one stack word 0). A record whose first dword is not 3 is a fatal
// Fog assert + exit in retail; the callback reports it and does not return here.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2c_port_4da20(uint32_t id, int32_t* (*lookupA60D0)(uint32_t eax),
                               void (*releaseB1420)(int32_t* ecx, uint32_t arg), void (*fatal161)()) {
    int32_t* t = lookupA60D0(id);
    if (!t) return;
    if (*t != 3) { fatal161(); return; }
    releaseB1420(t, 0);
}
