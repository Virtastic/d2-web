import {test} from 'node:test';
import assert from 'node:assert/strict';
import {bindEngineProtocol} from '../play/assets/realm-client.mjs';
import {parseSkillChoices} from '../play/assets/skill-select.mjs';
test('realm selected skill wrapper reads engine state and sends requests without optimistic selection',()=>{
  const heap=new Uint8Array(200000);let next=32;const calls=[];
  const Module={HEAPU8:heap,_malloc:n=>{const p=next;next+=n;return BigInt(p);},_free:()=>{},
    ccall:(name,result,types,args)=>{
      calls.push({name,args});
      if(name==='d2_realm_client_selected_skills')return 8n;
      if(name==='d2_realm_client_skill_choices'){
        const [guid,out,cap]=args;assert.equal(cap,128);
        if(guid!==0x12345678)return 0;
        const v=new DataView(heap.buffer);v.setUint16(Number(out),47,true);v.setInt8(Number(out)+2,2);
        v.setInt8(Number(out)+3,1);v.setUint16(Number(out)+4,12,true);v.setUint8(Number(out)+6,10);v.setUint8(Number(out)+7,3);
        return 1;
      }
      if(name==='d2_realm_d2gs_select_skill'){
        const [skill,left,owner,out]=args;const v=new DataView(heap.buffer);v.setUint8(Number(out),60);
        v.setUint32(Number(out)+1,skill|(left?0x80000000:0),true);v.setUint32(Number(out)+5,owner,true);return 9;
      }
      throw new Error(name);
    }};
  new Int32Array(heap.buffer,8,4).set([0,47,-1,-1]);
  const p=bindEngineProtocol(Module);
  assert.deepEqual(p.d2gsSelectedSkills(),{left:0,right:47,leftOwner:-1,rightOwner:-1});
  assert.deepEqual(parseSkillChoices(p.d2gsSkillChoices(0x12345678),1),[
    {id:47,listRow:2,charClass:1,iconCel:12,passive:false,leftSkill:true,usable:true,townBlocked:true,level:3},
  ]);
  assert.deepEqual([...p.d2gsSkillChoices(0x87654321)],[],'unknown roster GUID has no skills');
  assert.throws(()=>p.d2gsSkillChoices(0x12345678,129),RangeError);
  assert.throws(()=>p.d2gsSkillChoices(-1),RangeError);
  assert.deepEqual([...p.d2gsSelectSkill(36,false)],[60,36,0,0,0,255,255,255,255]);
  assert.equal(p.d2gsSelectedSkills().right,47,'request does not predict server acknowledgement');
  assert.throws(()=>p.d2gsSelectSkill(-1,false),RangeError);
  assert.throws(()=>p.d2gsSelectSkill(47,1),RangeError);
  assert.equal(calls.filter(x=>x.name==='d2_realm_d2gs_select_skill').length,1);
  p.free();
});
