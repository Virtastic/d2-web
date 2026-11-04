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

// 1.13c D2Client+0x5FD20  (DAT_6fbcc390 in {1,2}; DAT_6fbcc21c = lineY)
void sub_5FD20(Host& h, uint8_t mode, int32_t& lineY, bool& fatal) {
 if (mode != 1 && mode != 2) return;
 W buf[300] = {};
 unsigned a = wlen(h.str(0)), b = wlen(h.str(0));   // both Ordinal_10003 ids GUESS
 if (299 < a + 3 + b) { fatal = true; return; }     // retail: Ordinal_10265(0xb3e), _exit(-1)
 // Unicode::sprintf(300,buf,fmt) with fmt=str(str(0)) -- format args GUESS
 const W* s = h.str(0); unsigned n = wlen(s); if (n > 299) n = 299; std::memcpy(buf, s, n * 2);
 h.drawText(buf, 4, 0); h.emit(buf, 0, 0); lineY += 0x10;
}
// 1.13c D2Client+0x5FE10
void sub_5FE10(Host& h, const char* narrow /*Ordinal_10013 result GUESS*/, int32_t& lineY) {
 W buf[300] = {}; toW(buf, narrow, 300);
 h.drawText(buf, 4, 0); h.emit(buf, 0, 0); lineY += 0x10;
}
// 1.13c D2Client+0x5FE90   gameIf/version via vtable +0x5c, +0xb0 (GUESS: version, build char)
void sub_5FE90(Host& h, bool haveIf, int32_t ver, bool showBuild /*game+0x21f!=0*/, int8_t build, int32_t& lineY) {
 if (haveIf) {
  char t[100];
  if (!showBuild) std::snprintf(t, sizeof t, "v %d.%02d", ver / 100 + 1, ver % 100);
  else std::snprintf(t, sizeof t, "v %d.%02d %d", ver / 100 + 1, ver % 100, (int)build);
  W buf[100] = {}; toW(buf, t, 100);
  h.drawText(buf, 4, 0); h.emit(buf, 0, 0);
 }
 lineY += 0x10;
}
// 1.13c D2Client+0x5FF70
void sub_5FF70(Host& h, int32_t& lineY) {
 W buf[100] = {}; const W* s = h.str(0); unsigned n = wlen(s); if (n > 99) n = 99; std::memcpy(buf, s, n * 2);
 h.drawText(buf, 4, 0); h.emit(buf, 0, 0); lineY += 0x10;
}
// 1.13c D2Client+0x5FFE0  game = DAT_6fbcb980 (names at +0x1b, +0x33, +0x241 -- field meaning GUESS)
static void line(Host& h, const char* name, int32_t& lineY) {
 W nm[26] = {}, buf[100] = {}; toW(nm, name, 25);
 const W* p = h.str(0); unsigned n = wlen(p); if (n > 99) n = 99; std::memcpy(buf, p, n * 2);
 for (unsigned i = 0; n + i < 99 && nm[i]; i++) buf[n + i] = nm[i];
 h.drawText(buf, 4, 0); h.emit(buf, 0, 0); lineY += 0x10;
}
void sub_5FFE0(Host& h, const uint8_t* game, int32_t& lineY) {
 if (!game) return;
 bool a = game[0x1b] != 0, b = game[0x33] != 0;   // empty string => absent (retail strlen==1 check)
 if (!a && !b) return;
 if (a) line(h, (const char*)game + 0x1b, lineY);
 if (game[0x241]) line(h, (const char*)game + 0x241, lineY);
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
// 1.13c D2Client+0x60510  file-offset table update; Ordinals 10109..10112 = Storm file ops (names GUESS).
// Reaches the table through EBX/EDI registers; semantics unverified, so the control flow is kept as an interface.
struct FileOps {
 virtual ~FileOps() = default;
 virtual long open() = 0;                        // FUN_6fb0f3e0, -1 on fail
 virtual void read(void* p, int n) = 0;          // Ordinal_10110 GUESS
 virtual void seek(int32_t pos) = 0;             // Ordinal_10112 GUESS
 virtual void write(const void* p, int n) = 0;   // Ordinal_10111 GUESS
 virtual uint32_t size() = 0;                    // GetFileSize
 virtual void close() = 0;                       // Ordinal_10109
};
void sub_60510(FileOps& f, const int32_t* rec /*EBX*/, int32_t edi /*EDI*/) {
 if (f.open() == -1) return;
 uint32_t tab[104] = {};
 f.read(tab, 4);
 if (edi != 400) { std::memset(tab, 0, 400); f.seek(0); f.read(tab, 400); }
 uint32_t cur = tab[rec[0]], sz = f.size();
 if (cur == 0) tab[rec[0]] = sz;
 else { do { f.seek(0); f.read(&cur, 4); } while (cur != 0); f.seek(0); f.write(&cur, 4); }
 f.seek(0); f.write(tab, 400);
 for (int i = 0; i < 6; i++) f.write(&rec[i], 4);   // six 4-byte header writes (sources GUESS)
 f.write(&rec[1], 4);
 for (int i = 3; i < 7; i++) if (rec[i]) f.write(&rec[i], rec[i]);   // sized blobs (pointers GUESS)
 f.close();
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
// 1.13c D2Client+0x67AF0  truncate text until width <= panelW-0x32, then draw (x=param_3,y=param_4)
void sub_67AF0(Host& h, const W* text, int32_t panelW, int32_t x, int32_t y) {
 W buf[1024] = {}; unsigned n = wlen(text); if (n > 1023) n = 1023; std::memcpy(buf, text, n * 2);
 while (panelW - 0x32 < h.textWidth(buf) && wlen(buf)) buf[wlen(buf) - 1] = 0;
 h.emit(buf, x, y);
}
}
