// Types of the D2Client debug-overlay / hireable-label helpers (d2client_batch_5a5e0.cpp).
#pragma once
#include <cstdint>
namespace d2client_batch_5a5e0 {
struct Services {
 virtual ~Services()=default;
 virtual uint32_t* cacheStats()=0;           // Ordinal_10073: 0x1C-dword stats block or null
 virtual void cacheReset()=0;                // Ordinal_10064
 virtual void tileStats(int32_t& a,int32_t& b,int32_t& c,int32_t& d)=0; // Ordinal_10074
 virtual void drawText(int32_t x,int32_t y,const char* s)=0; // Ordinal_10050 (already formatted)
 virtual uint32_t tick()=0; virtual uint32_t videoMode()=0;
 virtual void memStatus(uint32_t& tp,uint32_t& ap,uint32_t& tf,uint32_t& af)=0;
 virtual int32_t gfxMem()=0; virtual int32_t spriteCacheStat()=0; // Ordinal_10032(Ordinal_10058())
 virtual int32_t asyncFiles()=0; virtual int32_t celCacheStat()=0; // Ordinal_10082(Ordinal_10010())
 virtual int32_t textWidth(const uint16_t* s)=0;   // 0xD35A (D2Win text width, current font)
 virtual void showText(const uint16_t* s,int32_t x,int32_t y1,int32_t y2)=0; // 0xD37E (ECX text, EDX x, stack y1, y2, 0)
 virtual uint32_t fontSet(uint32_t id)=0;            // 0xD3A8: sets the font, returns the previous one
 virtual void label(uint16_t* buf,int32_t a0,int32_t a1,int32_t a2)=0; // 0x5AC70 (ESI = buf, stack a0 a1 a2)
 virtual void setFlag(int32_t,int32_t,int32_t)=0; // FUN_72790
 virtual bool regQuery(const char* app,const char* key)=0; // Ordinal_423 (null result => default)
 virtual const uint16_t* stringById(int32_t id)=0;  // 0xD1EC (D2Lang 10003: the wide table string)
 virtual void freeEntry(int32_t h)=0; virtual void freeBuf()=0;
};
struct State {
 uint32_t fpsLast{},fpsNow{},fpsAcc{},fpsSkipBase{},fpsEnable{},fpsFlag{},detail{};
 int32_t ping{},skip{},cacheHandles[7]{},*hireTable{},hireCount{},hireMax{};
 uint32_t popupHireling{},hireFlag{};
 int32_t* dataTables{};
};
void drawLabel(Services& s,uint16_t* buf,int32_t a0,int32_t a1,int32_t a2);
void drawNumber(Services& s,int32_t n,int32_t yarg);
void drawAsciiLabel(Services& s,const char* a,int32_t b,int32_t c);
void drawHireableName(Services& s,State& st,int32_t idx,int32_t b,int32_t c,const uint8_t* rows,int32_t count);
void cacheOverlay(Services& s);
}
