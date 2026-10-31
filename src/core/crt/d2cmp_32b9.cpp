// 1.13c D2CMP+0x32B9 (write_char, VS2003 CRT; same bytes as D2Common+0x38F7, which output_3985.cpp keeps
// static). A string stream (flag 0x40 clear) or one with a base takes the byte: when the count runs
// out the byte goes to _flsbuf, else it is stored at the cursor. A -1 result poisons *count; otherwise
// *count rises by one.
#include "core/d2common/output_3985.h"
// 1.13c D2CMP+0x32B9
extern "C" void d2cmp_32b9(uint8_t b, D2CommonStringStream2814* s, const D2CommonOutputCalls3985* calls,
                           int* count) {
    if (!(s->flags & 0x40) || s->base) {
        int r;
        if (--s->remaining < 0) r = calls->flushByte((int8_t)b, s);
        else { *s->cursor++ = (char)b; r = b; }
        if (r == -1) { *count = -1; return; }
    }
    ++*count;
}
