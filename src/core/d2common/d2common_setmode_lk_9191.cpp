#include <cstdint>

// 1.13c D2Common+0x09191 __setmode_lk(fh, mode): MSVC CRT. Sets/clears the text-mode bit (0x80) in
// the osfile flag byte (+4 of the 0x24-byte record, 32 records per table slot); returns the previous
// mode (O_TEXT 0x4000 if bit was set, else O_BINARY 0x8000); other modes: errno=EINVAL(0x16), -1.
// Env seam: osfile table (DAT_6fdf1720) and errno setter (FUN_6fd544bc) are owner-supplied.
struct D2SetModeEnv {
    uint8_t** osfile;
    void (*setErrno)(int);
};

int d2common_setmode_lk_9191(D2SetModeEnv& e, uint32_t fh, int mode)
{
    uint8_t* p = e.osfile[(int32_t)fh >> 5] + 4 + (fh & 0x1f) * 0x24;
    uint8_t old = *p;
    if (mode == 0x8000) {
        *p &= 0x7f;
    } else if (mode == 0x4000) {
        *p |= 0x80;
    } else {
        e.setErrno(0x16);
        return -1;
    }
    return (old & 0x80) ? 0x4000 : 0x8000;
}
