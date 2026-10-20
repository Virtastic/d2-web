// Batch D2Client 0xA3040..0xA4B00. Decompile-derived; unresolved callees are seams (unverified).
// Register-passed args (EAX/ESI/EDI) are made explicit parameters. Pointers are uintptr_t.
#include <cstdint>
namespace d2client_batch_a3040 {
struct Seams {
 virtual ~Seams()=default;
 virtual void fatal(int32_t msg)=0;                       // Ordinal_10265 + 10024 + _exit(-1)
 virtual void sa750(uint32_t kind, uintptr_t p, int32_t* rec)=0; // FUN_6fada750(0x16,..)
 virtual uintptr_t ord10511(uintptr_t)=0; virtual int32_t ord10494(uintptr_t,int32_t)=0;
 virtual uint32_t ord10397(uintptr_t)=0; virtual int32_t ord10601(uintptr_t,int32_t line)=0;
 virtual int32_t skillsCount()=0; virtual uintptr_t skillsRow(int32_t idx)=0; // sgptDataTables+0xb98/0xba0, stride 0x23c
 virtual void sb4c0()=0; virtual void sb6450()=0; virtual void sb7290()=0;
 virtual uintptr_t ord11013(int32_t,int32_t,int32_t,uint32_t,uint32_t)=0;
 virtual void ord10911(uintptr_t,void(*)(uint32_t))=0; virtual void ord10807()=0;
 virtual void ord10184()=0; virtual int32_t ord10028(int32_t row)=0;
 virtual uintptr_t alloc(int32_t size)=0;                 // Ordinal_10042
 virtual void freeMem(uintptr_t)=0;                       // Ordinal_10043
 virtual uintptr_t ord10048()=0; virtual void sc60(uintptr_t,uintptr_t)=0; virtual bool s2090()=0;
 virtual uintptr_t ord10460(uintptr_t)=0; virtual uintptr_t ord10464(uintptr_t)=0; virtual uintptr_t ord11147(uintptr_t)=0;
 virtual bool s98c0(uint32_t,int32_t)=0;
};
template<class T> static T& at(uintptr_t b, uint32_t o) { return *(T*)(b + o); }
// 1.13c D2Client+0xA3040   (eax = optional {kind @0, .. @0xc})
uint32_t sub_A3040(Seams& s, uintptr_t param_1, const uint32_t* e) {
 int32_t r[7] = {0, -1, 6, 0, 0, 1, 0}; // local_1c[0..2], local_10(+3), local_c(+5)?
 // layout: local_1c[3], local_10, local_c, local_8, local_4
 r[3] = e ? (int32_t)e[3] : -1; if (e) r[2] = (int32_t)e[0];
 r[4] = 1; r[5] = 0; r[6] = 0;
 s.sa750(0x16, param_1, r);
 return 1;
}
// 1.13c D2Client+0xA3160
void sub_A3160(Seams& s, uint32_t param_1) {
 uintptr_t u = s.ord10511(param_1);
 if (!u || s.ord10494(param_1, 0x12) != 0 || (s.ord10397(u) & 2)) return;
 int32_t i = s.ord10601(u, 0x93c);
 if (i < 0 || i >= s.skillsCount()) return;
 uintptr_t row = s.skillsRow(i);
 if (!row) return;
 switch (at<uint16_t>(row, 0x2e)) { case 0x43: s.sb4c0(); break; case 0x4c: s.sb6450(); break; case 0x4d: case 0x4e: s.sb7290(); break; }
}
// 1.13c D2Client+0xA3220   (edi = optional {kind @0, .. @0xc}); cb = FUN_6fb53160
void sub_A3220(Seams& s, const uint32_t* e, void(*cb)(uint32_t)) {
 uint32_t a = e ? e[0] : 6, b = e ? e[3] : 0xffffffffu;
 uintptr_t h = s.ord11013(0, 6, 0x32, a, b);
 if (h) { s.ord10911(h, cb); s.ord10807(); }
}
// 1.13c D2Client+0xA32D0   (eax = idx, edi = menu)
void sub_A32D0(Seams& s, int32_t idx, uintptr_t m) {
 s.ord10184();
 uintptr_t it = m + (uintptr_t)idx * 0x110;
 s.ord10184();
 int32_t w = s.ord10028(idx), mw = at<int32_t>(m, 0x34);
 at<int32_t>(it, 0x160) = w;
 at<int32_t>(it, 0x15c) = mw <= w ? 0 : ((mw - w) + 1) / 2 + 1;
 s.ord10184();
}
// 1.13c D2Client+0xA3340   (eax = menu)
void sub_A3340(Seams& s, uintptr_t m, uint32_t v) { if (!m) s.fatal(0x529); at<uint32_t>(m, 0x5c) = v; }
// 1.13c D2Client+0xA3370   (param_1 = menu, esi = idx)
void sub_A3370(Seams& s, uintptr_t m, int32_t idx) {
 if (!m) s.fatal(0x4f2);
 if (idx >= at<int32_t>(m, 0x50)) s.fatal(0x4f8);
 if (idx == -1) { at<int32_t>(m, 0x44) = -1; return; }
 for (int32_t i = 0; i < at<int32_t>(m, 0x50); ++i)
  if (i == idx) { if (at<int32_t>(m, 0x174 + i * 0x110)) { at<int32_t>(m, 0x44) = idx; return; } s.fatal(0x51a); }
 s.fatal(0x51e);
}
// 1.13c D2Client+0xA3400   (esi = menu, edi = value): push node on list @0xb08
void sub_A3400(Seams& s, uintptr_t m, uint32_t v) {
 if (!m) s.fatal(0x358);
 uint32_t old = at<uint32_t>(m, 0xb08);
 uintptr_t n = s.alloc(0x35c);
 at<uintptr_t>(m, 0xb08) = n; // ponytail: wasm64 would need a wider slot; layout is the 32-bit original
 at<uint32_t>(n, 4) = old;
 at<uint32_t>(n, 0) = v;
}
// 1.13c D2Client+0xA3580   (eax = menu); scrW/scrH = DAT_6fb8bc48 / DAT_6fb8bc4c
void sub_A3580(Seams& s, uintptr_t m, int32_t scrW, int32_t scrH) {
 int32_t h = 0, w = 0, nul = 0, n = at<int32_t>(m, 0x50);
 if (!at<int32_t>(m, 0x58)) s.fatal(0x5aa);
 s.ord10184();
 for (int32_t i = 0; i < n; ++i) {
  int32_t t = s.ord10028(i); if (w < t) w = t;
  int32_t* p = &at<int32_t>(m, 0x174 + i * 0x110);
  if (!*p) ++nul;
  h += p[-7];
 }
 w += 0x14; h += 0xf;
 s.ord10184();
 int32_t x = at<int32_t>(m, 0x24) - w / 2;
 at<int32_t>(m, 0x2c) = x;
 int32_t c = at<int32_t>(m, 0x14) == 0 ? h / 3 : nul * 0x15;
 at<int32_t>(m, 0x30) = at<int32_t>(m, 0x28) - c;
 if (scrW - 0x50 < w) s.fatal(0x5dd);
 if (h > scrH - 0x62) s.fatal(0x5e2);
 if (scrW - 10 < x + w) at<int32_t>(m, 0x2c) = scrW - w;
 if (scrH - 0x3a < at<int32_t>(m, 0x30) + h) at<int32_t>(m, 0x30) = (scrH - h) - 0x30;
 if (at<int32_t>(m, 0x2c) < 0xb) at<int32_t>(m, 0x2c) = 10;
 if (at<int32_t>(m, 0x30) < 0xb) at<int32_t>(m, 0x30) = 10;
 at<int32_t>(m, 0x34) = w; at<int32_t>(m, 0x38) = h;
 for (int32_t i = 0; i < n; ++i) sub_A32D0(s, i, m);
}
// 1.13c D2Client+0xA3A20   (ecx = a, edx = b); tab = 0x6fbca2e4 records of 10 bytes {u16 val; i32 key; i32 flag}, 0x6fbca2e4..0x6fbca75a
void sub_A3A20(Seams& s, uint32_t a, int32_t key0, int32_t key1, const uint8_t* tab, uint32_t& g85c, uint32_t& g868) {
 const uint32_t N = (0x6fbca75a - 0x6fbca2e6) / 10 + 1;
 auto val = [&](uint32_t i){ return *(const uint16_t*)(tab + i*10); };
 auto key = [&](uint32_t i){ return *(const int32_t*)(tab + i*10 + 2); };
 auto flg = [&](uint32_t i){ return *(const int32_t*)(tab + i*10 + 6); };
 if (s.s98c0(a, 0x26))
  for (uint32_t i = 0; i < N; ++i)
   if (key(i) == key0 && flg(i) == 1) { if (val(i) != 0xffff && val(i) < 0xe0) g85c = val(i); break; }
 if (s.s98c0(a, key1))
  for (uint32_t i = 0; i < N; ++i)
   if (key(i) == key1 && flg(i) == 0) { if (val(i) != 0xffff && val(i) < 0xe0) g868 = val(i); return; }
}
// 1.13c D2Client+0xA3AD0   (head = DAT_6fbcbc00, active = DAT_6fbcbc04, cnt = DAT_6fbbb208)
void sub_A3AD0(Seams& s, uintptr_t& head, int32_t active, int32_t& cnt) {
 if (active) {
  s.sc60(s.ord10048(), 0x6fb8e858);
  if (s.s2090()) s.sc60(s.ord10048(), 0x6fb8e900);
  --cnt;
 }
 for (uintptr_t n = head; n;) { uintptr_t nx = at<uint32_t>(n, 0xb0c); s.freeMem(n); n = nx; }
 head = 0;
}
// 1.13c D2Client+0xA4A90   (in_EAX = a, param_1 = b, edi = target)
uint32_t sub_A4A90(Seams& s, uintptr_t b, uintptr_t a, uintptr_t target) {
 if (b == a) return 0;
 if (at<uint32_t>(a, 0x60))
  for (uintptr_t i = s.ord10460(at<uint32_t>(a, 0x60)); i; i = s.ord10464(i)) {
   uintptr_t r = s.ord11147(i);
   if (!r) s.fatal(0xa5b);
   if (r == target) return 1;
  }
 return 0;
}
// 1.13c D2Client+0xA4B00   (eax = obj)
void sub_A4B00(Seams& s, uintptr_t o, uint32_t v) { if (!o) s.fatal(0xa4a); at<uint32_t>(o, 0x68) = v; }
}
