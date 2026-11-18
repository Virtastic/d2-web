#include <test_seam.h>
#include "GAME/Game113c.h"

#include <cstdarg>
#include <cstdint>
#include <cstdio>

// Retail __flsbuf (D2Game 0x4239): the one-byte flush _sprintf only reaches after writing 0x7FFFFFFF bytes. A string stream has nowhere to flush to.
// Host stand-in, not a 1.13c body.
int __cdecl D2GAME_CRT_flsbuf_6FC24239(int nChar, D2CrtFileStrc* pFile)
{
    (void)nChar;
    (void)pFile;
    return -1;
}

// Retail _output (D2Game 0x43E0): the format engine. Host stand-in over vsprintf, not a 1.13c body: it prints at _ptr and advances _ptr/_cnt by what it wrote.
int __cdecl D2GAME_CRT_output_6FC243E0(D2CrtFileStrc* pFile, const char* szFormat, va_list vaArgs)
{
    const int nWritten = vsprintf(pFile->_ptr, szFormat, vaArgs);
    pFile->_ptr += nWritten;
    pFile->_cnt -= nWritten;
    return nWritten;
}

// 1.13c D2Game+0x2A6D (_sprintf)
// VS2003 unbounded formatted print: the destination FILE is fixed at {_cnt = 0x7FFFFFFF, _flag = 0x42, _base = _ptr = dest}, _output fills it and returns
// the length written, then the terminator goes in (through __flsbuf when the count has run out). A null destination skips the terminator.
int __cdecl D2GAME_CRT_sprintf_6FC22A6D(char* szDest, const char* szFormat, ...)
{
    D2CrtFileStrc tFile;
    tFile._cnt = 0x7FFFFFFF;
    tFile._flag = 0x42;
    tFile._base = szDest;
    tFile._ptr = szDest;

    va_list vaArgs;
    va_start(vaArgs, szFormat);
    const int nResult = D2_CALL(D2GAME_CRT_output_6FC243E0, &tFile, szFormat, vaArgs);
    va_end(vaArgs);

    if (szDest)
    {
        tFile._cnt -= 1;
        if (tFile._cnt < 0)
        {
            D2_CALL(D2GAME_CRT_flsbuf_6FC24239, 0, &tFile);
        }
        else
        {
            *tFile._ptr = '\0';
        }
    }
    return nResult;
}
