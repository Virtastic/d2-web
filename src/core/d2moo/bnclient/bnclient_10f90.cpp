// BNClient 0x10F90: the BNCS message 0x05 (SID_CLIENTID) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x10F90 (155 B): handler(ECX = payload, EDX = size).
// A missing payload or one under 16 bytes logs "ERROR BDClientId: invalid response" and is refused (0). Otherwise the four payload dwords are saved as registry values of key "Configuration" (Storm registry value save, thunk 0x8084, flags 2), in order "Registration Version", "Registration Authority", "Client ID", "Client Token"; then the log line "client acknowledged" between two log-control calls. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid05_10f90(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    if (!payload || size < 0x10) {
        bnSidCall(env, 0x17420, {}, { (uintptr_t)kBnSidClientIdBad });
        return 0;
    }
    bnSidCall(env, 0x8084, {}, { (uintptr_t)kBnSidConfiguration, (uintptr_t)kBnSidRegVersion, 2, bnSidU32(payload, 0) });
    bnSidCall(env, 0x8084, {}, { (uintptr_t)kBnSidConfiguration, (uintptr_t)kBnSidRegAuthority, 2, bnSidU32(payload, 1) });
    bnSidCall(env, 0x8084, {}, { (uintptr_t)kBnSidConfiguration, (uintptr_t)kBnSidClientId, 2, bnSidU32(payload, 2) });
    bnSidCall(env, 0x8084, {}, { (uintptr_t)kBnSidConfiguration, (uintptr_t)kBnSidClientToken, 2, bnSidU32(payload, 3) });
    bnSidCall(env, 0x17480, {}, { 2, 4 });
    bnSidCall(env, 0x17420, {}, { (uintptr_t)kBnSidClientAck });
    bnSidCall(env, 0x17480, {}, { 2, (uintptr_t)-4 });
    return 1;
}
