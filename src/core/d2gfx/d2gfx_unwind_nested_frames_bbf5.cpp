#include "d2gfx_eh_runtime.h"

thread_local D2GfxEhThreadState d2gfx_eh_thread_state;

// Prior art: D2MOO Fog/Exceptions.cpp has no implementation of this MSVC helper.
// Cross-check: D2Game's corrected EH3 projection preserves native registration
// addresses; no D2Game addresses or EH3 scope layout apply to this EH helper.
// 1.13c D2Gfx+0x0BBF5 (_UnwindNestedFrames, 82 bytes).
extern "C" void d2gfx_unwind_nested_frames_bbf5(
    D2GfxEhRegistration* target_frame, D2GfxEhExceptionRecord* exception_record) {
    D2GfxEhRegistration* const saved_head = d2gfx_eh_thread_state.exception_list;
    // The retail TargetIp is the instruction immediately following RtlUnwind
    // (D2Gfx+0xBC1E). wasm has no code addresses (computed goto is rejected by
    // clang), so the resume point is the statement after this call: the unwind
    // helper returns to its caller instead of jumping. TargetIp is a non-null
    // sentinel (this function's own address) that names the resume point.
    d2gfx_rtl_unwind_781e(target_frame,
        reinterpret_cast<void*>(&d2gfx_unwind_nested_frames_bbf5), exception_record, nullptr);
    exception_record->flags &= ~uint32_t{2};
    saved_head->previous = d2gfx_eh_thread_state.exception_list;
    d2gfx_eh_thread_state.exception_list = saved_head;
}
