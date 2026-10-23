// 1.13c D2Launch+0x3940 __unlock_file. A stream inside the static _iob (DAT_6fa60170 .. 0x6fa603d0, exclusive
// bounds 0x6fa6016f/0x6fa603d1) releases CRT lock (index + 0x10); any other stream owns a critical section at +0x20.
#include <cstdint>
#include <Windows.h>
extern void FUN_6fa43479(int32_t lockNum);   // 0x3479 _unlock
extern uint8_t g_iob[];                      // DAT_6fa60170

extern "C" void d2launch_unlock_file_3940(void* f) {
  uint8_t* p = (uint8_t*)f;
  if (p >= g_iob && p <= g_iob + 0x260) {
    FUN_6fa43479((int32_t)((p - g_iob) >> 5) + 0x10);
    return;
  }
  LeaveCriticalSection((LPCRITICAL_SECTION)(p + 0x20));
}
