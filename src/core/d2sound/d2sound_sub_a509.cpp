// 1.13c D2sound+0x0A509 -- CRT __fdivrp_sti_st stub (19 bytes): forwards the two x87 operands (ST1, ST0, 80-bit
// each) to the divide-fault handler at 0x9F40 and returns its result. 0x9F40 is a host seam (no port here).
#include <cstdint>

double D2sound_A509(double st1, double st0, double (*handler9F40)(double, double)) {
    return handler9F40(st1, st0);
}
