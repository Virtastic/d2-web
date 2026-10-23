// 1.13c Fog+0x24330: MSVC CRT x87 math-error thunk. Spills its register args into a frame and calls the
// CRT matherr dispatcher (Fog+0x247AA), then returns ST0 unchanged (the pre-call value).
extern "C" double fog_sub_24330(double st0) {  // 1.13c Fog+0x24330
    return st0;  // the dispatcher only reports the error; it never alters the returned ST0
}
