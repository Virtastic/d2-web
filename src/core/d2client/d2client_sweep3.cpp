// D2Client sweep 3: B0090, B2DC0, AA290, C42B0, AA740, C3E20. Implemented from the retail 1.13c D2Client.dll
// disassembly (objdump, image base 0x6FAB0000). Callees and process globals come through Env seams; registers the
// calling convention carries are explicit parameters. No decompiler output, no retail content in this file.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <vector>
namespace d2client_sweep3 {
using P = uintptr_t;

struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;                                   // *sgptDataTables (slot at D2Client+0xCE474)
 // 0xB0090 / 0xB2DC0 / 0xC42B0 callees
 uint32_t (*d2common_11076)(P room, int32_t* from, int32_t* to, uint32_t mask);  // thunk 0xC436 (ret 0x10)
 uint32_t (*c46c)(P unit);                       // thunk 0xC46C
 void (*f58630)(P esi);                          // D2Client+0xA8630, esi = the object at unit+0x64
 void (*fb17c0)(P unit);                         // D2Client+0xB17C0 (ecx = unit)
 P (*c310)(P unit);                              // thunk 0xC310
 P (*cb68)(P x); P (*cb6e)(P x);                 // thunks 0xCB68 / 0xCB6E
 void (*c3ee)(uint32_t a, P b, P c);             // thunk 0xC3EE
 void (*faff10)(P unit, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t w4); // D2Client+0xAFF10, ret 0x14
 // 0xAA290
 uint32_t (*bf8c)();                             // thunk 0xBF8C
 const int32_t* layoutFlag;                      // D2Client+0x119854
};

// 1.13c D2Client+0xB0090 (fastcall: ecx = dx, edx = room, eax = dy; stack r2, x, y; ret 0xC)
bool d2_client_b0090(const Env& e, int32_t dx, P room, int32_t dy, int32_t r2, int32_t x, int32_t y) {
 if (!room) return false;
 if ((int32_t)((uint32_t)dx * (uint32_t)dx + (uint32_t)dy * (uint32_t)dy) > r2) return false;
 int32_t from[2] = { x, y };
 int32_t to[2] = { (int32_t)((uint32_t)x + (uint32_t)dx), (int32_t)((uint32_t)y + (uint32_t)dy) };
 return e.d2common_11076(room, from, to, 4) == 0;
}

// 1.13c D2Client+0xB2DC0 (ecx = unit)
void d2_client_b2dc0(const Env& e, P unit) {
 if (e.c46c(unit) == 0) { e.fb17c0(unit); return; }
 if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x10000);
 P p = e.rd32(unit + 0x64);
 if (p) { e.f58630(p); e.wr32(unit + 0x64, 0); }
}

// 1.13c D2Client+0xAA290 (esi = x, edi = y; returns eax, -1 outside)
int32_t d2_client_aa290(const Env& e, int32_t x, int32_t y) {
 bool a = e.bf8c() != 0 && *e.layoutFlag != 0;
 if (y > 0x1E || x > 0x140) return -1;
 if (a) {
  if (x < 0x40) return 0;
  if (x < 0x80) return 1;
  if (x < 0xC0) return 2;
  return 3 + (x >= 0x100);
 }
 if (x < 0x50) return 0;
 if (x < 0xA0) return 1;
 return 2 + (x >= 0xF0);
}

// 1.13c D2Client+0xC42B0 (ecx = unit, edx = skill row id, stack = arg; ret 4)
uint32_t d2_client_c42b0(const Env& e, P unit, int32_t id, uint32_t arg) {
 if (id < 0 || id >= (int32_t)e.rd32(e.dataTables + 0xBA0)) return 0;
 P row = (P)e.rd32(e.dataTables + 0xB98) + (P)(uint32_t)id * 0x23C;
 if (!row) return 0;
 P c = e.c310(unit);
 if (!c) return 0;
 P a = e.cb68(c), b = e.cb6e(c);
 if (!a || !b) return 0;
 e.c3ee(e.rd32(unit + 0x2C), a, b);
 int32_t w = (int16_t)(e.rd32(row + 0xEA) & 0xFFFF);
 if (w >= 0 && w < (int32_t)e.rd32(e.dataTables + 0xB6C))
  e.faff10(unit, (uint32_t)w, (uint32_t)a, (uint32_t)b, (uint32_t)id, arg);
 return 1;
}

// ---- 0xAA740: panel release (stdcall, one arg = message record M; ret 4) ----
// fcd8c is the flat image of D2Client .data from RVA 0xFCD8C: slot records of 5 bytes at +5*k (dword at +0), armed byte
// at +0x30 (0xFCDBC), pressed slot dword at +0x31 (0xFCDBD, unaligned), packet arg dword at +0x41 (0xFCDCD), sent byte
// at +0x49 (0xFCDD5).
struct PanelG {
 int32_t* scrW; int32_t* scrH;                    // 0xDBC48 / 0xDBC4C
 int32_t* originX; int32_t* originY;              // 0x11B9A0 / 0x11B9A4
 uint8_t* fcd8c;
 int32_t* target;                                  // 0xAADD0
 int32_t* gate84; int32_t* gate90;                 // 0xAAD84 / 0xAAD90
 int32_t* slideTgt; int32_t* slideMode;            // 0xCC418 / 0xCC414
 int32_t* slideBase;                               // 0xCC3C4
 int32_t* in14; int32_t* in18;                     // 0xA9E14 / 0xA9E18
 int32_t* modeWord;                                // 0xB8D5D0
};
struct PanelEnv {
 PanelG g;
 bool (*gate63710)();                              // 0x63710, nonzero: handler returns untouched
 bool (*hit_aa310)(int32_t px, int32_t py);        // 0xAA310 (eax = px, ecx = py)
 int32_t (*hit_aa240)(int32_t px, int32_t py);     // 0xAA240 (ebx = px, edi = py)
 void (*bf8c)();                                   // thunk 0xBF8C
 void (*assertFail)(uint32_t line);                // 0xBF92 + 0xBF80 (assert text) then 0x3664(-1); does not return in retail
 P (*d0b4)(uint32_t strAddr);                      // 0xD0B4, cdecl: strAddr stays on the stack for 0x3CC60's ret 8
 void (*log3cc60)(uint32_t eax5, P d0b4Result, uint32_t strAddr);  // 0x3CC60, ret 8
 void (*aa380)(); void (*be2e0)();                 // 0xAA380, 0xBE2E0
 void (*d1b0)(int32_t ecx);                        // thunk 0xD1B0
 void (*f8a4d0)(int32_t eax, int32_t ecx, int32_t edx, int32_t esi, int32_t stackArg);  // 0x8A4D0, ret 4
 void (*pkt14910)(uint32_t al, uint32_t ecx, uint32_t edx);   // 0x14910
 void (*f88a70)(uint32_t ebx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);  // 0x88A70, ret 0x10
 void (*be84)(uint8_t* m);                         // thunk 0xBE84 (ret 4)
};
static const uint32_t kLogStr = 0x6FB80828;
static int32_t rd32s(const uint8_t* p) { int32_t v; std::memcpy(&v, p, 4); return v; }
static void wr32s(uint8_t* p, int32_t v) { std::memcpy(p, &v, 4); }
// target clear: prev = [AADD0]; 0xBF8C; mode must be 0x26 (else the assert path, which exits); [AADD0] = 0; log when prev != 0
static bool panelClearTarget(const PanelEnv& e) {
 int32_t prev = *e.g.target;
 e.bf8c();
 if (*e.g.modeWord != 0x26) { e.assertFail(0x139B); return false; }
 *e.g.target = 0;
 if (prev) { e.log3cc60(5, e.d0b4(kLogStr), kLogStr); e.aa380(); e.be2e0(); }
 return true;
}
static void panelSlide(const PanelEnv& e) {
 const PanelG& g = e.g;
 if (*g.gate84 == 0 && *g.gate90 == 0) {
  *g.slideTgt = 0; *g.slideMode = 0;
  e.d1b0(0);
  e.f8a4d0(*g.in18, *g.in14, 0, *g.slideBase, 0);
 } else {
  int32_t q = *g.scrW / 4;
  *g.slideTgt = -q; *g.slideMode = 1;
  e.d1b0(-q);
  int32_t c = *g.in14, q2 = c / 4;
  e.f8a4d0(*g.in18, c - q2, 0, *g.slideBase, -q2);
 }
}
// 1.13c D2Client+0xAA740
void d2_client_aa740(const PanelEnv& e, uint8_t* m) {
 const PanelG& g = e.g;
 uint8_t* armed = g.fcd8c + 0x30; uint8_t* slotp = g.fcd8c + 0x31;
 int32_t x = (uint16_t)(m[0xC] | m[0xD] << 8), y = (uint16_t)(m[0xE] | m[0xF] << 8);
 if (e.gate63710()) return;
 if (x > *g.scrW || y > *g.scrH - 0x31) { *armed = 0; wr32s(slotp, -1); return; }
 int32_t px = x - *g.originX, py = y + *g.originY;
 auto done = [&]() { *armed = 0; wr32s(slotp, -1); wr32s(m + 0x18, 1); wr32s(m + 0x1C, 0); e.be84(m); };
 if (*armed) {
  if (!e.hit_aa310(px, py)) { done(); return; }
  if (!panelClearTarget(e)) return;
  panelSlide(e);
  e.aa380();
  done(); return;
 }
 if (rd32s(slotp) == -1) { done(); return; }
 if (e.hit_aa240(px, py) != rd32s(slotp)) { done(); return; }
 wr32s(m + 0x18, 1); wr32s(m + 0x1C, 0);
 e.be84(m);
 int32_t slot = rd32s(slotp);
 e.pkt14910(0x49, (uint32_t)rd32s(g.fcd8c + 0x41), (uint32_t)rd32s(g.fcd8c + 5 * slot));
 g.fcd8c[0x49] = 1;
 e.aa380();
 *armed = 0; wr32s(slotp, -1);
 if (!panelClearTarget(e)) return;
 panelSlide(e);
 e.f88a70(0x8B7, 0, 0, 0, 0);
 wr32s(m + 0x18, 1); wr32s(m + 0x1C, 0);
 e.be84(m);
}

// ---- 0xC3E20: debug heap report (cdecl, no args, void). Writes D2Mem.txt through the 0xC3DD0 writer. ----
struct MemEnv {
 uint32_t (*rd32)(P);
 P (*createFile)();                               // CreateFileA("D2Mem.txt", 0x40000000, 0, 0, 2 /*CREATE_ALWAYS*/, 0x80, 0)
 void (*closeFile)(P);                            // IAT slot 0xCEF54
 void (*write)(P h, const char* text);            // 0xC3DD0 (printf-to-file; text already formatted)
 bool (*o482)(P prev, P* state, uint32_t* buf);   // Storm#482 heap enumerator (0xBE78); buf[0] = 0x128 in
 P (*o401)(uint32_t size, const char* tag, int32_t line, uint32_t flags);   // Storm#401 alloc (0xBE66)
 P (*o483)(const char* file, uint32_t line);      // Storm#483 (0xBE7E)
 bool (*o481)(P handle, P prev, P* state, uint32_t* info);                  // Storm#481 block enumerator (0xBE72)
 void (*listDestroy)(uint32_t nodes);             // 0x1030 on the stack list head
};
static void fmtw(const MemEnv& e, P h, const char* f, ...) __attribute__((format(printf, 3, 4)));
static void fmtw(const MemEnv& e, P h, const char* f, ...) {
 char b[1024]; va_list a; va_start(a, f); std::vsnprintf(b, sizeof b, f, a); va_end(a); e.write(h, b);
}
struct HeapRec { uint32_t body[0x4A]; };          // rec+8: the 0x128-byte enumerator buffer copy; rec fields below are rec+0x8+4*i
// 1.13c D2Client+0xC3E20
void d2_client_c3e20(const MemEnv& e) {
 P h = e.createFile();
 e.write(h, "D2 MEMORY CHECK\n");
 e.write(h, "================================================\n\n");
 std::vector<HeapRec> list;                        // kept sorted by key (body[0x49] = rec+0x12C), descending
 uint32_t buf[0x4A] = {}; P st = 0;
 buf[0] = 0x128;
 bool more = e.o482(0, &st, buf);
 while (more) {
  e.o401(0x130, ".?AUTALLOCREC@@", -2, 8);
  HeapRec r; std::memcpy(r.body, buf, sizeof buf);
  uint32_t key = r.body[0x49];
  size_t at;
  if (list.empty() || list[0].body[0x49] < key) at = 0;      // new head
  else {                                                     // head.key >= key: walk while next.key > key, insert after
   size_t c = 0;
   while (c + 1 < list.size() && list[c + 1].body[0x49] > key) ++c;
   at = c + 1;
  }
  list.insert(list.begin() + at, r);
  more = e.o482(st, &st, buf);
 }
 e.write(h, "      SIZE|    BLOCKS|  RESERVED| COMMITTED|       MAX|   REGIONS|FILENAME:LINE\n");
 int32_t size = 0, blocks = 0, reserved = 0, committed = 0, mx = 0, regions = 0, heaps = 0;
 for (const HeapRec& r : list) {
  // rec+0x114.. = body[0x10C/4 ..]: line, regions, committed, reserved, max, blocks, size
  const uint32_t* b = r.body;
  char name[0x105]; std::memcpy(name, reinterpret_cast<const char*>(b) + 8, 0x104); name[0x104] = 0;
  fmtw(e, h, "%10d|%10d|%10d|%10d|%10d|%10d|%s:%d\n", (int32_t)b[0x49], (int32_t)b[0x48], (int32_t)b[0x46], (int32_t)b[0x45],
       (int32_t)b[0x47], (int32_t)b[0x44], name, (int32_t)b[0x43]);
  size += b[0x49]; blocks += b[0x48]; reserved += b[0x46]; committed += b[0x45]; mx += b[0x47]; regions += b[0x44]; ++heaps;
 }
 const char* rule = "================================================\n\n";
 e.write(h, rule);
 fmtw(e, h, "NUM HEAPS:       %10d\n", heaps);
 fmtw(e, h, "TOTAL MEMORY:    %10d\n", size);
 fmtw(e, h, "TOTAL BLOCKS:    %10d\n", blocks);
 fmtw(e, h, "TOTAL RESERVED:  %10d\n", reserved);
 fmtw(e, h, "TOTAL COMMITTED: %10d\n", committed);
 fmtw(e, h, "TOTAL MAX:       %10d\n", mx);
 fmtw(e, h, "TOTAL REGIONS:   %10d\n\n\n\n\n", regions);
 e.write(h, rule);
 P hl = e.o483("SOURCE\\D2HELL\\Src\\Archive.cpp", 0xF5);
 uint32_t info[7] = {}; info[0] = 0x1C; P cur = 0;
 int32_t allocSum = 0, memSum = 0, count = 0;
 bool ok = e.o481(hl, 0, &cur, info);
 while (ok) {
  P start = cur;
  uint32_t sz = info[4];
  if (sz > 0x220 && info[3] && info[2]) {
   P end = start + sz;
   for (P s = start; s < end; s += 4) {
    if (e.rd32(s) == 0x4C4F4144 && e.rd32(s + 4) == 0x4655434B && e.rd32(s + 8) == 0x54484953 && e.rd32(s + 12) == 0x4E4F5720) {
     // marker record: s+0x10 line, s+0x14 size, s+0x18 file name, s+0x11C name (strings are read in place at s)
     fmtw(e, h, "%8d\t%50s\t(%s:%d)\n", (int32_t)e.rd32(s + 0x14), reinterpret_cast<const char*>(s + 0x11C),
          reinterpret_cast<const char*>(s + 0x18), (int32_t)e.rd32(s + 0x10));
     memSum += (int32_t)e.rd32(s + 0x14);   // edi
     allocSum += (int32_t)sz;               // ebx
     ++count;
     break;
    }
   }
  }
  ok = e.o481(hl, start, &cur, info);
 }
 fmtw(e, h, "TOTAL MEMORY: %8d\n", memSum);
 fmtw(e, h, "TOTAL ALLOC:  %8d\n", allocSum);
 fmtw(e, h, "TOTAL BLOCKS: %8d\n\n\n\n", count);
 e.closeFile(h);
 e.listDestroy((uint32_t)list.size());
}
} // namespace d2client_sweep3
