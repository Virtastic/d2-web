// Retail's configured Say actions are D2GS character-phrase requests (0x3F plus a little-endian
// phrase word), not ordinary typed chat text. Phrase IDs come from the protocol's 1.13c vocabulary.
const PHRASE_ACTIONS = Object.freeze([
  ["Say 'Help'", 0x19], ["Say 'Follow me'", 0x1a], ["Say 'This is for you'", 0x1b],
  ["Say 'Thanks'", 0x1c], ["Say 'Sorry'", 0x1d], ["Say 'Bye'", 0x1e],
  ["Say 'Now you die'", 0x1f], ["Say 'Retreat'", 0x20],
]);
const PHRASE_IDS = new Map(PHRASE_ACTIONS);

export function sendCharacterPhrase(action, transport) {
  const phrase = PHRASE_IDS.get(action);
  if (phrase === undefined) return { handled: false, sent: false };
  if (!transport?.isOpen || typeof transport.send !== 'function') return { handled: true, sent: false, phrase };
  const packet = Uint8Array.of(0x3f, phrase & 0xff, phrase >>> 8);
  return { handled: true, sent: transport.send(packet) !== false, phrase };
}
