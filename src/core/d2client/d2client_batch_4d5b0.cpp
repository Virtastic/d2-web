// Implementation batch (alpha lane): D2Client 0x4A8C0, 0x4D5B0..0x4FF10. Decompile-derived; no vectors yet.
#include "d2client_batch_4d5b0.h"
namespace d2client_batch_4d5b0 {

// 1.13c D2Client+0x4D5B0 (register-arg fragment: EDI = unit; the stored word is the 4th out of Ordinal_10292)
void sub_4D5B0(Svc& s,uint8_t* edi,uint32_t ebp){
 void* h=s.o10421(); int32_t n=s.o10034(); int32_t o[4]={0};
 for(int32_t i=0;i<(n>>8);++i){
  s.o10292(h,i<<8,i<<8,&o[0],&o[1],&o[2],&o[3]);
  wr<uint32_t>(edi,0x10,(uint32_t)o[0]); s.sub6A220(); (void)ebp;
 }
}

// 1.13c D2Client+0x4DA20
void sub_4DA20(Svc& s,uint32_t a){
 int32_t* t=s.sub560D0(a);
 if(t){ if(*t!=3) s.fatal(0x161); s.sub61420(t,0); }
}

// 1.13c D2Client+0x4EF50 (esi = third arg, register-passed)
void sub_4EF50(Svc& s,int32_t* p,uint16_t* q,int32_t esi){
 if(!p||!q) return;
 p[0x31]|=0x202;
 uint32_t id=*q;
 uint32_t r=s.o10988(id);
 uint32_t q2; std::memcpy(&q2,reinterpret_cast<uint8_t*>(q)+4,4);
 uint8_t buf[36];
 if(!s.o10458(q2,r,s.g6fbcc390(),buf)) return;
 uint8_t* e=s.o11156(s.g6fbc9854(),id,(uint32_t)esi); if(!e) return;
 int32_t d=esi-rd<int32_t>(e,0x1c); if(d<0) d=0;
 int32_t* sp=p;
 for(int i=0;i<6;++i){
  int32_t v=rd<int32_t>(e,0x78+i*4);
  if(v<1||s.dtRows()<=v) break;
  int32_t k=((int32_t)(int8_t)e[0xcc+i]*d>>5)+(int8_t)e[0xc6+i];
  if(k>0x1f) k=0x20;
  int8_t c=(int8_t)e[0xc0+i];
  s.o10302(sp,v,k,0,"..\\Source\\D2Client\\UNIT\\Monster.cpp",0x9c7);
  s.o10368(s.o10630(c));
 }
 s.o10003(); s.o10971(); s.sub32FE0(sp);
}

// 1.13c D2Client+0x4F720 (eax = index)
void sub_4F720(Svc& s,int32_t eax){
 uint32_t a=(uint32_t)(eax*4+0xa9c);
 for(int i=0;i<3;++i){ s.sub0DE90(a); s.sub0DE90(a+0x18); a+=0x30; }
}

// 1.13c D2Client+0x4FCE0 (esi = key; edx = second arg as seen after the 0x9D630 call)
int32_t* sub_4FCE0(Svc& s,uint32_t a,int32_t b,int32_t esi,int32_t edx){
 uint8_t* u=s.sub4D630(a,b);
 if(u && rd<uint32_t>(u,0x34) && edx!=esi){
  uint32_t head=rd<uint32_t>(u,0x34);
  for(uint32_t n=rd<uint32_t>(reinterpret_cast<uint8_t*>((uintptr_t)head),0);n;n=rd<uint32_t>(reinterpret_cast<uint8_t*>((uintptr_t)n),8))
   if(rd<int32_t>(reinterpret_cast<uint8_t*>((uintptr_t)n),0)==esi) return reinterpret_cast<int32_t*>((uintptr_t)n);
 }
 return nullptr;
}

// 1.13c D2Client+0x4FDD0 (eax = mask, esi = key)
void sub_4FDD0(Svc& s,uint32_t a,int32_t b,int32_t c,uint32_t eax,int32_t esi,int32_t edx){
 uint8_t* u=s.sub4D630(a,b);
 if(!u||!rd<uint32_t>(u,0x34)||edx==esi) return;
 int32_t* n=*reinterpret_cast<int32_t**>((uintptr_t)rd<uint32_t>(u,0x34));
 if(!n) return;
 while(*n!=esi){ n=reinterpret_cast<int32_t*>((uintptr_t)(uint32_t)n[2]); if(!n) return; }
 if(c) n[1]|=(int32_t)eax; else n[1]&=~(int32_t)eax;
}

// 1.13c D2Client+0x4FE20 (eax = key; edx = second arg for the 0x4FCE0 lookup)
void sub_4FE20(Svc& s,uint32_t a,int32_t b,int32_t eax,int32_t edx){
 uint8_t* u=s.sub4D630(a,b);
 if(!u||!rd<uint32_t>(u,0x34)||edx==eax) return;
 int32_t* head=reinterpret_cast<int32_t*>((uintptr_t)rd<uint32_t>(u,0x34));
 if(sub_4FCE0(s,a,edx,eax,edx)) return;
 int32_t* n=static_cast<int32_t*>(s.alloc(0x74,0));
 n[0]=0; n[1]=0; n[2]=0; n[0]=eax; n[2]=*head; *head=(int32_t)(uintptr_t)n;
}

// 1.13c D2Client+0x4FF10
int32_t sub_4FF10(Svc& s){
 int32_t x=s.gcb828(), y=s.gcb824();
 if(s.g8bc48()/2-0xc2<x && x<s.g8bc48()/2-0xa0 && s.g8bc4c()-0x2a<y && y<s.g8bc4c()-8) return 1;
 return 0;
}

// 1.13c D2Client+0x4A8C0
void sub_4A8C0(Svc& s,int32_t param_1){
 s.resetA();
 if(param_1==0||s.getCur()){ s.s48150(); return; }
 *s.gc9663()=1; *s.gc9729()=0;
 *s.gc966b()=s.o10973(s.gcbbfc(),0xe,0);
 s.s6e350(0,0); s.resetA();
 if(s.getCur()) s.fatal(0xdc9);
 s.s46070();
 int32_t* c=s.getCur(); if(c) c[0x31]&=~2;
 uint32_t old=s.gaada0();
 s.o10227();
 if(s.g8d5d0()!=0x26) s.fatal(0x139b);
 if(s.gaada0()==0){
  if(s.gc3a68()!=0){ s.s48150(); return; }
  for(uint32_t i=0;(int32_t)i<s.g8d5d0();++i){
   if(s.gaad80()[i]!=1) continue;
   int32_t t=s.entryType(i);
   if(t==0) continue;
   if(t==1) s.s72790(i,1,0);
   else if(t==4){ if(s.gc9721()) s.s47b20(); }
   else { s.s48150(); return; }
  }
 }
 uint32_t cur=s.gaada0();
 if(!s.gcbbfc()||(*s.gcbbfc()==0&&s.s55440()==0)){ cur=(cur==0); s.gaada0()=cur; }
 if(old!=0&&cur==0){ s.s6e2e0(); s.s492f0(); }
 s.s48930();
 int32_t i=s.s45c00();
 uint8_t* f=s.ga37bd(i);
 if(*f){
  s.getCur();
  if(s.s4cce0(1)){ s.s22eb0(); s.s38a70(s.gcbbfc(),0,1,0); }
  *f=0;
 }
}
}
