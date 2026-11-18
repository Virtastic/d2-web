// D2Common's statically linked VS2003 CRT string-stream writers behind _output (D2Common+0x3985).
// Prior art used: none in D2MOO (it does not port the bundled CRT); written from the 1.13c decompile.
// The stream is the logical view of vsprintf_2814.h {cursor, remaining, base, flags}; the _flsbuf path is the
// caller's flushByte (D2Common+0x37DE).
#include "core/d2common/output_3985.h"

// 1.13c D2Common+0x38F7 (_write_char): a string stream (flag 0x40 clear) or one with a base takes the byte.
// When the count runs out the byte goes to _flsbuf, else it is stored at the cursor. A -1 result poisons
// *count; otherwise *count rises by one.
extern "C" void D2Common_38F7_WriteChar(uint8_t b, D2CommonStringStream2814* s,
                                        const D2CommonOutputCalls3985* c, int* count)
{
    if (!(s->flags & 0x40) || s->base)
    {
        uint32_t r;
        if (--s->remaining < 0) r = (uint32_t)c->flushByte((int)(int8_t)b, s);
        else { *s->cursor++ = (char)b; r = b; }
        if (r == 0xffffffffu) { *count = -1; return; }
    }
    ++*count;
}

// 1.13c D2Common+0x392A (_write_multi_char): n copies of one byte, stopping once *count is -1.
extern "C" void D2Common_392A_WriteMultiChar(int* count, uint8_t b, int n, D2CommonStringStream2814* s,
                                             const D2CommonOutputCalls3985* c)
{
    while (n > 0)
    {
        --n;
        D2Common_38F7_WriteChar(b, s, c, count);
        if (*count == -1) return;
    }
}

// 1.13c D2Common+0x394E (_write_string): retail regs eax = &count, ecx = buffer, edi = stream, [esp+4] = length.
// A stream with neither the 0x40 flag clear nor a base only counts the length.
extern "C" void D2Common_394E_WriteString(D2CommonStringStream2814* s, const D2CommonOutputCalls3985* c,
                                          int* count, const char* p, int n)
{
    if (!(s->flags & 0x40) || s->base)
    {
        while (n > 0)
        {
            --n;
            D2Common_38F7_WriteChar((uint8_t)*p++, s, c, count);
            if (*count == -1) return;
        }
    }
    else
    {
        *count += n;
    }
}
