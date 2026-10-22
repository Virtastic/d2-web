// 1.13c D2Win batch b77.
// Skipped: 0xA470 (GetFileAttributesA), 0x1995/0x39C1/0x3C28/0x2D69/0x40C0/0x3A6C/0x693C/0x7B30 (CRT/Win32),
// 0x1077 (ExitProcess), 0x2633 (SEH).
#include <cstdint>
#include <initializer_list>
#define EMSCRIPTEN_KEEPALIVE

extern "C" {
// write_char(ch, stream, pcount) register-passed in retail; explicit params here.
__attribute__((weak)) void d2win_write_char(int32_t* pcount, void* stream);
}

// 1.13c D2Win+0x1B05  _write_string; in_EAX=pcount, unaff_EDI=stream become leading params
extern "C" EMSCRIPTEN_KEEPALIVE void write_string(int32_t* pcount, uint8_t* stream, int32_t n) {
  if ((stream[0xc] & 0x40) == 0 || *(int32_t*)(stream + 8) != 0) {
    do {
      if (n < 1) return;
      n--;
      d2win_write_char(pcount, stream);
    } while (*pcount != -1);
  } else {
    *pcount += n;
  }
}
