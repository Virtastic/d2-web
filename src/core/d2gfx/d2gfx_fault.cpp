// 1.13c D2Gfx+0x79C0: the fault-message hook chain 0x7AA0 reports through.
// __stdcall(ret 4): the fault record is the one stack word, dest and size are the caller's EBX/EDI live-ins.
// The DLL's data segment is not ported, so the three globals it reads come in as parameters and so does the
// handler the walk calls: a node's next is the dword at base+node+4, its handler the dword at node+8, and a
// non-zero return consumes the message. The CRITICAL_SECTION around the walk is not modelled -- nothing
// outside D2Gfx can observe it. Reads only 32-bit dwords, the exact layout retail walks.
#include <cstdint>
#include <cstddef>

// Storm #501 SStrCopy, the compat definition (src/core/d2moo/compat/stubs.cpp). Declared here rather than
// pulled in through compat/Storm.h: that header is the vendored squall one and needs the D2Dll.h
// __declspec machinery, which the plain src/core TUs do not enable.
size_t SStrCopy(char* dest, const char* src, size_t destsize);

extern "C" void d2gfx_sub_79c0(uint32_t enabled, uint32_t base, uint32_t head, uint32_t faultRec,
                               uint8_t* dest, uint32_t size,
                               uint32_t (*handler)(uint32_t faultRec, uint8_t* dest, uint32_t size)) {
    if (enabled) {
        for (uint32_t n = (int32_t)head > 0 ? head : 0; (int32_t)n > 0; n = *(const uint32_t*)(uintptr_t)(base + n + 4))
            if (handler(faultRec, dest, size)) return;
    }
    SStrCopy((char*)dest, "D2Hell Fault.cpp error.", size);
}
