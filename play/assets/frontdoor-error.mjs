/** Show a player-facing front-door error in the viewport's accessible alert. */
export function savedCharacterReadError(error) {
  return `Unable to read saved characters: ${String(error?.message || error)}`;
}

export function setFrontdoorError(element, message) {
  if (!element) return;
  const text = String(message ?? '').trim();
  element.textContent = text ? `Character select: ${text}` : '';
  element.hidden = !text;
}
