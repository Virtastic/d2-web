// Decompile-derived, 1.13c-only. Seams: GetLocalTime is injected.
#include <cstdint>

struct D2Game70130Time { uint16_t wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMs; };
struct D2Game70130Env { void (*getLocalTime)(D2Game70130Time* t); };

// 1.13c D2Game+0x70130
// ESI = 4-dword record: [0] day of week + 1, [1] month, [2] day, [3] year. Returns the record.
extern "C" uint32_t* d2game_70130_impl(uint32_t* rec, const D2Game70130Env* env)
{
    D2Game70130Time t;
    env->getLocalTime(&t);
    rec[0] = uint32_t(t.wDayOfWeek) + 1;
    rec[2] = t.wDay;
    rec[1] = t.wMonth;
    rec[3] = t.wYear;
    return rec;
}
