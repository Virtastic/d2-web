// 1.13c D2Win+0x7970 -- size check after a file read.
// Register args: ESI = handle, EDI = expected size. Retail: handle 0 is fatal (0x87); otherwise Ordinal_10104 (0x78B6)
// runs with ECX = handle and five stack words (EDI, &local, 0, 0, 0). It returns a status in eax and writes the size
// through &local. Status 0 is fatal (0x8f); size != expect is fatal (0x92); success returns the status in eax.
// Every failure is fatal, so fatal() is a seam the caller supplies.
#include <cstdint>

// read(handle, expect, &size) is retail's Ordinal_10104 thunk (0x78B6): status in eax, the file size through the out-pointer.
extern "C" uint32_t d2win_7970(uint32_t handle, uint32_t expect,
                               uint32_t (*read)(uint32_t handle, uint32_t expect, uint32_t* size), void (*fatal)(uint32_t id)) {
    if (handle == 0) { fatal(0x87); return 0; }
    uint32_t size = 0;
    uint32_t status = read(handle, expect, &size);
    if (status == 0) { fatal(0x8f); return 0; }        // retail also calls Ordinal_276 before this fatal
    if (size != expect) { fatal(0x92); return 0; }
    return status;
}
