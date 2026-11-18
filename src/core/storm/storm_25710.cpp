// 1.13c Storm+0x25710 -- hand one command-line token to the option engine.
#include "storm_batch_b144a.h"
#include <cstring>

namespace storm_b144a {
uint32_t handleToken(Ctx& c, const char* tok, uint32_t* cur, int32_t* st, uint32_t quoted, void* cb, void* errCb) {
    if (tok[0] == '@' && quoted == 0)
        return (c.load ? c.load(c, tok + 1, st, cur, cb, errCb) : loadFile(c, tok + 1, st, cur, cb, errCb)) ? 1 : 0;
    if (tok[0] && quoted == 0 && std::strchr(c.switches, tok[0])) {
        *st = 0;
        return (c.option ? c.option(c, tok + 1, st, errCb) : parseOption(c, tok + 1, st, errCb)) ? 1 : 0;
    }
    int32_t used = (int32_t)quoted;  // the stack slot doubles as the consumed-count out word
    if (*st != 0) return (c.chain ? c.chain(c, tok, st, &used) : applyChain(c, tok, st, &used)) ? 1 : 0;
    if (*cur != 0) {
        uint8_t* rec = c.st->resolve(*cur);
        if (!(c.arg ? c.arg(c, rec, tok, &used) : applyArg(c, rec, tok, &used))) return 0;
        rec = c.st->resolve(*cur);  // the slot is re-read after the callee
        int32_t nx; std::memcpy(&nx, rec + 4, 4);
        *cur = nx > 0 ? (uint32_t)nx : 0;
        return 1;
    }
    if (cb) return c.callTok ? c.callTok(cb, tok) : ((uint32_t(*)(const char*))cb)(tok);
    if (errCb) c.reportError(errCb, 0x85100065u, c.st->encode(tok));
    return 0;
}
}
