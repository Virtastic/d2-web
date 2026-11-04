// Prior art used: MSVC VC7.1 CRT _fclose_lk; FILE layout +0 _ptr, +4 _cnt, +8 _base, +0xC _flag, +0x10 _file,
// +0x14 _charbuf, +0x18 _bufsiz, +0x1C _tmpfname.
#include <cstdint>

struct D2Lang7741File {
    uint32_t ptr;
    uint32_t cnt;
    uint32_t base;
    uint32_t flag;
    int32_t file;
    uint32_t charbuf;
    uint32_t bufsiz;
    uint32_t tmpfname;
};

// 1.13c D2Lang+0x07741: __fclose_lk(FILE*). A stream that is not open (flag & 0x83 == 0) returns -1. Otherwise:
// flush (D2Lang+0x071EA), free the buffer (D2Lang+0x079B8), close the handle (D2Lang+0x0791D). A failed close
// returns -1 and skips the temp-name free (D2Lang+0x03091); otherwise a nonzero tmpfname is freed and cleared.
// The flag is cleared in every case. Returns the flush result on success.
int32_t d2lang_7741(D2Lang7741File* s,
                    int32_t (*flush)(D2Lang7741File*),
                    void (*freebuf)(D2Lang7741File*),
                    int32_t (*close)(int32_t fd),
                    void (*free_block)(uint32_t p))
{
    int32_t ret = -1;
    if ((s->flag & 0x83) != 0) {
        ret = flush(s);
        freebuf(s);
        if (close(s->file) < 0) {
            ret = -1;
        } else if (s->tmpfname != 0) {
            free_block(s->tmpfname);
            s->tmpfname = 0;
        }
    }
    s->flag = 0;
    return ret;
}
