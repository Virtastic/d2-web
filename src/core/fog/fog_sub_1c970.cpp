// 1.13c Fog+0x1C970 ordinal 10057: zeroes the MEMORYSTATUS fields, sets dwLength = 0x20, then
// hands the struct to GlobalMemoryStatus (seam, supplied by the caller).
#include <cstdint>

typedef struct {
    uint32_t dwLength;
    uint32_t dwMemoryLoad;
    uint32_t dwTotalPhys;
    uint32_t dwAvailPhys;
    uint32_t dwTotalPageFile;
    uint32_t dwAvailPageFile;
    uint32_t dwTotalVirtual;
    uint32_t dwAvailVirtual;
} fog_memory_status;

extern "C"
void fog_sub_1c970(fog_memory_status *p, void (*global_memory_status)(fog_memory_status *)) {
    p->dwLength = 0;
    p->dwMemoryLoad = 0;
    p->dwTotalPhys = 0;
    p->dwAvailPhys = 0;
    p->dwTotalPageFile = 0;
    p->dwAvailPageFile = 0;
    p->dwTotalVirtual = 0;
    p->dwAvailVirtual = 0;
    p->dwLength = 0x20;
    global_memory_status(p);
}
