// Alpha batch lane: two SEH unwind funclets, each FUN_6fab1000(*(ebp-0x10)) (operator delete, D2Client+0x1000).
// Same bodies as the CD55E sibling in d2client_batch_cc7ee.cpp; the delete is a host seam.
#include <cstdint>
namespace d2client_batch_cd593 {
typedef uint32_t P;
struct Env { uint32_t (*rd32)(P); void (*f1000)(P); };
// 1.13c D2Client+0xCD593
void Unwind_CD593(const Env& e, P frameSlot) { e.f1000(e.rd32(frameSlot)); }
// 1.13c D2Client+0xCD5C2
void Unwind_CD5C2(const Env& e, P frameSlot) { e.f1000(e.rd32(frameSlot)); }
}
