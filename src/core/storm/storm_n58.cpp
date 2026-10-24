// Storm exception-unwind thunks (n58): each calls one cleanup routine, a host seam.
#include <cstdint>
typedef void (*StormUnwindFn)(void);
// 1.13c STORM+0x041F55 (calls Storm+0x01F10)
extern "C" void Storm_Unwind_41F55(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x041F6A (calls Storm+0x01FA0)
extern "C" void Storm_Unwind_41F6A(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x041F90 (calls Storm+0x02290)
extern "C" void Storm_Unwind_41F90(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x041FCA (calls Storm+0x02290)
extern "C" void Storm_Unwind_41FCA(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x042020 (calls Storm+0x03820)
extern "C" void Storm_Unwind_42020(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x042044 (calls Storm+0x03AC0)
extern "C" void Storm_Unwind_42044(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x042069 (calls Storm+0x03A30)
extern "C" void Storm_Unwind_42069(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x042080 (calls Storm+0x02740)
extern "C" void Storm_Unwind_42080(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x0420C0 (calls Storm+0x03A30)
extern "C" void Storm_Unwind_420C0(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x0420D2 (calls Storm+0x03820)
extern "C" void Storm_Unwind_420D2(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x0420E7 (calls Storm+0x03AC0)
extern "C" void Storm_Unwind_420E7(StormUnwindFn cleanup) { cleanup(); }
// 1.13c STORM+0x042125 (calls Storm+0x02740)
extern "C" void Storm_Unwind_42125(StormUnwindFn cleanup) { cleanup(); }
