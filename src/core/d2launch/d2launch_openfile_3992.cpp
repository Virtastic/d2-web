// 1.13c D2Launch+0x3992 __openfile. Parse the fopen mode string into _open flags and FILE flags, __sopen it, init FILE.
#include <cstdint>
struct OpenFileFILE { char* _ptr; int32_t _cnt; char* _base; uint32_t _flag; int32_t _file; int32_t _charbuf; int32_t _bufsiz; char* _tmpfname; };
extern int32_t __sopen(const char* name, uint32_t oflag, int32_t shflag, int32_t pmode);  // 0x6851
extern uint32_t g_fmode_flag;   // DAT_6fa6510c ORed into every stream's _flag
extern int32_t g_stream_count;  // DAT_6fa64f90

extern "C" OpenFileFILE* d2launch_openfile_3992(const char* name, const char* mode, int32_t shflag, OpenFileFILE* f) {
  uint32_t of, fl;
  bool seqRand = false, commit = false;
  switch (*mode) {
    case 'a': of = 0x109; fl = g_fmode_flag | 2; break;
    case 'r': of = 0;     fl = g_fmode_flag | 1; break;
    case 'w': of = 0x301; fl = g_fmode_flag | 2; break;
    default: return nullptr;
  }
  for (++mode; *mode; ++mode) {
    char c = *mode;
    if (c == 'T') { if (of & 0x1000) break; of |= 0x1000; }
    else if (c == '+') { if (of & 2) break; of = (of & ~1u) | 2; fl = (fl & ~3u) | 0x80; }
    else if (c == 'D') { if (of & 0x40) break; of |= 0x40; }
    else if (c == 'R') { if (seqRand) break; seqRand = true; of |= 0x10; }
    else if (c == 'S') { if (seqRand) break; seqRand = true; of |= 0x20; }
    else if (c == 'b') { if (of & 0xc000) break; of |= 0x8000; }
    else if (c == 'c') { if (commit) break; commit = true; fl |= 0x4000; }
    else if (c == 'n') { if (commit) break; commit = true; fl &= ~0x4000u; }
    else if (c == 't') { if (of & 0xc000) break; of |= 0x4000; }
    else break;
  }
  int32_t fd = __sopen(name, of, shflag, 0x1a4);
  if (fd < 0) return nullptr;
  g_stream_count++;
  f->_flag = fl; f->_cnt = 0; f->_ptr = nullptr; f->_base = nullptr; f->_tmpfname = nullptr; f->_file = fd;
  return f;
}
