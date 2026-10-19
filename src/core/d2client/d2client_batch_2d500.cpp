// src/core/d2client/d2client_batch_2d500.cpp -- 1.13c D2Client.dll+0x2D500.
// Stage A mapping from the cached 1.13c decompile. Retail's indirect calls remain explicit seams;
// this gives the control-flow entry a named implementation without claiming vector parity.
#include <cstdint>
#include <emscripten.h>

struct D2Client2D500Env {
    uint32_t (*ready)();                                      // Ordinal_10747
    void* (*get_default_context)(void* client);               // Ordinal_11139(client)
    void* (*get_selected_context)(void* client, uint32_t selector); // Ordinal_11139(client, 5)
    uint32_t (*is_valid)(void* context);                      // Ordinal_11006
    uint32_t (*check_kind)(uint32_t value, uint32_t kind);    // Ordinal_10744
    uint32_t (*is_active)(void* value);                       // Ordinal_10061
    void (*finalize_context)();                               // Ordinal_10164
    uint32_t (*sub_2C230)();                                  // D2Client+0x2C230
    uint32_t (*sub_2C190)();                                  // D2Client+0x2C190
    void (*return_callback)(uint32_t value, void* client);    // incoming return-address callback
    void (*clear_context)();                                  // retail's context reset on sub_2C190 failure
};

// 1.13c D2Client+0x2D500 (370 B): validate the client's +0x60 context, select its state-5
// context, then dispatch the two callbacks when both that state and the caller-supplied kind-0x26
// value are accepted. ECX is a distinct indirect-call target; EAX remains the client object used at
// +0x60, cleared on the sub_2C190 failure path, and passed to the return callback. Register and
// stack inputs are explicit for later vector binding.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2client_sub_2D500(
    void* eax_object, void (*incoming_ecx_callback)(void* object, uint32_t value),
    int32_t kind_value, uint32_t stack_value,
    uint32_t* esi, uint32_t* ebp, uint32_t ebx, uint32_t edi,
    const D2Client2D500Env* env) {
    auto* eax_bytes = static_cast<uint8_t*>(eax_object);
    void* initial_context = *reinterpret_cast<void**>(eax_bytes + 0x60); // retail EAX[0x18]
    if (initial_context == nullptr || env->ready() == 0) return 0;

    void* first_state = env->get_default_context(initial_context);
    if (env->is_valid(first_state) != 0) env->check_kind(stack_value, 0x26);

    void* selected_state = env->get_selected_context(initial_context, 5);
    if (env->is_valid(selected_state) == 0 || env->check_kind(static_cast<uint32_t>(kind_value), 0x26) != 0)
        kind_value = 0;

    if (selected_state != nullptr && kind_value != 0) {
        if (env->sub_2C230() == 0) {
            *esi = 0;
            *ebp = 0;
        } else {
            incoming_ecx_callback(ebp, ebx);
        }
        if (env->sub_2C190() == 0) {
            env->clear_context();
            *static_cast<uint32_t*>(eax_object) = 0;
            return 1;
        }
        incoming_ecx_callback(eax_object, edi);
        return 1;
    }

    uint32_t active_context = env->is_active(initial_context);
    if (active_context != 0 && env->is_valid(reinterpret_cast<void*>(static_cast<uintptr_t>(active_context))) != 0)
        env->finalize_context();
    env->return_callback(4, eax_object);
    return 1;
}
