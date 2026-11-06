// BNClient 0xBA00: sends BNCS frame 0x5B carrying three strings back to back, each with its NUL.
// __fastcall (ECX = first, EDX = second) plus one stack word (third), ret 4. A null or empty first string is fatal
// (codes 0x5CD / 0x5CE); a null or empty second or third returns 0 without sending; a string of 0x100 or more
// bytes with its NUL is fatal (0x5D8, 0x5E3, 0x5EE). The fatal tail (0x80E4, 0x80DE, 0x1569) never returns.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef uint32_t (*Bn5bLenFn)(const char* s);                                                 // 1.13c BNClient+0x8042 (lstrlenA)
typedef int32_t (*Bn5bSendFn)(uint32_t length, uint8_t id, const uint8_t* payload);           // 1.13c BNClient+0xEB00
typedef void (*Bn5bFatalFn)(uint32_t code);                                                   // 1.13c BNClient+0x80E4/0x80DE/0x1569

// 1.13c BNClient+0x0BA00
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_triple_ba00(const char* a, const char* b, const char* c,
                                                                   Bn5bLenFn len, Bn5bSendFn send, Bn5bFatalFn fatal) {
    uint8_t buffer[0x304];
    if (!a) fatal(0x5CD);
    if (!*a) fatal(0x5CE);
    if (!b || !*b || !c || !*c) return 0;
    uint32_t la = len(a) + 1;
    if (la > 0x100) fatal(0x5D8);
    memcpy(buffer, a, la);
    uint8_t* p = buffer + la;
    uint32_t lb = len(b) + 1;
    if (lb > 0x100) fatal(0x5E3);
    memcpy(p, b, lb);
    uint32_t lc = len(c) + 1;
    if (lc > 0x100) fatal(0x5EE);
    memcpy(p + lb, c, lc);
    return send(la + lb + lc, 0x5B, buffer) >= 0 ? 1 : 0;
}
