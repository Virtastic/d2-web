/** Select the next DC6 frame for an enabled type-6 D2Win control while it is held. */
export function d2winHeldFrame(frames, baseFrame = 0, pressed = false, enabled = true) {
  const heldFrame = baseFrame + 1;
  return pressed && enabled && frames?.[heldFrame] ? heldFrame : baseFrame;
}
