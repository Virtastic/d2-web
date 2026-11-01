// 1.13c BNClient+0x12810 -- DAT_6ff3f640 once when set, then DAT_6ff3f63c (count = DAT_6ff3f8d0 - DAT_6ff3f8dc) times when positive. Globals passed in.
#include <cstdint>
extern "C" void bnclient_12810(void (*f640)(void), void (*f63c)(void), int32_t count) {
    if (f640) f640();
    if (f63c && count > 0) {
        do { f63c(); } while (--count != 0);
    }
}
