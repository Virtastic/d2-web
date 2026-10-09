import test from 'node:test';
import assert from 'node:assert/strict';
import { sendCharacterPhrase } from '../play/assets/chat-macros.mjs';

test('configured Say actions send the native D2GS 0x3F phrase word', () => {
  const actions = ["Say 'Help'", "Say 'Follow me'", "Say 'This is for you'", "Say 'Thanks'",
    "Say 'Sorry'", "Say 'Bye'", "Say 'Now you die'", "Say 'Retreat'"];
  for (const [i, action] of actions.entries()) {
    let sent;
    const transport = { isOpen: true, send(packet) { sent = packet; return true; } };
    assert.deepEqual(sendCharacterPhrase(action, transport), { handled: true, sent: true, phrase: 0x19 + i });
    assert.ok(sent instanceof Uint8Array);
    assert.deepEqual([...sent], [0x3f, 0x19 + i, 0]);
  }
});

test('Say actions are consumed without sending when no realm session is open', () => {
  assert.deepEqual(sendCharacterPhrase("Say 'Help'", null), { handled: true, sent: false, phrase: 0x19 });
  assert.deepEqual(sendCharacterPhrase("Say 'Help'", { isOpen: false, send() { throw new Error('must not send'); } }),
    { handled: true, sent: false, phrase: 0x19 });
  assert.deepEqual(sendCharacterPhrase('Inventory Screen', { isOpen: true, send() {} }), { handled: false, sent: false });
});
