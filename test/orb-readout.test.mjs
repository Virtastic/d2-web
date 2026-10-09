import test from 'node:test';
import assert from 'node:assert/strict';
import { orbAt, drawOrbReadouts } from '../play/assets/orb-readout.mjs';

test('native orb hit rectangles include edges and reject adjacent HUD space', () => {
  for (const [x,y,orb] of [[30,525,'life'],[110,585,'life'],[689,525,'mana'],[769,585,'mana']])
    assert.equal(orbAt(800,600,x,y),orb);
  for (const [x,y] of [[29,550],[111,550],[688,550],[770,550],[70,524],[730,586]])
    assert.equal(orbAt(800,600,x,y),null);
  assert.equal(orbAt(1024,768,913,693),'mana');
  assert.equal(orbAt(400,300,365,275),'mana');
});

test('readouts use fixed-point engine values, native centering and independent pins', () => {
  const ctx={save(){},scale(){},restore(){}}, painted=[];
  const font={measure:()=>112,drawAnchored:(_ctx,text,x,y)=>painted.push({text,x,y})};
  const snapshot={mode:5,player:{hp:128,hpMax:50*256,mana:10*256+255,manaMax:10*256}};
  const line=index=>index===4165?'Life: %d / %d':'Mana: %d / %d';
  assert.deepEqual(drawOrbReadouts(ctx,font,snapshot,800,600,{x:799,y:300},{},line),[]);
  const mana=drawOrbReadouts(ctx,font,snapshot,800,600,{x:799,y:300},{mana:true},line);
  assert.equal(mana.length,1);assert.equal(mana[0].text,'Mana: 10 / 10');
  assert.equal(mana[0].x,664);assert.equal(mana[0].y,505);
  const life=drawOrbReadouts(ctx,font,snapshot,800,600,{x:70,y:550},{},line);
  assert.equal(life[0].text,'Life: 1 / 50');assert.equal(life[0].pinned,false);
  snapshot.mode=17;
  assert.equal(drawOrbReadouts(ctx,font,snapshot,800,600,{x:70,y:550},{},line)[0].text,'Life: 0 / 50');
  assert.equal(painted.length,3);
});
