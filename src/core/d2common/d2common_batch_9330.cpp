#include <cstdint>
#include "../d2moo/include/Path/Step.h"

// Prior art: D2MOO Path/Step.cpp PATH_RecacheRoomIfNeeded and the existing
// D2DDraw CRT mantissa helpers. All four bodies were checked against cached
// 1.13c D2Common decompiles. Native pointers remain native on wasm64.

// 1.13c D2Common+0x09330
char* d2common_strrchr_9330(char* text, int character)
{
    char* last = nullptr;
    const char needle = static_cast<char>(character);
    do
    {
        if (*text == needle)
            last = text;
    } while (*text++ != '\0');
    return last;
}

// 1.13c D2Common+0x87D1E
void d2common_shr_12_87d1e(uint32_t* words)
{
    const uint32_t middle = words[1];
    words[1] = (middle >> 1) | (words[2] << 31);
    words[2] >>= 1;
    words[0] = (words[0] >> 1) | (middle << 31);
}

// 1.13c D2Common+0x87CF0
void d2common_shl_12_87cf0(uint32_t* words)
{
    const uint32_t low = words[0];
    const uint32_t middle = words[1];
    words[0] = low << 1;
    words[1] = (middle << 1) | (low >> 31);
    words[2] = (words[2] << 1) | (middle >> 31);
}

// 1.13c D2Common+0x0DB40
void __stdcall d2common_recache_center_db40(D2DynamicPathStrc* path)
{
    PATH_RecacheRoomAtCoordIfNeeded(path, nullptr,
        PATH_FP16FitToCenter(path->tGameCoords.dwPrecisionX),
        PATH_FP16FitToCenter(path->tGameCoords.dwPrecisionY));
}
