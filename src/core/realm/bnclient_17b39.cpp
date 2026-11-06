// src/core/realm/bnclient_17b39.cpp -- the BNClient CRT CallCatchBlock (VC7.1, cdecl with the handler in ECX, SEH frame 0x1B078).
// Saves the registration node's state dword (regNode-4), builds a FrameInfo for the thrown object on its own frame,
// saves the thread's current exception record and context (ptd+0x7C, +0x80) and installs rec and ctx, then runs the
// catch funclet through _CallCatchBlock2(regNode, funcInfo, handler, catchDepth, nlgCode). The __finally (0x17C8E)
// writes the state dword back, unlinks the FrameInfo, restores the saved pair, and destroys the thrown object when the
// record is an MSVC C++ throw (0xE06D7363, three params, magic 0x19930520/21), a continuation came back and
// IsExceptionObjectToBeDestroyed says so. Returns the continuation address.
#include <cstdint>

struct BnCatchSeams {
    uint32_t* (*getptd)();                                        // 0x3255
    uint32_t (*create_frame_info)(uint32_t* frame, uint32_t obj);   // 0x177FA (the FrameInfo lives on this frame)
    uint32_t (*call_catch_block2)(uint32_t* reg, uint32_t func_info, uint32_t handler, uint32_t depth, uint32_t nlg); // 0x1788F
    void (*find_and_unlink)(uint32_t frame_info);                  // 0x17843
    int32_t (*to_be_destroyed)(uint32_t obj);                      // 0x17822
    uint32_t (*abnormal_termination)();                            // 0x58DA
    void (*destruct)(const uint32_t* rec, uint32_t abnormal);       // 0x17AD5
};

// 1.13c BNClient+0x17B39 (157 B, finally 0x17C8E 111 B; caller 0x17E79).
extern "C" uint32_t bn_call_catch_block(uint32_t handler, const uint32_t* rec, uint32_t* reg, uint32_t ctx, uint32_t func_info,
                                        uint32_t depth, uint32_t nlg, const BnCatchSeams* s) {
    uint32_t frame[2];
    const uint32_t saved_state = reg[-1];
    const uint32_t fi = s->create_frame_info(frame, rec[6]);
    const uint32_t old_rec = s->getptd()[0x7c / 4];
    const uint32_t old_ctx = s->getptd()[0x80 / 4];
    s->getptd()[0x7c / 4] = (uint32_t)(uintptr_t)rec;
    s->getptd()[0x80 / 4] = ctx;
    const uint32_t cont = s->call_catch_block2(reg, func_info, handler, depth, nlg);
    reg[-1] = saved_state;
    s->find_and_unlink(fi);
    s->getptd()[0x7c / 4] = old_rec;
    s->getptd()[0x80 / 4] = old_ctx;
    if (rec[0] == 0xE06D7363u && rec[4] == 3 && (rec[5] == 0x19930520u || rec[5] == 0x19930521u) && cont != 0 &&
        s->to_be_destroyed(rec[6]) != 0)
        s->destruct(rec, s->abnormal_termination());
    return cont;
}
