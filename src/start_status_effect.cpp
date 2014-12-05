/*
* TownRush
* Copyright (c) 2014 Frederic Brachfogel
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Frederic Brachfogel may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY FREDERIC BRACHFOGEL ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL FREDERIC BRACHFOGEL BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* STRONG TOWER*/
case TR_status_effect_strong_tower: {
  un->unit_class.armor += STRONG_TOWER_ARMOR_BONUS;
} break;

/* STUN */
case TR_status_effect_stun: {
  TR_floating_text_ptr ft = FT_New(gi);
  if(ft != TR_null) {
    TR_floating_text_t *flt = &floating_text(ft);
    flt->x = un->x;
    flt->y = un->y;
    flt->offset_y = 0;
    flt->s = "STUNNED";
    flt->c = TCOD_orange;
  }
  //UO_FreeAll(gi, u);
  un->stun++;
} break;

/* NO AGGRO */
case TR_status_effect_no_aggro: {
  un->no_aggro++;
} break;

/* ENRAGE */
case TR_status_effect_enrage: {
	un->unit_class.attack_speed -= 1000;
  un->unit_class.move_speed += 3;
  un->unit_class.anim_speed *= 1.5;
  un->unit_class.attack_damage += 10;
  un->unit_class.armor += 5;
  un->silenced++;
  un->uncontrolable++;
} break;

/* DIVINE FIRE */
case TR_status_effect_divine_fire: {
  {START_PARTICLE("divine_fire", 0, 0, 0.02, 0.03*MODEL_SCALE(un))
   UN_AttachParticleSystem(gi, u, ps);
   st->ps = ps;}
  {START_PARTICLE("divine_fire", 0, 0, 0.02, 0.03*MODEL_SCALE(un))
   UN_AttachParticleSystem(gi, u, ps);
   st->ps2 = ps;}
  un->unit_class.move_speed += 60;
  un->unit_class.anim_speed *= 1.5;
} break;

/* KING STRIKE */
case TR_status_effect_king_strike: {
  Ogre::Bone *bone = un->ent[0]->getSkeleton()->getBone("sword");
  START_PARTICLE("king_strike", UNIT_X(un), UNIT_Y(un), 0, 0.015)
  Ogre::Matrix4 mat = un->node->_getFullTransform() * bone->_getFullTransform();
  Ogre::Vector4 off = mat * Ogre::Vector4(0, 0, 1, 0);
  Ogre::Vector3 pos = mat.getTrans() + Ogre::Vector3(off.x, off.y, off.z);
  psys->x = pos.x * (1<<POS_SHIFT);
  psys->y = pos.z * (1<<POS_SHIFT);
  psys->z = pos.y;
  st->ps = ps;
  st->user_data = un->skill_marks[0];
  un->unit_class.attack_damage += 10;
} break;

/* GOD LIGHT */
case TR_status_effect_god_light: {
  START_PARTICLE("blind", 0, 0, BAR_HEIGHT(un)-0.25, 1)
  UN_AttachParticleSystem(gi, u, ps);
  st->ps = ps;
  UO_FreeAll(gi, u);
  un->blind++;
} break;

/* CAULDRON SLOW */
case TR_status_effect_cauldron_slow: {
  un->unit_class.move_speed -= 20;
} break;

/* WOODMISTRESS ROOT */
case TR_status_effect_woodmistress_root: {
  un->unit_class.move_speed -= 30;
} break;

/* WIZARD WRATH */
case TR_status_effect_wizard_wrath: {
  un->skill_speed_factors[0] /= 2;
  ANIM_SPEED(un) *= 2;
} break;

/* INVINCIBILITY */
case TR_status_effect_invincibility: {
  START_PARTICLE("divine_fire", 0, 0, 0.02, 0.03)
  UN_AttachParticleSystem(gi, u, ps);
  st->ps = ps;
  un->invincibility++;
} break;

/* KING AURA */
case TR_status_effect_king_aura: {
  START_PARTICLE("king_aura_woosh", 0, 0, 0.02, 0.015)
  UN_AttachParticleSystem(gi, u, ps);
  st->ps = ps;
} break;

/* KING AURA ARMOR BUFF */
case TR_status_effect_king_aura_buf: {
  st->buf = true;
  st->buf_material = "king_aura_mat";
  un->unit_class.armor += 2;
} break;

/* WOODWOLF PACK BUF */
case TR_status_effect_woodwolf_pack_aura_buf: {
  st->buf = true;
  st->buf_material = "pack_aura_mat";
  un->unit_class.armor += 100;
} break;

/* ICY STARE DEBUF */
case TR_status_effect_icy_stare_debuf: {
  st->buf = true;
  st->buf_material = "icy_starea_mat";
  un->unit_class.attack_speed = 130 * un->unit_class.attack_speed / 100;
} break;

/* PLATE ARMOR */
case TR_status_effect_plate_armor: {
  st->buf = true;
  st->buf_material = "plate_armor_particle_mat";
  st->cache = HP(5);
  un->unit_class.max_hp += st->cache;
  un->hp                += st->cache;
} break;

/* BRAVE AURA BUF */
case TR_status_effect_brave_aura_buf: {
  st->buf = true;
  st->buf_material = "brave_aura_mat";
  st->cache = 1;
  un->unit_class.attack_damage += DAMAGE(st->cache);
} break;

/* ZOMBIE AURA BUF */
case TR_status_effect_zombie_aura_buf: {
  TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u, TR_status_effect_invocation_fate);
  if(s2 != TR_null) {
    st->buf = true;
    st->buf_material = "zombie_aura_mat";
    st->cache = 5;
    TR_status_effect_t *st2 = &status_effect(s2);
    st2->amount       += ARMOR_TIME(st->cache);
    TIME_TO_LIVE(un)  += ARMOR_TIME(st->cache);
  }
} break;

/* MUMMY AURA BUF */
case TR_status_effect_mummy_aura_buf: {
  st->buf = true;
  st->buf_material = "mummy_aura_mat";
  st->cache = HP(1);
  un->unit_class.max_hp += st->cache;
  un->hp += st->cache;
} break;