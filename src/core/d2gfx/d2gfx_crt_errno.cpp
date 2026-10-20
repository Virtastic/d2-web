#include <cstdint>

// 1.13c D2Gfx+0x3858 -- MSVC 2003 Release CRT errno getter (__getptd 0x28FE, then &ptd->_terrno).
// Callers ask for the address of the calling thread's errno cell whenever an error path stores an errno;
// the getter itself writes nothing. Returns: a pointer to a thread-local 32-bit int, zero at thread start.
// Ours: a C++ thread_local cell. __getptd's per-thread block (allocated lazily, 0x8C bytes) is not
// ported; the cell is the only part of it this getter exposes.
// _tiddata layout (MSVC 2003): _terrno at +8, _tdoserrno at +0xC. The probe passes the ptd __getptd returned.
extern "C" int* d2gfx_errno_ptd(uint8_t* ptd) { return (int*)(ptd + 8); }
extern "C"
int* d2gfx_errno(void) {
    static thread_local uint8_t ptd[0x8C];
    return d2gfx_errno_ptd(ptd);
}

// 1.13c D2Gfx+0x3861 -- MSVC 2003 Release CRT _doserrno getter (__getptd, then &ptd->_tdoserrno).
// Same shape as 0x3858 one word later in the ptd; returns a pointer to a thread-local 32-bit unsigned cell,
// zero at thread start, and writes nothing itself.
extern "C" uint32_t* d2gfx_doserrno_ptd(uint8_t* ptd) { return (uint32_t*)(ptd + 0xC); }
extern "C"
uint32_t* d2gfx_doserrno(void) {
    static thread_local uint8_t ptd[0x8C];
    return d2gfx_doserrno_ptd(ptd);
}
