// Which quest a message string id belongs to: D2MOO's StringIds.h names them A<act>Q<n><Init|AfterInit|EarlyReturn|Successful><NPC>
// (src/core/d2moo/include/DataTbls/StringIds.h). Generated -- [first id, last id, act, quest].
// A menu-2 message of the engine's 0x27 list is that quest's Talk topic; its label is the quest's own name string 'qsts a<act> q<n>'.
export const QUEST_MESSAGE_RANGES = [[64,80,1,1],[81,96,1,2],[97,126,1,4],[127,145,1,5],[146,165,1,3],[166,184,1,6],[304,334,2,1],[335,339,2,2],[340,372,2,3],[373,407,2,4],[408,429,2,5],[430,452,2,6],[527,542,3,4],[543,548,3,2],[549,570,3,1],[571,593,3,3],[594,627,3,5],[628,663,3,6],[670,677,4,1],[678,680,4,3],[681,685,4,2],[20000,20001,4,2],[20077,20095,5,1],[20096,20115,5,2],[20116,20136,5,3],[20137,20152,5,4],[20153,20168,5,5],[20169,20180,5,6]];
export const questOfMessage = (id) => { const r = QUEST_MESSAGE_RANGES.find(([a, b]) => id >= a && id <= b); return r ? { act: r[2], quest: r[3] } : null; };
// P7b: an NPC's Introduction and Gossip lines (StringIds.h: <NPC>IntroGossip1 [+ the class variant], <NPC>Gossip5: the line retail's captures speak). Retail speaks ONE
// fixed gossip line per NPC (talk-*-r22: the same on 3-4 clicks). Measured for Kashya and Akara only; keyed by MonStats class id.
export const NPC_SPEECH = { 150: { intro: 24, gossip: 30 }, 148: { intro: 11, gossip: 17 } };
export const npcSpeech = (classId) => NPC_SPEECH[classId] || null;
// F21: the speech box's scroll and its rate now live with the box that draws it -- speech-box.mjs's
// SPEECH_PX_PER_S (9 px/s, fitted to talk-*-r22's ~20 s Kashya / ~60 s Akara speeches, the only
// constant in that model no retail vector measures).
