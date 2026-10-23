// 1.13c Storm+0x38F10 -- max value over list nodes whose key (either slot) matches `key` with a zero flag and nonzero flag2.
#include <cstdint>
struct S38F10Node {
    S38F10Node* next;  // +4 (retail: 32-bit link; +0 unused here)
    int8_t a;          // +8
    uint8_t aPad, aZero, aSet;  // +9..+B
    int8_t b;          // +C
    uint8_t bPad, bZero, bSet;  // +D..+F
    int32_t value;     // +0x14
};
int Storm_38F10(S38F10Node* head, int8_t key) {
    int best = 0;
    for (S38F10Node* n = head; n; n = n->next)
        if (((n->a == key && !n->aZero && n->aSet) || (n->b == key && !n->bZero && n->bSet)) && best < n->value)
            best = n->value;
    return best;
}
