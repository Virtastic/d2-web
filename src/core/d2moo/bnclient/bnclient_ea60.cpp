// 1.13c BNClient+0x0EA60 -- the BNCS frame pump. EAX = buffer, stack = length (ret 4). Skip to the next 0xFF byte (the scan may run to the end of the length);
// a frame with < 4 bytes left or a header length (u16 at +2) above the bytes left is a partial one: it is moved to the buffer start and its size returned
// (unsigned compare against 4, signed against the length). Else id = byte +1: id < 0x5E calls handler[id] (table DLL+0x3E128) with ECX = frame+4,
// EDX = length-4 when set, then sets the DWORD flag seen[id] (DLL+0x3ED58) to 1; advance by the header length.
#include <cstdint>
#include <cstring>
extern "C" int bnclient_ea60(uint8_t* buf, int32_t len, void (*const* handler)(uint8_t*, uint32_t), uint32_t* seen) {
    uint8_t* p = buf;
    for (;;) {
        if (*p != 0xFF) {
            int32_t i = (int32_t)(p - buf);
            do { if (len <= i) break; ++p; ++i; } while (*p != 0xFF);
        }
        const uint16_t flen = *(const uint16_t*)(p + 2);
        const int32_t rem = len - (int32_t)(p - buf);
        if ((uint32_t)rem < 4 || rem < (int32_t)flen) {
            memmove(buf, p, (size_t)(uint32_t)rem);
            return rem;
        }
        const uint8_t id = p[1];
        if (id < 0x5E) {
            if (handler[id]) handler[id](p + 4, (uint32_t)flen - 4);
            seen[id] = 1;
        }
        p += flen;
    }
}
