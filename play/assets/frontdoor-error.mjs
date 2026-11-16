import { setOverlayText } from './overlay-text.mjs';

/** Show a player-facing front-door error in the viewport's accessible alert. */
export function savedCharacterReadError(error) {
  return `Unable to read saved characters: ${String(error?.message || error)}`;
}

export function setFrontdoorError(element, message) {
  if (!element) return;
  const text = String(message ?? '').trim();
  setOverlayText(element, text ? `Character select: ${text}` : '', { wrap: 760 });
  element.hidden = !text;
}
