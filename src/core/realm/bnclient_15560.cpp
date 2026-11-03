// The gateway-access default-list write on retail's layout: thiscall (ECX = the gateway object), no return value.
// It logs "Writing default gateway list", loads DATA\GLOBAL\gateways.txt through Storm's file load (name, &buffer,
// &size = 0, 0, 0; a zero answer is fatal) and, when a buffer came back, hands (object, buffer) to the INI update.
#include <cstdint>

struct Bn15560Env {
    void (*log)(uint32_t fmt);
    uint32_t (*loadFile)(uint32_t name, uint32_t* buffer, uint32_t* size, uint32_t a, uint32_t b);
    void (*update)(uint32_t self, uint32_t buffer);
    void (*fatal)(uint32_t code);
    uint32_t logText, fileName;   // module strings
};

void BNGatewayAccess_WriteDefaultGatewayList32(uint32_t self, const Bn15560Env& e)
{
    uint32_t buffer, size = 0;
    e.log(e.logText);
    if (!e.loadFile(e.fileName, &buffer, &size, 0, 0)) { e.fatal(0x1B8); return; }
    if (buffer) e.update(self, buffer);
}
