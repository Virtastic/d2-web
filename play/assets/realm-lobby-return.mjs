// Leaving a Battle.net game lands in the lobby, as retail does, with the character still logged on.
// The D2GS socket is closed and the game's engine state freed by the shell; this is the realm side:
// keep what the BNCS login left (server, realm handoff, chat socket), dial MCP again, log the same
// character on, and hand back a flow the lobby's CREATE and JOIN can use. Pure of the page.
export async function returnToLobby({ flow, character, dialMcp, bindProtocol, mcpReturnToLobby }) {
  if (!flow?.realmReply || !flow.server) throw Object.assign(new Error('no realm session to return to'), { sessionLost: true });
  const mcp = await dialMcp(flow.server);
  let protocol = null;
  try {
    protocol = bindProtocol();
    const roster = await mcpReturnToLobby(mcp, protocol, { realmReply: flow.realmReply, character });
    return { characters: roster.characters,
      flow: { server: flow.server, realmReply: flow.realmReply, realms: flow.realms, mpq: flow.mpq, bncs: null,
        chat: flow.chat, chatSession: flow.chatSession, chatLink: flow.chatLink, mcp, protocol, mcpSession: roster.session } };
  } catch (error) {
    try { mcp.close(); } catch { /* already closed */ }
    try { protocol?.free(); } catch { /* engine gone */ }
    throw Object.assign(error, { sessionLost: true });
  }
}
