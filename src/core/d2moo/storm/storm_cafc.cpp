// CRT wctomb wrapper.
// wctomb(dst, wc): take the thread's locale info; when it is not the global one, refresh it through
// ___updatetlocinfo (Storm+0x6D32); then ___wctomb_mt (Storm+0xCA9C).
#include <cstdint>
typedef uint32_t u32;

struct StormWctombSeam {
  int* errnoPtr;
  int (*wideCharToMultiByte)(u32 cp, const uint16_t* w, char* dst, int dstLen, int* usedDefault);
};
int Storm_CA9C(const StormWctombSeam& s, const u32* lc, char* dst, uint16_t wc);

// 1.13c Storm+0x0CAFC  cdecl _wctomb(dst, wc)
extern "C" int storm_cafc(char* dst, uint16_t wc, const u32* threadLoc, const u32* globalLoc,
                          const u32* (*updatetlocinfo)(), const StormWctombSeam* seam) {
    const u32* lc = threadLoc;
    if (lc != globalLoc) lc = updatetlocinfo();
    return Storm_CA9C(*seam, lc, dst, wc);
}
