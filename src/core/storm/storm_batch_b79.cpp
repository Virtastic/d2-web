// Storm implementation batch b79 (decompile-derived). Register-arg hidden inputs are leading params.
#include <cstdint>
#include <cstddef>
#include <initializer_list>
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
int FUN_6fc15710(uint32_t*, int*, int, void*, void*) __attribute__((weak));
void FUN_6fc2b3a0(int*, char*, char*, const char*, int*) __attribute__((weak));

// 1.13c Storm+0x1A5A0 (EAX=in_EAX block header, ESI=out_a)
EMSCRIPTEN_KEEPALIVE void FUN_6fc0a5a0(uint16_t* in_EAX, int* unaff_ESI, uint32_t* self, int param_1) {
  if ((((uint8_t*)in_EAX)[3] & 4) != 0) {
    *unaff_ESI = *(int*)(intptr_t)(param_1 - 0x10);
    *self = (uint8_t)in_EAX[1] + 0x1c;
    return;
  }
  uint16_t v = in_EAX[1];
  *self = (uint8_t)v + 8;
  if ((((uint8_t*)in_EAX)[3] & 1) != 0) *self = (uint8_t)v + 10;
  *unaff_ESI = (uint32_t)*in_EAX - *self;
}

// 1.13c Storm+0x257F0
EMSCRIPTEN_KEEPALIVE bool FUN_6fc157f0(char** param_1, int* param_2, uint32_t* param_3, void* param_4, void* param_5) {
  char c = **param_1;
  while (c != '\0') {
    char* cur = *param_1;
    int n = 0;
    char buf[256];
    FUN_6fc2b3a0((int*)&cur, buf, (char*)0x100, " ,;\"\t\n\r\x1a", &n);
    if (buf[0] != '\0' && FUN_6fc15710(param_3, param_2, n, param_4, param_5) == 0) break;
    *param_1 = cur;
    c = *cur;
  }
  return **param_1 == '\0';
}
}
