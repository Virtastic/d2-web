// src/core/d2sound/d2sound_8a50.cpp -- 1.13c D2sound+0x8A50
// stdcall(out0, out1, out2) with EBX = the file name (ret 0xC): the file-version query. Every callee is the caller's seam (the retail calls):
//   q84a0(ecx = buffer or 0, name, &size)         0x84A0, nonzero = read
//   alloc(size, line) / release(ptr, line)        Fog#10042 / #10043 via 0x5F62 / 0x5F68 (fastcall: ecx, edx = source file, stack: line, 0)
//   verQuery(buf, "\\", &fixed, &len)              VERSION!VerQueryValueA via 0x5F14
//   getLastError() / setLastError(v)               kernel32
// Flow: q84a0(0, name, &size) must fail (zero) with GetLastError == 0x7A (ERROR_INSUFFICIENT_BUFFER) and size != 0, else return 0. SetLastError(0),
// alloc(size, 0x21B), a second size cell holding the first size, q84a0(buf, name, &size2) (zero: release(buf, 0x220), return 0), verQuery
// (zero: release(buf, 0x228), return 0). Success outputs the VS_FIXEDFILEINFO words +10, +8, +0xC (16 bits each) and returns 1; the buffer is not freed.
#include <cstdint>

struct D2snd8A50Env {
    uint32_t (*q84a0)(uint32_t buf, uint32_t name, uint32_t* size);
    uint32_t (*alloc)(uint32_t size, uint32_t line);
    void (*release)(uint32_t ptr, uint32_t line);
    uint32_t (*verQuery)(uint32_t buf, const char* sub, uint32_t* fixed, uint32_t* len);
    uint32_t (*getLastError)(void);
    void (*setLastError)(uint32_t);
};

// 1.13c D2sound+0x8A50
uint32_t D2sound_8A50(uint32_t name, uint32_t* out0, uint32_t* out1, uint32_t* out2, const D2snd8A50Env* e) {
    uint32_t size = 0;
    if (e->q84a0(0, name, &size)) return 0;
    if (e->getLastError() != 0x7A) return 0;
    e->setLastError(0);
    if (!size) return 0;
    const uint32_t buf = e->alloc(size, 0x21B);
    uint32_t size2 = size;
    if (!e->q84a0(buf, name, &size2)) { e->release(buf, 0x220); return 0; }
    uint32_t fixed = 0, len = 0;
    if (!e->verQuery(buf, "\\", &fixed, &len)) { e->release(buf, 0x228); return 0; }
    const uint8_t* f = (const uint8_t*)(uintptr_t)fixed;
    *out0 = (uint32_t)f[10] | (uint32_t)f[11] << 8;
    *out1 = (uint32_t)f[8] | (uint32_t)f[9] << 8;
    *out2 = (uint32_t)f[12] | (uint32_t)f[13] << 8;
    return 1;
}
