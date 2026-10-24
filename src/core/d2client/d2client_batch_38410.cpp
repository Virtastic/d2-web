// Prior art: none (D2MOO has no D2Client module); Ghidra decompile only. Imported-DLL calls go through Services.
// Register-passed args (EAX/ESI/EDI/EBX) are named after the register in each signature.
#include <cstdint>
#include <cstring>
#include <cctype>
namespace d2client_batch_38410 {
struct Bind { int32_t cmd; uint16_t key; int32_t mod; };      // 10-byte packed record at D2Client+0xCA2E0 (key @+4, mod @+6 in file)
constexpr unsigned kBinds = 114;                              // (0xCA754-0xCA2E0)/10
struct Cmd { uint32_t a; uint32_t handler; uint32_t flag; };  // 12-byte records at D2Client+0xA4F10
struct Services {
 virtual ~Services()=default;
 virtual bool badCodePtr(uint32_t)=0; virtual void callHandler(uint32_t)=0;
 virtual void ord511(void*)=0; [[noreturn]] virtual void fatal(int32_t)=0;
 virtual bool f2060()=0; virtual bool f2030()=0; virtual bool f20e0()=0; virtual bool f2090()=0;
 virtual void ord10003(uint32_t key)=0;           // low confidence: decompile shows no args
 virtual uint8_t jumpByte(uint32_t key)=0;        // byte table at D2Client+0xEA15C (not recovered)
 virtual void f391e0(uint32_t idx,int32_t mod)=0;
 virtual void ord(int32_t n)=0;                   // Ordinal_NNN no-arg calls: 512-515, 414-417
 virtual void sendClose(void* hwnd)=0; virtual void destroyObj(void* o)=0;
 virtual void ord10043(int32_t a,int32_t b)=0; virtual void ord10065(int32_t)=0;
 virtual int32_t listFirst()=0; virtual int16_t* listData(int32_t)=0; virtual int32_t listCount(int32_t)=0; virtual int32_t listNext(int32_t)=0;
};
struct State { Services& api; Cmd cmds[0xE0]{}; Bind binds[kBinds]{}; int32_t mode{}; };

// 1.13c D2Client+0x38410
void sub_38410(Services& api,uint32_t* p,uint32_t vtA,uint32_t vtB){ p[0]=vtA; if(p[2]) api.destroyObj(reinterpret_cast<void*>(uintptr_t(p[2]))); p[0]=vtB; }
// 1.13c D2Client+0x38770
void sub_38770(Services& api,uint32_t* p,uint32_t vt,uint32_t baseVt){
 p[0]=vt; if(p[10]){api.ord10043(0x8e,0);p[10]=0;} if(p[11]){api.ord10043(0x93,0);p[11]=0;}
 int32_t v; std::memcpy(&v,reinterpret_cast<uint8_t*>(p)+0x52,4); api.ord10065(v); if(v)api.ord10043(0x7a,0); p[0]=baseVt; }
// 1.13c D2Client+0x38AD0
void sub_38AD0(State& s,uint8_t* msg){
 uint32_t key; std::memcpy(&key,msg+8,4); unsigned i=0;
 for(;i<kBinds;++i){ int32_t c=s.binds[i].cmd; if(s.binds[i].key==key && c!=0xffff && c<=0x38 && c>=0 && s.cmds[c].handler) break; }
 if(i==kBinds)return; const Cmd& c=s.cmds[s.binds[i].cmd];
 if(s.mode!=2 || c.flag){ if(s.api.badCodePtr(c.handler)) s.api.fatal(0x834); s.api.callHandler(s.binds[i].cmd);
  uint32_t one=1,zero=0; std::memcpy(msg+0x18,&one,4); std::memcpy(msg+0x1c,&zero,4); s.api.ord511(msg); } }
// 1.13c D2Client+0x38CD0  (key in ESI)
bool sub_38CD0(uint32_t esi){ uint32_t k=esi&0xffff;
 if(k<0x2b){ if(k==0x2a)return true;
  switch(k){ case 1:case 2:case 3:case 4:case 0x15:case 0x17:case 0x18:case 0x19:case 0x1b:case 0x1c:case 0x1d:case 0x1e:case 0x1f:case 0x25:case 0x26:case 0x27:case 0x28:case 0x29:return false;
   case 8:case 9:case 0xc:case 0xd:case 0x10:case 0x11:case 0x12:case 0x13:case 0x14:case 0x20:case 0x21:case 0x22:case 0x23:case 0x24:return true; default:break; } }
 else { switch(k){ case 0x2b:case 0x5b:case 0x5c:case 0x5d:case 0x90:return false; default:break; }
  if((k>=0x2c&&k<=0x2f)||(k>=0x60&&k<=0x87)||k==0x91||(k>=0xba&&k<=0xc0)||(k>=0xdb&&k<=0xde)||(k>=0x100&&k<=0x104))return true; }
 return k<256 && (std::isalpha(int(k))||std::iscntrl(int(k))); }
// 1.13c D2Client+0x39BA0  (key in AX, out in ESI, cmd in EDI)
bool sub_39BA0(State& s,uint16_t ax,uint16_t* esi,int32_t edi){
 bool ok=true;
 switch(ax){ case 0x100: ok=s.api.f2060(); if(!ok&&esi)*esi=0xf89; return ok;
  case 0x101:case 0x102: ok=s.api.f2030(); if(!ok&&esi)*esi=0xf89; return ok;
  case 0x103:case 0x104: if((!s.api.f20e0()&&!s.api.f2090())||s.cmds[edi].handler){ if(esi)*esi=0xf8a; return false; } return true;
  default: return true; } }
// 1.13c D2Client+0x39F30  (cmd in EAX, mod in EDX). Low confidence: jumpByte table, ord10003 args.
void sub_39F30(State& s,int32_t eax,int32_t edx){
 uint32_t idx=0,key=0xffff;
 for(;idx<kBinds;++idx) if(s.binds[idx].cmd==eax&&s.binds[idx].mod==edx){key=s.binds[idx].key;break;}
 if(key-8<0xfd){ idx=s.api.jumpByte(key);
  switch(key){ case 8:case 0x10:case 0x11:case 0x1b:case 0x21:case 0x22:case 0x2c:case 0x2d:case 0x2e:
   case 0x60:case 0x61:case 0x62:case 99:case 100:case 0x65:case 0x66:case 0x67:case 0x68:case 0x69:case 0x6a:case 0x6b:case 0x6c:case 0x6d:case 0x6f:
   case 0x90:case 0x91:case 0x100:case 0x101:case 0x102:case 0x103:case 0x104: s.api.ord10003(key);return; default:break; } }
 s.api.f391e0(idx,edx); }
// 1.13c D2Client+0x3A270  (cmd=ECX, mod=param_1, key=param_2, out=EAX)
bool sub_3A270(State& s,int32_t cmd,int32_t mod,int16_t key,uint16_t* out){
 if(out)*out=0; if(cmd>=0x38)return false;
 if(sub_38CD0(uint16_t(key))){
  if(!sub_39BA0(s,uint16_t(key),out,cmd))return false;
  if(key!=0x2c||!s.cmds[cmd].a){
   unsigned old=0x72; for(unsigned i=0;i<kBinds;++i) if(int16_t(s.binds[i].key)==key){s.binds[i].key=0xffff;old=i;break;}
   for(unsigned i=0;i<kBinds;++i) if(s.binds[i].cmd==cmd&&s.binds[i].mod==mod){s.binds[i].key=uint16_t(key);return true;}
   if(old!=0x72)s.binds[old].key=uint16_t(key); return false; } }
 if(out)*out=0xf8b; return false; }
static void dispatch(Services& api,bool nonnull,int32_t sel,int32_t id,int32_t e0,int32_t ec,int32_t base,int32_t special,int32_t fatalNull,int32_t fatalMain){
 if(nonnull){ switch(sel){ case 0: if(id!=0x111){api.ord(base+3);return;} api.fatal(fatalMain); case 1: api.ord(base);return; case 2: api.ord(base+2);return; case 3: api.ord(base+1);default:return; } }
 api.fatal(fatalNull); (void)e0;(void)ec;(void)special; }
// 1.13c D2Client+0x3CA80
void sub_3CA80(Services& api,bool nonnull,int32_t sel,int32_t id){ dispatch(api,nonnull,sel,id,0,0,414,0,0x136,0x13a); }
// 1.13c D2Client+0x3CBC0
void sub_3CBC0(Services& api,uint32_t* p){ api.sendClose(reinterpret_cast<void*>(uintptr_t(p[0]))); p[6]=1; p[7]=0; }
// 1.13c D2Client+0x3CCC0  (count=EAX; sel is ESI inherited from caller)
void sub_3CCC0(Services& api,int32_t count,bool param1,const uint8_t* arr,int32_t sel){
 if(!param1)api.fatal(0x179);
 for(const uint8_t* e=arr+4;count>0;--count,e+=12){ int32_t id; std::memcpy(&id,e,4); sub_3CA80(api,true,sel,id); } }
// 1.13c D2Client+0x3CE60  (nonnull=EAX, want=EBX); always returns 0
uint32_t sub_3CE60(Services& api,bool nonnull,int32_t want){
 if(nonnull){ int32_t it=api.listFirst();
  while(it){ int16_t* d=api.listData(it); if(d&&api.listCount(it)>=1&&*d==want)break; it=api.listNext(it); } }
 return 0; }
}
