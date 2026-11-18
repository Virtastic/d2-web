// 1.13c D2Common+0x2260 strchr: search includes the terminating NUL and narrows the key. the 1.10f source has no twin (host CRT).
#include <cstdint>

// 1.13c D2Common+0x2260
extern "C" char* d2common_strchr_2260(char* text, int32_t value)
{
    const uint8_t key = static_cast<uint8_t>(value);
    for (;;) {
        const uint8_t byte = static_cast<uint8_t>(*text);
        if (byte == key) return text;
        if (byte == 0) return nullptr;
        ++text;
    }
}
