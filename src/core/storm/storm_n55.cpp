// Storm exception-unwind thunks: each calls one cleanup routine. The callee is a host seam (retail EH funclets have no wasm analogue).
#include <cstdint>
typedef void (*StormUnwindFn)(void);
// 1.13c Storm+0x419B0 (calls Storm+0x4C20)
extern "C" void Storm_Unwind_419B0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41A10 (calls Storm+0x1080)
extern "C" void Storm_Unwind_41A10(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41AC0 (calls Storm+0x4C20)
extern "C" void Storm_Unwind_41AC0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41DB0 (calls Storm+0x3AC0)
extern "C" void Storm_Unwind_41DB0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41DD0 (calls Storm+0x3A30)
extern "C" void Storm_Unwind_41DD0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41DF0 (calls Storm+0x3A30)
extern "C" void Storm_Unwind_41DF0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41E91 (calls Storm+0x1FA0)
extern "C" void Storm_Unwind_41E91(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41E99 (calls Storm+0x1F10)
extern "C" void Storm_Unwind_41E99(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41EA1 (calls Storm+0x1E90)
extern "C" void Storm_Unwind_41EA1(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41EC0 (calls Storm+0x5150)
extern "C" void Storm_Unwind_41EC0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41EEA (calls Storm+0x3A30)
extern "C" void Storm_Unwind_41EEA(StormUnwindFn cleanup) { cleanup(); }
// 1.13c Storm+0x41F40 (calls Storm+0x1E90)
extern "C" void Storm_Unwind_41F40(StormUnwindFn cleanup) { cleanup(); }
