#pragma once

#include <cstdint>

namespace d2::monspawn {

// These 1.13c quest callbacks return zero even when they changed object mode and
// performed their work. Preserve ordinary callback return semantics everywhere
// else, and accept these exact callbacks only when the object actually changed.
inline bool acceptedObjectOperation(uint32_t classId, uint32_t operateFn,
                                    int32_t callbackResult, bool modeChanged) {
    if (callbackResult != 0) return true;
    // The well (OperateFn 22, 1.13c D2Game+0x6CEE0) always returns 0, refilled or dry; accepted as it was
    // while ours returned 1.
    if (operateFn == 22) return true;
    const bool zeroReturningQuestObject = (classId == 404 && operateFn == 53) ||
                                         (classId == 405 && operateFn == 57) ||
                                         (classId == 406 && operateFn == 59) ||
                                         (classId == 407 && operateFn == 58) ||
                                         (classId == 392 && operateFn == 54) ||
                                         (classId == 393 && operateFn == 52) ||
                                         (classId == 394 && operateFn == 55) ||
                                         (classId == 395 && operateFn == 52) ||
                                         (classId == 396 && operateFn == 56);
    return zeroReturningQuestObject && modeChanged;
}

// Keep the accepted callback result as the session command's return value. A
// native handler may successfully perform its work without changing the
// object's animation mode; modeChanged is only the success witness for the
// exact callbacks above that return zero.
inline int32_t sessionObjectOperationResult(uint32_t classId, uint32_t operateFn,
                                            int32_t callbackResult, bool modeChanged) {
    return acceptedObjectOperation(classId, operateFn, callbackResult, modeChanged) ? 1 : 0;
}

} // namespace d2::monspawn
