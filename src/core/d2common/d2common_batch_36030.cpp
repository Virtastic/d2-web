// D2Common 1.13c batch: dynamic path allocation/initialisation. Retail 32-bit pointers are read as uint32_t words.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"

extern "C" {
__attribute__((weak)) void* Fog_Alloc10045(const char* file, int32_t line, int32_t zero);
__attribute__((weak)) void Fog_Fatal10265_10024(int32_t msg);   // 10265 + 10024 then _exit(-1)
uint32_t D2Common_35100_MonsterPathType(struct D2UnitStrc* unit); // now src/core/d2common/d2common_35100.cpp
__attribute__((weak)) void D2Common_359D0(void* unit);
__attribute__((weak)) int32_t D2Common_35360(void* unit, int32_t size);
__attribute__((weak)) void D2Common_34F80(void* path, int32_t type);
__attribute__((weak)) int32_t d2_sub_30F80(const int32_t* unit);
__attribute__((weak)) uint32_t D2Common_66800(void* unit, uint32_t room);

}
