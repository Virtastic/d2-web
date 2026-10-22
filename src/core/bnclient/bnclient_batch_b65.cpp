// BNClient.dll alpha-lane batch b65-bnc: three rows transcribed from the cached decompile.
// Skipped: 0xC520/0xC860/0xC960/0xC9E0 (garbled stack decompiles), 0xCDD0/0xD280/0xD320 (direct Win32
// CreateProcess/SetFileTime/LoadLibrary), 0xCE60/0xD610 (garbled ordinal+stack), 0xD610 also huge.
#include <cstdint>
#include <cstring>
#include <initializer_list>

// 1.13c BNClient+0xCF10
// Hidden register input EAX = reference FILETIME (as ref_lo/ref_hi). Opens file; if |write_time - ref|
// < 0x2625A01 (100ns ticks) returns its size, else 0. File ops injected (Ordinal_10107/10109, Win32).
uint32_t bn_sub_CF10(uint32_t ref_lo, uint32_t ref_hi, bool (*open)(), void (*write_time)(uint32_t& lo, uint32_t& hi),
                     uint32_t (*size)(), void (*close)()) {
    if (!open()) return 0;
    uint32_t lo = 3, hi = 0x8000000;  // decompiler's initial values, overwritten by GetFileTime
    write_time(lo, hi);
    uint32_t d = lo - ref_lo;
    int32_t h = (int32_t)(hi - ref_hi - (lo < ref_lo ? 1 : 0));
    if (h < 1) {
        if (h < 0) {
            bool nz = d != 0;
            d = (uint32_t)-(int32_t)d;
            h = -(h + (nz ? 1 : 0));
        }
        if (h == 0 && d < 0x2625a01) {
            uint32_t s = size();
            close();
            return s;
        }
    }
    close();
    return 0;
}

// 1.13c BNClient+0xCFA0
// Hidden register input EAX = bytes already received (done). Receives total-done bytes in <=0x5B4 chunks,
// writes each and stamps the file time. Returns 1 on completion, 0 on open/recv failure or abort flag.
uint32_t bn_sub_CFA0(uint32_t done, uint32_t total, bool report_progress, int32_t* progress_pct,
                     bool (*open)(), void (*seek)(uint32_t), int32_t (*recv)(uint8_t* buf, uint32_t n),
                     void (*write)(const uint8_t* buf, int32_t n), void (*set_time)(), void (*close)(),
                     const int32_t* abort_flag) {
    uint8_t buf[0x5b4];
    if (!open()) return 0;
    if (done != 0) seek(0);
    if (report_progress) *progress_pct = (int32_t)((done * 100u) / total);
    while (done < total) {
        uint32_t n = total - done;
        if (n > 0x5b4) n = 0x5b4;
        int32_t r = recv(buf, n);
        if (r < 1 || *abort_flag != 0) { close(); return 0; }
        write(buf, r);
        set_time();
        done += (uint32_t)r;
        if (report_progress) *progress_pct = (int32_t)((done * 100u) / total);
    }
    close();
    return 1;
}

// 1.13c BNClient+0xD160 (string half only; the file-open tail is a garbled ordinal sequence)
// Hidden input EAX = src. Copies src to dst, cuts at the first '.', then appends 5 bytes
// (DAT_6FF3AFC4 dword + DAT_6FF3AFC8 byte) at the terminator. dst must hold strlen(src)+5.
char* bn_sub_D160(const char* src, char* dst, uint32_t suffix_dword, uint8_t suffix_byte) {
    std::strcpy(dst, src);
    if (char* dot = std::strchr(dst, '.')) *dot = '\0';
    char* end = dst + std::strlen(dst);
    std::memcpy(end, &suffix_dword, 4);
    end[4] = (char)suffix_byte;
    return dst;
}
