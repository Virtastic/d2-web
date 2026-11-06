// D2Launch n31 batch types shared with the raw-seam cores (d2launch_ee10.cpp).
#pragma once
#include <cstdint>
#include <string>
#include <functional>
namespace d2launch_n31 {
struct BnClient { std::function<int(int)> vt; };  // vt(offset): 0x10 disconnect, 0x4c, 0x48, 0x28, 0x68, 0x80, 0x98...

struct Host {
  std::function<void(const char*)> debugOut;       // OutputDebugStringA
  std::function<void()> o10018;                    // Ordinal_10018
  std::function<void(const char*)> messageBox;     // MessageBoxA "D2Multi" MB_ICONERROR
  std::function<void(int)> fatal;                  // Ordinal_10265(id)+Ordinal_10024+_exit(-1)
  std::function<bool(void**, int*, int*)> loadBnclient;  // iface, stage(0 ok,1 load,2 QueryInterface), err
  std::function<std::u16string(int)> str;          // Ordinal_10003 string by id
  std::function<void(const std::u16string&, std::function<void()>, int)> dlgOk;       // FUN_6fa55090
  std::function<void()> dlgInfo;                   // FUN_6fa555b0
  std::function<void(const std::u16string&, const std::u16string&, const std::u16string&)> dlgYesNo;  // FUN_6fa55400
  std::function<void(const std::u16string&)> dlg6;  // FUN_6fa55230
  std::function<uint32_t()> tick;                  // GetTickCount
  std::function<int(int)> ctl;                     // BNclient vtable-free call by offset
  std::function<void(int, int)> dlgSel;            // optional: dialog kind (1 = 0x155B0, 2 = 0x15400) and handler index, before the dialog seam
};

struct State {
  BnClient* bn = nullptr;            // DAT_6fa656c4
  uint8_t* ctx = nullptr;            // DAT_6fa65abc
  int ea0 = 0, ea8 = 0, eb0 = 0, e6c = 0, dac = 0, a294 = 0;
  uint8_t d2sOut[0x14f + 0x20];      // DAT_6fa65570
  int cnt = 0; int32_t list[0x40];   // DAT_6fa65e34 / DAT_6fa653c0
  int32_t hStatus = 0, hStatus2 = 0, strId = -1, a554 = 0; uint32_t startTick = 0;  // 65548/6554c/65550/65554/65558
  uint8_t classDefault[16] = {0};    // DAT_6fa64d9c
};

struct ConnSeams {
  std::function<void()> fc40, f582d0, f57df0, f549e0, saveGateway, terminate, o10014a, o10014b, o10032;
  std::function<void(const char*, const char*, int, int)> o426;
  int ad4 = 0; int a0 = 0; int a4 = 0;               // DAT_6fa65ad4, DAT_6fa65ea0 , DAT_6fa65ea4
  int errCode = 0;                                   // aiStack_c[0] after call 0x28
  int out48 = 0;                                     // the out word slot 0x48 fills (the retail EDI test is this word)
  int be0 = 0;                                       // DAT_6fa65be0
  std::function<int(const uint32_t*)> e510;          // the E510 callee as a seam (null: the port's own)
};
int FUN_6fa4e510(State& s, Host& h, const uint32_t* msg, const char* arg, const std::function<std::u16string(const char*)>& win2uni);
int FUN_6fa4ee10(State& s, Host& h, ConnSeams& c, const std::function<std::u16string(const char*)>& w, int edi);
}  // namespace d2launch_n31
