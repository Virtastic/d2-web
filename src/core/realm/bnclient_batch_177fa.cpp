// BNClient 1.13c Stage A: CRT exception links, signal lookup, and BNCS state leaves.
// Prior art used: network schemas; D2MOO D2MCPClient is a stub.
// Cached decompiles cross-checked with retail x86 for BE60 (ECX/EDX payload),
// 183D2 (EDX table, ESI signal, EAX result), and the SEH registration layout.
#include <cstdint>
#include <cstdlib>
#include <cstddef>
namespace d2::bnclient::leaves {
using U32=std::uint32_t;
struct FrameInfo { void* object; FrameInfo* next; }; // native links, retail offsets 0/4
struct ThreadState { FrameInfo* current_exception; void (*terminate_handler)(); };
// 1.13c BNClient+0x177FA -- frame on stack, exception object on stack.
FrameInfo* create_frame(ThreadState& thread, FrameInfo* frame, void* object) {
    frame->object=object;
    frame->next=thread.current_exception;
    thread.current_exception=frame;
    return frame;
}
// 1.13c BNClient+0x17822 -- protect objects still owned by any live frame.
int destroyable(const ThreadState& thread, const void* object) {
    for (auto* frame=thread.current_exception;frame;frame=frame->next)
        if (frame->object==object) return 0;
    return 1;
}
struct SehScope { std::int32_t unused[3]; std::int32_t level; };
static_assert(offsetof(SehScope,level)==12);
struct SehRegistration { SehRegistration* previous; const void* handler;
                         std::int32_t level; const SehScope* scope; };
// 1.13c BNClient+0x058DA -- FS:0 registration, handler identity 0x5850.
int abnormal_termination(const SehRegistration& registration, const void* finally_handler) {
    return registration.handler==finally_handler && registration.level==registration.scope->level;
}
// 1.13c BNClient+0x18244 -- x86 SEH filter returns 1: handler faults fall
// through to abort. Native C++ exceptions use the corresponding catch boundary.
[[noreturn]] void terminate(ThreadState& thread, void (*abort_import)()) {
    try { if (thread.terminate_handler) thread.terminate_handler(); } catch (...) {}
    abort_import();
    std::abort(); // imported retail abort never returns
}
// 1.13c BNClient+0x18279 -- the inconsistency hook is distinct from the thread handler.
[[noreturn]] void inconsistency(ThreadState& thread, void (*inconsistency_handler)(),
                               void (*abort_import)()) {
    try { if (inconsistency_handler) inconsistency_handler(); } catch (...) {}
    terminate(thread,abort_import);
}
struct SignalAction { std::int32_t exception; std::int32_t signal; U32 action; };
static_assert(sizeof(SignalAction)==12);
// 1.13c BNClient+0x183D2 -- count is DAT_6ff3d55c, table EDX, signal ESI.
SignalAction* signal_lookup(SignalAction* table, U32 count, std::int32_t signal) {
    // Retail probes the first entry even for zero count; a zero count cannot
    // return it because the final bounds check fails.
    U32 index=0;
    do { if (table[index].signal==signal) break; ++index; } while(index<count);
    return index<count && table[index].signal==signal ? table+index : nullptr;
}
// 1.13c BNClient+0x062F0 -- code page arrives in EAX, result is a language ID.
U32 language_for_codepage(U32 codepage) {
    switch(codepage) { case 0x3a4:return 0x411; case 0x3a8:return 0x804;
        case 0x3b5:return 0x412; case 0x3b6:return 0x404; default:return 0; }
}
struct SessionState {
    U32 f1d8, f1f4, f508;
    std::uint8_t f670, f671;
    void* lock_f1dc; void* lock_f8e8;
};
struct Locks { void (*enter)(void*); void (*leave)(void*); };
// 1.13c BNClient+0x0FC40 -- DAT_6ff3f1d8 assignment under lock f1dc.
U32 set_f1d8(SessionState& state,const Locks& locks,U32 value) {
    locks.enter(state.lock_f1dc); state.f1d8=value; locks.leave(state.lock_f1dc); return 1;
}
// 1.13c BNClient+0x0FC70 -- DAT_6ff3f1f4 assignment under the SAME lock.
U32 set_f1f4(SessionState& state,const Locks& locks,U32 value) {
    locks.enter(state.lock_f1dc); state.f1f4=value; locks.leave(state.lock_f1dc); return 1;
}
// 1.13c BNClient+0x0FCF0 -- first nonzero status wins, test occurs inside lock.
U32 set_f1f4_if_zero(SessionState& state,const Locks& locks,U32 value) {
    locks.enter(state.lock_f1dc); if (!state.f1f4) state.f1f4=value;
    locks.leave(state.lock_f1dc); return 1;
}
// 1.13c BNClient+0x0D2F0 -- clear two bytes before installing f508 under f8e8.
void reset_request(SessionState& state,const Locks& locks,U32 value) {
    locks.enter(state.lock_f8e8); state.f670=0; state.f671=0; state.f508=value;
    locks.leave(state.lock_f8e8);
}
using SendPacket=U32 (*)(U32,std::uint8_t,const void*);
// 1.13c BNClient+0x0BE60 -- ECX value, EDX boolean; EB00 receives eight bytes
// through EDI, ECX length 8, and stack ID 0x16. Send result is discarded.
U32 send_start_advertisement(U32 value,U32 flag,SendPacket send_eb00) {
    const U32 payload[2]={value,flag!=0 ? 1u:0u};
    send_eb00(8,0x16,payload); return 1;
}
} // namespace d2::bnclient::leaves
