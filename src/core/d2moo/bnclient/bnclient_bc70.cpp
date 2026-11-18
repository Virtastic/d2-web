// BNClient 0xBC70: sends BNCS frame 0x26. __fastcall (ECX = name, EDX = count) plus two stack words (extra, array
// of count string pointers), ret 8. One Fog block (size = 12 + the strings with their NULs, file 0x1AFF0 line 0x536)
// holds {1, count, extra} then the name and each array string, NUL separated; it goes out as the whole payload
// (id 0x26, ECX = bytes used), DAT+0x3EDF0 and DAT+0x3EF70 are cleared, and the block is freed (line 0x551).
#include <cstdint>
#include <emscripten.h>

typedef uint32_t (*Bn26LenFn)(const char* s);                                                  // 1.13c BNClient+0x8042 (lstrlenA)
typedef uint8_t* (*Bn26AllocFn)(uint32_t size, uint32_t line);                                 // 1.13c BNClient+0x80EA
typedef void (*Bn26FreeFn)(uint8_t* block, uint32_t line);                                     // 1.13c BNClient+0x80F0
typedef uint32_t (*Bn26CopyFn)(uint8_t* dst, const char* src, uint32_t max);                   // 1.13c BNClient+0x804E (returns the length copied)
typedef int32_t (*Bn26SendFn)(uint32_t length, uint8_t id, const uint8_t* payload);            // 1.13c BNClient+0xEB00

// 1.13c BNClient+0x0BC70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_account_bc70(const char* name, uint32_t count, uint32_t extra,
                                                                    const char* const* list, uint32_t* word_edf0,
                                                                    uint32_t* word_ef70, Bn26LenFn len, Bn26AllocFn alloc,
                                                                    Bn26FreeFn release, Bn26CopyFn copy, Bn26SendFn send) {
    uint32_t total = len(name) + 1;
    for (uint32_t i = 0; i < count; i++) total += len(list[i]) + 1;
    uint8_t* block = alloc(total + 12, 0x536);
    uint32_t* head = (uint32_t*)block;
    head[0] = 1; head[1] = count; head[2] = extra;
    uint8_t* p = block + 12;
    p += copy(p, name, 0x7FFFFFFF) + 1;
    for (uint32_t i = 0; i < count; i++) p += copy(p, list[i], 0x7FFFFFFF) + 1;
    *word_edf0 = 0;
    *word_ef70 = 0;
    int32_t sent = send((uint32_t)(p - block), 0x26, block);
    release(block, 0x551);
    return sent >= 0 ? 1 : 0;
}
