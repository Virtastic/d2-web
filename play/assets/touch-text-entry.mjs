// Touch text entry (platform): a phone has no keyboard events for a canvas, so a screen's text box is
// edited through one hidden <input> that holds the box's text. Tapping the box focuses it (which
// raises the on-screen keyboard); every `input` event is diffed against the text the screen last
// reported and replayed as the same typeKey(code, key) calls a physical keyboard makes (Backspace for
// what was removed, one call per character added), so autocorrect and composition reach the box too.
// Enter / Go is typeKey('Enter'). The screen owns the text and its rules; this only moves characters.
export function createTouchTextEntry({ documentObject = globalThis.document, typeKey, getValue, onChange = () => {}, onBlur = () => {} }) {
  let el = null, last = '', composing = false;
  const make = () => {
    const i = documentObject.createElement('input');
    i.type = 'text'; i.autocomplete = 'off'; i.autocapitalize = 'off'; i.spellcheck = false;
    i.setAttribute('enterkeyhint', 'send'); i.setAttribute('aria-label', 'chat');
    Object.assign(i.style, { position: 'fixed', left: '0', bottom: '0', width: '1px', height: '1px', opacity: '0', border: '0', padding: '0', fontSize: '16px' });
    i.addEventListener('compositionstart', () => { composing = true; });
    i.addEventListener('compositionend', () => { composing = false; sync(); });
    i.addEventListener('input', () => { if (!composing) sync(); });
    i.addEventListener('keydown', (e) => {
      if (e.key !== 'Enter' && e.code !== 'Enter') return;
      e.preventDefault(); typeKey('Enter', 'Enter'); last = getValue() ?? ''; i.value = last; onChange();
    });
    i.addEventListener('blur', () => { composing = false; onBlur(); });
    documentObject.body.appendChild(i);
    return i;
  };
  const sync = () => {
    const now = el.value; let p = 0;
    while (p < last.length && p < now.length && last[p] === now[p]) p++;
    for (let k = last.length; k > p; k--) typeKey('Backspace', 'Backspace');
    for (const ch of [...now.slice(p)]) typeKey('', ch);
    // The screen has the last word (a limit, a refused character): show its text, not the input's.
    last = getValue() ?? ''; el.value = last; onChange();
  };
  return {
    // Must run inside the tap's own event handler: a browser raises the keyboard only for a focus() in a gesture.
    open() { const v = getValue(); if (v == null) return false; el ??= make(); last = v; el.value = v; el.focus(); return true; },
    close() { if (el && documentObject.activeElement === el) el.blur(); },
    get element() { return el; },
  };
}
