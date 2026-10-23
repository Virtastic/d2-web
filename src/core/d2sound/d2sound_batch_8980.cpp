// 1.13c D2sound+0x8980 -- WAV container parse. Hidden regs of 0x8790 (data, tag) made explicit.
#include <cstdint>
#include <cstring>
int32_t* D2sound_8790(uint32_t* size, int32_t* data, int32_t tag, uint32_t* outLen);

// 1.13c D2sound+0x8980
// `riff` = file image, *size = its byte count. Checks "RIFF"/"WAVE", finds "fmt " (>=16 bytes), copies it
// into the 18-byte WAVEFORMATEX `wfx` (cbSize=0), then finds "data"; returns its payload, *size = its length.
int32_t* D2sound_8980(int32_t* riff, uint32_t* size, uint16_t* wfx) {
    if (*size < 0x20) return nullptr;
    *size -= 0xc;
    if (riff[0] != 0x46464952 || riff[2] != 0x45564157) return nullptr;
    uint32_t len = 0;
    int32_t* fmt = D2sound_8790(size, riff + 3, 0x20746d66, &len);
    if (!fmt || len < 0x10 || len > *size) return nullptr;
    *size -= len;
    std::memcpy(wfx, fmt, 16);
    wfx[8] = 0;
    int32_t* d = D2sound_8790(size, (int32_t*)((uint8_t*)fmt + len), 0x61746164, &len);
    if (!d || len > *size) return nullptr;
    *size = len;
    return d;
}
