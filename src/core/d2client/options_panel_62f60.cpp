// D2Client batch s3481,s3494-s3503: options-panel (0x550-stride entry table) teardown/load and INI-saved settings.
// Derived from cached 1.13c decompile; no prior art. Ordinal imports and globals come in as callbacks/references.
// Ordinal_426 = writeIni(section,key,default,value); Ordinal_10065 = release(handle); Ordinal_10043(0x7a,0) = after-release hook.
// Unrecovered arguments are marked GUESS.
#include <cstdint>
#include <cstring>
#include <cstdio>
void D2Lang_Unicode_sprintf(int cap, char16_t* out, const char16_t* fmt, ...);
namespace d2client_optpanel {
using U=uint32_t; using I=int32_t; using B=uint8_t;
static U rd32(const void*p,U o){U v;std::memcpy(&v,(const B*)p+o,4);return v;}
static void wr32(void*p,U o,U v){std::memcpy((B*)p+o,&v,4);}
struct Imp { void(*release)(U); void(*afterRel)(U,U); void(*ini)(const char*,const char*,U,U); void(*fatal)(U); };

// release handle h (if nonzero) and clear *slot  (inlined 10065/10043 pair, repeated throughout)
static void rel(const Imp&f,U*slot){ U h=*slot; if(h){ f.release(h); f.afterRel(0x7a,0); *slot=0; } }

// 1.13c D2Client+0x62F60  (g1=DAT_6fb929fc, h0=DAT_6fbcc080, h1=DAT_6fbcc084)
void miniPanelFree_62f60(const Imp&f,U g1,U*h0,U*h1){ f.ini("Diablo II","Mini Panel",1,g1==0); rel(f,h0); rel(f,h1); }

// 1.13c D2Client+0x63270  (EAX = tbl base; param_1 = &count). entry stride 0x550: +0x78 kind, +0x120 n, +0x424 handle, +0x420+... sub-handles at +0x120+0x420
void entriesFree_63270(const Imp&f,I*cnt,B*tbl){
 if(!cnt){ f.fatal(0x5ba); return; }
 if(!tbl){ f.fatal(0x5bb); return; }
 U*p=(U*)(tbl+0x120);
 for(I i=0;i<*cnt;i++,p+=0x154){
  rel(f,&p[0x107]);
  if(p[-0x48]==1){
   U n=*p; if(n==0||n>4){ f.fatal(0x5d8); return; }
   for(U k=0;k<n;k++) rel(f,&p[0x108+k]);
  }
 }
}

// 1.13c D2Client+0x63520  (ECX = obj). f5f970 = D2Client+0x5F970, called with EAX = the clamped fade value
void autoMapFade_63520(const Imp&f,B*o,void(*f5f970)(U)){ if(rd32(o,0x124)>4) wr32(o,0x124,0); f5f970(rd32(o,0x124)); f.ini("Diablo II","AutoMapFade",0,rd32(o,0x124)); }

// 1.13c D2Client+0x635B0  (ECX = obj). o10033 = Ordinal_10033(setBlendedShadows)
void blendedShadows_635b0(const Imp&f,B*o,void(*o10033)(U)){ f.ini("Diablo II","Blended Shadows",0,rd32(o,0x124)); o10033(rd32(o,0x124)); }

// 1.13c D2Client+0x63940  (EAX = which 0..7; stack x, y; ret 8). One help line: the command's name (D2Lang string
// 0x1049+which, 0x1050 for 7) followed, for cases 0..6, by " %s" (string 0x1052) per bound key, slot 1 first then
// slot 0; drawn by D2Win#10085(line, x-3, y-0x1a, color 0, centered 1).
struct Imp63940 {
 const char16_t*(*getString)(U);                 // D2Lang#10003 (ECX = id)
 I(*keyOf)(U cmd,U slot);                        // D2Client+0x38E50 (ESI = cmd, EDX = slot); 0xFFFF = unbound
 const char16_t*(*keyName)(U cmd,U slot);        // D2Client+0x391E0
 void(*drawText)(const char16_t*,I x,I y,U color,U centered); // D2Win#10085
};
static void u16cpy(char16_t*d,const char16_t*s){ while((*d++=*s++)!=0){} }
static void u16cat(char16_t*d,const char16_t*s){ while(*d) d++; u16cpy(d,s); }
void nameLine_63940(U which,I x,I y,const Imp63940&f){
 static const U kCmd[7]={0,1,0xc,2,7,3,4};
 if(which>7) return;
 char16_t line[400]; std::memset(line,0,sizeof line);
 u16cpy(line,f.getString(which==7?0x1050u:0x1049u+which));
 if(which<7){
  U k=kCmd[which];
  for(I slot=1;slot>=0;slot--){
   if((int16_t)f.keyOf(k,U(slot))==-1) continue;
   char16_t buf[30]; std::memset(buf,0,sizeof buf);
   D2Lang_Unicode_sprintf(0x1e,buf,f.getString(0x1052),(uintptr_t)f.keyName(k,U(slot)));
   u16cat(line,buf);
  }
 }
 f.drawText(line,x-3,y-0x1a,0,1);
}

// 1.13c D2Client+0x63BF0  (__thiscall ECX = y; stack obj, mode, tall; ret 0xc). A slider: two rects split at the
// knob, the caption cel, then the knob cel. hdrY = DAT_6fbcc05c[+0x10], screenW = DAT_6fb8bc48,
// cels = DAT_6fbcc070 (off) / DAT_6fbcc074 (on) / DAT_6fbcc078 (knob). obj: +0x120 steps, +0x124 value, +0x128 on.
// The retail x87 r = trunc((value/(steps-1)) * -265.0f) equals -(265*value/(steps-1)) for every steps <= 256.
struct Imp63bf0 {
 void(*drawRect)(I x0,I y0,I x1,I y1,U color,U mode);           // D2gfx#10014
 void(*drawCel)(U cel,I x,I y,U frame,U mode,I pal);             // D2Win#10138 (ECX = cel, EDX = x)
 void(*drawCtx)(B*ctx48,I x,I y,I a,U mode,U b);                 // D2gfx#10041
};
void layout_63bf0(I y0,const B*o,U mode,I tall,I hdrY,I screenW,U celOff,U celOn,U celKnob,const Imp63bf0&f){
 I y=y0+hdrY, half=screenW/2;
 U steps=rd32(o,0x120)-1, v=rd32(o,0x124), on=rd32(o,0x128);
 I r=-I((265u*v)/steps);
 I base=tall?half-0x90:half-0x3b, split=base+12-r;
 f.drawRect(base,y-0x1e,split,y,0,U(on==0)+1);
 f.drawRect(split,y-0x1e,base+0x121,y,0,U(on!=0));
 f.drawCel(on?celOn:celOff,tall?half:half+0xe6,y,tall?1u:2u,mode,-1);
 B ctx[0x48]; std::memset(ctx,0,sizeof ctx); wr32(ctx,0x34,celKnob);
 f.drawCtx(ctx,(tall?half-0x91:half-0x3c)-r,y-I(on==0)-1,-1,mode,0);
}

// 1.13c D2Client+0x63D90  tbls = {95570,93ac8,92ac0,(10227?9a550:98008),(10227?9efe0:9cfe8)}; handles DAT_6fbcc070/74/78
void panelsFree_63d90(const Imp&f,U*cur0,U*cur1,B*const t[5],I*const cnt[5],U*h70,U*h74,U*h78){
 *cur0=0;*cur1=0;
 for(int i=0;i<5;i++) entriesFree_63270(f,cnt[i],t[i]);
 rel(f,h70);rel(f,h74);rel(f,h78);
}

// 1.13c D2Client+0x63E90  (param_1 = &count, param_2 = table). Loads "DATA\\LOCAL\\UI\\<lang>\\<name>" per entry;
// lang is the 4-byte code D2Lang#10004 writes. Sub-cels land in the entry's own slots p[0x108+k].
struct Imp63e90 { U(*o10227)(); void(*o10004)(char*,U); U(*load)(const char*,U); void(*fatal)(U); };
void entriesLoad_63e90(const Imp63e90&f,const I*cnt,B*tbl){
 if(!cnt){ f.fatal(0x524); return; }
 if(!tbl){ f.fatal(0x525); return; }
 char lang[4]={0,0,0,0}; f.o10004(lang,0);
 char path[0x108];
 U*p=(U*)(tbl+0x120);
 for(I i=0;i<*cnt;i++,p+=0x154){
  if(p[-0x47]==0||f.o10227()!=0){
   if(*(const char*)(p-0x45)){
    std::memset(path,0,sizeof path);
    std::snprintf(path,sizeof path,"%s\\UI\\%s\\%s","DATA\\LOCAL",lang,(const char*)(p-0x45));
    p[0x107]=f.load(path,0);
    if(p[-0x48]==1){
     U n=*p; if(n==0||n>4){ f.fatal(0x561); return; }
     for(U k=0;k<n;k++){ const char*nm=(const char*)(p+3+k*0x41);
      if(!nm[0]){ f.fatal(0x564); return; }
      std::snprintf(path,sizeof path,"%s\\UI\\%s\\%s","DATA\\LOCAL",lang,nm);
      p[0x108+k]=f.load(path,0); }
    }
   }
   typedef void(*Cb)(B*entry,U); Cb c=(Cb)(uintptr_t)p[-2]; if(c) c((B*)(p-0x48),0);
  }
 }
}

// 1.13c D2Client+0x64080  (EBX = mouseX; scr = DAT_6fb8bc4c; hdr = DAT_6fbcc05c {count,width}; ents = DAT_6fbcc060). Returns hit index or -1.
I hitTest_64080(I mx,I scrH,const I*hdr,const B*ents,I(*pred)(const B*e)){
 I n=hdr[0], c=(scrH-0x50)/2, w=(hdr[1]*n)/2, r=-1;
 if(!(c-w<mx&&mx<c+w)) return -1;
 for(I i=0;i<n;i++){ I v=(I)rd32(ents,U(i)*0x550+8); if(v&&v+n<=mx) r=i; }
 if(r<0) return -1;
 const B*e=ents+U(r)*0x550;
 if(rd32(e,0)!=0xffffffffu){ U cb=rd32(e,0x110); if(!cb||pred(e)) return r; }
 return -1;
}

// 1.13c D2Client+0x64110  (ECX = obj). scale = D2Client+0x13620(EAX=100, ECX=0, EDX=obj) (x87; caller-supplied);
// then D2gfx#10068(ECX=0xb, EDX=obj[0x124]) and D2gfx#10034(0).
void contrast_64110(const Imp&f,const B*o,U(*f13620)(U,U,const B*),void(*o10068)(U,U),void(*o10034)(U)){
 f.ini("Diablo II","Contrast",0,f13620(100,0,o)); o10068(0xb,rd32(o,0x124)); o10034(0);
}

// 1.13c D2Client+0x641E0  (ECX = obj; qual = DAT_6fb8d854). o10062 = Ordinal_10062(lightFlag)
void lightQuality_641e0(const Imp&f,const B*o,U*qual,void(*o10062)(U)){
 U v=rd32(o,0x124); f.ini("Diablo II","Light Quality",0,v);
 if(v==0){ o10062(1); *qual=0; } else if(v==1){ o10062(0); *qual=0; } else if(v==2){ o10062(0); *qual=1; }
}
}
