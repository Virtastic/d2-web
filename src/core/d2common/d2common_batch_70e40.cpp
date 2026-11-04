// D2Common 1.13c batch: room-pair bounding box, then hand to 0x70BB0.
// Token style: the param and the rooms are retail 32-bit blobs, read at retail offsets through uintptr_t casts; callees come in through Env seams.
#include <cstdint>
#include <cstring>

struct D2Common70e40Env {
 void (*sub3BA60)(uint32_t room, int32_t* out);                    // out[0..7] = room ? room[+0x4C..] : 0
 void (*sub70BB0)(int32_t ecx, const int32_t* box, uint32_t p);    // ECX = box[3], EDX = &box, stack = param
};

static uint32_t r32_70e40(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }

// 1.13c D2Common+0x70E40
extern "C" void D2Common_70E40_BoundingBox(uint32_t p, const D2Common70e40Env* env) {
 *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(p + 0x14)) = static_cast<uint8_t>(r32_70e40(p + 0x14) << 1);
 const uint32_t room = r32_70e40(p + 8), other = r32_70e40(p + 0xc);
 int32_t b[8], a[8], c[8];
 env->sub3BA60(room, b);
 for (int i = 0; i < 8; ++i) a[i] = b[i];
 if (other != 0 && room != other) {
  env->sub3BA60(other, c);
  if (c[0] < a[0]) { b[0] = c[0]; b[2] = (a[2] - c[0]) + a[0]; }
  else { b[0] = a[0]; b[2] = (c[2] - a[0]) + c[0]; }
  if (c[1] < a[1]) { b[3] = (a[3] - c[1]) + a[1]; b[1] = c[1]; }
  else { b[3] = (c[3] - a[1]) + c[1]; b[1] = a[1]; }
 }
 if (b[2] == a[2]) { b[0] -= 10; b[2] += 0x14; }
 if (b[3] == a[3]) { b[1] -= 10; b[3] += 0x14; }
 env->sub70BB0(b[3], b, p);
}
