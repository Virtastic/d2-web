// 1.13c D2Game+0x53F10 D2GAME_SAVE_WriteFileToDisk and +0x54000 D2GAME_SAVE_WriteFileOnRealm
// Retail-layout rules: words are retail's 32-bit values, the callees are seams.
#pragma once
#include <cstdint>

// 0x6FD2D8BC gbWriteSaveFile and 0x6FD31F84 (realm saves skipped as unchanged): the module's save state.
uint32_t* D2GAME_SAVE_WriteSaveFileFlag();
uint32_t* D2GAME_SAVE_SameSaveCount();

struct D2SaveDiskEnv {
    void  (*getSavePath)(char* dir, uint32_t cap);                               // 0xA316 Fog #10115 (ECX dir, EDX cap)
    void  (*createSaveFile)(uint32_t ecx, uint32_t edx, uint8_t* out, uint32_t* size, uint32_t cap, uint32_t s3, uint32_t s4);   // 0xEC7F0
    void* (*fileOpen)(const char* path, const char* mode);                     // 0x2FD0 fopen
    void  (*fileWrite)(const void* p, uint32_t size, uint32_t n, void* f);     // 0x321F fwrite
    void  (*fileClose)(void* f);                                               // 0x32B7 fclose
    void  (*trace)(const char* fmt, const char* arg);                          // 0xA31C
};
// stdcall, 5 stack words: s0 goes to EC7F0's EDX, s1 to its ECX, s2 = character name, s3/s4 are EC7F0's last two words.
void D2GAME_SAVE_WriteFileToDisk_53F10(uint32_t s0, uint32_t s1, const char* name, uint32_t s3, uint32_t s4, const D2SaveDiskEnv* e);

struct D2SaveRealmEnv {
    void     (*createSaveFile)(uint32_t ecx, uint32_t edx, uint8_t* out, uint32_t* size, uint32_t cap, uint32_t s3, uint32_t s4);   // 0xEC7F0
    uint8_t* (*playerData)(uint8_t* unit);                                     // 0xA4C0 D2Common #10920
    uint32_t (*checksum)(const uint8_t* data, uint32_t size);                  // 0x53170 (ESI data, stack size)
    uint8_t* (*oldSave)(uint8_t* client);                                      // 0x49790 (EAX client)
    uint32_t (*oldSaveSize)(uint8_t* client);                                  // 0x49760 (EAX client)
    void     (*attachSave)(uint8_t* client, uint32_t size, const uint8_t* data);   // 0x496B0 (ECX client, EAX size, stack data)
    uint8_t* (*callbackTable)();                                               // [0x6FD31C3C]
    int32_t  (*isBadCodePtr)(uint32_t fn);                                     // IAT IsBadCodePtr
    void     (*relock)(uint32_t fn, uint32_t* s5, uint32_t s1, uint32_t s2);   // [tbl+0x38](ECX &s5, EDX s1, stack s2)
    void     (*save)(uint32_t fn, uint32_t* s5, uint32_t s1, uint32_t s2, const uint8_t* stream, uint32_t len, uint32_t s3);   // [tbl+0xC]
    void     (*fatal)(uint32_t line);                                          // Fog assert (does not return in retail)
};
// ECX = c, EDX = player unit, 6 stack words s0..s5 (ret 0x18); returns 1 when the save went to the realm callback.
uint32_t D2GAME_SAVE_WriteFileOnRealm_54000(uint32_t c, uint8_t* player, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
                                           uint32_t s5, const D2SaveRealmEnv* e);
