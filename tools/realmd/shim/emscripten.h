// Native build shim: the realm sources are written for the wasm engine and mark their exports with
// EMSCRIPTEN_KEEPALIVE. Compiled natively for realmd there is no emscripten, and nothing needs
// keeping alive, so the macro is empty. This exists so the SAME sources serve both targets -- a
// second native copy of a protocol codec would be a second thing to get wrong.
#pragma once
#define EMSCRIPTEN_KEEPALIVE
