// src/core/d2client/d2client_batch_c0450.h -- the Env of d2client_batch_c0450.cpp (its FN probes include it).
#pragma once
#include <cstdint>
namespace d2client_batch_c0450 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t); uint8_t (*rd8)(P); void (*wr8)(P, uint8_t);
 uint16_t (*rd16)(P); void (*wr16)(P, uint16_t);
 P dataTables;                       // *sgptDataTables
 P bitMasks10;                       // gdwBitMasks[0x10]
 P str_loadScreenGlobal, str_loadScreenLocal, str_fmtGlobal, str_fmtLocal; // sprintf format/dir strings
 P buf_c980; P str_c8359f; P tbl_fb80580, tbl_fb80864, tbl_fba51bc; // misc data addresses
 P (*f4d850)(P); P (*f4d630)(P, int32_t); bool (*f4dc60)(int32_t, int32_t, int32_t);
 void (*win2Unicode)(P, P, int32_t); void (*personalize)(P, P, P, int32_t, int32_t); void (*ustrcpy)(P, P);
 uint32_t (*o10009)(); P (*o10003)(); void (*o10070)(int32_t, int32_t, int32_t, int32_t);
 int32_t (*o10651)(P); int32_t (*o11142)(P); uint32_t (*o11080)(P); uint32_t (*o11168)(P);
 int32_t (*o10012)(uint32_t, uint32_t); void (*o10023)(uint32_t, uint32_t, int32_t, int32_t*, int32_t*); int32_t (*o10013)();
 uint32_t (*o10265)(uint32_t); void (*o10024)(P, uint32_t); void (*exit_)(int32_t);
 P (*f55d90)(); int32_t (*f70590)(); void (*f54da0)(P s0, P ebx);   // 0xA4DA0: one stack word (ret 4) and EBX
 void (*f6e860)(); void (*f0f090)(); void (*f31750)(); void (*f6e020)();
 uint32_t (*getTick)(); void (*o10065)(uint32_t); int32_t (*o10043)(int32_t, int32_t);
 P (*o10331)(P, uint32_t); int32_t (*o10031)(); int32_t (*o10494)(P, int32_t); int32_t (*o10871)(P, int32_t);
 P (*f12b0)(int32_t); int32_t (*o10973)(P, int32_t, int32_t); void (*f1b190)(P, int32_t, int32_t, int32_t);
 void (*f07c0)(P, int32_t); void (*f4b00)(); int32_t (*o10085)(int32_t, int32_t, int32_t, int32_t);
 void (*f71580)(); uint8_t (*o10190)(int32_t, int32_t, int32_t); void (*o10014)(int32_t, int32_t, int32_t, int32_t, uint32_t, int32_t);
 int32_t (*o10086)(uint32_t); void (*o10041)(const int32_t*, int32_t, int32_t, uint32_t, int32_t, int32_t); void (*o10040)();
 P (*fdb420)(const char*, int32_t);
 uint32_t (*ustrlen)(P); P (*f1a80)(); int32_t (*o10083)(); void (*f6f370)(int32_t, int32_t, uint32_t);
 void (*o10227)(); void (*f72790)(int32_t, int32_t, int32_t); void (*faf7b20)(); int32_t (*f55440)();
 void (*feccc0)(int32_t, P); void (*fecc60)(int32_t, P); int32_t (*o10048)(); int32_t (*o423)(const char*, const char*, int32_t, P);
 void (*o10066)();
 int32_t (*o10043z)(); void (*o10021z)(int32_t); void (*o10062z)(); // register/stack-arg forms (args not recovered)
 void (*fd6cd0)(); void (*fed470)(); void (*f27d30)(); void (*f2cb80)(); void (*f12f60)(); void (*fe5920)(); void (*ffffc0)();
 void (*f0afa0)(); void (*ff6640)(); void (*f11440)(); void (*f5a380)(); void (*f5a190)(); void (*f09740)(int32_t);
 void (*f6f720)(); void (*f3bf10)(); void (*f4fa90)(); void (*fccad0)(); void (*f3b840)(); void (*f53ad0)(); void (*f13d90)();
 void (*o10897)(int32_t, uint32_t);
 // C14F0 register/stack-exact seams (disasm): Fog#10043 free(ecx=blk, edx=file, 0x7a, 0); D2Win#10065 via thunk D39C takes cl; D2CMP#10024 one stack word; f31750 takes eax
 void (*freeFile)(P, P, int32_t, int32_t); void (*o10065cl)(uint8_t); void (*o10024w)(uint32_t); void (*f31750a)(uint32_t);
};
void sub_C0CE0(const Env& e);
void sub_C1760(const Env& e, int32_t p);   // the join loading-screen frame: one stdcall word
} // namespace d2client_batch_c0450
