// 1.13c D2Win+0x07B30 -- locate `name` first beside the module, then on each CD-ROM drive root.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Win7b30Env {
    void (*sub_7920)(char* buf);                         // module directory -> buf
    void (*ord503)(char* dst, const char* src, int32_t max); // string append
    void (*ord501)(char* dst, const char* src, int32_t max); // string copy
    int32_t (*ord266)(char* buf, int32_t mode, int32_t a, int32_t b); // nonzero = buf names a usable file
    uint32_t (*getLogicalDriveStrings)(uint32_t n, char* out); // NUL-separated, double-NUL ended
    uint32_t (*getDriveType)(const char* root);
    int32_t (*getVolumeInformation)(const char* root);
};

// 1.13c D2Win+0x07B30 (buf in ESI, name in EBX, a/b on the stack)
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_7b30(char* buf, const char* name, int32_t a, int32_t b, const D2Win7b30Env* e) {
    e->sub_7920(buf);
    e->ord503(buf, "\\", 0x7fffffff);
    e->ord503(buf, name, 0x7fffffff);
    if (e->ord266(buf, 2, a, b)) return 1;
    char drives[0x104];
    uint32_t n = e->getLogicalDriveStrings(0x104, drives);
    if (!n || n > 0x104) return 0;
    while (*name == '\\') name++;
    for (const char* d = drives; *d; d += strlen(d) + 1) {
        if (e->getDriveType(d) != 5 || !e->getVolumeInformation(d)) continue;
        e->ord501(buf, d, 0x7fffffff);
        e->ord503(buf, name, 0x7fffffff);
        if (e->ord266(buf, 2, a, b)) return 1;
    }
    return 0;
}
