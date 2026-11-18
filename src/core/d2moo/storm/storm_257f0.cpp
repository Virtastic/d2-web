// 1.13c Storm+0x257F0 -- tokenize a command line and hand each token to 0x25710.
#include "storm_batch_b144a.h"

void Storm_Sub3B3A0(char** str, char* out, char* size, const char* delims, uint32_t* quoted);  // 0x3B3A0 SStrTokenize

namespace storm_b144a {
bool tokenize(Ctx& c, char** cursor, int32_t* st, uint32_t* cur, void* cb, void* errCb) {
    char buf[0x100];
    while (**cursor) {
        char* p = *cursor;
        uint32_t quoted = 0;
        Storm_Sub3B3A0(&p, buf, (char*)0x100, " ,;\"\t\n\r\x1a", &quoted);
        if (buf[0] && !handleToken(c, buf, cur, st, quoted, cb, errCb)) break;
        *cursor = p;
    }
    return **cursor == 0;
}
}
