#include <cstdint>
namespace storm_z9 {
// 1.13c Storm+0x36A50: build a bounding rect + RLE (transparent/opaque run pairs) of an 8-bit image against a key colour.
// rect (ECX) = {left,top,right,bottom} or null; EAX = out bounds {minX,minY,maxX,maxY}. Output stream in `out` (may be null).
void Storm_Sub36A50(int32_t* bounds /*EAX*/, const int32_t* rect /*ECX*/, int32_t stride /*EDX*/, const int8_t* img,
                    int32_t h, int8_t key, int32_t skipScan, uint8_t* out, uint32_t* sizeOut, uint32_t* total) {
    uint32_t cnt = 0;
    // retail: base = rect ? rect[1]*stride + rect[0] : 0, added to the image pointer (param_3)
    const int8_t* p7 = img + (rect ? (rect[1] * stride + rect[0]) : 0);
    int32_t w = stride;
    if (rect) { h = rect[3] - rect[1]; w = rect[2] - rect[0]; }
    bounds[0] = 0x7fffffff; bounds[1] = 0x7fffffff; bounds[2] = 0; bounds[3] = 0;
    uint8_t* o = out;
    if (skipScan == 0) {
        const int8_t* p5 = p7;
        for (int32_t y = 0; y < h; y++) {
            bool any = false;
            for (int32_t xx = 0; xx < w; xx++, p5++) {
                if (*p5 != key) {
                    cnt++;
                    if (o) *o++ = (uint8_t)*p5;
                    if (xx < bounds[0]) bounds[0] = xx;
                    if (bounds[2] <= xx) bounds[2] = xx + 1;
                    any = true;
                }
            }
            if (any) { if (y < bounds[1]) bounds[1] = y; if (bounds[3] <= y) bounds[3] = y + 1; }
            p5 += stride - w;
        }
    }
    if (bounds[2] < bounds[0]) bounds[0] = bounds[2];
    if (bounds[3] < bounds[1]) bounds[1] = bounds[3];
    uint32_t m = cnt & 3;
    if (m) { if (o) o += 4 - m; cnt += 4 - m; }
    if (total) *total = cnt;
    for (int32_t rows = h; rows != 0; rows--) {
        int32_t left = w;
        for (;;) {
            uint8_t a = 0, b = 0; bool flag = true;
            if (left == 0) {  // end-of-row marker
                cnt += 2;
                if (o) { o[0] = 0; o[1] = 0; o += 2; }
                break;
            }
            for (;;) {
                int32_t l1 = left - 1;
                const int8_t* nx = p7 + 1;
                bool match = ((*p7 != key) == flag);
                if (match) {
                    if (!flag) b++; else a++;
                } else {
                    if (!flag) { goto emit; }   // run ended: pixel not consumed
                    flag = false;
                    b++;
                }
                p7 = nx; left = l1;
                if (a == 0xfc || b == 0xfc || l1 == 0) goto emit;
            }
        emit:
            cnt += 2;
            if (o) { o[0] = a; o[1] = b; o += 2; }
        }
        p7 += stride - w;
    }
    if (sizeOut) *sizeOut = cnt;
}
}  // namespace storm_z9
