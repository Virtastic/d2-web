// Retail's Battle.net failure-event table (1.13c D2Launch+0xE510, read off its disassembly: the decompile hides
// the string arguments). An event number picks a title, a body, an optional detail line, a box kind and what the
// box's OK does. Kinds: 1 = the message box (title, body), 2 = the three-string box (title, body, detail).
// `disconnect` closes the Bnclient connection after raising the box; `handler` is where OK goes.
//   - 'menu'        D2Launch+0x7DF0: back to the main menu
//   - 'login'       D2Launch+0x19290: the login screen is rebuilt
//   - 'create'      D2Launch+0x17780: the create form is rebuilt
// Events 0xD..0xF are the logon refusals, 0x13..0x1A the create refusals, 1..12 the key/version refusals; 0, 0x1C and
// up fall to the default (connect failure) row.
const row = (title, body, kind, extra = {}) => ({ title, body, detail: null, kind, disconnect: true, handler: 'menu', ...extra });
const AUTH_TITLE = 5247;
export const BNET_FAILURE_EVENTS = {
  1: row(5245, 5187, 2, { detail: 5188 }),
  2: row(5246, 5183, 1),
  3: row(AUTH_TITLE, 5186, 2, { detail: 5194 }), 7: row(AUTH_TITLE, 10912, 2, { detail: 5194 }),
  4: row(AUTH_TITLE, 5199, 1), 8: row(AUTH_TITLE, 10913, 1),
  5: row(AUTH_TITLE, 5202, 1), 9: row(AUTH_TITLE, 5202, 1),
  6: row(AUTH_TITLE, 5200, 1, { detail: 5201, owner: true }), 10: row(AUTH_TITLE, 10914, 1, { detail: 5201, owner: true }),
  11: row(AUTH_TITLE, 5356, 1),
  12: row(AUTH_TITLE, 5189, 2, { detail: 5190 }),
  13: row(5244, 5208, 1, { disconnect: false, handler: 'login' }),
  14: row(5244, 5207, 1, { disconnect: false, handler: 'login', repeatBody: 11107 }),
  15: row(5244, 5209, 1, { disconnect: false, handler: 'login' }),
  16: row(5248, 5349, 1), 17: row(5248, 5354, 1), 18: row(5248, 5347, 1),
  19: row(5249, 5231, 1, { disconnect: false, handler: 'create' }), 20: row(5249, 5232, 1, { disconnect: false, handler: 'create' }),
  21: row(5249, 5233, 1, { disconnect: false, handler: 'create' }), 22: row(5249, 5239, 1, { disconnect: false, handler: 'create' }),
  23: row(5249, 5234, 1, { disconnect: false, handler: 'create' }), 24: row(5249, 5235, 1, { disconnect: false, handler: 'create' }),
  25: row(5249, 5236, 1, { disconnect: false, handler: 'create' }), 26: row(5249, 5237, 1, { disconnect: false, handler: 'create' }),
  // 0x1B: a logon refusal that carries the server's own text as the body.
  27: row(5244, null, 1, { disconnect: false, handler: 'login', serverText: true }),
};
const DEFAULT_EVENT = row(5248, 5189, 2, { detail: 5190 });
export const failureEvent = (event) => BNET_FAILURE_EVENTS[event] ?? DEFAULT_EVENT;

// D2Launch+0xE510 event 0xE's counter (0x6fa64dac) and the name it last saw (0x6fa656c8, 16 bytes). The same name
// (a case-sensitive compare) counts up; any other name restarts at 1. From the second refusal for one name the body
// is 11107 (and the login screen gains its extra control); the first is 5207.
export function createWrongPasswordCounter() {
  let name = '', count = 0;
  return {
    refuse(account) {
      // The compare is against the whole typed name, the store keeps 15 characters: a 16-character name never matches itself.
      const typed = String(account ?? '');
      if (typed === name) count++; else { count = 1; name = typed.slice(0, 15); }
      return count;
    },
    get count() { return count; },
    get name() { return name; },
  };
}
// The one process-wide counter, like retail's global.
export const wrongPasswords = createWrongPasswordCounter();

// The box for an event: strings resolved through `string`. `account` feeds event 0xE's counter, `text` the server's
// own text (events 6/10 owner name, 0x1B reason).
export function failureBox(event, { string, account = '', text = '', counter = wrongPasswords } = {}) {
  const e = failureEvent(event), s = (id) => string(id) || '';
  let body = e.serverText ? String(text ?? '') : s(e.body);
  if (event === 14 && counter.refuse(account) >= 2) body = s(e.repeatBody);
  const lines = [body];
  if (e.owner && text) lines.push(String(text));
  if (e.detail) lines.push(s(e.detail));
  return { title: s(e.title), body: lines.join('\n'), kind: e.kind, disconnect: e.disconnect, handler: e.handler };
}
