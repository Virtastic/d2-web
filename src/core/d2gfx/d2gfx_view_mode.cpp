#include <cstdint>

// D2Gfx+0x11270: 1.13c's selector slot written by ordinal 10006.
extern "C" uint32_t d2gfx_view_mode_selector = 0;

extern "C" void d2gfx_set_view_mode_selector(uint32_t selector) {
  // 1.13c D2Gfx+0x0B190
  d2gfx_view_mode_selector = selector;
}
