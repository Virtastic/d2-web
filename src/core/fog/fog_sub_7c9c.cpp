// 1.13c Fog+0x7C9C: CPU feature probe. Sets two globals: sse2 (cpuid.1 edx bit 26 and
// the OS-support check Fog+0x7C67 passes) and flag2 (sse2 and NOT an AMD K8-class part, i.e. vendor != "AuthenticAMD"
// or family+extfamily != 0xF). Returns 0. Everything hardware is a seam.
#include <cstdint>
#include <cstring>

struct FogCpuEnv {
    int (*has_cpuid)();                              // EFLAGS.ID toggle test
    void (*cpuid0)(uint32_t vendor[3]);              // ebx, edx, ecx (12 bytes "AuthenticAMD" order)
    void (*cpuid1)(uint32_t* eax, uint32_t* edx);    // leaf 1: version, feature edx
    int (*os_sse2_ok)();                             // Fog+0x7C67
};
struct FogCpuOut { int32_t sse2; int32_t flag2; };   // 0x6FFA7130, 0x6FFA712C

extern "C" int fog_sub_7c9c(FogCpuOut* o, const FogCpuEnv* e) {
    uint32_t edx = 0, ver = 0, vendor[3] = {0, 0, 0};
    if (e->has_cpuid()) {
        e->cpuid0(vendor);
        e->cpuid1(&ver, &edx);
    }
    o->sse2 = 0;
    o->flag2 = 0;
    if ((edx & 0x4000000) && e->os_sse2_ok()) {
        o->sse2 = 1;
        uint32_t fam = (ver >> 8) & 0xf;
        uint32_t ext = fam != 0xf ? 0 : (ver >> 20) & 0xff;
        if (strncmp("AuthenticAMD", reinterpret_cast<char*>(vendor), 12) != 0 || ext + fam != 0xf) o->flag2 = 1;
    }
    return 0;
}
