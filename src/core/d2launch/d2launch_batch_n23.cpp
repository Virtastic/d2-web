// 1.13c D2Launch+0x12340 count of '-' and '_' in buf[0..len)
#include <cstdint>
int32_t d2launch_count_dash_underscore(const char* buf, int32_t len) {
    int32_t n = 0;
    for (int32_t i = 0; i < len; i++) if (buf[i] == '-' || buf[i] == '_') n++;
    return n;
}
