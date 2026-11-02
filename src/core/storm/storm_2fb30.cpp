// 1.13c Storm+0x2FB30 -- the "handle never released" debug report (decompile-derived; no spec of its own).
// Prior art used: build-wasm/ghidra-storm/decomp/0x2FB30.c; the callers at 0x36D40, 0x38F90 and 0x17D60 pass the
// resource name ("HSTRANS", "HSCODESTREAM", ...) and a null detail string.
// Port notes: the first call latches DAT_6fc45a74 and reads the "Internal"/"Debug Memory" option through 0x2C1D0,
// which is not ported; the option is read as off (DAT_6fc45a70 = 0), so the report is a no-op, as retail is with the
// option unset. When it is on, the report goes to Storm_Sub1F590 (0x1F590, the error sink) while DAT_6fc43138 is zero,
// and to OutputDebugStringA otherwise. DAT_6fc43138 has no port either and reads as zero.
#include <cstdint>
#include <cstdio>
#include <Windows.h>

// Storm+0x1F590 (the error sink) has no external port: storm_batch_b54b.cpp defines it as an internal stub. The sink
// records its code through the last-error word, which is the part a caller can observe, so that is what is bound here.
void Storm_SetError45388(uint32_t e);   // storm_batch_26e20.cpp

namespace {
uint32_t g_dbgLatched;     // DAT_6fc45a74  first-call latch
uint32_t g_dbgMemOn;       // DAT_6fc45a70  "Debug Memory" option (read by 0x2C1D0; unported, so 0)
uint32_t g_dbgUiSink;      // DAT_6fc43138  non-zero routes the report to OutputDebugStringA
}  // namespace

// 1.13c Storm+0x2FB30 (param_1 = resource name, param_2 = detail or null)
void Storm_Sub2FB30(const char* name, const char* detail) {
  if (g_dbgLatched == 0) g_dbgLatched = 1;
  if (g_dbgMemOn == 0) return;
  char msg[0x100];
  if (detail == nullptr || *detail == '\0') std::snprintf(msg, sizeof msg, "%s", name);
  else std::snprintf(msg, sizeof msg, "%s (%s)", name, detail);
  if (g_dbgUiSink == 0) {
    Storm_SetError45388(0x8510007f);
    return;
  }
  char out[0x100 + 0x40];
  std::snprintf(out, sizeof out, "Storm Error : handle never released -- %s\n", msg);
  OutputDebugStringA(out);
}
