// BNClient 0xBD40: sends BNCS frame 0x27. __fastcall (ECX = name, EDX = count) plus two stack words (first array,
// second array, each count string pointers), ret 8. One Fog block (size = 8 + the strings with their NULs, file
// 0x1AFF0 line 0x4F4) holds {1, count} then the name, every string of the first array, then every string of the
// second; it goes out whole (id 0x27, ECX = bytes used), DAT+0x3EDF4 and DAT+0x3EF74 are cleared, and the block is
// freed (line 0x510). The size pass asks the length of first[i] then second[i] for each i.
#include <cstdint>
#include <emscripten.h>

typedef uint32_t (*Bn27LenFn)(const char* s);                                                  // 1.13c BNClient+0x8042 (lstrlenA)
typedef uint8_t* (*Bn27AllocFn)(uint32_t size, uint32_t line);                                 // 1.13c BNClient+0x80EA
typedef void (*Bn27FreeFn)(uint8_t* block, uint32_t line);                                     // 1.13c BNClient+0x80F0
typedef uint32_t (*Bn27CopyFn)(uint8_t* dst, const char* src, uint32_t max);                   // 1.13c BNClient+0x804E (returns the length copied)
typedef int32_t (*Bn27SendFn)(uint32_t length, uint8_t id, const uint8_t* payload);            // 1.13c BNClient+0xEB00

// 1.13c BNClient+0x0BD40
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_send_account_bd40(const char* name, uint32_t count,
                                                                    const char* const* first, const char* const* second,
                                                                    uint32_t* word_edf4, uint32_t* word_ef74, Bn27LenFn len,
                                                                    Bn27AllocFn alloc, Bn27FreeFn release, Bn27CopyFn copy,
                                                                    Bn27SendFn send) {
    uint32_t total = len(name) + 1;
    for (uint32_t i = 0; i < count; i++) {
        total += len(first[i]) + 1;
        total += len(second[i]) + 1;
    }
    uint8_t* block = alloc(total + 8, 0x4F4);
    uint32_t* head = (uint32_t*)block;
    head[0] = 1; head[1] = count;
    uint8_t* p = block + 8;
    p += copy(p, name, 0x7FFFFFFF) + 1;
    for (uint32_t i = 0; i < count; i++) p += copy(p, first[i], 0x7FFFFFFF) + 1;
    for (uint32_t i = 0; i < count; i++) p += copy(p, second[i], 0x7FFFFFFF) + 1;
    *word_edf4 = 0;
    *word_ef74 = 0;
    int32_t sent = send((uint32_t)(p - block), 0x27, block);
    release(block, 0x510);
    return sent >= 0 ? 1 : 0;
}
