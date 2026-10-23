#include <cstdint>
// 1.13c D2Launch+0x14710 name accepted iff 1<len<16 and no '_'/'-' at either end
bool d2launch_charname_ok(const char* s, int32_t len) {
    return len > 1 && len < 16 && s[len-1] != '_' && s[0] != '_' && s[len-1] != '-' && s[0] != '-';
}
