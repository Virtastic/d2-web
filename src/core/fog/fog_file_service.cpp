// Prior art: D2MOO Fog/include/Fog.h export contracts, PlugY D2Funcs.h,
// and the existing layered MpqVfs/StormLib implementation. Retail bodies forward
// directly to Storm. Keep native handle ownership in the same VFS used at boot.
#include "fog_file_service.h"
#include <StormLib.h>
#include "../../assets/mpqvfs.h"
namespace d2 { MpqVfs& sharedVfs(); }

// 1.13c Fog+0x17E40
extern "C" uint32_t FOG_FOpenFile(const char* name, void** file) {
    return d2::sharedVfs().openFile(name, file);
}

// 1.13c Fog+0x17E30
extern "C" uint32_t FOG_FCloseFile(void* file) {
    // Storm #253 (0x26E20) returns 1 for null after its sentinel-list path.
    // Foreign/stale non-null pointers remain rejected by our native owner; retail
    // dereferences them and provides no equivalent invalid-handle contract.
    return !file || d2::sharedVfs().closeFile(file);
}

// 1.13c Fog+0x17DF0
extern "C" uint32_t FOG_FGetFileSize(void* file, uint32_t* high) {
    return d2::sharedVfs().fileSize(file, high);
}

// Fog 0x17E00 (#10104) is deliberately not mapped here: its Storm #289
// target accepts asynchronous OVERLAPPED/request/event parameters, swapping the
// final two Fog words. MpqVfs::readFile implements the synchronous path only.

namespace d2 { uint32_t nativeFileLastError() { return SErrGetLastError(); } }
