// 1.13c D2Launch+0x396F FID_conflict:__lock_file2 / __unlock_file2 (identical bytes, same body).
#include <cstdint>
#include <Windows.h>
extern void FUN_6fa43479(int32_t lockNum);   // 0x3479 _unlock

extern "C" void d2launch_lock_file2_396f(int32_t index, void* f) {
  if (index < 0x14) {
    FUN_6fa43479(index + 0x10);
    return;
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)((uint8_t*)f + 0x20));
}
