// src/core/fog/fog_sub_1ca70.cpp -- 1.13c Fog.dll ordinal 10056, reimplemented from
// build-wasm/ghidra-fog/decomp/0x1CA70.c (alpha lane: NAMED, no spec, no vector).
// Fog+0x17F60 (GetSavePath, ordinal 10115) and the two kernel32 disk-space calls it probes by name
// are separate, unported rows: host seams here, injected so a probe can replay them.
#include <cstdint>

namespace {

// Fog+0x17F60: writes the save directory into buf (NUL-terminated, trailing '\\').
void noSavePath(char* buf, uint32_t size) {
    if (size != 0) buf[0] = '\0';
}

// kernel32!GetDiskFreeSpaceExA, present (return 1) on every host retail runs on; writes the three
// 64-bit outputs.
int noDiskFreeSpaceEx(const char* path, uint64_t* freeToCaller, uint64_t* total, uint64_t* totalFree) {
    (void)path;
    *freeToCaller = 0;
    *total = 0;
    *totalFree = 0;
    return 0;
}

// kernel32!GetDiskFreeSpaceA (Win9x fallback), on the drive root.
void noDiskFreeSpace(const char* root, uint32_t* sectorsPerCluster, uint32_t* bytesPerSector,
                     uint32_t* freeClusters, uint32_t* totalClusters) {
    (void)root;
    *sectorsPerCluster = 0;
    *bytesPerSector = 0;
    *freeClusters = 0;
    *totalClusters = 0;
}

void (*g_getSavePath)(char*, uint32_t) = noSavePath;
int (*g_getDiskFreeSpaceEx)(const char*, uint64_t*, uint64_t*, uint64_t*) = noDiskFreeSpaceEx;
void (*g_getDiskFreeSpace)(const char*, uint32_t*, uint32_t*, uint32_t*, uint32_t*) = noDiskFreeSpace;

}  // namespace

// 1.13c Fog+0x1CA70
// Free space, in bytes, of the save directory. GetDiskFreeSpaceExA gives it directly (its first
// output, free bytes available to the caller). On the hosts where that export is missing, the path
// is cut back to its drive root (first '\\') for the Win9x GetDiskFreeSpaceA, whose value is
// freeClusters * bytesPerSector * sectorsPerCluster.
extern "C"
uint64_t fog_sub_1ca70(void) {
    char path[0x400];
    g_getSavePath(path, 0x400);

    uint64_t freeToCaller = 0;
    uint64_t total = 0;
    uint64_t totalFree = 0;
    if (g_getDiskFreeSpaceEx(path, &freeToCaller, &total, &totalFree))
        return freeToCaller;

    for (uint32_t i = 0; i < 0x400; ++i) {
        if (path[i] == '\\') {
            path[i + 1] = '\0';
            break;
        }
    }
    uint32_t sectorsPerCluster = 0;
    uint32_t bytesPerSector = 0;
    uint32_t freeClusters = 0;
    uint32_t totalClusters = 0;
    g_getDiskFreeSpace(path, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters);
    return (uint64_t)freeClusters * bytesPerSector * sectorsPerCluster;
}
