// 1.13c BNClient+0x07CE7 (76 B). __fclose_lk(FILE*): close one stream under its lock.
// Own file. A pure function of the stream flag word, the temp-name field and the callee results; the
// flush, buffer-free, close and free calls are seams.
#include <cstdint>

struct BnStream {
    uint32_t flag;       // _flag
    int32_t  file;       // _file: the OS handle
    void*    tmpfname;   // _tmpfname: heap name of a temporary file, or null
};

struct BnCloseSeams {
    int32_t (*flush)(BnStream* s);       // __flush (0x7AED)
    void    (*freebuf)(BnStream* s);     // __freebuf (0x7F92)
    int32_t (*close)(int32_t file);      // __close (0x7EF7): negative on failure
    void    (*free_name)(void* p);       // _free (0x3CF4)
};

// Rule: result = -1. If _flag & 0x83 is set: result = flush(); freebuf(); if close(_file) < 0 then
// result = -1, else if _tmpfname is set, free it and clear it. Finally _flag = 0; return result.
extern "C" int32_t bn_fclose_lk_7ce7_seam(BnStream* s, const BnCloseSeams* c) {
    int32_t result = -1;
    if ((s->flag & 0x83u) != 0) {
        result = c->flush(s);
        c->freebuf(s);
        if (c->close(s->file) < 0) {
            result = -1;
        } else if (s->tmpfname != nullptr) {
            c->free_name(s->tmpfname);
            s->tmpfname = nullptr;
        }
    }
    s->flag = 0;
    return result;
}
