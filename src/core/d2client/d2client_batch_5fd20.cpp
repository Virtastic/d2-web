// Implementation batch, D2Client RVAs 0x5FD20..0x67AF0. Decompile-derived; no spec, no vector.
// Imported-DLL calls go through Host (ordinal named in each comment). GUESS = argument/role not verified.
#include <cstdint>
#include <cstring>
#include <cstdio>
namespace d2client_batch_5fd20 {
using W = char16_t;
struct Host {
 virtual ~Host() = default;
 virtual const W* str(int32_t id) = 0;               // D2Lang Ordinal_10003 (string by id; id GUESS)
 virtual void drawText(const W*, int32_t a, int32_t b) = 0;   // Ordinal_10028 (GUESS: set font/color a,b)
 virtual void emit(const W*, int32_t, int32_t) = 0;  // Ordinal_10150 (GUESS: draw text at x,y)
 virtual int32_t textWidth(const W*) = 0;            // Ordinal_10171
 virtual void setFont(int32_t) = 0;                  // Ordinal_10184 (GUESS)
};
static void toW(W* d, const char* s, int n) { int i = 0; for (; i < n - 1 && s[i]; i++) d[i] = (uint8_t)s[i]; d[i] = 0; }
static unsigned wlen(const W* s) { unsigned n = 0; while (s[n]) n++; return n; }
struct Globals { int32_t& lineY; uint8_t mode; const uint8_t* game; };

// The four status-line writers 0x5FD20, 0x5FE10, 0x5FE90, 0x5FF70 share one tail: width := D35A(ECX = buf), then
// D37E(ECX = buf, EDX = rightEdge - width - 0x10; stack lineY, 4, 0), then lineY += 0x10 (rightEdge = DAT_6fba9e14).
struct Line {
 virtual ~Line() = default;
 virtual const W* str(int32_t id) = 0;                                 // D2Lang Ordinal_10003 (fastcall ECX = id)
 virtual const char* narrow(int32_t n) = 0;                            // 0xC01C calls Ordinal_10013 (no stack argument; n is toUnicode's max)
 virtual void fmt(W* buf, int32_t n, const W* f, const W* a) = 0;      // Unicode::sprintf(n, buf, f, a)
 virtual int32_t measure(const W* buf) = 0;                            // 0xD35A (ECX = buf)
 virtual void draw(const W* buf, int32_t x, int32_t y) = 0;            // 0xD37E (ECX = buf, EDX = x; stack y, 4, 0)
};
static void tail(Line& h, const W* buf, int32_t rightEdge, int32_t& lineY) {
 int32_t w = h.measure(buf);
 h.draw(buf, rightEdge - w - 0x10, lineY);
 lineY += 0x10;
}
// 1.13c D2Client+0x5FD20  mode = DAT_6fbcc390 (1 selects string 0x1423, 2 selects 0x1422); fatal = the length >= 300 branch (Ordinal_10265 + _exit)
void sub_5FD20(Line& h, uint8_t mode, int32_t rightEdge, int32_t& lineY, bool& fatal) {
 if (mode != 1 && mode != 2) return;
 const int32_t id = mode == 1 ? 0x1423 : 0x1422;
 W buf[300] = {};
 unsigned a = wlen(h.str(id)), b = wlen(h.str(0x1057));
 if (299 < a + 3 + b) { fatal = true; return; }
 const W* g = h.str(id); const W* f = h.str(0x1057);   // sprintf(300, buf, fmt = string 0x1057, arg = string id)
 h.fmt(buf, 300, f, g);
 tail(h, buf, rightEdge, lineY);
}
// 1.13c D2Client+0x5FE10: the Ordinal_10013(300) string widened (Unicode::toUnicode, 300)
void sub_5FE10(Line& h, int32_t rightEdge, int32_t& lineY) {
 W buf[300] = {}; toW(buf, h.narrow(300), 300);
 tail(h, buf, rightEdge, lineY);
}
// 1.13c D2Client+0x5FE90: obj = DAT_6fbcc378 (vtable +0x5c version, +0xb0 build char); showBuild = *(DAT_6fbcb980 + 0x21f) != 0
void sub_5FE90(Line& h, int32_t rightEdge, bool haveIf, int32_t ver, bool showBuild, int8_t build, int32_t& lineY) {
 if (haveIf) {
  char t[100];
  if (!showBuild) std::snprintf(t, sizeof t, "v %d.%02d", ver / 100 + 1, ver % 100);
  else std::snprintf(t, sizeof t, "v %d.%02d %d", ver / 100 + 1, ver % 100, (int)build);
  W buf[100] = {}; toW(buf, t, 100);
  tail(h, buf, rightEdge, lineY);
 } else lineY += 0x10;
}
// 1.13c D2Client+0x5FF70: Unicode::strcpy of string 0x58CA into a 50-char buffer
void sub_5FF70(Line& h, int32_t rightEdge, int32_t& lineY) {
 W buf[50] = {}; const W* s = h.str(0x58ca); unsigned n = wlen(s); if (n > 49) n = 49; std::memcpy(buf, s, n * 2);
 tail(h, buf, rightEdge, lineY);
}
// 1.13c D2Client+0x5FFE0  game = DAT_6fbcb980 (names at +0x1b, +0x33, +0x241); each present name is widened (toUnicode, 25), prefixed by string 0x1055
// (+0x1b) or 0x1056 (+0x241), and written through the shared measure/draw tail.
static void nameLine(Line& h, int32_t id, const char* name, int32_t rightEdge, int32_t& lineY) {
 W nm[25] = {}, buf[100] = {}; toW(nm, name, 25);
 const W* p = h.str(id); unsigned n = wlen(p); if (n > 99) n = 99; std::memcpy(buf, p, n * 2);
 for (unsigned i = 0; n + i < 99 && nm[i]; i++) buf[n + i] = nm[i];
 tail(h, buf, rightEdge, lineY);
}
void sub_5FFE0(Line& h, const uint8_t* game, int32_t rightEdge, int32_t& lineY) {
 if (!game) return;
 bool a = game[0x1b] != 0, b = game[0x33] != 0;   // empty string => absent
 if (!a && !b) return;
 if (a) nameLine(h, 0x1055, (const char*)game + 0x1b, rightEdge, lineY);
 if (game[0x241]) nameLine(h, 0x1056, (const char*)game + 0x241, rightEdge, lineY);
}
// 1.13c D2Client+0x601A0  (string arrives in EBX: unaff_EBX; param_1 unused GUESS)
void sub_601A0(Host& h, const char* s /*EBX*/, int32_t y /*param_2*/, uint8_t color /*param_3*/) {
 if (!s[0]) return;
 if (color > 0xc) color = 0;
 h.setFont(0);   // Ordinal_10184 args GUESS
 W buf[24] = {}; toW(buf, s, 23);
 h.drawText(buf, color, 0); h.emit(buf, y - 10, 0);
 h.setFont(0);
}
// 1.13c D2Client+0x60510  record append to the save-index file (EBX = record dword array; file-open helper 0x5F3E0).
// File ops are the retail thunks: C028 seek(handle ECX, pos EDX), C034 read(ECX, buf EDX, len, &got), C046 write(ECX, buf EDX, len, &put),
// C03A close(ECX), kernel32 GetFileSize(handle, 0). The file is a 100-dword table of chain heads, then records: each record is
// {next=0, rec[0..6], rec[7..10] blobs sized by rec[3..6]}; the table slot rec[0] heads a singly-linked chain by file offset.
struct FileEnv {
 int32_t (*open)();
 void (*seek)(int32_t h, int32_t pos);
 void (*read)(int32_t h, void* buf, int32_t len, int32_t* got);
 void (*write)(int32_t h, const void* buf, int32_t len);
 uint32_t (*size)(int32_t h);
 void (*close)(int32_t h);
};
void sub_60510(const FileEnv& f, const int32_t* rec /*EBX*/) {
 const int32_t h = f.open();
 if (h == -1) return;
 uint32_t tab[100] = {};
 int32_t got = 0;
 f.read(h, tab, 0x190, &got);
 if (got != 0x190) { std::memset(tab, 0, sizeof tab); f.seek(h, 0); f.write(h, tab, 0x190); }
 uint32_t cur = tab[rec[0]];
 uint32_t sz = f.size(h);
 if (cur == 0) tab[rec[0]] = sz;
 else {
  uint32_t last;
  do { f.seek(h, (int32_t)cur); last = cur; f.read(h, &cur, 4, &got); } while (cur != 0);
  f.seek(h, (int32_t)last); f.write(h, &sz, 4);
 }
 f.seek(h, 0); f.write(h, tab, 0x190);
 f.seek(h, (int32_t)sz);
 uint32_t zero = 0; f.write(h, &zero, 4);
 for (int i = 0; i < 7; i++) f.write(h, &rec[i], 4);
 for (int i = 3; i < 7; i++) if (rec[i]) f.write(h, (const void*)(uintptr_t)rec[i + 4], rec[i]);
 f.close(h);
}
// 1.13c D2Client+0x60AA0  (DAT_6fbcbbfc = handle; Ordinal_10331 lookup, 10826 + FUN_6fb6e240 GUESS)
void sub_60AA0(Host& h, int32_t looked /*Ordinal_10331(handle)*/, int32_t handle, int32_t& lineY) {
 if (!handle || !looked) return;
 // Ordinal_10826(looked); FUN_6fb6e240() -- side effects supplied by host (GUESS)
 W buf[1] = {};
 h.drawText(buf, 4, 0); h.emit(buf, 0, 0); lineY += 0x10;
}
// 1.13c D2Client+0x645A0  options menu table and art init. Table addresses are image addresses minus the 6FAB0000 base (RVAs).
struct UiInit {
 virtual ~UiInit() = default;
 virtual void table(uint32_t cntRva, uint32_t entsRva) = 0;   // D2Client+0x63E90(count*, entries*)
 virtual int32_t loadCel(const char* path) = 0;               // D2Client+0x2B420(path, 0)
 virtual int32_t expansion() = 0;                             // Ordinal_10227
 virtual uint32_t count(uint32_t h) = 0;                      // Ordinal_10086(DAT_6fbcb9c0)
 virtual int32_t entryWidth(uint32_t* req) = 0;               // Ordinal_10060(Ordinal_10021(req)); req = 0x48-byte request, req[0]=index, req[13]=handle
};
// out: [0]=+0xCC05C [1]=+0xCC060 [2]=+0xCC070 [3]=+0xCC074 [4]=+0xCC078 [5]=+0xCC07C; h = +0xBCB9C0
void sub_645A0(UiInit& u, uint32_t h, int32_t out[6]) {
 u.table(0xe2ac0, 0xe2ad8); u.table(0xe3ac8, 0xe3ae0); u.table(0xe5570, 0xe5588);
 if (!u.expansion()) { u.table(0xe8008, 0xe8020); u.table(0xecfe8, 0xed000); }
 else { u.table(0xea550, 0xea568); u.table(0xeefe0, 0xeeff8); }
 out[0] = 0xe2ac0; out[1] = 0xe2ad8;
 out[2] = u.loadCel("DATA\\GLOBAL\\UI\\Widgets\\OptBar");
 out[3] = u.loadCel("DATA\\GLOBAL\\UI\\Widgets\\OptBarC");
 out[4] = u.loadCel("DATA\\GLOBAL\\UI\\Widgets\\OptSkull");
 uint32_t req[18] = {}; req[13] = h;
 int32_t m = 0; uint32_t n = u.count(h);
 for (req[0] = 0; req[0] < n; req[0]++) { int32_t w = u.entryWidth(req); if (m <= w) m = w; }
 out[5] = m;
}
// 1.13c D2Client+0x67940  destructor: vtable reset, delete owned member [2]
struct Obj { const void* vt; int32_t f1; struct Del { virtual void d(int) = 0; }* owned; };
void sub_67940(Obj* o) { if (o->owned) o->owned->d(1); /* vtable PTR_LAB_6fb85048 restored */ }
// 1.13c D2Client+0x679F0  press handler; this = panel (+0x180708 x,+0x18070c y,+0x180710 w,+0x180714 h)
struct Panel {
 int32_t x, y, w, h, sbX, sbY, active, sbDown, itemDown, trackDown, trackY, hasList, listSel, sbState /*+0x18076e, cleared by both button presses*/;
 int32_t (*hitItem)(Panel*, const uint16_t*); int32_t (*hitTrack)(const uint16_t*); int32_t (*listIdx)(Panel*);
};
int32_t sub_679F0(Panel* p, const uint16_t* ev /*mouse x at +0xc, y at +0xe*/) {
 int mx = ev[0], my = ev[1];
 if (mx <= p->x || p->w + p->x <= mx || my <= p->y || p->h + p->y <= my) return 0;
 p->active = 1;
 if (p->sbX < mx && mx < p->sbX + 0x10 && p->sbY - 0xc < my && my < p->sbY) { p->sbDown = 1; p->sbState = 0; return 1; }
 if (p->hitItem(p, ev) == 1) { p->itemDown = 1; p->sbState = 0; return 1; }
 if (p->hitTrack(ev) == 1) { p->trackDown = 1; p->trackY = my; return 1; }
 if (p->hasList) p->listSel = p->listIdx(p);
 return 1;
}
// 1.13c D2Client+0x67AF0  (EBX = list object, stack: text, a1, a2, a3). The limit is *(this+0x180710) - 0x32 (limitField is that dword).
// Copy the text into a 1024-slot buffer, measure it (Ordinal_10171 via 0xD3E4: ECX = buffer, EDX = its length); a width at or past the limit measures
// once more and truncates the last character until the width is <= limit; draw (0xD37E: ECX = buffer, EDX = a1, words a2, a3, 0). The text is clamped to
// 1023 characters here (retail overflows its buffer past 1024: not a real input).
struct Env67AF0 {
 int32_t (*measure)(const W* buf, int32_t len);
 void (*draw)(const W* buf, int32_t a1, int32_t a2, int32_t a3);
};
void sub_67AF0(const Env67AF0& e, const W* text, int32_t limitField, int32_t a1, int32_t a2, int32_t a3) {
 W buf[1024] = {}; unsigned n = wlen(text); if (n > 1023) n = 1023; std::memcpy(buf, text, n * 2);
 int32_t lim = limitField - 0x32;
 int32_t w = e.measure(buf, (int32_t)wlen(buf));
 if (w >= lim) {
  w = e.measure(buf, (int32_t)wlen(buf));
  while (w > lim) { buf[wlen(buf) - 1] = 0; w = e.measure(buf, (int32_t)wlen(buf)); }
 }
 e.draw(buf, a1, a2, a3);
}
}
