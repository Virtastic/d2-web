// src/core/d2gfx/d2gfx_create_frame_info.cpp -- 1.13c D2Gfx CRT _CreateFrameInfo.
// Guest layout: FrameInfo {+0 pExceptionObject, +4 pNext}; ptd = __getptd() block, _curexception at ptd+0x88.
#include <cstdint>
// 1.13c D2Gfx+0x0BEAB
extern "C" uint32_t* d2gfx_create_frame_info(uint32_t* frame, uint32_t obj, uint8_t* ptd) {
    frame[0] = obj;
    frame[1] = *(uint32_t*)(ptd + 0x88);   // link to the previous frame
    *(uint32_t*)(ptd + 0x88) = (uint32_t)(uintptr_t)frame;
    return frame;
}
