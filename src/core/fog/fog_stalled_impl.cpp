// Fog rows that were STALLED on "no wasm analogue": QServer hash increment, the fatal reporter and the logger.
// Win32-only effects (MessageBox, log FILE, GetThreadContext, stack walk) have no wasm host; stderr stands in.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
extern "C" {
uint32_t* q_fog_stalled_impl_FUN_6ff6b560(int32_t size, uint8_t* pMgr);   // Fog+0x1B560 pool alloc
void FUN_6ff60f30(int type, const char* file, int line, const char* fmt, ...);
void FUN_6ff68770(const char* ch, int p2, int p3, const char* fmt, va_list ap);
void Fog_Ordinal_116();
extern const char DAT_6ff768ec[];
}
static int g_fog_reporting;   // DAT_6ffa6e70 re-entrancy flag
static void (*g_fog_hook)();  // DAT_6ffa6e6c

// 1.13c Fog+0x0D110 -- count key *key in the srv hash (size +0x10, buckets +0x40, node: key, count, next); EBX = srv, EAX = key ptr
extern "C" uint32_t Fog_D110(uint8_t* srv, const uint32_t* key) {
  uint32_t k = *key, size = *(uint32_t*)(srv + 0x10);
  uint32_t* slot = (uint32_t*)((uintptr_t)*(uint32_t*)(srv + 0x40)) + k % size;
  for (uint32_t* n = (uint32_t*)(uintptr_t)*slot; n; n = (uint32_t*)(uintptr_t)n[2])
    if (n[0] == k) { n[1]++; return 1; }
  uint32_t* n = q_fog_stalled_impl_FUN_6ff6b560(0, nullptr);
  if (!n) FUN_6ff60f30(2, "..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x914, DAT_6ff768ec);
  n[0] = k; n[1] = 1; n[2] = *slot; *slot = (uint32_t)(uintptr_t)n;
  return 1;
}

// 1.13c Fog+0x10F30 ordinal 10028 FogError(type, file, line, fmt, ...): re-entrancy guard, report, run hook; types 5/6/2 stay latched.
extern "C" void Fog_Ordinal_10028(int type, const char* file, int line, const char* fmt, ...) {
  if (type == 10) file = "..\\Source\\Fog\\Src\\ErrorManager.cpp";
  if (g_fog_reporting) return;
  g_fog_reporting = 1;
  char buf[0x400];
  va_list ap; va_start(ap, fmt);
  vsnprintf(buf, sizeof buf, fmt ? fmt : "", ap);
  va_end(ap);
  std::fprintf(stderr, "Fog error %d: %s(%d): %s\n", type, file ? file : "", line, buf);
  if (g_fog_hook) g_fog_hook();
  std::fflush(stderr);
  if (type != 5 && type != 6 && type != 2) g_fog_reporting = 0;
}

// 1.13c Fog+0x19100 ordinal 10029 logger(fmt, ...): NULL fmt is fatal (type 6, exit -1); else forward to the log writer (Fog+0x18770).
extern "C" void Fog_Ordinal_10029(const char* fmt, ...) {
  if (!fmt) {
    Fog_Ordinal_116();
    Fog_Ordinal_10028(6, "", 0x118, "Unrecoverable internal error %08x");
    std::exit(-1);
  }
  va_list ap; va_start(ap, fmt);
  FUN_6ff68770(DAT_6ff768ec, 1, 1, fmt, ap);
  va_end(ap);
}
