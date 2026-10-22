// 1.13c D2Win+0x161B0 (size 340, caller 0x18560): allocate and link a 0x284-byte (0xA1 dword) window control.
// Decompile oddities: unaff_EBX / unaff_retaddr are stored at +0x14/+0x18 as register/caller-frame values (kept as
// explicit args e_ebx, e_ret); the six pointer-sized callbacks (0x5A80/0x4A20, 0x5A10/0x4140, 0x35E0) are seams.
#include <cstdint>
#include <cstring>
struct D2WinCtlEnv {
  uint32_t* (*fogAlloc)(uint32_t size, uint32_t arg);   // Fog #10042(0x39,0) -> 0x284 bytes
  uint32_t*& head;                                      // DAT_6f9014a0 (control list head)
  uint32_t gdwBitMasks[3];                              // gdwBitMasks[0], [2]
  uint32_t cbA[2], cbB[2], cbDefault;                   // 5A80,4A20 / 5A10,4140 / 35E0
};
// 1.13c D2Win+0x161B0
uint32_t* d2win_161B0(D2WinCtlEnv& e, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5,
                      uint32_t p6, uint32_t p7, uint32_t p8, const uint32_t* p9, uint32_t e_ebx, uint32_t e_ret) {
  uint32_t* c = e.fogAlloc(0x39, 0);
  std::memset(c, 0, 0xA1 * 4);
  c[0] = 1;
  c[2] = e.gdwBitMasks[0] | e.gdwBitMasks[2];
  c[1] = p5; c[5] = e_ebx; c[6] = e_ret;
  c[0x10] = p3; c[0x11] = p4; c[0x12] = 0x100; c[3] = p1; c[4] = p2;
  c[0x13] = 0; c[0x14] = 0; c[0x15] = 0xffffffff;
  c[7] = e.cbA[(p8 & 8) == 0]; c[9] = e.cbB[(p8 & 8) == 0];
  c[0xd] = p7; c[0x99] = p6; c[10] = e.cbDefault;
  c[0xb] = c[0xc] = c[0x9a] = c[0xf] = 0;
  std::memset(&c[0x17], 0, 2);
  c[0x97] = 0; c[0x98] = p8;
  if (!p9) { c[0x9c] = 1; c[0x9d] = 0; } else { c[0x9c] = p9[0]; c[0x9d] = p9[1]; }
  c[0x9e] = c[0x9f] = 0;
  if (!e.head) { e.head = c; return c; }
  uint32_t* t = e.head;                                 // append at tail via next @+0x3c (32-bit ptrs, low heap)
  while (t[0xf]) t = (uint32_t*)(uintptr_t)t[0xf];
  t[0xf] = (uint32_t)(uintptr_t)c;
  return c;
}
