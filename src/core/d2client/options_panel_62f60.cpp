// D2Client batch s3481,s3494-s3503: options-panel (0x550-stride entry table) teardown/load and INI-saved settings.
// Derived from cached 1.13c decompile; no prior art. Ordinal imports and globals come in as callbacks/references.
// Ordinal_426 = writeIni(section,key,default,value); Ordinal_10065 = release(handle); Ordinal_10043(0x7a,0) = after-release hook.
// Unrecovered arguments are marked GUESS.
#include <cstdint>
#include <cstring>
#include <cstdio>
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

// 1.13c D2Client+0x63520  (ECX = obj). saveShadow = FUN_6fb0f970 (0x5F970)
void autoMapFade_63520(const Imp&f,B*o,void(*f5f970)()){ if(rd32(o,0x124)>4) wr32(o,0x124,0); f5f970(); f.ini("Diablo II","AutoMapFade",0,rd32(o,0x124)); }

// 1.13c D2Client+0x635B0  (ECX = obj). o10033 = Ordinal_10033(setBlendedShadows)
void blendedShadows_635b0(const Imp&f,B*o,void(*o10033)(U)){ f.ini("Diablo II","Blended Shadows",0,rd32(o,0x124)); o10033(rd32(o,0x124)); }

// 1.13c D2Client+0x63940  (EAX = which 0..7; param_2 = anchor). Builds a 400-char name line then Ordinal_10085(anchor-0x1a,0,1).
struct Imp63940 { const uint16_t*(*o10003)(U); I(*f38e50)(U); U(*f391e0)(U); void(*o10085)(I,U,U); };
// ukey: the two Ordinal_10003 string ids are not visible in the decompile (args lost in registers): GUESS 0 and 0.
void nameLine_63940(U which,I anchor,const Imp63940&f,uint16_t*line400){
 bool b=false; I k=0x39;
 switch(which){case 0:b=1;k=0;break;case 1:b=1;k=1;break;case 2:b=1;k=0xc;break;case 3:b=1;k=2;break;
  case 4:b=1;k=7;break;case 5:b=1;k=3;break;case 6:b=1;k=4;break;case 7:break;default:return;}
 std::memset(line400,0,800);
 const uint16_t*s=f.o10003(0); /* GUESS arg */ size_t n=0; while(s[n]&&n<399){line400[n]=s[n];n++;}
 (void)b;(void)k;(void)f.f38e50;(void)f.f391e0; // ponytail: per-hand sprintf/strcat of 0x38e50/0x391e0 results needs Unicode::sprintf (not in tree); host does it
 f.o10085(anchor-0x1a,0,1);
}

// 1.13c D2Client+0x63BF0  (ECX = x0; p1 = obj with flag @+0x128; p3 = tall flag). Pure layout: returns the three rects fed to Ordinal_10014.
struct Rect63bf0 { I x0,x1,y0,y1; U a,b; };
Rect63bf0 layout_63bf0(I x0,const B*p1,I p3,I panelX,I screenW,I fontH){
 I half=screenW/2, x=panelX+x0, xl=x-0x1e, base=p3?half-0x90:half-0x3b, y=base+(0xc-fontH);
 return Rect63bf0{xl,x,y,y+0x121-(0xc-fontH),U(rd32(p1,0x128)==0)+1,U(rd32(p1,0x128)!=0)}; // GUESS: field mapping of Ordinal_10014 args
}

// 1.13c D2Client+0x63D90  tbls = {95570,93ac8,92ac0,(10227?9a550:98008),(10227?9efe0:9cfe8)}; handles DAT_6fbcc070/74/78
void panelsFree_63d90(const Imp&f,U*cur0,U*cur1,B*const t[5],I*const cnt[5],U*h70,U*h74,U*h78){
 *cur0=0;*cur1=0;
 for(int i=0;i<5;i++) entriesFree_63270(f,cnt[i],t[i]);
 rel(f,h70);rel(f,h74);rel(f,h78);
}

// 1.13c D2Client+0x63E90  (param_1 = &count, param_2 = table). Loads art paths "%s\\UI\\%s\\%s" ("DATA\\LOCAL", dir) per entry.
struct Imp63e90 { U(*o10227)(); void(*o10004)(char*,U); U(*load)(const char*); void(*fatal)(U); };
void entriesLoad_63e90(const Imp63e90&f,const I*cnt,B*tbl,const char*dir,U*handlesBase,U*frameBase){
 if(!cnt){ f.fatal(0); return; } // GUESS: code lost in register (Ordinal_10265 arg)
 if(!tbl){ f.fatal(0x525); return; }
 char path[0x108]; std::memset(path,0,sizeof path); f.o10004(path,0);
 U*p=(U*)(tbl+0x120); (void)frameBase;
 for(I i=0;i<*cnt;i++,p+=0x154){
  if(p[-0x47]==0||f.o10227()!=0){
   if((char)p[-0x45]){
    std::snprintf(path,sizeof path,"%s\\UI\\%s\\%s","DATA\\LOCAL",dir,(const char*)(p-0x45));
    p[0x107]=f.load(path);
    if(p[-0x48]==1){
     U n=*p; if(n==0||n>4){ f.fatal(0x561); return; }
     for(U k=0;k<n;k++){ const char*nm=(const char*)(p+3+k*0x41);
      if(!nm[0]){ f.fatal(0x564); return; }
      std::snprintf(path,sizeof path,"%s\\UI\\%s\\%s","DATA\\LOCAL",dir,nm);
      handlesBase[0x150+i*0x154+k]=f.load(path); } // GUESS: dest = caller frame array indexed (0x150+i*0x154+k)
    }
   }
   typedef void(*Cb)(); Cb c=(Cb)(uintptr_t)p[-2]; if(c) c();
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

// 1.13c D2Client+0x64110  contrast = FUN_6fb13620() (0x63620); 10068 = apply, 10034(0) = refresh
void contrast_64110(const Imp&f,I contrast,void(*o10068)(),void(*o10034)(U)){ f.ini("Diablo II","Contrast",0,(U)contrast); o10068(); o10034(0); }

// 1.13c D2Client+0x641E0  (ECX = obj; qual = DAT_6fb8d854). o10062 = Ordinal_10062(lightFlag)
void lightQuality_641e0(const Imp&f,const B*o,U*qual,void(*o10062)(U)){
 U v=rd32(o,0x124); f.ini("Diablo II","Light Quality",0,v);
 if(v==0){ o10062(1); *qual=0; } else if(v==1){ o10062(0); *qual=0; } else if(v==2){ o10062(0); *qual=1; }
}
}
