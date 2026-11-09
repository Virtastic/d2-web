// 1.13c D2Client+0x87230 (122 B): Warden module tag. stdcall (digest, a1, a2), ret 0xC.
// Writes the 16 digest bytes as 32 lowercase hex chars (CRT sprintf "%02x" per byte) into a stack buffer, appends ".mod" with Storm 501
// (SStrCopy, limit 5). If the Warden context 0x11BF54 is set: GetSystemTimeAsFileTime into a stack FILETIME, then the context's +0xD0 entry
// with ECX = the tag, EDX = a1, stack (a2, &filetime, -1, -1), callee-popped.
#include <cstdint>
#include <emscripten.h>

struct D2c87230Callees {
    void (*fileTime)(uint32_t ft[2]);                                                         // kernel32 GetSystemTimeAsFileTime
    void (*report)(uint32_t ctx, const char* tag, uint32_t a1, uint32_t a2, uint32_t* ft, int32_t m1, int32_t m2);   // [ctx+0xD0]
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87230(const uint8_t* digest, uint32_t a1, uint32_t a2, uint32_t ctx, const D2c87230Callees* cb) {
    static const char hx[] = "0123456789abcdef";
    char tag[0x28];
    for (int i = 0; i < 16; ++i) { tag[2 * i] = hx[digest[i] >> 4]; tag[2 * i + 1] = hx[digest[i] & 15]; }
    const char mod[] = ".mod";
    for (int i = 0; i < 5; ++i) tag[32 + i] = mod[i];
    if (!ctx) return;
    uint32_t ft[2];
    cb->fileTime(ft);
    cb->report(ctx, tag, a1, a2, ft, -1, -1);
}
