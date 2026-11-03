// 1.13c D2Common+0x5D770 -- write one DATA\GLOBAL\EXCEL file: the path is "DATA\GLOBAL\EXCEL\<name>", opened "wb";
// a 4-byte header word (the function's fourth stack argument) then the payload are written; the file is closed.
// Seams: the open, write and close calls (the CRT wrappers at 0x52043, 0x215D and 0x21F5). The path is composed
// with the same "%s\%s" format retail passes to wsprintfA.
#include <cstdint>
#include <cstdio>

struct D2Common5D770Env {
    void* (*open)(const char* path, const char* mode);
    void  (*write)(const void* p, uint32_t size, uint32_t count, void* f);
    void  (*close)(void* f);
};

extern "C" void D2Common_5D770_WriteExcel(const char* szName, const void* pBuf, uint32_t nLen,
    uint32_t nHdr, const D2Common5D770Env* env)
{
    char path[260];
    std::snprintf(path, sizeof path, "%s\\%s", "DATA\\GLOBAL\\EXCEL", szName);
    void* f = env->open(path, "wb");
    if (f != nullptr) {
        env->write(&nHdr, 4, 1, f);
        env->write(pBuf, nLen, 1, f);
        env->close(f);
    }
}
