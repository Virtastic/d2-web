#pragma once
#include <cstdint>
// Existing D2Win control buffer, type 4. Pointer fields are untouched.
extern "C" {
int32_t d2win_sub_8290(const void* control);
int32_t d2win_sub_82D0(const void* control);
void d2win_sub_8110(void* control, int32_t columns);
}
