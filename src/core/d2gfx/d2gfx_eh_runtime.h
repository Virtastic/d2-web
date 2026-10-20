#pragma once
#include <cstdint>

// Native runtime objects, not packed x86 records. Resolve serialized addresses
// before constructing them. The first registration field is the FS:[0] link.
struct D2GfxEhRegistration {
    D2GfxEhRegistration* previous;
    void* handler;
};
struct D2GfxEhExceptionRecord {
    uint32_t code;
    uint32_t flags;
    D2GfxEhExceptionRecord* nested;
    void* address;
    uint32_t parameter_count;
    uintptr_t information[15];
};
struct D2GfxEhThreadState {
    D2GfxEhRegistration* exception_list = nullptr;
};
// D2Gfx's thread runtime owns the native equivalent of the TEB exception list.
extern thread_local D2GfxEhThreadState d2gfx_eh_thread_state;

// D2Gfx import thunk 0x781E: Windows RtlUnwind(TargetFrame, TargetIp,
// ExceptionRecord, ReturnValue). TargetIp is an executable continuation address,
// never a retail RVA cast to a pointer. A platform implementation unwinds the
// thread's registration chain and resumes at that address (or returns to it).
extern "C" void d2gfx_rtl_unwind_781e(D2GfxEhRegistration* target_frame,
    void* target_ip, D2GfxEhExceptionRecord* exception_record, void* return_value);
extern "C" void d2gfx_unwind_nested_frames_bbf5(
    D2GfxEhRegistration* target_frame, D2GfxEhExceptionRecord* exception_record);
