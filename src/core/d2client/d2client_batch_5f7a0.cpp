// AVL-style rotations on a balance-tagged binary node (balance int16 at +0x0A, links at +0x0C/+0x10).
// Retail nodes hold 32-bit pointers; links are native pointers here, offsets are logical.
// Fatal paths (retail: Ordinal_10265/10024 then _exit(-1)) throw.
#include <cstdint>
#include <stdexcept>
#include <cstdlib>
extern "C" uint32_t Fog_Ordinal_10265(uint32_t);
extern "C" void Fog_Ordinal_10024(const void*, const void*, uint32_t);
namespace d2client_avl_5f7a0 {
[[noreturn]] static void fatal(uint32_t c) { static const char m[] = ""; Fog_Ordinal_10024(m, 0, Fog_Ordinal_10265(c)); std::_Exit(-1); }
struct Node { int16_t balance; Node* a; Node* b; };  // balance @+0x0A, a @+0x0C, b @+0x10
// 1.13c D2Client+0x5F7A0
static void rotateA(Node** p) {
 Node* n = *p; if (!n) throw std::runtime_error("5F7A0 null");
 Node* r = n->a; if (!r) throw std::runtime_error("5F7A0 no link");
 n->a = r->b; r->b = n; *p = r;
}
// 1.13c D2Client+0x5F7F0
static void rotateB(Node** p) {
 Node* n = *p; if (!n) throw std::runtime_error("5F7F0 null");
 Node* r = n->b; if (!r) throw std::runtime_error("5F7F0 no link");
 n->b = r->a; r->a = n; *p = r;
}
// 1.13c D2Client+0x60750
static void rebalanceB(Node** pp) {
 Node* n = *pp; Node* c = n->b;
 if (c->balance != -1 && c->balance != 1) fatal(0x294);
 if (c->balance == 1) { n->balance = 0; c->balance = 0; rotateB(&n); *pp = n; return; }
 Node* g = c->a;
 if (g->balance == -1) { n->balance = 0; c->balance = 1; }
 else if (g->balance == 0) { n->balance = 0; c->balance = 0; }
 else if (g->balance == 1) { n->balance = -1; c->balance = 0; }
 g->balance = 0;
 rotateA(&n->b); rotateB(&n); *pp = n;
}
// 1.13c D2Client+0x60810
static void rebalanceA(Node** pp) {
 Node* n = *pp; Node* c = n->a;
 if (c->balance != -1 && c->balance != 1) fatal(0x259);
 if (c->balance == -1) { n->balance = 0; c->balance = 0; rotateA(&n); *pp = n; return; }
 Node* g = c->b;
 if (g->balance == -1) { n->balance = 1; c->balance = 0; }
 else if (g->balance == 0) { n->balance = 0; c->balance = 0; }
 else if (g->balance == 1) { n->balance = 0; c->balance = -1; }
 g->balance = 0;
 rotateB(&n->a); rotateA(&n); *pp = n;
}
}
