// CCritSect for the single-threaded wasm build: the class has no mutex member here (WHOA_SYSTEM_* unset),
// so every operation is a no-op. Game.cpp's hash table calls Enter(int)/Leave(int).
#include <storm/thread/CCritSect.hpp>

CCritSect::CCritSect() {}
CCritSect::~CCritSect() {}
void CCritSect::Enter() {}
void CCritSect::Enter(int32_t) {}
void CCritSect::Leave() {}
void CCritSect::Leave(int32_t) {}
