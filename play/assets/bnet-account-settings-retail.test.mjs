// The Account Settings screens against retail's own frames of them (headless front-end capture of 1.13c D2Launch:
// the menu +0x10844, CHANGE PASSWORD +0x15F10, GET NEW PASSWORD +0x10A40, CHANGE EMAIL +0x103C0, and the notice or
// message box each one raises). The plain forms are tick 0 with empty boxes; the box states are tick 2 after the
// form's OK ran on the typed boxes. Identity is path + frame + colour shift, the anchor and the mode, in call
// order (the capture has no frame bytes). Skipped without the retail archives or the captures.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { diffDrawLists, parseDrawListVector } from './drawlist.mjs';
import { createBnetLoginScreen, createAccountError, changePasswordError } from './bnet-login-screen.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const vec = (name) => join(root, 'vectors', 'drawlist', `${name}.txt`);
const have = (['account-settings', 'cpass', 'gpass', 'cemail', 'cpass-error', 'cemail-error', 'cemail-same', 'gpass-receipt', 'gpass-offline', 'cpass-refused-first', 'cpass-refused-repeat'].concat([1,2,3,4,5,6,7,8].map((n) => `create-refused-${n}`)))
  .every((n) => existsSync(vec(`bnet-${n}`)));
const rd = have ? openRetail() : null;

// D2Lang indices as the screen asks: below 10000 string.tbl, 10000s patchstring.tbl, 20000s expansionstring.tbl.
function stringTable(read) {
  const tbl = (name, i) => { const d = Buffer.from(read(`data\\local\\lng\\eng\\${name}.tbl`)); const n = d.readUInt16LE(2);
    const e = 21 + n * 2 + d.readUInt16LE(21 + i * 2) * 17, vo = d.readUInt32LE(e + 11); return d.toString('latin1', vo, d.indexOf(0, vo)); };
  return { get: (i) => (i < 10000 ? tbl('string', i) : i < 20000 ? tbl('patchstring', i - 10000) : tbl('expansionstring', i - 20000)) };
}
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (t, k) => (k in t ? t[k] : (t[k] = () => ({ data: new Uint8ClampedArray(4) }))), set: (t, k, v) => { t[k] = v; return true; } });
const documentObject = { createElement: () => ({ width: 1, height: 1, getContext: () => ctx }) };

// Retail's clock in the capture is tick * 250 ms: the caret shows on odd ticks.
function build(options = {}) {
  const strings = stringTable(rd);
  const clock = { ms: options.startMs ?? 0 };
  const screen = createBnetLoginScreen({ read: rd, decodeDc6, decodeDatPalette, string: (i) => strings.get(i) ?? '', documentObject, region: 'test',
    now: () => clock.ms, onChangePassword: async () => 1, onResetPassword: async () => {}, onChangeEmail: async () => {}, ...options });
  screen.clock = clock;
  return screen;
}
const type = (screen, s) => { for (const k of s) screen.typeKey('Key', k); };
const identity = (l) => l.filter((r) => (r.op === 'cel' || r.op === 'celcolor') && !/ohand/i.test(r.path))
  .map((r) => ({ ...r, path: `${r.path}#${r.extra ?? 0}`, hash: 0 }));

function compare(name, screen, tickMs) {
  screen.clock.ms = tickMs;
  globalThis.__d2DrawList = [];
  try { screen.draw(ctx, 800, 600); } finally { var l = globalThis.__d2DrawList; delete globalThis.__d2DrawList; }
  const retail = identity(parseDrawListVector(readFileSync(vec(name), 'utf8')));
  const ours = identity(l);
  const d = diffDrawLists(retail, ours);
  const show = (x) => `${x.op} ${x.path} #${x.frame} @${x.x},${x.y} m${x.mode}`;
  console.log(`${name}: retail ${retail.length}, ours ${ours.length}, differing records ${d.diffs} (missing ${d.missing.length}, extra ${d.extra.length}, differing ${d.differing.length})`);
  for (const m of d.missing.slice(0, 10)) console.log('  missing', show(m));
  for (const m of d.extra.slice(0, 10)) console.log('  extra', show(m));
  for (const m of d.differing.slice(0, 10)) console.log('  differs', m.draw, m.diff);
  assert.ok(diffDrawLists(retail, ours.slice(0, -1)).diffs > 0, 'the comparison cannot fail');
  assert.equal(d.diffs, 0);
}

// The launcher's saved account is "ama" in every state capture, and the plain forms have it empty.
const toSettings = (screen, account) => { type(screen, account); screen.activate('settings'); };

test('the account settings menu draws retail tick 0', { skip: !rd }, () => {
  compare('bnet-account-settings', (() => { const s = build(); s.activate('settings'); return s; })(), 0);
});
test('CHANGE PASSWORD draws retail tick 0', { skip: !rd }, () => {
  const s = build(); toSettings(s, ''); s.activate('change-password'); compare('bnet-cpass', s, 0);
});
test('GET NEW PASSWORD draws retail tick 0', { skip: !rd }, () => {
  const s = build(); toSettings(s, ''); s.activate('get-password'); compare('bnet-gpass', s, 0);
});
test('CHANGE EMAIL draws retail tick 0', { skip: !rd }, () => {
  const s = build(); toSettings(s, ''); s.activate('change-email'); compare('bnet-cemail', s, 0);
});
test('CHANGE PASSWORD with mismatched new passwords draws retail notice', { skip: !rd }, () => {
  const s = build({ startMs: 250 }); toSettings(s, 'ama'); s.activate('change-password');
  type(s, 'old1'); s.typeKey('Tab', 'Tab'); type(s, 'new1'); s.typeKey('Tab', 'Tab'); type(s, 'new2'); s.typeKey('Enter', 'Enter');
  compare('bnet-cpass-error', s, 500);
});
// Retail's frames of a refused change (headless front-end capture: the form filled with ama / oldpass / newpass / newpass, OK
// run with the reply the stub getter returns, tick 5): the login failure box over the form. Any reply but 1 raises it
// (the capture tried 0, 2 and 3: the same frame). D2Launch counts refusals in ONE process-wide counter (0x6fa64dac), so the
// first refusal is 5207 and every one after it 11107, for any account. The two tests run in this order in one process.
async function refuse(reply) {
  const s = build({ startMs: 250, onChangePassword: async () => reply }); toSettings(s, 'ama'); s.activate('change-password');
  type(s, 'oldpass'); s.typeKey('Tab', 'Tab'); type(s, 'newpass'); s.typeKey('Tab', 'Tab'); type(s, 'newpass');
  s.activate('field-old'); // the capture filled the boxes of a form built with the account in place, so its caret stays on the current password
  s.typeKey('Enter', 'Enter');
  await new Promise((r) => setTimeout(r, 0));
  return s;
}
test('the first refused password change draws retail 5207 box over the form', { skip: !rd }, async () => {
  compare('bnet-cpass-refused-first', await refuse(0), 1250);
});
test('the repeat refusal, for any account, draws retail 11107 box', { skip: !rd }, async () => {
  compare('bnet-cpass-refused-repeat', await refuse(2), 1250);
  for (const reply of [0, 2, 3, 4, 'x']) assert.ok((await refuse(reply)).evidence.message, `reply ${reply} raises the failure box`);
});
test('an accepted password change raises no box and logs on with the new password', { skip: !rd }, async () => {
  const calls = [];
  const s = build({ startMs: 250, onChangePassword: async () => 1, onAuthenticate: async (c) => { calls.push(c); } });
  toSettings(s, 'ama'); s.activate('change-password');
  type(s, 'oldpass'); s.typeKey('Tab', 'Tab'); type(s, 'newpass'); s.typeKey('Tab', 'Tab'); type(s, 'newpass'); s.typeKey('Enter', 'Enter');
  await new Promise((r) => setTimeout(r, 0));
  assert.equal(s.evidence.message, null);
  assert.deepEqual(calls, [{ account: 'ama', password: 'newpass' }]);
});
// Retail's frames of a refused account creation (capture: create form filled ama / newpass / newpass, its OK, PLEASE READ,
// its OK with the stub getter returning n, tick 5): title 5249, the body by n (1 and 9+ share 5231), over the create form,
// whose OK rebuilds the form. Reply 0 is the success: no box, straight on to the character select.
for (const n of [1, 2, 3, 4, 5, 6, 7, 8]) {
  test(`a create refusal ${n} draws retail box`, { skip: !rd }, async () => {
    const s = build({ startMs: 250, onCreateAccount: async () => { throw Object.assign(new Error('refused'), { createResult: n }); } });
    s.activate('create'); s.activate('field-account'); type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'newpass'); s.typeKey('Tab', 'Tab'); type(s, 'newpass');
    s.typeKey('Enter', 'Enter'); s.activate('read-ok');
    await new Promise((r) => setTimeout(r, 0));
    compare(`bnet-create-refused-${n}`, s, 1250);
    s.activate('ok');
    assert.equal(s.mode, 'create');
  });
}
test('a created account raises no box and logs on with it', { skip: !rd }, async () => {
  const calls = [];
  const s = build({ startMs: 250, onCreateAccount: async () => {}, onAuthenticate: async (c) => { calls.push(c); } });
  s.activate('create'); type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'newpass'); s.typeKey('Tab', 'Tab'); type(s, 'newpass');
  s.typeKey('Enter', 'Enter'); s.activate('read-ok'); await new Promise((r) => setTimeout(r, 0));
  assert.equal(s.evidence.message, null);
  assert.deepEqual(calls, [{ account: 'ama', password: 'newpass' }]);
});
test('create refusal codes past 8 show the same text as 1', { skip: !rd }, async () => {
  const body = async (n) => {
    const s = build({ startMs: 250, onCreateAccount: async () => { throw Object.assign(new Error('refused'), { createResult: n }); } });
    s.activate('create'); type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'pw'); s.typeKey('Tab', 'Tab'); type(s, 'pw');
    s.typeKey('Enter', 'Enter'); s.activate('read-ok'); await new Promise((r) => setTimeout(r, 0));
    return s.evidence.message.body;
  };
  assert.equal(await body(9), await body(1));
});
test('CHANGE EMAIL with mismatched new addresses draws retail message', { skip: !rd }, () => {
  const s = build({ startMs: 250 }); toSettings(s, 'ama'); s.activate('change-email');
  s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Tab', 'Tab'); type(s, 'x@y.z'); s.typeKey('Tab', 'Tab'); type(s, 'q@r.s'); s.typeKey('Enter', 'Enter');
  compare('bnet-cemail-error', s, 500);
});
test('CHANGE EMAIL with the old address repeated draws retail message', { skip: !rd }, () => {
  const s = build({ startMs: 250 }); toSettings(s, 'ama'); s.activate('change-email');
  s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Enter', 'Enter');
  compare('bnet-cemail-same', s, 500);
});
test('GET NEW PASSWORD receipt draws retail message', { skip: !rd }, async () => {
  const s = build({ startMs: 250 }); toSettings(s, 'ama'); s.activate('get-password');
  s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Enter', 'Enter');
  await new Promise((r) => setTimeout(r, 0));
  compare('bnet-gpass-receipt', s, 500);
});
test('GET NEW PASSWORD offline draws retail connect failure notice', { skip: !rd }, async () => {
  const s = build({ startMs: 250, onResetPassword: async () => { throw new Error('offline'); } }); toSettings(s, 'ama'); s.activate('get-password');
  s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Enter', 'Enter');
  await new Promise((r) => setTimeout(r, 0));
  compare('bnet-gpass-offline', s, 500);
});

// The create form's own rules (headless capture: the boxes set through D2Win's edit set-text, the form's OK run, tick 5): the first
// failing rule names the notice, the form stays under it with the typed text (the account box scrolls to the tail of a long name,
// the password boxes show at most 15 stars). 2..15 for the account, 2..20 for the password, whatever the notice text says.
const createRules = {
  'account-short': ['a', 'abcd', 'abcd'], 'account-long': ['abcdefghijklmnop', 'abcd', 'abcd'], mismatch: ['abcd', 'abcd', 'abce'],
  'password-short': ['abcd', 'a', 'a'], 'password-long': ['abcd', 'abcdefghijklmnopqrstu', 'abcdefghijklmnopqrstu'],
};
for (const [rule, [account, password, verify]] of Object.entries(createRules)) {
  // The password boxes hold 13 characters when typed (the capture's set-text is not limited), so a password past 20 cannot be typed:
  // its notice is compared by rule and text, the vector is kept as the evidence of the draw.
  if (rule === 'password-long') {
    test('create form rule password-long: 21 characters name notice 5220, whose text is the 15-character one', { skip: !rd }, () => {
      assert.equal(createAccountError(account, password, verify), 5220);
      assert.equal(createAccountError(account, 'a'.repeat(20), 'a'.repeat(20)), null);
      assert.match(stringTable(rd).get(5220), /more than 15/);
    });
    continue;
  }
  test(`create form rule ${rule} draws retail notice over the form`, { skip: !rd || !existsSync(vec(`bnet-create-error-${rule}`)) }, () => {
    const s = build({ startMs: 250 });
    s.activate('create'); s.activate('field-account'); type(s, account); s.typeKey('Tab', 'Tab'); type(s, password); s.typeKey('Tab', 'Tab'); type(s, verify);
    s.activate('field-account'); // the capture set the boxes without moving the focus, so the caret stays in the account box
    s.typeKey('Enter', 'Enter');
    compare(`bnet-create-error-${rule}`, s, 1250);
  });
}

// CHANGE PASSWORD's own rules (the same capture with the form of +0x15F10 and the OK of +0x1A030, tick 5): the same order and notices as
// the create form's. The new password box holds 13 characters when typed, so the 21-character case is by rule and vector only.
for (const [rule, [account, password, verify]] of Object.entries(createRules)) {
  if (rule === 'password-long') continue;
  test(`change password rule ${rule} draws retail notice over the form`, { skip: !rd || !existsSync(vec(`bnet-cpass-error-${rule}`)) }, () => {
    const s = build({ startMs: 250 }); toSettings(s, account); s.activate('change-password');
    s.activate('field-old'); type(s, 'oldpass'); s.typeKey('Tab', 'Tab'); type(s, password); s.typeKey('Tab', 'Tab'); type(s, verify);
    s.activate('field-account'); // the capture set the boxes without moving the focus
    s.typeKey('Enter', 'Enter');
    compare(`bnet-cpass-error-${rule}`, s, 1250);
  });
}
test('change password rule password-long: 21 characters name notice 5220', { skip: !rd }, () => {
  assert.equal(changePasswordError('abcd', 'abcdefghijklmnopqrstu', 'abcdefghijklmnopqrstu'), 5220);
});

// CHANGE EMAIL with valid old and new addresses (headless capture: the four boxes set, tick 3, no OK): SEND (the left button, 627,572) is
// drawn disabled (mode 1) while the verify address is not an address, and enabled (mode 5) once it is. Setting the text runs the test.
for (const [name, verify] of [['bad-verify', 'bad'], ['valid', 'x@y.z']]) {
  test(`CHANGE EMAIL with a ${name} verify address draws retail SEND state`, { skip: !rd || !existsSync(vec(`bnet-cemail-${name}`)) }, () => {
    const s = build({ startMs: 250 }); toSettings(s, 'ama'); s.activate('change-email');
    s.typeKey('Tab', 'Tab'); type(s, 'a@b.c'); s.typeKey('Tab', 'Tab'); type(s, 'x@y.z'); s.typeKey('Tab', 'Tab'); type(s, verify);
    s.activate('field-account');
    compare(`bnet-cemail-${name}`, s, 750);
  });
}

// GET NEW PASSWORD and CHANGE EMAIL with the realm socket down (Bnclient is-connected slot answering 0 when the button is pressed; tick 3):
// no form, the 5347 notice over the account settings menu. Both frames are the same.
for (const [name, button] of [['gpass', 'get-password'], ['cemail', 'change-email']]) {
  test(`${button} with the realm socket down draws retail 5347 notice over the menu`, { skip: !rd || !existsSync(vec(`bnet-${name}-disconnected`)) }, () => {
    const s = build({ startMs: 250, isConnected: () => false }); toSettings(s, 'ama'); s.activate(button);
    compare(`bnet-${name}-disconnected`, s, 750);
    assert.equal(s.mode, 'settings');
  });
}
