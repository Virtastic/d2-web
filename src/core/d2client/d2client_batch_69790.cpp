// Prior art: none (D2MOO has no D2Client module); transcribed from Ghidra decompile of 1.13c D2Client.
// Gfx.cpp resource-cache / per-unit cel-list helpers. Imports are abstract Services; args marked GUESS are unverified.
#include <cstdint>
#include <cstring>
namespace d2client_batch_69790 {
struct CacheNode { CacheNode* next; uint32_t flags; char name[0xc0]; void* data; /* +0x14 per decomp: puVar[5] */ };
struct Link { int32_t id, kind; void* cache; int32_t pad; Link* next; };   // 0x434-byte alloc, 5 ints used
struct Holder { uint8_t pad[0x28]; Link* links; };                         // *(unit+0x54)
struct Unit { int32_t type, classId; uint8_t pad[0x4c]; uint8_t* flags54; };  // GUESS layout: only +0x54 is read
struct Services {
 virtual ~Services()=default;
 [[noreturn]] virtual void fatal(int32_t)=0;
 virtual int32_t currentId()=0;                         // D2Client+0x1AF0
 virtual int32_t mode()=0;                              // D2Client+0x1A80
 virtual void* find(const char*,void* table)=0;         // +0x69350
 virtual void* load(void* table)=0;                     // +0x69970
 virtual int32_t canonId(int32_t)=0;                    // +0x69090
 virtual bool lookupName(void*,int32_t)=0;              // +0x69010
 virtual void a69850(int32_t)=0; virtual void a69830(int32_t)=0; virtual void a69770(int32_t)=0;
 virtual void* allocLink()=0;
 virtual bool pending(int32_t)=0; virtual void flush()=0;
 // used by 0x69CC0
 virtual void* root44c8()=0;                                        // &DAT_6fbc44c8, the component cache root
 virtual void nameOf(int32_t hdr0,int32_t* out,int32_t a1,char* name)=0;   // 0x69410 (EAX = hdr0, EDI = out; stack a1, name)
 virtual void* loadNamed(const char* name,void* table)=0;          // 0x69970 (EAX = name; stack table)
 virtual const uint8_t* compOrder(const uint8_t* list,int32_t a,int32_t b)=0;  // 0x6D6D0 (EAX = list)
 virtual int32_t fillRec(int32_t key,void* node,uint32_t sub,uint32_t* rec,int32_t a1,int16_t* hdr,void* a0)=0; // 0x6DD30 (EAX, ECX, EDX, ESI)
 virtual int32_t cmpLoad(uint32_t* rec,int32_t a,int32_t b)=0;     // D2CMP#10005
 virtual void cmpRelease(uint32_t* rec)=0;                         // D2CMP#10103
};
// 1.13c D2Client+0x69790
void clearHolder(Services& s, int32_t n, Unit* u) {
 if (n > 0xf) s.fatal(0x65f);
 if (u && u->flags54) for (int i = 0; i < 0x20; i += 2) u->flags54[i] = 0;
 if (!u || !u->flags54) s.fatal(0x668);
 Holder* h = reinterpret_cast<Holder*>(u->flags54);
 for (Link* l = h->links; l; l = l->next) { /* Ordinal_10043(0x66d,0) frees each node */ }
 h->links = nullptr;
}
// 1.13c D2Client+0x69870 (GUESS: eax=id, ecx=unit, out=*out)
int32_t findLinkOrLookup(Services& s, Unit* u, int32_t kind, int32_t id, Link** out, uint32_t* name) {
 if (!u || !u->flags54) s.fatal(0x3f0);
 if (id == -1) id = s.currentId();
 Holder* h = reinterpret_cast<Holder*>(u->flags54);
 Link* l = h->links;
 while (l && l->id != id) l = l->next;
 if (!l) return 1;
 if (l->kind == kind) { *out = l; return 0; }
 *out = nullptr; *name = (uint32_t)s.canonId(id);
 if (s.lookupName(name, 0)) return 1;  // GUESS second arg
 s.fatal(0x406);
}
// 1.13c D2Client+0x69910
Link* findLink(Services& s, Unit* u, int32_t id, int32_t kind) {
 if (!u || !u->flags54) s.fatal(0x3ac);
 if (id == -1) id = s.currentId();
 for (Link* l = reinterpret_cast<Holder*>(u->flags54)->links; l; l = l->next)
  if (l->id == id && l->kind == kind) return l;
 return nullptr;
}
// 1.13c D2Client+0x69970 (GUESS: name lower-cased by caller; table is the cache root)
void* loadCache(Services& s, void* table, const char* name, int32_t maxCount, uint8_t countByte) {
 (void)name; (void)maxCount;
 void* n = s.find(name, table);
 if (n) return n;
 n = s.load(table);
 if (n && countByte > 0x40) s.fatal(0xec);
 return n;
}
// 1.13c D2Client+0x69A90
void dispatchByMode(Services& s, int32_t slot, int32_t arg) {
 if (slot > 0xf) return;
 switch (s.mode()) { case 0: s.a69850(arg); return; case 1: s.a69830(arg); return; case 2: s.a69770(arg); return; }
 s.fatal(0xd3e);
}
// 1.13c D2Client+0x69B40 (GUESS: unit* via eax, link out via param_1)
int32_t addLink(Services& s, Unit* u, int32_t id, Link** out) {
 if (!u || !u->flags54) return 0;
 Holder* h = reinterpret_cast<Holder*>(u->flags54);
 Link* l = static_cast<Link*>(s.allocLink());
 if (!l) s.fatal(0x435);
 *l = Link{};
 void* c = s.find("", nullptr);  // GUESS: table 0x6fbc4cd8 (mode 0) or 0x6fbc44c8 (else)
 if (!c && !(c = s.load(nullptr))) return 0;
 l->id = id; l->kind = 1; l->cache = c; l->next = h->links; h->links = l;
 (void)out;
 return s.canonId(id);
}
// 1.13c D2Client+0x69CC0  (stdcall a0, a1, hdr, mode; ret 0x10). Resolves the cache node named by 0x69410 (found,
// else loaded; fatal 0xec3), takes its component list node+0x14 (fatal 0xec9) and the draw order 0x6D6D0(list,0,0)
// (fatal 0xed0). For each of list[0] slots: component = order[i] (fatal 0xed7 if >= 16); for each sub index
// 0..hdr byte 3 - 1 (once, sub 0, when that byte is 0) a zeroed 0x48-byte record with byte 5 = component is filled by
// 0x6DD30 and, if it accepts, loaded by D2CMP#10005 (mode 0: (0,0); else (1,1), released by #10103 on failure).
void spawnRecords(Services& s, void* a0, int32_t a1, int16_t* hdr, int32_t mode) {
 char name[0x104]; std::memset(name, 0, sizeof name);
 int32_t key = 0;
 s.nameOf(hdr[0], &key, a1, name);
 void* root = s.root44c8();
 void* node = s.find(name, root);
 if (!node && !(node = s.loadNamed(name, root))) s.fatal(0xec3);
 const uint8_t* list = *reinterpret_cast<const uint8_t* const*>(static_cast<const uint8_t*>(node) + 0x14);
 if (!list) s.fatal(0xec9);
 const uint8_t* order = s.compOrder(list, 0, 0);
 if (!order) s.fatal(0xed0);
 const uint8_t* subs = reinterpret_cast<const uint8_t*>(hdr) + 3;
 for (int32_t i = 0; i < (int32_t)list[0]; i++) {
  uint8_t c = order[i];
  if (c >= 0x10) s.fatal(0xed7);
  uint32_t sub = 0;
  do {
   uint32_t rec[18] = {0};
   reinterpret_cast<uint8_t*>(rec)[5] = c;
   if (s.fillRec(key, node, sub, rec, a1, hdr, a0)) {
    if (!mode) s.cmpLoad(rec, 0, 0);
    else if (!s.cmpLoad(rec, 1, 1)) { rec[13] = 0; s.cmpRelease(rec); }
   }
   sub++;
  } while (sub < *subs);
 }
}
// 1.13c D2Client+0x69F20 (GUESS)
int32_t ensureLinkMode2(Services& s, Unit* u, int32_t id) {
 if (u && u->flags54 && u->flags54[id * 2]) return 0;
 Link* l = nullptr;
 if (!addLink(s, u, id, &l)) s.fatal(0x4d4);
 if (u && u->flags54) u->flags54[id * 2] = 1;
 return 1;
}
// 1.13c D2Client+0x6A000 (GUESS)
int32_t ensureLink(Services& s, Unit* u, uint32_t id) {
 if (!s.pending((int32_t)id)) s.fatal(0x486);
 if ((!u || !u->flags54 || !u->flags54[id * 2])) {
  Link* l = nullptr;
  if (addLink(s, u, (int32_t)id, &l)) { if (u && u->flags54) u->flags54[id * 2] = 1; return 1; }
 }
 return 0;
}
// 1.13c D2Client+0x6A0C0 (GUESS: iterates DAT_6fb928b0[0..DAT_6fb928ec) palette/layer ids, spawnRecords for set bits)
void forEachSetBit(Services& s, const uint8_t* mask, const int32_t* ids, int32_t n, void* rec, int32_t cls, int32_t arg) {
 int16_t a[26] = {}; a[0] = (int16_t)cls;
 for (int i = 0; i < n; i++) if (mask[0xf0 + (ids[i] >> 3)] & (1u << (ids[i] & 7))) spawnRecords(s, rec, ids[i], a, arg);
}
// 1.13c D2Client+0x6A180 (GUESS: 0xe maps to local; 0xc also runs 0x68EA0/0x68CB0(0))
void ensureSlot(Services& s, Unit* u, uint32_t id) { if (id == 0xe) id = 0; if (!s.pending((int32_t)id)) return; if (id == 0xc) s.flush(); ensureLink(s, u, id); }
// 1.13c D2Client+0x6A2F0 (GUESS: mode 0 add, 1 ensureSlot, 2 mode2; returns findLink(-1/id))
Link* getLink(Services& s, Unit* u, int32_t id, int32_t mode) {
 if (mode == 1) { ensureSlot(s, u, (uint32_t)id); return findLink(s, u, id, 1); }
 if (mode == 0) { if (addLink(s, u, id, nullptr) && u && u->flags54) u->flags54[id * 2] = 1; return findLink(s, u, -1, 1); }
 if (mode != 2) return nullptr;
 ensureLinkMode2(s, u, id); return findLink(s, u, -1, 1);
}
}
