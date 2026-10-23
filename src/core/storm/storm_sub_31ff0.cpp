// 1.13c Storm+0x31FF0: copy rows bottom-up (vertical flip) into a bounded buffer.
#include <cstdint>
#include <cstring>

// rows=EAX, dst=EDX; src/dstSize/rowBytes/stride on stack. Returns false (last error 0x57) on bad args.
extern "C"
bool Storm_Sub31FF0(int32_t rows, uint8_t* dst, const uint8_t* src, int32_t dstSize, int32_t rowBytes,
                    int32_t stride, int32_t* lastError) {
    if (!dst || !src || !dstSize || !rowBytes || !stride || !rows) {
        *lastError = 0x57;
        return false;
    }
    const uint8_t* row = src + (rows - 1) * stride;
    uint8_t* p = dst;
    do {
        int32_t n = (int32_t)(dst + dstSize - p);
        if (rowBytes < n) n = rowBytes;
        if (n == 0) return true;
        memcpy(p, row, (size_t)n);
        row -= stride;
        p += n;
    } while (--rows);
    return true;
}
