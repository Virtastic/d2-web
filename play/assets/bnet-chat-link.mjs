// The Battle.net chat connection's life across a session: the BNCS socket the login left open can be
// closed by a proxy or the network while a game runs. This keeps one chat session alive for the lobby:
// on return to the lobby, and on any send, a dead socket is dialled again (a fresh BNCS logon with the
// credentials held in memory), chat is entered again and the last channel is joined again; the MCP
// character logon is not touched. Pure of the page: `dial` and `createChat` come from the shell.
export function createChatLink({ dial, createChat, onState = () => {}, onRedial = () => {}, onFail = () => {} }) {
  let session = null, redial = null, lastChannel = '', wantChat = false, dead = false;
  const queue = [];
  const flush = () => { while (queue.length && session?.open) { if (!session.say(queue[0])) break; queue.shift(); } };
  const start = (conn, user, rejoin) => {
    const mine = createChat({ ...conn, ...user,
      onEvent: (e) => {
        if (e.event === 0x07) { lastChannel = e.text; if (session === mine) flush(); }
        return user.onEvent?.(e) ?? false;
      },
      onClose: () => { if (session === mine) onState(false); } });
    session = mine; mine.enter(); onState(true);
    if (rejoin) mine.join(lastChannel || undefined, lastChannel ? 2 : undefined);
    return mine;
  };
  return {
    get session() { return session; },
    get lastChannel() { return lastChannel; },
    // First start from the login's own connection.
    attach(conn, user) { session = null; return start(conn, user, false); },
    // A named channel is a forced join (flags 2); no name is the first join into the product's own channel.
    join(name) { wantChat = true; session?.join(name || undefined, name ? 2 : undefined); },
    channels() { return session?.open ? session.channels() : false; },
    // True when the socket is usable now; otherwise a redial is under way (single flight) and resolves
    // to the new session. A failed dial leaves the link dead and reports through onFail.
    ensure(user) {
      if (dead) return Promise.resolve(null);
      if (session?.open) return Promise.resolve(session);
      if (redial) return redial;
      const old = session; try { old?.close(); } catch { /* already closed */ }
      onState(false); onRedial();
      redial = (async () => {
        try { const conn = await dial(); if (dead) { try { conn.transport?.close(); } catch { /* done */ } return null; } const rejoin = wantChat || Boolean(lastChannel); const next = start(conn, user, rejoin); if (!rejoin) flush(); return next; }
        catch (error) { queue.length = 0; if (!dead) onFail(error); return null; }
        finally { redial = null; }
      })();
      return redial;
    },
    // A typed line: sent now on a live socket, else held until the redial has rejoined the channel.
    say(text, user) {
      if (session?.open && session.say(text)) return true;
      queue.push(text); void this.ensure(user);
      return true;
    },
    close() { dead = true; try { session?.close(); } catch { /* done */ } session = null; queue.length = 0; },
  };
}
