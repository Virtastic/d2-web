// 1.13c D2Win+0x7B30 -- sub_7B30.
// Stdcall, ret 8: EAX = name, ESI = buf, stack words a and b. Looks for the file beside the module first (#266 mode 2
// with a and b forwarded), then on each CD-ROM drive root with a readable volume (GetDriveTypeA 5, GetVolumeInformationA).
// Win32 calls, the module-directory helper (0x7920) and the Storm thunks are seams.
#include <cstdint>
#include <cstring>

struct D2Win7b30Env {
    void (*sub_7920)(char* buf);                                        // module directory -> buf
    void (*ord503)(char* dst, const char* src, int32_t max);            // string append
    void (*ord501)(char* dst, const char* src, int32_t max);            // string copy
    int32_t (*ord266)(char* buf, int32_t a, int32_t mode, int32_t b);   // nonzero = buf names a usable file
    uint32_t (*getLogicalDriveStrings)(uint32_t n, char* out);          // NUL-separated, double-NUL ended
    uint32_t (*getDriveType)(const char* root);
    int32_t (*getVolumeInformation)(const char* root);
};

extern "C" uint32_t d2win_7B30(const char* name, char* buf, int32_t a, int32_t b, const D2Win7b30Env* e) {
    e->sub_7920(buf);
    e->ord503(buf, "\\", 0x7fffffff);
    e->ord503(buf, name, 0x7fffffff);
    if (e->ord266(buf, a, 2, b)) return 1;
    char drives[0x104];
    uint32_t n = e->getLogicalDriveStrings(0x104, drives);
    if (n == 0 || n > 0x104) return 0;
    while (*name == '\\') name++;
    for (const char* d = drives; *d != '\0'; d += std::strlen(d) + 1) {
        if (e->getDriveType(d) != 5) continue;
        if (!e->getVolumeInformation(d)) continue;
        e->ord501(buf, d, 0x7fffffff);
        e->ord503(buf, name, 0x7fffffff);
        if (e->ord266(buf, a, 2, b)) return 1;
    }
    return 0;
}
