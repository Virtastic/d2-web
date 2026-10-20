// Prior art: none (D2MOO has no D2Client module); Ghidra decompile only.
// Native-pointer layouts: the 32-bit retail offsets are read through byte accessors.
// Args marked GUESS are unverified against a vector.
#include <cstdint>
#include <cstring>
namespace d2client_batch_68b80 {
template<class T> T rd(const void* p, unsigned o) { T v; std::memcpy(&v,static_cast<const uint8_t*>(p)+o,sizeof v); return v; }
template<class T> void wr(void* p, unsigned o, T v) { std::memcpy(static_cast<uint8_t*>(p)+o,&v,sizeof v); }
struct Services { // imported DLL calls / globals
 virtual ~Services()=default;
 [[noreturn]] virtual void fatal(int32_t line)=0;      // Ordinal_10265 + 10024 + _exit(-1)
 virtual void freeMem(void*,int32_t line)=0;           // Ordinal_10043(line,0) on the pointer
 virtual uint32_t hashSeed(const char*)=0;             // Ordinal_502 (GUESS: string hash)
 virtual void releaseSub(void*)=0;                     // sub_68810
 virtual void releaseNode(void*)=0;                    // sub_689F0
 virtual void releaseHandle(void*)=0;                  // sub_A8630
 virtual void notify(int32_t,void*)=0;                 // sub_19830
 virtual int32_t* totalAlloc()=0;                      // DAT 0x..CC044
};
static void* clientData(Services& s, const void* unit){ if(!unit) s.fatal(0); void* d=(void*)(uintptr_t)rd<uint64_t>(unit,0x54); if(!d) s.fatal(0); return d; }
// 1.13c D2Client+0x68B80 (esi = string)
uint32_t sub_68B80(Services& s, const char* str){
 if(std::strlen(str)>7) s.fatal(0x79);
 uint32_t h=s.hashSeed(str); return (((h>>8^h)>>16)^(h>>8)^h)&0x1ff;
}
// 1.13c D2Client+0x68BE0 and +0x68C30: free a 0x80C-byte cache block (0x203 dwords)
static void freeBlock(Services& s, uint8_t* blk, int32_t line){
 s.releaseSub(blk); *s.totalAlloc()-=rd<int32_t>(blk,8);
 if(rd<uint64_t>(blk,4)) s.freeMem((void*)(uintptr_t)rd<uint64_t>(blk,4),line);
 std::memset(blk,0,0x203*4);
}
void sub_68BE0(Services& s, uint8_t* blk){ freeBlock(s,blk,0xe61); }
void sub_68C30(Services& s, uint8_t* blk){ freeBlock(s,blk,0xe50); }
// 1.13c D2Client+0x68CB0 (eax = unit, dl = value): fill 20 words' low bytes
void sub_68CB0(Services& s, void* unit, uint8_t v){
 uint8_t* a=(uint8_t*)clientData(s,unit); for(int i=0;i<0x14;i++) a[i*2]=v;
}
// 1.13c D2Client+0x68E30 (eax = unit): drop list nodes whose type is neither 0 nor 0x11
void sub_68E30(Services& s, void* unit){
 uint8_t* d=(uint8_t*)clientData(s,unit);
 uint8_t* prev=nullptr; uint8_t* n=(uint8_t*)(uintptr_t)rd<uint64_t>(d,0x28);
 while(n){ uint8_t* next=(uint8_t*)(uintptr_t)rd<uint64_t>(n,0x10); int32_t t=rd<int32_t>(n,0);
  if(t!=0&&t!=0x11){ s.freeMem(n,0x847); if(prev) wr<uint64_t>(prev,0x10,(uintptr_t)next); else wr<uint64_t>(d,0x28,(uintptr_t)next); }
  else prev=n; n=next; }
}
// 1.13c D2Client+0x68EA0 (eax = unit): free the whole list
void sub_68EA0(Services& s, void* unit){
 uint8_t* d=(uint8_t*)clientData(s,unit); uint8_t* n=(uint8_t*)(uintptr_t)rd<uint64_t>(d,0x28);
 while(n){ uint8_t* next=(uint8_t*)(uintptr_t)rd<uint64_t>(n,0x10); s.freeMem(n,0x830); n=next; }
 wr<uint64_t>(d,0x28,0);
}
// 1.13c D2Client+0x69010 (eax = unit, edx = key): unlink node with field+8 == key
bool sub_69010(Services& s, void* unit, uintptr_t key){
 uint8_t* d=(uint8_t*)clientData(s,unit); uint8_t* prev=nullptr; uint8_t* n=(uint8_t*)(uintptr_t)rd<uint64_t>(d,0x28);
 while(n && (uintptr_t)rd<uint64_t>(n,8)!=key){ prev=n; n=(uint8_t*)(uintptr_t)rd<uint64_t>(n,0x10); }
 if(!n) return false;
 uint64_t next=rd<uint64_t>(n,0x10); s.freeMem(n,0x3d0);
 if(prev) wr<uint64_t>(prev,0x10,next); else wr<uint64_t>(d,0x28,next);
 return true;
}
// 1.13c D2Client+0x69180 (this = node, param = unit): unlink from list at +0x2c, release, free
void sub_69180(Services& s, uint8_t* node, void* unit){
 uint8_t* d=(uint8_t*)clientData(s,unit); uint8_t* prev=nullptr; uint8_t* n=(uint8_t*)(uintptr_t)rd<uint64_t>(d,0x2c);
 while(n && n!=node){ prev=n; n=(uint8_t*)(uintptr_t)rd<uint64_t>(n,0x29*4); }
 if(!n) return;
 if(rd<uint64_t>(n,0x10*4)){ s.releaseHandle((void*)(uintptr_t)rd<uint64_t>(n,0x10*4)); wr<uint64_t>(n,0x10*4,0); }
 uint64_t next=rd<uint64_t>(n,0x29*4);
 if(prev) wr<uint64_t>(prev,0x29*4,next); else wr<uint64_t>(d,0x2c,next);
 if(rd<int32_t>(n,0)==8) s.releaseNode(nullptr);
 s.freeMem(n,0x1e1);
}
// 1.13c D2Client+0x69350 (edx = name, ecx = unused, param_3 = table): bucketed lookup by lowercased name
// GUESS: key is the first 8 bytes of the lowercased name (words at +0xc/+0x10 of the entry).
void* sub_69350(Services& s, const char* name, uint8_t* table){
 char buf[0x108]={}; std::strncpy(buf,name,0x100); for(char* p=buf;*p;p++) if(*p>='A'&&*p<='Z') *p+=32;
 uint32_t k0,k1; std::memcpy(&k0,buf,4); std::memcpy(&k1,buf+4,4);
 uint32_t b=sub_68B80(s,buf);
 for(uint8_t* e=(uint8_t*)(uintptr_t)rd<uint64_t>(table,0xc+b*4); e; e=(uint8_t*)(uintptr_t)rd<uint64_t>(e,0))
  if(rd<uint32_t>(e,12)==k0&&rd<uint32_t>(e,16)==k1) return e;
 return nullptr;
}
// 1.13c D2Client+0x693C0 (param_2 = unit-like): notify(7,p) when type 0 and not one of the excluded modes
void sub_693C0(Services& s, int32_t* p){
 if(p[0x31]&0x10000) return;
 int32_t mode=p[4], type=p[0];
 if(type==0){ if(mode==0||mode==0x11) return; }
 else if(type==1){ if(mode==0||mode==0xc) return; }
 if(type==0) s.notify(7,p);
}
// 1.13c D2Client+0x68CF0 (esi = optional {type,id}): colour index by ownership / health percent
// GUESS: needs data tables and Ordinals 10190/10175/10350/10913/10973; surface only.
struct ColorServices { virtual ~ColorServices()=default; virtual uint8_t rgb(int,int,int)=0; virtual bool isTarget(int32_t type,int32_t id,int32_t& cls)=0;
 virtual bool flagA(int32_t cls)=0; virtual int32_t maxLife()=0; virtual int32_t life()=0; };
uint8_t sub_68CF0(ColorServices& c, const int32_t* esi, uint8_t pal[4]){
 int32_t cls; bool t=c.isTarget(esi?esi[0]:6, esi?esi[1]:-1, cls);
 if(t){ if(c.flagA(cls)) return pal[3]; int32_t m=c.maxLife(); if(m){ int32_t pc=c.life()*100/m; if(pc<0x21) return pal[2]; if(pc<0x42) return pal[1]; } }
 return pal[0];
}
// 1.13c D2Client+0x69410 (eax = monstat index, param_1 = mode): COF path builder. GUESS: surface only, not implemented
// (reads sgptDataTables monstats 0x1a8-byte rows, builds "%s\\%s\\COF\\%s%s%s.COF").
}
