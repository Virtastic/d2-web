// 1.13c Storm+0x34C40 -- log-line timestamp prefix. Prior art: none (decompile only).
#include <cstdint>
#include <cstring>
#include <cstdio>
struct Storm34c40Log { uint32_t id; void* next; void* file; uint32_t used, pending, counter, field18; char* buffer; };
struct Storm34c40Time { uint32_t tick; uint32_t month, day, hour, minute, second, ms; };
struct Storm34c40State { uint32_t last_tick; uint32_t len; char stamp[0x40]; };
// 1.13c Storm+0x34C40
void Storm_Sub34C40(Storm34c40State& s, const Storm34c40Time& now, Storm34c40Log* log, int32_t write_stamp) {
    if (!log->field18) return;
    if (now.tick != s.last_tick) {
        s.last_tick = now.tick;
        snprintf(s.stamp, sizeof s.stamp, "%u/%u %02u:%02u:%02u.%03u  ", now.month, now.day, now.hour, now.minute, now.second, now.ms);
        s.len = (uint32_t)strlen(s.stamp);
    }
    char* dst = log->buffer + log->used;
    if (write_stamp) memcpy(dst, s.stamp, s.len + 1);
    else { memset(dst, ' ', s.len); dst[s.len] = 0; }
    log->used += s.len;
    int32_t pad = (int32_t)log->pending;
    if (pad > 0) {
        if (pad > 0x80) pad = 0x80;
        memset(log->buffer + log->used, ' ', (uint32_t)pad);
        log->buffer[log->used + (uint32_t)pad] = 0;
        log->used += (uint32_t)pad;
    }
}
