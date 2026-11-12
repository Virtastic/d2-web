// The browser sound player's draw for a Sounds.txt group variant (1.13c D2Client+0x5EE70 draws from the client hero seed
// through 0x89640). Exported so the variant pick consumes the same seed the client's other draws use.
#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2_session_client_rand(uint32_t n);

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_sound_rand(uint32_t n) { return d2_session_client_rand(n); }
