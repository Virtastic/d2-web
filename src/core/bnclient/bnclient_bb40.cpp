// BNClient 0xBB40: sends BNCS frame 0x5A carrying two strings back to back, each with its NUL. __fastcall
// (ECX = first, EDX = second), plain ret. A null or empty first string is fatal (0x5A6 / 0x5A7); a null or empty
// second returns 0 without sending; a string of 0x100 or more bytes with its NUL is fatal (0x5AF, 0x5B9).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef uint32_t (*Bn5aLenFn)(const char* s);                                                 // 1.13c BNClient+0x8042 (lstrlenA)
typedef int32_t (*Bn5aSendFn)(uint32_t length, uint8_t id, const uint8_t* payload);           // 1.13c BNClient+0xEB00
typedef void (*Bn5aFatalFn)(uint32_t code);                                                   // 1.13c BNClient+0x80E4/0x80DE/0x1569

// 1.13c BNClient+0x0BB40
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_pair_bb40(const char* a, const char* b, Bn5aLenFn len,
                                                                 Bn5aSendFn send, Bn5aFatalFn fatal) {
    uint8_t buffer[0x204];
    if (!a) fatal(0x5A6);
    if (!*a) fatal(0x5A7);
    if (!b || !*b) return 0;
    uint32_t la = len(a) + 1;
    if (la > 0x100) fatal(0x5AF);
    memcpy(buffer, a, la);
    uint32_t lb = len(b) + 1;
    if (lb > 0x100) fatal(0x5B9);
    memcpy(buffer + la, b, lb);
    return send(la + lb, 0x5A, buffer) >= 0 ? 1 : 0;
}
