#include <cstdint>

// 1.13c D2Common+0x021A9 (__fclose_lk): MSVC 2003 CRT, caller holds the stream lock. Only a stream with
// any of the 0x83 flags (_IOREAD|_IOWRT|_IORW) closes: flush (0x286B), free the buffer (0x4D9C), close the
// fd (retail _close); a negative close forces -1, otherwise a non-null _tmpfname is freed (0x4C0D) and
// cleared. _flag is cleared on every path; returns the flush result or -1.
// ponytail: D2Common_21F5_FClose uses host std::fclose, which subsumes this; the CRT seams are passed in.
struct D2CrtFile { uint32_t _ptr, _cnt, _base, _flag, _file, _charbuf, _bufsiz, _tmpfname; };
static_assert(sizeof(D2CrtFile) == 0x20, "FILE");
struct D2CrtFcloseEnv {
    int32_t (*flush)(D2CrtFile*);
    void (*freebuf)(D2CrtFile*);
    int32_t (*close_)(int32_t);
    void (*free_)(uint32_t);
};
extern "C" int32_t D2Common_21A9_FcloseLk(D2CrtFile* f, const D2CrtFcloseEnv* env)
{
    int32_t rc = -1;
    if ((f->_flag & 0x83) != 0) {
        rc = env->flush(f);
        env->freebuf(f);
        if (env->close_((int32_t)f->_file) < 0) {
            rc = -1;
        } else if (f->_tmpfname != 0) {
            env->free_(f->_tmpfname);
            f->_tmpfname = 0;
        }
    }
    f->_flag = 0;
    return rc;
}
