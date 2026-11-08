// Touch text entry (platform): a phone has no keyboard events for a canvas, so a screen's text box is
// edited through one hidden <input> that holds the box's text. Tapping the box focuses it (which
// raises the on-screen keyboard); every `input` event is diffed against the text the screen last
// reported and replayed as the same typeKey(code, key) calls a physical keyboard makes (Backspace for
// what was removed, one call per character added), so autocorrect and composition reach the box too.
// Enter / Go is typeKey('Enter'). The screen owns the text and its rules; this only moves characters.
export function createTouchTextEntry({ documentObject = globalThis.document, typeKey, getValue, isSecret = () => false, onChange = () => {}, onBlur = () => {} }) {
  let el = null, last = '', composing = false, enterAt = -Infinity;
  const make = () => {
    const i = documentObject.createElement('input');
    i.type = 'text'; i.autocomplete = 'off'; i.autocapitalize = 'off'; i.spellcheck = false;
    i.setAttribute('enterkeyhint', 'send'); i.setAttribute('aria-label', 'chat');
    Object.assign(i.style, { position: 'fixed', left: '0', bottom: '0', width: '1px', height: '1px', opacity: '0', border: '0', padding: '0', fontSize: '16px' });
    i.addEventListener('compositionstart', () => { composing = true; });
    i.addEventListener('compositionend', () => { composing = false; sync(); });
    i.addEventListener('input', () => { if (!composing) sync(); });
    // Enter: a physical key and most keyboards send keydown 'Enter'; some mobile keyboards (key 'Unidentified',
    // keyCode 229) send only a beforeinput line break. Either is one Enter; the pair is counted once.
    // An Enter that arrives mid-composition first commits the composed text.
    const enter = (stamp) => {
      enterAt = stamp; if (composing) { composing = false; sync(); }
      typeKey('Enter', 'Enter'); last = getValue() ?? ''; i.value = last; onChange();
    };
    i.addEventListener('keydown', (e) => {
      if (e.key !== 'Enter' && e.code !== 'Enter' && e.code !== 'NumpadEnter') return;
      e.preventDefault(); enter(e.timeStamp ?? 0);
    });
    i.addEventListener('beforeinput', (e) => {
      if (e.inputType !== 'insertLineBreak' && e.inputType !== 'insertParagraph') return;
      e.preventDefault();
      if (Math.abs((e.timeStamp ?? 0) - enterAt) > 100) enter(e.timeStamp ?? 0);
    });
    // The browser commits a composition on blur (compositionend usually comes first; if not, it is flushed here).
    i.addEventListener('blur', () => { if (composing) { composing = false; sync(); } onBlur(); });
    documentObject.body.appendChild(i);
    return i;
  };
  const sync = () => {
    const was = [...last], is = [...el.value]; let p = 0;
    while (p < was.length && p < is.length && was[p] === is[p]) p++;
    for (let k = was.length; k > p; k--) typeKey('Backspace', 'Backspace');
    for (const ch of is.slice(p)) typeKey('', ch);
    // The screen has the last word (a limit, a refused character): show its text, not the input's.
    last = getValue() ?? ''; el.value = last; onChange();
  };
  return {
    // Must run inside the tap's own event handler: a browser raises the keyboard only for a focus() in a gesture.
    open() { const v = getValue(); if (v == null) return false; el ??= make(); el.type = isSecret() ? 'password' : 'text'; last = v; el.value = v; el.focus(); return true; },
    close() { if (el && documentObject.activeElement === el) el.blur(); },
    get element() { return el; },
  };
}
