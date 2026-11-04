// 1.13c BNClient+0x07B4A (46 B). sub_7B4A(FILE*): per-stream flush and commit step of the CRT flush-all walk.
// Own file. A pure function of the stream flag word and the callee results; __flush and the commit call are seams.
#include <cstdint>

struct BnStream {
    uint32_t flag;   // _flag
    int32_t  file;   // _file: the OS handle the commit call takes
};

struct BnStreamSeams {
    int32_t  (*flush)(BnStream* s);      // __flush (0x7AED): 0 on success
    uint32_t (*commit)(int32_t file);    // 0x7D84: nonzero on success
};

// Rule: if __flush fails, return -1. If _flag has 0x4000, return -(commit(_file) != 0);
// otherwise return 0.
extern "C" int32_t bn_stream_7b4a_seam(BnStream* s, const BnStreamSeams* c) {
    if (c->flush(s) != 0) return -1;
    if ((s->flag & 0x4000u) != 0) return -(int32_t)(c->commit(s->file) != 0);
    return 0;
}
