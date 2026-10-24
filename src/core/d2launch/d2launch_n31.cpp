// D2Launch n31 batch: BNet connect/disconnect, d2s header probes, loading-screen status.
// Retail globals live in State; Windows/Ordinal calls are Host seams (args).
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <string>
#include <functional>

namespace d2launch_n31 {

struct BnClient { std::function<int(int)> vt; };  // vt(offset): 0x10 disconnect, 0x4c, 0x48, 0x28, 0x68, 0x80, 0x98...

struct Host {
  std::function<void(const char*)> debugOut;       // OutputDebugStringA
  std::function<void()> o10018;                    // Ordinal_10018
  std::function<void(const char*)> messageBox;     // MessageBoxA "D2Multi" MB_ICONERROR
  std::function<void(int)> fatal;                  // Ordinal_10265(id)+Ordinal_10024+_exit(-1)
  std::function<bool(void**, int*, int*)> loadBnclient;  // iface, stage(0 ok,1 load,2 QueryInterface), err
  std::function<std::u16string(int)> str;          // Ordinal_10003 string by id
  std::function<void(const std::u16string&, std::function<void()>, int)> dlgOk;       // FUN_6fa55090
  std::function<void()> dlgInfo;                   // FUN_6fa555b0
  std::function<void(const std::u16string&, const std::u16string&, const std::u16string&)> dlgYesNo;  // FUN_6fa55400
  std::function<void(const std::u16string&)> dlg6;  // FUN_6fa55230
  std::function<uint32_t()> tick;                  // GetTickCount
  std::function<int(int)> ctl;                     // BNclient vtable-free call by offset
};

struct State {
  BnClient* bn = nullptr;            // DAT_6fa656c4
  uint8_t* ctx = nullptr;            // DAT_6fa65abc
  int ea0 = 0, ea8 = 0, eb0 = 0, e6c = 0, dac = 0, a294 = 0;
  uint8_t d2sOut[0x14f + 0x20];      // DAT_6fa65570
  int cnt = 0; int32_t list[0x40];   // DAT_6fa65e34 / DAT_6fa653c0
  int32_t hStatus = 0, hStatus2 = 0, strId = -1, a554 = 0; uint32_t startTick = 0;  // 65548/6554c/65550/65554/65558
  uint8_t classDefault[16] = {0};    // DAT_6fa64d9c
};

// 1.13c D2Launch+0xE250
void FUN_6fa4e250(State& s, Host& h) {
  if (!s.bn) return;
  s.eb0 = 1;
  s.ctx[0x35d] = 0; s.ctx[0x37d] = 0;
  h.debugOut("Disconnect from BNET\n");
  h.o10018();
  s.bn->vt(0x10);
}

// 1.13c D2Launch+0xE320
int FUN_6fa4e320(State& s, Host& h, uint8_t* p) {
  if (!p) { h.fatal(0x4d); return 0; }
  if (*(void**)(p + 0x221) == nullptr) {
    void* iface = nullptr; int stage = 0, err = 0;
    h.loadBnclient(&iface, &stage, &err);
    char b[100];
    if (stage == 1) { snprintf(b, sizeof b, "Cannot load Bnclient.dll: Error %d", err); h.messageBox(b); return 0; }
    if (stage == 2) { snprintf(b, sizeof b, "Unable to QueryInterface for Bnclient.dll: Error %d", err); h.messageBox(b); return 0; }
    s.bn = (BnClient*)iface;
    *(void**)(p + 0x221) = iface;
    s.bn->vt(0);
  }
  return 1;
}

// 1.13c D2Launch+0xE510  (msg = param_1[0], arg = param_1[1] as utf8/win text)
int FUN_6fa4e510(State& s, Host& h, const uint32_t* msg, const char* arg, const std::function<std::u16string(const char*)>& win2uni) {
  bool disc = true; int dlgStr = -1, titleStr = -1, kind;  // kind 0 ok-cb, 1 info, 2 yesno
  std::function<void()> cb = nullptr;  // FUN_6fa57df0 (default) / FUN_6fa59290 / LAB_6fa57780
  int cbSel = 0;                       // 0 57df0, 1 59290, 2 57780
  std::u16string t1 = h.str(0), t2, t3;
  switch (msg[0]) {
    case 1: dlgStr = 0x147d; titleStr = 0x1444; kind = 2; break;
    case 2: dlgStr = 0x147e; kind = 1; break;
    case 3: case 7: dlgStr = 0x147f; titleStr = 0x144a; kind = 2; break;
    case 4: case 8: case 5: case 9: case 0xb: dlgStr = 0x147f; kind = 1; break;
    case 6: case 10:
      if (!arg) { h.fatal(0x2ad); return 0; }
      h.dlg6(win2uni(arg)); return 0;
    case 0xc: dlgStr = 0x147f; titleStr = 0x1446; kind = 2; break;
    case 0xd: dlgStr = 0x147c; kind = 1; disc = false; cbSel = 1; break;
    case 0xe:
      if (h.ctl(-1) == 0) s.dac++;                  // Ordinal_508 compare of last-error string
      else { s.dac = 1; h.ctl(-2); }                // Ordinal_501 remember string
      disc = false; dlgStr = 0x147c; kind = 1; cbSel = 1;
      if (s.dac > 1) s.e6c = 1;
      break;
    case 0xf: dlgStr = 0x147c; kind = 1; disc = false; cbSel = 1; break;
    case 0x10: case 0x11: case 0x12: dlgStr = 0x1480; kind = 1; break;
    case 0x13: case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: case 0x19: case 0x1a:
      dlgStr = 0x1481; kind = 1; cbSel = 2; disc = false; break;
    case 0x1b: h.dlgInfo(); return 0;
    default: dlgStr = 0x1480; titleStr = 0x1446; kind = 2; break;
  }
  (void)cb; (void)cbSel;
  t1 = h.str(0x1445);                                // Ordinal_10003 base string (id lost in decompile)
  if (titleStr != -1) t2 = h.str(titleStr);
  if (dlgStr != -1) t3 = h.str(dlgStr);
  if (kind == 0) h.dlgOk(t1, cb, 1);
  else if (kind == 1) h.dlgInfo();
  else h.dlgYesNo(t3, t1, t2);
  if (disc) { h.o10018(); s.bn->vt(0x10); }
  return 0;
}

// 1.13c D2Launch+0xEA90
void FUN_6fa4ea90(State& s, Host& h, const std::function<std::u16string(const char*)>& w) {
  uint32_t m[2] = {0, 0}; FUN_6fa4e510(s, h, m, nullptr, w);
}

// 1.13c D2Launch+0xEE10  (edi = cmdline-restart flag; gw callbacks as seams)
struct ConnSeams {
  std::function<void()> fc40, f582d0, f57df0, f549e0, saveGateway, terminate, o10014a, o10014b, o10032;
  std::function<void(const char*, const char*, int, int)> o426;
  int ad4 = 0; int a0 = 0; int a4 = 0;               // DAT_6fa65ad4, DAT_6fa65ea0 , DAT_6fa65ea4
  int errCode = 0;                                   // aiStack_c[0] after call 0x28
};
int FUN_6fa4ee10(State& s, Host& h, ConnSeams& c, const std::function<std::u16string(const char*)>& w, int edi) {
  if (!s.bn) return 0;
  if (s.ea8) { h.fatal(0x128); return 0; }
  s.ea8 = 1; s.eb0 = 1; s.a294 = 0;
  h.debugOut("------------------\nConnect to Battle.net\n------------------\n");
  h.o10018();
  if (s.bn->vt(0x10) == 0) {
    s.ea8 = 0; uint32_t m[2] = {0, 0}; return FUN_6fa4e510(s, h, m, nullptr, w);
  }
  c.a4 = 0; c.fc40(); c.o10014a();
  if (c.a0 == 0) {
    if (s.bn->vt(0x4c) != 0x66) {
      c.f582d0(); c.o10014b();
      if (s.a294 != 0 || s.bn->vt(0x4c) != 0x65) {
        s.bn->vt(0x10); s.ea8 = 0; c.f57df0(); return 0;
      }
      int r = s.bn->vt(0x48);
      if (edi != 0) {
        c.o426("Diablo II", "UseCmdLine", 0, 1);
        c.o426("Diablo II", "Skip To Open", 0, c.ad4 == 3);
        c.o10032(); s.bn->vt(0x10); s.ea8 = 0; s.bn->vt(0x98);
        c.saveGateway(); c.terminate(); return 0;
      }
      if (r == 0) { s.ea8 = 0; uint32_t m[2] = {0x12, 0}; return FUN_6fa4e510(s, h, m, nullptr, w); }
    }
    c.f549e0(); s.bn->vt(0x28); s.ea8 = 0;
    if (c.errCode == 0) return s.bn->vt(0x68) != 0;
    uint32_t m[2] = {(uint32_t)c.errCode, 0}; return FUN_6fa4e510(s, h, m, nullptr, w);
  }
  c.f549e0(); s.bn->vt(0x10); s.ea8 = 0; return 0;
}

// 1.13c D2Launch+0xF030  (buf = 0x2000 read of "<dir><name>.d2s"; returns result of write)
struct SaveSeams {
  std::function<bool()> open;                         // Ordinal_10107
  std::function<int(uint8_t*, int)> read;             // Ordinal_10110
  std::function<void()> seek0, close;                 // 10112, 10109
  std::function<int(int, uint8_t*, int)> write;       // Ordinal_10111(mode, buf, 0)
  std::function<uint32_t(uint8_t*, int)> checksum;    // Ordinal_10229
};
int FUN_6fa4f030(SaveSeams& q, uint8_t* buf /*0x2000*/) {
  if (!q.open()) return 0;
  int n = q.read(buf, 0x2000);
  uint32_t magic, ver; memcpy(&magic, buf, 4); memcpy(&ver, buf + 4, 4);
  if (n < 8 || magic != 0xaa55aa55u) return 0;
  uint32_t f;
  if (ver < 0x5c) {
    if (n <= 0x81) return 0;
    memcpy(&f, buf + 0x14, 4); f |= 0x20; memcpy(buf + 0x14, &f, 4);
    q.seek0(); int r = q.write(4, buf, 0); q.close(); return r;
  }
  if (n <= 0x14e) return 0;
  memcpy(&f, buf + 0x18, 4); f |= 0x20; memcpy(buf + 0x18, &f, 4);
  memset(buf + 0xc, 0, 4);
  uint32_t cs = q.checksum(buf, n); memcpy(buf + 0xc, &cs, 4);
  q.seek0(); int r = q.write(0, buf, 0); q.close(); return r;
}

// 1.13c D2Launch+0xF190
int FUN_6fa4f190(uint8_t* out, const char* name, const uint8_t* classDefault, const std::function<int(uint8_t*, int)>& readSave,
                 std::function<void(int)> fatal, uint8_t esiHi, uint16_t ebx) {
  if (!*name) { fatal(0xf9); return 0; }
  static uint8_t b[0x2000]; int n = readSave(b, 0x2000);
  if (n < 0) return 0;
  uint32_t mg, ver; memcpy(&mg, b, 4); memcpy(&ver, b + 4, 4);
  if (n <= 7 || mg != 0xaa55aa55u) return 0;
  const uint8_t *p7, *p10; uint8_t cls;
  if (ver < 0x5c) { if (n < 0x82 || ver < 0x47 || ver > 0x5b) return 0; p7 = b + 0x26; p10 = p7 + 0x10; cls = b[0x22]; }
  else { if (n < 0x14f || ver < 0x5c || ver > 0x60) return 0; p7 = b + 0x88; p10 = p7 + 0x10; cls = b[0x28]; }
  out[0] = (uint8_t)(ebx & 0xff); out[1] = (uint8_t)(ebx >> 8);
  uint8_t* d = out + 2;
  if (*p7 == 0) { memcpy(d, classDefault, 11); d += 11; }
  else { memcpy(d, p7, 11); d += 11; }
  *d = cls + 1; memcpy(d + 1, p10, 11); d[12] = esiHi;
  d[13] = 0; d[14] = 0; d[15] = 0; d[16] = 0;
  d[17] = d[18] = d[19] = 0xff; d[20] = 0;
  return 1;
}

// 1.13c D2Launch+0xF5B0  (out = in_EAX; cls = param_1; name = param_2; flagsLo = unaff_BX)
void FUN_6fa4f5b0(State& s, uint8_t* out, char cls, const char* name, uint16_t flagsLo) {
  uint8_t* o = s.d2sOut; memset(o, 0, 0x14f);
  uint32_t v = 0xaa55aa55; memcpy(o, &v, 4); v = 0x60; memcpy(o + 4, &v, 4);
  strncpy((char*)o + 0x14, name, 0x10);
  uint16_t fl = flagsLo | 1; o[0x2b] = 1; o[0x28] = cls;
  uint32_t t = (uint32_t)time(nullptr); memcpy(o + 0x2c, &t, 4);
  v = 0x14f; memcpy(o + 8, &v, 4); o[0x29] = 0x10; o[0x2a] = 0x1e;
  if (cls == 6 || cls == 5) fl |= 0x20;
  memcpy(o + 0x24, &fl, 2); memcpy(o + 0x30, &t, 4);
  memcpy(o + 0x88, s.classDefault, 16); memset(o + 0x98, 0xff, 16);
  memcpy(out + 2, s.classDefault, 11);
  uint8_t* p = out + 13; p[0] = cls + 1; memset(p + 1, 0xff, 11); p[12] = 1;
  p[13] = (uint8_t)fl; p[14] = (uint8_t)(fl >> 8); p[15] = 0; p[16] = 0;
  p[17] = p[18] = p[19] = 0xff; p[20] = 0;
}

// 1.13c D2Launch+0xF800
void FUN_6fa4f800(State& s, Host& h) {
  if (!s.hStatus) return;
  switch (s.bn->vt(0x80)) {
    case 0: s.strId = 0x1433; break; case 1: s.strId = 0x1434; break; case 2: s.strId = 0x1435; break;
    case 3: s.strId = 0x1436; break; case 4: s.strId = 0x1437; break; case 5: s.strId = 0x1438; break;
    case 6: s.strId = 0x1439; break; default: s.strId = -1;
  }
  if (s.strId > -1) h.str(s.strId);
  char dots[256];
  snprintf(dots, sizeof dots, "%.*s", (int)(((h.tick() - s.startTick) / 0xfa) % 0x14) + 1, "....................");
  h.dlgInfo();
}

// 1.13c D2Launch+0xFC40
void FUN_6fa4fc40(State& s, Host& h, const std::function<int32_t(int)>& mkCtl, const std::function<void(int32_t)>& del) {
  s.startTick = h.tick();
  for (int i = 0; i < s.cnt; i++) del(s.list[i]);
  s.cnt = 0;
  for (int k = 0; k < 5; k++) {
    if (s.cnt >= 0x40) { h.fatal(0x60d); return; }
    int32_t c = mkCtl(k); s.list[s.cnt++] = c;
    if (k == 3) s.hStatus = c; else if (k == 4) s.hStatus2 = c;
  }
  s.strId = -1; s.a554 = 0;
  FUN_6fa4f800(s, h);
}

// 1.13c D2Launch+0x110C0  (screenMode = Ordinal_10009(); tbl = the 16 layout words in retail order)
void FUN_6fa510c0(int screenMode, int32_t* const tbl[16]) {
  static const int32_t v[16] = {0x9b,0x14e,0x184,0x1ba,0xf0,0x158,0x9c,0x1fa,0x22b,0x23f,0x22b,0x22b,0x23f,0x253,0x46,0xee};
  if (screenMode != 9) return;
  for (int i = 0; i < 16; i++) *tbl[i] = v[i];
}

// 1.13c D2Launch+0x11AA0
void FUN_6fa51aa0(State& s, char m) {
  uint8_t* c = s.ctx; c[0x1ea] = m;
  for (int o = 0x81; o <= 0x87; o++) c[o] = 0;
  static const uint8_t off[7] = {0x81, 0x83, 0x84, 0x82, 0x85, 0x86, 0x87};  // mode 0..6
  if (m >= 0 && m <= 6) c[off[(int)m]] = 1;
}

}  // namespace d2launch_n31
