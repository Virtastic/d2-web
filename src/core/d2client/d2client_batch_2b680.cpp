// Implementation batch (RE-DONE lane): D2Client+0x2B680..0x2DBF0. Transcribed from the cached decompile;
// imported-DLL ordinals and unrecovered D2Client callees are Services, named by ordinal / RVA.
// Register-passed args (fastcall edx / unaff_*) are explicit parameters.
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace d2client_batch_2b680 {
struct Services {
 virtual ~Services()=default;
 virtual int32_t ord(uint32_t n, int32_t a=0, int32_t b=0, int32_t c=0, int32_t d=0)=0; // D2 import by ordinal
 virtual int32_t rva(uint32_t r, int32_t a=0, int32_t b=0, int32_t c=0, int32_t d=0)=0;  // un-recovered D2Client callee
 virtual int32_t dataTables()=0;            // *sgptDataTables
 virtual int32_t global(uint32_t r)=0;      // D2Client data global by RVA
 virtual const char16_t* string(int32_t id)=0; // Ordinal_10003 result (id taken from call order)
};
static int32_t rd32(const void* p,unsigned o){int32_t v;std::memcpy(&v,(const uint8_t*)p+o,4);return v;}
static void wr32(void* p,unsigned o,int32_t v){std::memcpy((uint8_t*)p+o,&v,4);}
static int32_t skillRecord(Services& s,int32_t id){ // 0xBA0 count, 0xB98 table, stride 0x23C
 int32_t dt=s.dataTables(); if(id<0||id>=rd32((void*)(intptr_t)dt,0xba0))return 0; return id*0x23c+rd32((void*)(intptr_t)dt,0xb98);}
// 1.13c D2Client+0x2B680
int32_t sub_2B680(Services& s,int32_t* p){ s.rva(0xA1860,(int32_t)(intptr_t)p,0,0,3); (void)0xa283; return 0; }
// 1.13c D2Client+0x2B960
void sub_2B960(Services& s,int32_t* unit,int32_t flag){
 if(!s.ord(10511))return; s.ord(10651); s.ord(11142);
 int32_t a=0,b=0; s.ord(10604,(int32_t)(intptr_t)unit,(int32_t)(intptr_t)&a); int32_t other=b;
 int32_t u=(int32_t)(intptr_t)unit; // sign of the unit word
 if(flag==0){ if(u>=0)return; } else if(u<0)return;
 int32_t x=other<0?-other:other, y=u<0?-u:u, d;
 d = y<x ? y+x*2 : x+y*2; d/=2; if(d==0)d=1;
 int32_t sx=(other*15)/d, sy=(u*15)/d;
 int32_t saved=unit[0x11]>>8; int32_t n=s.ord(10179);
 int32_t cx=sy, ox=sx, oy=sy; (void)ox;(void)oy;
 int32_t lo=-sy, hi=n*sy, lo2=-sx, hi2=n*sx, step=sx;
 for(;n>0;--n){
  int32_t f=(unit[0x11]>>8)-1; if(f<0)f+=8; unit[0x11]=f<<8;
  int32_t px=0,py=0,bx=0,by=0;
  if(other!=0){ int32_t r=s.ord(11168,u,0x60); r=s.ord(11080,u,r); s.ord(10002,r);
   bx=px+(s.global(0xC9960)-s.global(0xCC418)); by=py+s.global(0xC995C)-8; }
  int32_t dx=cx, dy=step; if(b!=0){dx=hi;dy=hi2;} (void)dx;(void)dy;
  s.rva(0x6CC00,u,0,bx,by);
  cx+=sx; sy+=0; hi+=lo2; hi2+=lo;
 }
 unit[0x11]=saved<<8;
}
// 1.13c D2Client+0x2BB50
int32_t sub_2BB50(Services& s,int32_t unit,int32_t skillId){
 if(!s.ord(10331)||s.ord(10057))return 0;
 if(skillId<0)return 0; int32_t rec=skillRecord(s,skillId); if(!rec)return 0;
 int32_t cls=s.rva(0x40550,unit); int32_t dt=s.dataTables();
 if(cls<0||cls>=rd32((void*)(intptr_t)dt,0xb6c))return 0;
 int32_t r=s.ord(10786,unit,s.rva(0,rec+100),skillId); if(r<=0)return 0;
 int32_t r2=s.ord(10786,unit,s.rva(0,rec+100),1); if(r2<=0)return 0;
 uint32_t seed=(uint32_t)s.rva(0x1330,unit+32);
 int32_t n=((r*r)/(r2*r2))/(int32_t)(seed+1);
 s.rva(0x1630,unit); s.rva(0x1660,unit);
 for(;n>0;--n){
  uint32_t a=(uint32_t)s.rva(0x1380,unit+32), b=(uint32_t)s.rva(0x1380,unit+32);
  if((int32_t)((b-r)*(b-r)+(a-r)*(a-r))<r*r){
   int32_t o=s.rva(0xB30F0,0x2001,unit); if(o){ s.ord(10706,o,s.rva(0x1330,unit+32)); } }
 }
 return 1;
}
// 1.13c D2Client+0x2BD20
int32_t sub_2BD20(Services& s,int32_t unit,int32_t skillId,int32_t extra){
 if(!skillRecord(s,skillId))return 0;
 int32_t cls=s.rva(0x40550,unit); if(cls<0||cls>=rd32((void*)(intptr_t)s.dataTables(),0xb6c))return 0;
 (void)cls; // unit+0xC4 |= 0x40 happens on the live unit (caller's struct)
 int32_t cur=s.rva(0xA5F70); if(!cur)return 0;
 int32_t o=s.rva(0xB30F0,1,unit,s.rva(0x1630,cur),s.rva(0x1660,cur));
 (void)extra;
 if(!o)return 0;
 s.ord(10637,o); s.ord(10019,o); return 1;
}
// 1.13c D2Client+0x2BE30
int32_t sub_2BE30(Services& s,int32_t unit,int32_t skillId,int32_t extra){
 if(!skillRecord(s,skillId))return 0;
 int32_t cls=s.rva(0x40550,unit); if(cls<0||cls>=rd32((void*)(intptr_t)s.dataTables(),0xb6c))return 0;
 if(!s.rva(0xA1340))return 0; (void)extra;
 int32_t o=s.rva(0xB30F0,0x20,unit); if(!o)return 0;
 int32_t h=s.rva(0,o); if(!h)return 0;
 s.ord(10647,h,0xe); s.ord(10334,h,o,0); return 1;
}
// 1.13c D2Client+0x2BF40
int32_t sub_2BF40(Services& s,int32_t* unit,int32_t skillId){
 int32_t p=s.ord(10511,(int32_t)(intptr_t)unit); if(!p)return 0;
 int32_t rec=skillRecord(s,skillId); if(!rec)return 0;
 if(s.ord(10601,p)==skillId && (int32_t)(intptr_t)unit==s.global(0xCBBFC) && (s.ord(10397,p)&1))return 0;
 if(unit[0xb]==0)return 0;
 int32_t cur=s.rva(0xA5F70);
 if(unit[0]==0 && cur && s.ord(11138,(int32_t)(intptr_t)unit,cur,0)) return s.rva(0xB3040,(int32_t)(intptr_t)unit);
 bool go = unit[0]==0 ? rd32((void*)(intptr_t)rec,0x102)<<16>>16 >=1 : (unit[0]==1 && (int32_t)s.rva(0xB779C0)>=1);
 if(go) s.rva(0xB38A70,(int32_t)(intptr_t)unit,0,0,0);
 int32_t sub; 
 if(!cur||s.ord(10494,cur,0x36)){ s.ord(10243,p,6); sub=0; } else { s.ord(10243,p,rd32((void*)(intptr_t)cur,0)); sub=rd32((void*)(intptr_t)cur,12); }
 s.ord(10554,p,sub); int32_t spd=0x100; s.ord(11031,p,7);
 if(unit[0]==0){ int32_t q=s.rva(0x1300); if(q) spd=(int32_t)(uint8_t)rd32((void*)(intptr_t)q,0x41)<<8; }
 else { int32_t id=unit[1]; int32_t q=s.rva(0x1230);
  if(q){ spd=(int32_t)(int16_t)rd32((void*)(intptr_t)q,0x34)<<8; if(id==0xd3)s.ord(11031,p,0xf); s.ord(10168,p,0x1e);} }
 s.ord(10973,(int32_t)(intptr_t)unit,0x43,0);
 s.rva(0x2D20); int32_t a=s.rva(0x14A0,100,spd);
 s.ord(10488,sub,a); s.ord(10334,sub,(int32_t)(intptr_t)unit,0);
 if(cur)s.rva(0xB5FB0);
 s.ord(10137,a,0x1001); s.ord(10748,a,0); s.rva(0xB3220); return 1;
}
// 1.13c D2Client+0x2C2D0   (eax/esi are caller registers: inEax, esi60)
int32_t sub_2C2D0(Services& s,int32_t* out,int32_t inEax,int32_t esi60,int32_t gate,uint32_t* flagOut){
 if(!gate)return 0; if(!s.ord(10858))return 0;
 *out=s.ord(10621);
 if(inEax||(esi60&&(inEax=s.ord(10061,esi60)))) if(s.ord(11088,inEax)==0x26){*out=0;}
 int32_t t=0; if(s.ord(10494))t=s.ord(10804);
 *out=(*out*(t+s.ord(10910)+s.ord(10653)))/100+*out;
 *flagOut=0; if(s.ord(10402))*flagOut=3; if(s.ord(11038))*flagOut=1; return 1;
}
// 1.13c D2Client+0x2D680
void sub_2D680(Services& s,int32_t unit60,int32_t p2,int32_t p4,int32_t* o1,int32_t* o2){
 if(unit60)s.ord(10061,unit60); *o1=0;*o2=0; s.rva(0x2C4F0,(int32_t)(intptr_t)o1,(int32_t)(intptr_t)o2,p4,p2);
}
// 1.13c D2Client+0x2D6E0  (al = mode, EBX/EDI accumulators; ord10973 = unit stat read, args per call site)
void sub_2D6E0(Services& s,uint8_t mode,int32_t* x,int32_t* y){
 switch(mode){
 case 1: case 2: case 3: case 4: case 0xc: *x+=s.ord(10973); *y+=s.ord(10973); return;
 case 5: { int32_t v=s.ord(10973); if(v<1){s.ord(10973);s.ord(10973);} int32_t a=s.ord(10973),b=s.ord(10973);
  *x+=(a*0x3a)>>8; *y+=(b*0x3a)>>8; } }
}
static void app(char16_t* d,const char16_t* s_){ if(!s_)return; size_t n=0; while(d[n])++n; while((d[n++]=*s_++)); }
static void num(char16_t* dst,int32_t v,bool plus){ char b[32]; std::snprintf(b,32,(plus&&v>=0)?"+%d":"%d",v); size_t n=0; for(;b[n];++n)dst[n]=(uint8_t)b[n]; dst[n]=0; }
// 1.13c D2Client+0x2D810  (dst = edi, ids are call order of Ordinal_10003)
int32_t sub_2D810(Services& s,char16_t* dst,int32_t v,const int32_t id[5]){
 const char16_t *a=s.string(id[0]),*b=s.string(id[1]),*c=s.string(id[2]),*d=s.string(id[3]); char16_t n[32]; num(n,v,false);
 app(dst,b); app(dst,n); app(dst,d); app(dst,a); app(dst,c); app(dst,s.string(id[4])); return 1;
}
// 1.13c D2Client+0x2DB30
int32_t sub_2DB30(Services& s,char16_t* dst,int32_t v,int32_t plus,const int32_t id[3]){
 if(!v)return 0; char16_t n[32]; num(n,v,plus!=0); app(dst,n); for(int i=0;i<3;++i)app(dst,s.string(id[i])); return 1;
}
// 1.13c D2Client+0x2DBF0
int32_t sub_2DBF0(Services& s,char16_t* dst,int32_t v,int32_t plus,const int32_t id[2]){
 if(!v)return 0; char16_t n[32]; num(n,v,plus!=0); app(dst,n); for(int i=0;i<2;++i)app(dst,s.string(id[i])); return 1;
}
}
