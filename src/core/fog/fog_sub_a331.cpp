// 1.13c Fog+0x0A331: CRT lock wrapper, calls sub_3C3F(10) (_lock(10)); no args, no return.
// Win32 critical-section lock is a host seam (single-threaded wasm: no-op by default).
extern "C" __attribute__((weak)) void fog_sub_3c3f(int) {}

extern "C" void fog_sub_a331() {  // 1.13c Fog+0x0A331
    fog_sub_3c3f(10);
}
