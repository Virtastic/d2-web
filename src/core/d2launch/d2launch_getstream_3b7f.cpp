// D2Launch 0x3B7F __getstream. Under CRT lock 1 scan the stream table for a free FILE (flag & 0x83 == 0),
// allocating a 0x38-byte record (FILE + CRITICAL_SECTION at +0x20) for the first empty slot; the claimed stream is
// returned locked and reset (file = -1).
#include <cstdint>
#include <cstdlib>
#include <Windows.h>
struct GsFILE { char* _ptr; int32_t _cnt; char* _base; uint32_t _flag; int32_t _file; int32_t _charbuf, _bufsiz; char* _tmpfname; };
extern void __lock(int32_t);                 // 0x43B8
extern void FUN_6fa43c98();                  // 0x3C98 unlock(1)
extern int32_t __mtinitlocknum(int32_t);     // 0x352E
extern void FUN_6fa4396f(int32_t, void*);    // 0x396F lock_file2
extern int32_t ___crtInitCritSecAndSpinCount(void* cs, uint32_t spin);
extern GsFILE** g_streams;                   // DAT_6fa670a0
extern int32_t g_nstream;                    // DAT_6fa680c0

// 1.13c D2Launch+0x3B7F
extern "C" GsFILE* d2launch_getstream_3b7f(void) {
  GsFILE* f = nullptr;
  __lock(1);
  for (int32_t i = 0; i < g_nstream; i++) {
    GsFILE* s = g_streams[i];
    if (!s) {
      g_streams[i] = (GsFILE*)malloc(0x38);
      if (g_streams[i]) {
        if (!___crtInitCritSecAndSpinCount((uint8_t*)g_streams[i] + 0x20, 4000)) { free(g_streams[i]); g_streams[i] = nullptr; }
        else { EnterCriticalSection((LPCRITICAL_SECTION)((uint8_t*)g_streams[i] + 0x20)); f = g_streams[i]; }
      }
      break;
    }
    if ((s->_flag & 0x83) == 0) {
      if (i > 2 && i < 0x14 && __mtinitlocknum(i + 0x10) == 0) break;
      FUN_6fa4396f(i, g_streams[i]);
      f = g_streams[i];
      if ((f->_flag & 0x83) == 0) break;
      FUN_6fa4396f(i, f);
      f = nullptr;   // retail leaves _File set but the loop continues; the next pass reassigns it
    }
  }
  if (f) { f->_cnt = 0; f->_flag = 0; f->_base = nullptr; f->_ptr = nullptr; f->_tmpfname = nullptr; f->_file = -1; }
  FUN_6fa43c98();
  return f;
}
