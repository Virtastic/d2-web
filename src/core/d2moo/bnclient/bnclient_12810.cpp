// 1.13c BNClient+0x12810 -- DAT_6ff3f640 once when set; then, when DAT_6ff3f63c is set, once per entry from index (total-1) down to
// base (n = DAT_6ff3f8d0 - DAT_6ff3f8dc > 0) over the 0x408-byte array at DAT_6ff3f8d4: callee(ECX = dword at entry+4, EDX = entry+8).
// Declared void in retail; the value returned is the EAX it leaves: 0 when DAT_6ff3f63c is null, n when n <= 0, else the last callee's EAX.
#include <cstdint>
extern "C" int32_t bnclient_12810(void (*f640)(void), uint32_t (*f63c)(uint32_t, uint8_t*), int32_t total, int32_t base, uint8_t* arr) {
    if (f640) f640();
    if (!f63c) return 0;
    int32_t n = total - base;
    if (n <= 0) return n;
    uint32_t eax = 0;
    for (int32_t i = total; n > 0; --n, --i) {
        uint8_t* e = arr + (int64_t)(i - 1) * 0x408;
        eax = f63c(*(uint32_t*)(e + 4), e + 8);
    }
    return (int32_t)eax;
}
