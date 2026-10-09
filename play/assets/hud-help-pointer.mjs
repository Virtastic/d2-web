import { hudHelpButtonContains } from './hud-help.mjs';
import { createUiButtonGesture } from './ui-button-gesture.mjs';

/** Bind the Help socket's press/release gesture to a canvas-like event target. */
export function bindHudHelpPointer(target, {
  point,
  hit,
  enabled,
  canActivate,
  activate,
  focus = () => {},
  changed = () => {},
}) {
  const gesture = createUiButtonGesture();
  const consume = (event) => {
    event.__d2HelpPointerHandled = true;
    event.preventDefault?.();
  };
  const setDown = (pressed) => {
    changed(pressed);
  };

  target.addEventListener('pointerdown', (event) => {
    if (gesture.token !== null) {
      // Keep the current pointer as owner; another pointer cannot replace its held gesture.
      if (event.button === 0 && enabled()) consume(event);
      return;
    }
    const [x, y] = point(event);
    if (event.button !== 0 || !enabled() || !hudHelpButtonContains(x, y, hit())) return;
    gesture.press(event.pointerId, 'hud-help');
    setDown(true);
    try { target.setPointerCapture(event.pointerId); } catch {}
    consume(event);
    focus();
  });
  target.addEventListener('pointerup', (event) => {
    if (event.button !== 0 || gesture.token !== 'hud-help') return;
    const [x, y] = point(event), pressedHit = hit();
    const activation = gesture.release(event.pointerId,
      hudHelpButtonContains(x, y, pressedHit) ? 'hud-help' : null);
    if (activation === null && gesture.token !== null) return;
    consume(event);
    if (activation === 'hud-help' && enabled() && canActivate()) activate();
    setDown(false);
    focus();
  });
  const cancel = (event) => {
    if (!gesture.cancel(event.pointerId)) return;
    setDown(false);
  };
  target.addEventListener('pointercancel', cancel);
  target.addEventListener('lostpointercapture', cancel);

  return {
    get pressed() { return gesture.token === 'hud-help'; },
    clear() { if (gesture.cancel()) setDown(false); },
  };
}
