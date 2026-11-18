// 1.13c D2Win+0x18BF0 -- cdecl, one argument = pointer to a double: returns 0 when the value is below the module constant 0.0 (or not a number), 1 otherwise.
#include <cstdint>
uint32_t d2win_18bf0(const double* v) { return *v < 0.0 || *v != *v ? 0u : 1u; }
