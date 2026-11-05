// 1.13c D2CMP+0x076FA: statically linked CRT _flushall helper (flush one FILE, then commit it to disk if _IOCOMMIT is set).
// FILE layout (32-bit retail): +0 _ptr, +4 _cnt, +8 _base, +0xC _flag, +0x10 _file.
#include <cstdint>

struct D2cmp76faSeams {
    int32_t (*flush)(void* ctx, uint32_t* file);        // 0x769D __flush
    int32_t (*commit)(void* ctx, uint32_t fd);          // 0x8453 _commit (0 ok, -1 error)
    void* ctx;
};

// 1.13c D2CMP+0x076FA
extern "C" int32_t d2cmp_76fa_run(uint32_t* file, const D2cmp76faSeams* s) {
    if (s->flush(s->ctx, file) != 0) return -1;
    if ((file[3] & 0x4000) != 0) {
        uint32_t r = (uint32_t)s->commit(s->ctx, file[4]);
        return -(int32_t)(r != 0);
    }
    return 0;
}
