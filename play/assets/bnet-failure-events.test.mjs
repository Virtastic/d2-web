import test from 'node:test';
import assert from 'node:assert/strict';
import { BNET_FAILURE_EVENTS, createWrongPasswordCounter, failureBox, failureEvent } from './bnet-failure-events.mjs';
import { BNET_CREATE_REFUSAL_STRINGS, BNET_AUTH_REFUSAL_STRINGS, BNET_LOGIN_ERROR_STRINGS, loginErrorBody, loginErrorMessage } from './bnet-login-screen.mjs';

const string = (i) => `s${i}`;

test('the event table is the one D2Launch+0xE510 holds: every event 1..0x1B has a row, the rest fall to the connect failure', () => {
  for (let e = 1; e <= 0x1b; e++) assert.ok(BNET_FAILURE_EVENTS[e], `event ${e}`);
  for (const e of [0, 0x1c, 0x40, -1]) assert.equal(failureEvent(e), failureEvent(0));
  assert.deepEqual(failureBox(0, { string }), { title: 's5248', body: 's5189\ns5190', kind: 2, disconnect: true, handler: 'menu' });
});

test('logon refusals 0xD/0xF are box 5244 with 5208/5209, rebuild the login screen and keep the connection', () => {
  assert.deepEqual(failureBox(0xd, { string }), { title: 's5244', body: 's5208', kind: 1, disconnect: false, handler: 'login' });
  assert.deepEqual(failureBox(0xf, { string }), { title: 's5244', body: 's5209', kind: 1, disconnect: false, handler: 'login' });
  assert.deepEqual(failureBox(0x12, { string }), { title: 's5248', body: 's5347', kind: 1, disconnect: true, handler: 'menu' });
  assert.equal(failureBox(0x10, { string }).body, 's5349');
  assert.equal(failureBox(0x11, { string }).body, 's5354');
  assert.equal(failureBox(0x1b, { string, text: 'banned' }).body, 'banned');
});

test('wrong password: 5207 the first time, 11107 on a repeat for the same account, 5207 again for another account', () => {
  const counter = createWrongPasswordCounter();
  const body = (account) => failureBox(0xe, { string, account, counter }).body;
  assert.deepEqual([body('Ama'), body('Ama'), body('Ama'), body('Bob'), body('Bob'), body('Ama')], ['s5207', 's11107', 's11107', 's5207', 's11107', 's5207']);
  assert.equal(body('ama'), 's5207', 'the compare is case-sensitive');
});

test('a name past fifteen characters never matches the copy the counter keeps', () => {
  const counter = createWrongPasswordCounter(), long = 'abcdefghijklmnop';
  assert.deepEqual([1, 2, 3].map(() => counter.refuse(long)), [1, 1, 1]);
  assert.equal(counter.name, long.slice(0, 15));
});

test('the login screen\'s wrong-password body follows the per-account counter', () => {
  const body = (account) => loginErrorBody({ logonResult: 2 }, account, string);
  const first = body('Zed');
  assert.ok(first === 's5207' || first === 's11107');
  assert.equal(body('Other1'), 's5207');
  assert.equal(body('Other1'), 's11107');
  assert.equal(body('Other2'), 's5207');
});

test('the create refusals 0x13..0x1A are the screen\'s 5249 table', () => {
  for (const [result, body] of Object.entries(BNET_CREATE_REFUSAL_STRINGS.byResult)) {
    const e = failureEvent(0x12 + Number(result));
    assert.equal(e.title, BNET_CREATE_REFUSAL_STRINGS.title);
    assert.equal(e.body, body);
    assert.equal(e.handler, 'create');
    assert.equal(e.disconnect, false);
  }
});

test('the key and version refusals are the screen\'s table, per event', () => {
  const A = BNET_AUTH_REFUSAL_STRINGS;
  const check = (kind, ev, expansion) => {
    const e = failureEvent(ev);
    assert.equal(e.title, A[kind].title, `${kind} title`);
    assert.equal(e.body, A[kind].body[expansion ? 1 : 0], `${kind} body`);
    assert.equal(e.detail ?? null, A[kind].detail, `${kind} detail`);
  };
  check('version', 1, false);
  check('invalid', 3, false); check('invalid', 7, true);
  check('inUse', 6, false); check('inUse', 10, true);
  check('disabled', 4, false); check('disabled', 8, true);
  check('wrongProduct', 5, false); check('wrongProduct', 9, true);
});

test('logon result codes map to the events: 1 no account, 2 wrong password, 3 closed, 6 the server\'s text, others the generic box', () => {
  const S = BNET_LOGIN_ERROR_STRINGS;
  const msg = (logonResult, extra = {}) => loginErrorMessage({ logonResult, ...extra }, 'Acct', string);
  assert.deepEqual(msg(1), { title: 's5244', body: `s${S.noAccount}` });
  assert.deepEqual(msg(3), { title: 's5244', body: `s${S.closed}` });
  assert.deepEqual(msg(6, { logonReason: 'because' }), { title: 's5244', body: 'because' });
  for (const r of [0, 4, 5, 7, 255]) assert.deepEqual(msg(r), { title: 's5248', body: 's5347' });
});
