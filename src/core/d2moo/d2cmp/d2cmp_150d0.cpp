// src/core/d2cmp/d2cmp_150d0.cpp -- 1.13c D2CMP.dll 0x150D0, transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2cmp/decomp/0x150D0.c). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/src/D2Comp.cpp (the same tail, calling
// sub_6F8A4B10("invgrey2", 7) and sub_6F8A4B10("invgreybrown", 8)) -- the eight name/value pairs are
// the palette-transform table, registered in this order.
#include <cstdint>

// Host seam for the callee the cached decompile names but src/ does not implement:
// 0x14F20(name, value), the palette-transform registrar.
// TODO(spec): 0x14F20's record layout and failure return are unmapped; this row is the eight calls.
struct D2CMP150d0Env {
    void (*registerTransform)(const char* name, uint32_t value);   // 0x14F20
};

// 1.13c D2CMP+0x150D0 (export #10047, 120 bytes): register the eight palette transforms.
// No arguments and no return value; the whole body is the eight tail-ordered calls below.
// 1.13c D2CMP+0x150D0 (src/core/d2cmp/d2cmp_150d0.cpp:d2cmp_150d0)
extern "C"
void d2cmp_150d0(const D2CMP150d0Env* env) {
    env->registerTransform("grey", 1);
    env->registerTransform("grey2", 2);
    env->registerTransform("gold", 3);
    env->registerTransform("brown", 4);
    env->registerTransform("greybrown", 5);
    env->registerTransform("invgrey", 6);
    env->registerTransform("invgrey2", 7);
    env->registerTransform("invgreybrown", 8);
}
