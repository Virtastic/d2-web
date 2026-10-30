// D2Client batch s3505-s3515: automap mode, list-panel (0x300e-stride line table) helpers,
// small vtable/ctor stubs. Derived from cached 1.13c decompile; no prior art (D2MOO has no D2Client).
// Globals, vtables and ordinal imports are passed in by the caller (host callbacks); arguments
// that the x86 takes in EAX/ESI are named below. Guessed names are marked GUESS.
#include <cstdint>
#include <cstring>
namespace d2client_listpanel {
using U=uint32_t; using I=int32_t; using B=uint8_t;
static U rd32(const void*p,U o){U v;std::memcpy(&v,(const B*)p+o,4);return v;}
static void wr32(void*p,U o,U v){std::memcpy((B*)p+o,&v,4);}
static U rd16(const void*p,U o){uint16_t v;std::memcpy(&v,(const B*)p+o,2);return v;}
static void wr16(void*p,U o,U v){uint16_t x=(uint16_t)v;std::memcpy((B*)p+o,&x,2);}

// 1.13c D2Client+0x647A0  (ECX = settings obj; mode word at +0x124)
struct AutomapGlobals { U curMode; U step; }; // DAT_6fbcc1b0, DAT_6fba16b0
void automap_mode_647a0(B*obj,AutomapGlobals&g,void(*rebuild)(),void(*refresh)(I),void(*saveIni)(U)){
 if(rd32(obj,0x124)>2) wr32(obj,0x124,0);
 U m=rd32(obj,0x124);
 if(I(m)!=I(g.curMode)){ g.step=10; if(m==1) g.step=0x14; g.curMode=m; rebuild(); refresh(1); }
 saveIni(rd32(obj,0x124)); // Ordinal_426("Diablo II","AutoMapMode",0,mode)
}

// 1.13c D2Client+0x66620  (ESI = e; cur = DAT_6fbcbbfc, a unit pointer). Returns 1/0; sets/clears e[0x32]&0x80.
struct Imp66620 { void*(*o10331)(void*); U(*o10826)(void*); I(*o10855)(U); I(*o10839)(void*,U*,U); };
I flag_66620(U*e,void*cur,const Imp66620&f){
 if(!e) return 1;
 bool chk=false;
 switch(e[0]){
  case 0: chk = cur!=(void*)e && e[4]!=0 && e[4]!=0x11; break;
  case 1: chk = e[4]!=0 && e[4]!=0xc; break;
  case 3: case 4: chk=true; break;
 }
 if(chk && cur){
  void*r=f.o10331(cur);
  if(r && f.o10855(f.o10826(r))==0){ e[0x32]|=0x80; return 1; }
  if(f.o10839(cur,e,2)){ e[0x32]&=0xffffff7f; return 0; }
 }
 e[0x32]|=0x80; return 1;
}

// 1.13c D2Client+0x66950  (ESI = e). getter = D2Client+0x68990 (no args), pre = D2Client+0x6A220(EAX = e, push mode):
// mode 0 for kinds 0 and 2, 1 for kind 1; fatal(code) does not return.
void bind_66950(U*e,U(*getter)(),void(*pre)(U*,U),void(*fatal)(U)){
 U p=getter(); e[0x15]=p;
 if(!p){ fatal(0x16); return; }
 switch(e[0]){ case 0: case 2: pre(e,0); return; case 1: pre(e,1); return; case 3: case 4: return; default: fatal(0x28); }
}

// 1.13c D2Client+0x66B80  (ECX = o). vt* are the two vtable addresses; delChild calls (*child)->vt[0](1).
void dtor_66b80(U*o,U vtMid,U vtBase,void(*delChild)(U)){ o[0]=vtMid; if(o[2]) delChild(o[2]); o[0]=vtBase; }
// 1.13c D2Client+0x66C40
void dtor_66c40(U*o,U vtMid,U vtBase){ o[0]=vtMid; o[0]=vtBase; }

// List panel (`this` = obj). Layout: lines at +0x300e*i, text[6] at +8+0x800*k (0x3ff chars + NUL), tag at +0x300a,
// count u16 @0x180748, selected @0x180738, first-visible @0x180730, visible-count @0x18074e.
// 1.13c D2Client+0x67080  (EAX = idx, ECX = this); fatal(code) does not return.
U tag_67080(const B*t,I idx,void(*fatal)(U)){
 U n=rd16(t,0x180748);
 if(idx>=0 && idx<I(n) && idx<0x80) return rd32(t,U(idx)*0x300e + 0x300a);
 if(n<0x80){ if(I(n)<idx){ if(idx>=0) return 0; fatal(0x46e);} else fatal(0x46d); } else fatal(0x46c);
 return 0;
}

// 1.13c D2Client+0x67160  (EAX = pt {u16 @0xc x, @0xe y}, EDX = t). Returns hit line or -1; stores to +0x180738.
I hit_67160(const B*pt,B*t){
 if(I(rd32(t,0x18075a))-0xf < I(rd16(pt,0xc)) || rd32(t,0x18074a)==0) return -1;
 I i=I(rd32(t,0x180730)), acc=0; I n=I(rd16(t,0x180748)); I res=-1;
 for(; i<n; i++){
  const B*row=t+U(i)*0x300e + 0x808; I a=acc;
  for(int k=-1;k<=4;k++) if(rd16(row+k*0x800,0)) a+=I(rd32(t,0x180766)); // psVar4[-0x400..0x1000 step 0x400]
  I y=I(rd16(pt,0xe)), base=I(rd32(t,0x18070c))+0xc;
  if(base+acc<y && y<base+a){ res=i; goto done; }
  acc=a+I(rd32(t,0x18076a));
 }
 res=-1;
done:
 wr32(t,0x180738,U(res)); return res;
}

// 1.13c D2Client+0x67280  (EAX = t): scroll so selection is visible.
void scroll_67280(B*t){
 I s=I(rd32(t,0x180738));
 if(s<I(rd32(t,0x180730))) wr32(t,0x180730,U(s));
 if(I(rd32(t,0x18074e))+I(rd32(t,0x180730))<=s) wr32(t,0x180730,rd32(t,0x180730)+1);
}

// 1.13c D2Client+0x672B0  (thiscall t, msg). vt[7]=scrollUp-ish(0x1c), vt[8]=(0x20); names GUESS. o511 = Ordinal_511.
U wheel_672b0(B*t,B*msg,void(*vt1c)(B*),void(*vt20)(B*),void(*o511)(B*)){
 short d=(short)rd16(msg,10);
 if(d>0){ wr32(t,0x180738,rd32(t,0x180730)); vt1c(msg); }
 else if(d<0){ wr32(t,0x180738,rd32(t,0x18074e)-1+rd32(t,0x180730)); vt20(msg); }
 wr32(msg,0x18,1); wr32(msg,0x1c,0); o511(msg); return 1;
}

// 1.13c D2Client+0x67360  (EAX = t, EDX = pt): point inside list rect?
U inrect_67360(const B*t,const B*pt){
 I x=I(rd16(pt,0xc)), y=I(rd16(pt,0xe)), L=I(rd32(t,0x18075a)), T=I(rd32(t,0x18075e));
 return L<x && x<L+0x10 && y<T-0xc+I(rd32(t,0x180762))*0xc && T<y;
}

// 1.13c D2Client+0x67430  (thiscall t; s[6] = 16-bit wide strings or null). Appends a line.
void add_67430(B*t,const uint16_t*const s[6],U tag,U colorArg,I useDefault,U extra){
 U n=rd16(t,0x180748); if(n>=0x80) return;
 B*ln=t+n*0x300e;
 wr32(ln,0x300a,tag);
 if(I(rd32(t,0x180738))==-1) wr32(t,0x180738,0);
 for(int k=0;k<6;k++) if(s[k]){
  uint16_t*d=(uint16_t*)(ln+8+k*0x800); size_t i=0;
  for(;i<0x3ff&&s[k][i];i++) d[i]=s[k][i];
  for(;i<0x3ff;i++) d[i]=0;
  wr16(ln,0x806+k*0x800,0);
  wr32(t,0x18073c,rd32(t,0x18073c)+1);
 }
 wr32(t,(n+1)*0x300e,useDefault?rd32(t,0x180740):colorArg); // GUESS: (n+1)*0x300e is the next line's slot 0
 wr32(ln,0x3012,extra);
 wr16(t,0x180748,n+1);
}
}
