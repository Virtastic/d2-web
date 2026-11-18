// D2Client batch b64: 1.13c layouts read as raw bytes. Guest pointers are 32-bit values turned
// into host pointers by a Res callback; imports (ordinals, internal calls) are explicit callbacks.
// Not implemented here: 0x6CC00 (draw, many globals), 0x6DA40 (stack-switch decompile garbage).
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace d2client_b64 {
typedef const uint8_t* (*Res)(uint32_t guest);
typedef void (*Fatal)(uint32_t msgId);              // Ordinal_10265+10024+_exit(-1)
static inline int32_t rd32(const uint8_t* p, uint32_t o) { int32_t v; memcpy(&v, p + o, 4); return v; }

// 1.13c D2Client+0x69130 (EAX=pUnit hidden): is param_1 an id in unit->+0x54->+0x2c list (next +0xa4, id +4)
int sub_69130(const uint8_t* unit, int32_t id, Res r, Fatal f) {
  if (!unit || !rd32(unit, 0x54)) { f(0x1ea); return 0; }
  const uint8_t* n = r((uint32_t)rd32(r((uint32_t)rd32(unit, 0x54)), 0x2c));
  for (; n; n = r((uint32_t)rd32(n, 0xa4))) if (rd32(n, 4) == id) return 1;
  return 0;
}

// 1.13c D2Client+0x69610 (EDI=pUnit hidden): call 0x69180(node, unit) for each node, next read before the call
void sub_69610(const uint8_t* unit, Res r, Fatal f, void (*cb69180)(const uint8_t* node, const uint8_t* unit)) {
  if (!unit || !rd32(unit, 0x54)) { f(0xccf); return; }
  const uint8_t* n = r((uint32_t)rd32(r((uint32_t)rd32(unit, 0x54)), 0x2c));
  while (n) { const uint8_t* nx = r((uint32_t)rd32(n, 0xa4)); cb69180(n, unit); n = nx; }
}

// 1.13c D2Client+0x69660 (EDI=pUnit hidden; param_2=index): find node with +4==idx or type 4/9 with +8/+0xc==idx
// dt = sgptDataTables base: +0xbc0 count, +0xbbc table (0x84 stride)
void sub_69660(const uint8_t* unit, int32_t idx, const uint8_t* dt, Res r, Fatal f,
               void (*cb69180)(const uint8_t* node, const uint8_t* unit)) {
  if (!unit || !rd32(unit, 0x54)) return;
  if (idx < 0 || rd32(dt, 0xbc0) <= idx || (uint32_t)(idx * 0x84 + rd32(dt, 0xbbc)) == 0) { f(0xca9); return; }
  const uint8_t* n = r((uint32_t)rd32(r((uint32_t)rd32(unit, 0x54)), 0x2c));
  while (n) {
    int32_t t = rd32(n, 0);
    if (rd32(n, 4) == idx || ((t == 4 || t == 9) && (rd32(n, 8) == idx || rd32(n, 0xc) == idx))) { cb69180(n, unit); return; }
    n = r((uint32_t)rd32(n, 0xa4));
  }
}

// 1.13c D2Client+0x69700 (EAX=pUnit hidden): walk list at +0x54->+0x28 (next +0x10) firing Ordinal_10043(0x81a,0) each,
// then 0x68FD0 and Ordinal_10043(0x820,0)
void sub_69700(const uint8_t* unit, Res r, Fatal f, void (*ord10043)(uint32_t a, uint32_t b), void (*cb68FD0)()) {
  if (!unit || !rd32(unit, 0x54)) { f(0x814); return; }
  const uint8_t* n = r((uint32_t)rd32(r((uint32_t)rd32(unit, 0x54)), 0x28));
  while (n) { n = r((uint32_t)rd32(n, 0x10)); ord10043(0x81a, 0); }
  cb68FD0();
  ord10043(0x820, 0);
}

// 1.13c D2Client+0x6D700 (ECX=table, EDI=key hidden): table[0]=count, entries 9 bytes from +0x1c, key at +0, value at +2
uint8_t sub_6D700(const uint8_t* t, int32_t key) {
  if (!t || key >= 0x10) return 0;
  const uint8_t* p = t + 0x1c;
  for (int i = 0; i < t[0]; i++, p += 9) if (*p == (uint32_t)key) return p[2];
  return 0;
}

// 1.13c D2Client+0x6D840 (EAX=pUnit, ESI=flag, EDI=out6 hidden): copy 6 bytes from record+0x116, else zero 6
// ord11139 = unit->player, ord11006 = test, ord10695 = lookup by unit id (returns record)
void sub_6D840(const uint8_t* unit, int32_t flag, uint8_t* out, const uint8_t* (*ord11139)(), int (*ord11006)(const uint8_t*),
               uint8_t* (*ord10695)(uint32_t id)) {
  if (!unit) return;
  const uint8_t* p = ord11139();
  if (flag == 0 && ord11006(p)) {
    uint8_t* rec = ord10695(p ? (uint32_t)rd32(p, 4) : 0xffffffffu);
    if (rec) { memcpy(out, rec + 0x116, 6); return; }
  } else { memset(out, 0, 6); }
}

// 1.13c D2Client+0x6D8C0 (EAX=idx hidden; param_1=kind): kind0 ord10919(idx)->+0x20, kind1 dt+0xa78 (0x1a8) -> +0x10, kind2 ord10290 -> +0x20
uint32_t sub_6D8C0(int32_t idx, int32_t kind, const uint8_t* dt, Res r, const uint8_t* (*ord10919)(), const uint8_t* (*ord10290)()) {
  const uint8_t* rec;
  if (kind == 0) rec = ord10919();
  else if (kind == 1) {
    if (idx < 0 || rd32(dt, 0xa80) <= idx) return 0;
    uint32_t a = (uint32_t)(idx * 0x1a8 + rd32(dt, 0xa78));
    if (!a) return 0;
    return (uint32_t)rd32(r(a), 0x10);
  } else if (kind == 2) rec = ord10290();
  else return 0;
  return rec ? (uint32_t)rd32(rec, 0x20) : 0;
}

// 1.13c D2Client+0x6D920 (EAX=pUnit hidden; param_1=out struct): out+0x2c = dt+0xb64 table[unit->+4]*0x1a4 + 0x138
// tableBase = guest address stored at dt+0xb64; returns new value or 0 when unchanged
uint32_t sub_6D920(const uint8_t* unit, const uint8_t* dt, uint8_t* out) {
  if (!unit) return 0;
  int32_t i = rd32(unit, 4);
  if (i < 0 || i >= rd32(dt, 0xb6c)) return 0;
  uint32_t a = (uint32_t)(i * 0x1a4 + rd32(dt, 0xb64));
  if (!a) return 0;
  a += 0x138; memcpy(out + 0x2c, &a, 4); return a;
}

// 1.13c D2Client+0x6D960 (EAX=kind, EBX=slot, EDI=row hidden; param_1=unit): returns a flag byte per kind
// gSelf=DAT_6fbcbbfc (local player), noCtl=DAT_6fbcc054, tbl54e4=DAT_6fbc54e4 (guest ptr), ord1260/ord1580/cb6D7C0 = internal calls
int8_t sub_6D960(int32_t kind, int32_t slot, int32_t row, const uint8_t* unit, const uint8_t* gSelf, int32_t noCtl, const uint8_t* tbl54e4,
                 const uint8_t* (*c1260)(), const uint8_t* (*c1580)(), int8_t (*c6D7C0)()) {
  if (slot < 0) return 1;
  int8_t c = 1;
  switch (kind) {
    case 0: return (noCtl == 0 && unit == gSelf) ? 0x10 : 8;
    case 1: {
      const uint8_t* p = c1260();
      if (p) { c = (int8_t)p[0xf4 + slot];
        if (c == 8 && tbl54e4) { const uint8_t* e = tbl54e4 + row * 0xa0; if (rd32(e, slot * 4) == 0) return 4; } }
      break; }
    case 2: case 4: return 1;
    case 3: { const uint8_t* p = c1580(); if (p) return (int8_t)p[0x1a0]; break; }
    case 6: c = c6D7C0();
  }
  return c;
}
}  // namespace d2client_b64
