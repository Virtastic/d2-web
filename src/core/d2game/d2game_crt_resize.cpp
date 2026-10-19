// MSVC 2003 CRT file-resize helper. OS and per-thread CRT state are explicit seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameCrtResizeEnv {
    uint32_t (*seek)(uint32_t fd, int32_t offset, uint32_t origin); // 0x08097
    uint32_t (*getOsfHandle)(uint32_t fd);                          // 0x0889A
    int32_t  (*setEndOfFile)(uint32_t osHandle);                    // kernel32
    int32_t  (*setMode)(uint32_t fd, int32_t mode);                 // 0x0A131
    int32_t  (*read)(uint32_t fd, void* buffer, uint32_t count);    // 0x056BD
    int32_t* (*errnoPtr)(void);                                     // 0x04E65
    uint32_t* (*doserrnoPtr)(void);                                 // 0x04E6E
    uint32_t (*lastError)(void);                                    // kernel32
};

// 1.13c D2Game+0x09D6A
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_9d6a(
    uint32_t fd, int32_t targetSize, const D2GameCrtResizeEnv* env) {
    uint8_t buffer[0x1000];
    const uint32_t originalPosition = env->seek(fd, 0, 1);
    if (originalPosition == UINT32_MAX) return -1;
    const uint32_t currentSize = env->seek(fd, 0, 2);
    if (currentSize == UINT32_MAX) return -1;

    const int32_t difference = static_cast<int32_t>(static_cast<uint32_t>(targetSize) - currentSize);
    int32_t result = 0;
    if (difference < 0) {
        (void)env->seek(fd, targetSize, 0);
        const uint32_t osHandle = env->getOsfHandle(fd);
        if (env->setEndOfFile(osHandle) == 0) {
            result = -1;
            *env->errnoPtr() = 13;
            *env->doserrnoPtr() = env->lastError();
        }
    } else if (difference > 0) {
        std::memset(buffer, 0, sizeof(buffer));
        const int32_t savedMode = env->setMode(fd, 0x8000);
        uint32_t remaining = static_cast<uint32_t>(difference);
        while (remaining != 0) {
            const uint32_t amount = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
            const int32_t readCount = env->read(fd, buffer, amount);
            if (readCount == -1) {
                if (*env->doserrnoPtr() == 5) *env->errnoPtr() = 13;
                result = -1;
                break;
            }
            remaining -= static_cast<uint32_t>(readCount);
        }
        (void)env->setMode(fd, savedMode);
    }
    (void)env->seek(fd, static_cast<int32_t>(originalPosition), 0);
    return result;
}
