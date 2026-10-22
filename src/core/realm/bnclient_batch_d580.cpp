// BNClient batch b51: 0xD580 0xEA60.
// Skipped (decompile register/stack-garbled, not faithful): 0xD610 0xDB70 0xDC50 0xE120 0xE2E0 0xE740.
#include <cstdint>
#include <cstring>

// 1.13c BNClient+0x0D580
// Under the DLL critical section: Ordinal_491(buf, a, b) copy, clear DAT+0x3F508 (progress). Always true.
extern "C" uint32_t d2_bnclient_d580(void (*enter)(), void (*copy)(), int* progress, void (*leave)()) {
    enter();
    copy();
    *progress = 0;
    leave();
    return 1;
}

// 1.13c BNClient+0x0EA60
// BNCS frame pump over buf[0..len): skip to 0xFF, a partial frame (<4 bytes or length > remaining)
// is moved to the buffer start and its size returned; else dispatch handler[id] (id < 0x5E) if set,
// mark seen[id]=1, advance by the frame length.
extern "C" int d2_bnclient_ea60(uint8_t* buf, int len, void (*const* handler)(), uint8_t* seen) {
    uint8_t* p = buf;
    for (;;) {
        if (*p != 0xFF) {
            int i = (int)(p - buf);
            do {
                if (len <= i) break;
                ++p; ++i;
            } while (*p != 0xFF);
        }
        uint16_t flen = *(uint16_t*)(p + 2);
        int rem = len - (int)(p - buf);
        if (rem < 4 || rem < (int)flen) {
            memmove(buf, p, (size_t)rem);  // original copies forward dword/byte-wise
            return rem;
        }
        uint8_t id = p[1];
        if (id < 0x5E) {
            if (handler[id]) handler[id]();
            seen[id] = 1;
        }
        p += flen;
    }
}
