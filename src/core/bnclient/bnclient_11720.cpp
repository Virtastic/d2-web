// BNClient 0x11720: the BNCS message 0x06 (SID_STARTVERSIONING) entry of the receive dispatch table.
#include <emscripten.h>
#include "bnclient_sid.h"

// 1.13c BNClient+0x11720 (94 B): handler(ECX = payload, EDX = size).
// The archive name is the string at payload+8. When the payload is longer than that name (strlen + 9 < size) the string after it is copied into 0x1F770 (Storm string copy, capacity 0x104). Then the download of the archive starts through 0xE120(0x80000001, 3, 0, 0, name, 0). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_sid06_11720(const uint8_t* payload, uint32_t size, const BnSidEnv* env) {
    (void)payload; (void)size; (void)env;
    const char* name = (const char*)payload + 8;
    const uint32_t len = (uint32_t)strlen(name);
    if (len + 9 < size) {   // a second string follows the name: keep it, at most 0x103 characters
        const char* next = name + len + 1;
        uint32_t i = 0;
        while (i < 0x103 && next[i]) { g_bn_6ff3f770[i] = next[i]; ++i; }
        g_bn_6ff3f770[i] = 0;
    }
    bnSidCall(env, 0xE120, {}, { 0x80000001u, 3, 0, 0, (uintptr_t)name, 0 });
    return 1;
}
