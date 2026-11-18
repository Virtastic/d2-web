// 1.13c D2Win+0x11450 (size 78, caller 0x116A0): destroy a type-9 control record (Fog seams: 10265/10024 fatal, 10052 release).
#include <cstdint>
struct D2WinDestroyEnv {
  void (*fatal)(uint32_t id);              // Fog #10265(100) -> #10024("", it), _exit(-1); does not return
  void (*release)(void* p);                // Fog #10052(param_1 + 0x50 dwords)
  int (*deleteFile)(const char* path);     // DeleteFileA(param_1 + 0x10 dwords)
  uint32_t keepFile;                       // DAT_6f9a9e20
};
// 1.13c D2Win+0x11450
uint32_t d2win_11450(D2WinDestroyEnv& e, int* p) {
  if (*p != 9) e.fatal(100);
  e.release(p + 0x50);
  if (e.keepFile == 0) e.deleteFile((const char*)(p + 0x10));
  return 1;
}
