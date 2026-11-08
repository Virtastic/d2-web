// Retail's local slash commands. D2Multi sends a typed line through its slash dispatcher (0xB090): the
// history push, then /r and /reply (0xADC0), then /d2notify (0xAFA0), and only a line none of them
// handles goes to the server. A command word matches case-insensitively and must be followed by a
// space or the end of the line.

export const NOBODY_TO_REPLY = 'Nobody to reply to!';
export const NOTIFY_ON = 'Player entering/leaving notifications are enabled.';
export const NOTIFY_OFF = 'Player entering/leaving notifications are disabled.';

const isWord = (line, cmd) => line.length >= cmd.length && line.slice(0, cmd.length).toLowerCase() === cmd
  && (line.length === cmd.length || line[cmd.length] === ' ');

// state: { replyTo: the last whisperer's name ('' when none), notify: the /d2notify flag }.
// Returns null when the line is not local (it goes to the server), else what the screen does:
// { notice } for a local line, plus { send } when the reply goes out as a whisper.
// A reply with no text is handled (it is not sent) and shows the usage line with the command word as matched.
// Every notice carries its colour: red for the two /r errors, blue for the /d2notify lines.
export function localSlash(line, state) {
  if (isWord(line, '/r') || isWord(line, '/reply')) {
    const cmd = isWord(line, '/r') ? '/r' : '/reply';
    if (!state.replyTo) return { handled: true, notice: NOBODY_TO_REPLY, colour: 1 };
    const rest = line.slice(cmd.length).replace(/^ +/, '');
    return rest ? { handled: true, send: `/w *${state.replyTo} ${rest}` } : { handled: true, notice: `Correct usage is: ${cmd} (message)`, colour: 1 };
  }
  if (isWord(line, '/d2notify')) {
    state.notify = !state.notify;
    return { handled: true, notice: state.notify ? NOTIFY_ON : NOTIFY_OFF, colour: 3 };
  }
  return null;
}
