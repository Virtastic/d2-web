// 1.13c D2Game+0x53F10 D2GAME_SAVE_WriteFileToDisk.
#include "d2game_53f10.h"
#include <cstdio>

static uint32_t g_writeSaveFile;    // 0x6FD2D8BC
static uint32_t g_sameSaveCount;    // 0x6FD31F84
uint32_t* D2GAME_SAVE_WriteSaveFileFlag() { return &g_writeSaveFile; }
uint32_t* D2GAME_SAVE_SameSaveCount() { return &g_sameSaveCount; }

// 1.13c D2Game+0x53F10: with file writing on, "<save dir><name>.d2s" gets the 0x2000-byte save stream EC7F0 builds; a failed
// fopen is traced. EC7F0 gets the stack words crossed (s1 in ECX, s0 in EDX) and both trailing words.
void D2GAME_SAVE_WriteFileToDisk_53F10(uint32_t s0, uint32_t s1, const char* name, uint32_t s3, uint32_t s4, const D2SaveDiskEnv* e)
{
    if (g_writeSaveFile == 0) return;
    char dir[0x400];
    e->getSavePath(dir, 0x400);
    char path[0x1F4];
    std::snprintf(path, sizeof path, "%s%s.d2s", dir, name);
    uint8_t buf[0x2000];
    uint32_t size = 0;
    e->createSaveFile(s1, s0, buf, &size, 0x2000, s3, s4);
    if ((int32_t)size <= 0) return;
    void* f = e->fileOpen(path, "wb");
    if (f) { e->fileWrite(buf, size, 1, f); e->fileClose(f); return; }
    e->trace("Unable to open player save file %s", path);
}
