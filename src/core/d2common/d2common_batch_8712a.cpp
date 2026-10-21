// D2Common 1.13c batch: MSVC 2003 CRT __forcdecpt (printf %#g helper).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Unwired CRT callees: D2Common+0x2AEC _tolower, +0x90F0 _isdigit (both cdecl int->int).
__attribute__((weak)) int d2common_crt_tolower_2aec(int c);
__attribute__((weak)) int d2common_crt_isdigit_90f0(int c);
// DAT_6fde9964: the locale decimal-point char (1 byte).
__attribute__((weak)) char d2common_dat_6fde9964[1];

// 1.13c D2Common+0x8712A: insert a decimal point after the first digit run of a "%e"-style buffer.
// Oddity kept: the first test is tolower(*buf)=='e'; thereafter the scan advances while the
// NEXT char is NOT a digit-test passing... i.e. it stops once isdigit()==0 (loop is do-while-shaped).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_8712a(intptr_t buf) {
 char* p = reinterpret_cast<char*>(buf);
 bool stop = d2common_crt_tolower_2aec(*p) == 0x65;
 while (!stop) {
  ++p;
  stop = d2common_crt_isdigit_90f0(*p) == 0;
 }
 char carry = *p;
 *p = d2common_dat_6fde9964[0];
 char nx;
 do {
  ++p;
  nx = *p;
  *p = carry;
  carry = nx;
 } while (*p != '\0');
}
}
