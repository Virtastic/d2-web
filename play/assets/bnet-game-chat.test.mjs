import test from 'node:test';
import assert from 'node:assert/strict';
import { gameChatPacket, isSayable, gameChatLine, gameChatBubble, routeGameLine, chatString, fillFormat } from './bnet-game-chat.mjs';

// the client packet sizer (net 0x65D0 case 0x15) must frame exactly what we build
function sizerLength(p) {
  const a = p.indexOf(0, 3) - 3, nameEnd = p.indexOf(0, 3 + a + 1);
  const t = a + 5 + (nameEnd - (3 + a + 1));
  return t + 1 + (p[t] << 24 >> 24);
}
test('say packet is 0x15, type 1, language, text, empty recipient, zero count', () => {
  const p = gameChatPacket('hi', { language: 0 });
  assert.deepEqual([...p], [0x15, 1, 0, 0x68, 0x69, 0, 0, 0]);
  assert.equal(sizerLength(p), p.length);
  assert.equal(p.length, 2 + 6);
});
test('whisper packet carries the recipient after the text', () => {
  const p = gameChatPacket('yo', { whisperTo: 'Bob' });
  assert.deepEqual([...p], [0x15, 2, 0, 0x79, 0x6f, 0, 0x42, 0x6f, 0x62, 0, 0]);
  assert.equal(sizerLength(p), p.length);
});
test('text is cut to 255 and stripped of NULs', () => {
  const p = gameChatPacket('a\0b'.repeat(200));
  assert.equal(p.indexOf(0, 3) - 3, 255);
  assert.equal(sizerLength(p), p.length);
});
test('slash lines and empty lines are not said', () => {
  assert.equal(isSayable('hello'), true);
  assert.equal(isSayable('/f l'), false);
  assert.equal(isSayable(''), false);
});
test('server chat lines', () => {
  const bytes = (s) => [...s].map((c) => c.charCodeAt(0));
  assert.equal(gameChatLine({ messageType: 1, subType: 2, name: bytes('Ann'), message: bytes('hey') }), 'ÿc4Annÿc0: hey');
  assert.equal(gameChatLine({ messageType: 4, name: [], message: bytes('sys') }), 'sys');
  assert.equal(gameChatLine({ messageType: 9, name: [], message: [] }), null);
  assert.equal(gameChatLine(null), null);
});

const bytes = (s) => [...s].map((c) => c.charCodeAt(0));
test('a whisper from a player reads "name whispers: text"; one with a bad sender name is the text alone', () => {
  assert.equal(gameChatLine({ messageType: 2, subType: 2, name: bytes('Ann'), message: bytes('psst') }), 'Ann whispers: psst');
  assert.equal(gameChatLine({ messageType: 2, subType: 2, name: [], message: bytes('psst') }), 'psst');
});
test('your own whisper is echoed as "You whispered to name: text" in white, the name in red', () => {
  assert.equal(gameChatLine({ messageType: 6, name: bytes('Bob'), message: bytes('yo') }), 'ÿc0You whispered to ÿc1Bobÿc0: yo');
});
test('the engine string wins over the built-in text, and a mojibake colour code is repaired', () => {
  const lookup = (id) => (id === 3654 ? ' murmurs: ' : id === 3658 ? 'Ã¿c0To Ã¿c1%sÃ¿c0: %s' : '');
  assert.equal(gameChatLine({ messageType: 2, name: bytes('Ann'), message: bytes('x') }, lookup), 'Ann murmurs: x');
  assert.equal(gameChatLine({ messageType: 6, name: bytes('B'), message: bytes('x') }, lookup), 'ÿc0To ÿc1Bÿc0: x');
  assert.equal(chatString(1, null, {}), '');
  assert.equal(fillFormat('%s and %s', 'a'), 'a and ');
});
test('a say from unit type 0 or 1 is not drawn as a line; an overhead line is a bubble, not a log line', () => {
  assert.equal(gameChatLine({ messageType: 1, subType: 0, name: bytes('Ann'), message: bytes('hey') }), null);
  assert.equal(gameChatLine({ messageType: 5, subType: 0, unitGuid: 7, name: bytes('Ann'), message: bytes('hey') }), null);
  assert.deepEqual(gameChatBubble({ messageType: 5, subType: 0, unitGuid: 7, name: [], message: bytes('hey') }), { unitType: 0, unitGuid: 7, text: 'hey' });
  assert.equal(gameChatBubble({ messageType: 1, message: [] }), null);
});
test('overhead packet is opcode 0x14, type 0', () => {
  assert.deepEqual([...gameChatPacket('hi', { overhead: true })], [0x14, 0, 0, 0x68, 0x69, 0, 0, 0]);
});
const roster = { find: (n) => (n.toLowerCase() === 'bob' ? { name: 'Bob' } : null) };
test('Enter: a plain line is said, "!" is overhead, "@name" is a whisper', () => {
  assert.deepEqual([...routeGameLine('hello', { roster })[0].send], [...gameChatPacket('hello')]);
  assert.deepEqual([...routeGameLine('!look', { roster })[0].send], [...gameChatPacket('look', { overhead: true })]);
  assert.deepEqual([...routeGameLine('@Bob hi there', { roster })[0].send], [...gameChatPacket('hi there', { whisperTo: 'Bob' })]);
  assert.deepEqual(routeGameLine('', { roster }), []);
  assert.deepEqual(routeGameLine('!', { roster }).map((a) => 'send' in a), [true]);
});
test('Enter: /w to a player in the game is a game whisper, to anyone else it goes to the realm chat', () => {
  assert.deepEqual([...routeGameLine('/w bob  hi  all', { roster })[0].send], [...gameChatPacket('hi  all', { whisperTo: 'bob' })]);
  assert.deepEqual([...routeGameLine('/MSG BOB yo', { roster })[0].send], [...gameChatPacket('yo', { whisperTo: 'BOB' })]);
  assert.deepEqual(routeGameLine('/whisper *Zed hey you', { roster }), [{ realm: '/whisper *Zed hey you' }]);
  assert.deepEqual(routeGameLine('/m Zed hey', { roster }), [{ realm: '/m Zed hey' }]);
});
test('Enter: a whisper without a name and a message shows the usage line', () => {
  assert.deepEqual(routeGameLine('/w', { roster }), [{ notice: '<BattleNet>: Correct usage is: /w (name) (message)' }]);
  assert.deepEqual(routeGameLine('/msg bob', { roster }), [{ notice: '<BattleNet>: Correct usage is: /msg (name) (message)' }]);
  assert.deepEqual(routeGameLine('/wx bob hi', { roster }), [{ realm: '/wx bob hi' }]);
});
test('Enter: /help and /? alone go to the realm; with more words they show the usage line', () => {
  assert.deepEqual(routeGameLine('/help', { roster }), [{ realm: '/help' }]);
  assert.deepEqual(routeGameLine('/? ', { roster }), [{ realm: '/? ' }].map((a) => ({ realm: a.realm })));
  assert.deepEqual(routeGameLine('/help me', { roster }), [{ notice: '<BattleNet>: Correct usage is: /help' }]);
});
test('Enter: other slash lines go to the realm chat and are never said; the client words stay local', () => {
  assert.deepEqual(routeGameLine('/f l', { roster }), [{ realm: '/f l' }]);
  assert.deepEqual(routeGameLine('/FPS', { roster }), [{ client: 'fps' }]);
  assert.deepEqual(routeGameLine('/framerate', { roster }), [{ client: 'fps' }]);
  assert.deepEqual(routeGameLine('/nopickup', { roster }), [{ client: 'nopickup' }]);
});
