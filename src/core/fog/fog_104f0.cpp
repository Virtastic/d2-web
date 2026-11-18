// Prior art used: none; the arithmetic was read from the 1.13c bytes (disassembly of Fog+0x104F0).
// Constants read from the image: 365.25 (0x29038), 30.6001 (0x28FE8), 0.01 (0x29028), -0.25 (0x29020), 1720995.0 (0x29030).
// Callees: Fog+0x2EC0 is floor (std::floor here); Fog+0x110AC is the CRT _ftol (truncation toward zero).
#include <cmath>
#include <cstdint>

// _ftol: truncate toward zero, low 32 bits as the retail EAX.
static int32_t fog_ftol32(double x) { return (int32_t)(int64_t)x; }

// 1.13c Fog+0x104F0: date to day number. EAX = year, EBX = month, stack arg = day, ret 4.
// Year 0 returns 0. Months 1-2 count as months 13-14 of the previous year; the Gregorian correction applies
// from 1582-10-15 (31*(month + 12*year) + day >= 0x8FC1D, a signed compare).
int32_t fog_104f0(int32_t year, int32_t month, int32_t day)
{
    if (year == 0)
        return 0;
    int32_t y = year < 0 ? year + 1 : year;
    int32_t m;
    if (month <= 2) {
        y -= 1;
        m = month + 13;
    } else {
        m = month + 1;
    }
    double yd = (double)y;
    double fm = std::floor((double)m * 30.6001);
    double fy = std::floor(yd * 365.25);
    int32_t r = fog_ftol32(fy + fm + (double)day + 1720995.0);
    int32_t g = (int32_t)(31u * ((uint32_t)month + 12u * (uint32_t)year) + (uint32_t)day);
    if (g >= 0x8FC1D) {
        int32_t a = fog_ftol32(yd * 0.01);
        int32_t b = fog_ftol32((double)a * -0.25);
        r += 2 - b - a;
    }
    return r;
}
