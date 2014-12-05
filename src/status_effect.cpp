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

/* INVOCATION FATE */
case TR_status_effect_invocation_fate: {
  st->amount -= gi->game_time_step;
  if(st->amount <= 0) {
    UN_DealInstantDamage(gi, GI_GetPlayerPtr(gi, un->player_id), u, 999999, TR_attack_type_chaos, false);
    {START_PARTICLE("divine_fire_woosh", UNIT_X(un), UNIT_Y(un), 0.02, 0.020*MODEL_SCALE(un))}
  }
} break;

/* TAUNT */
case TR_status_effect_taunt: {
  if((gi->game_frame_mark&15)==0) {
    {START_PARTICLE("orc_red_woosh", UNIT_X(un), UNIT_Y(un), 0.02, 1)}
  }
  TR_get_unit_here_t data;
  UN_GetUnitsHere(&data, gi, u, un->team_id, 12, un->x, un->y, true, true, TR_get_unit_here_mode_enemies);
  DO_TIMES(data.count) {
    TR_unit_ptr u2 = data.result[f];
    TR_unit_t *un2 = &unit(u2);
    if(UN_GetDist(gi, u, u2) <= (4<<POS_SHIFT))
      PL_GenForcedAction(gi, u2, true, gi->default_skill_type_cb(gi, u2, u), gi->cmd_id++, un->x, un->y, u, u);
  }
} break;

/* ENRAGE */
case TR_status_effect_enrage: {
  if(un->user_data_type == USER_DATA_PLAYER_UNIT) {
    TR_player_unit_t *pun = &player_unit(un->user_data);
    pun->rage -= gi->game_time_step;
    st->span = pun->rage;
  }
} break;

/* KING STRIKE */
case TR_status_effect_king_strike: {
  if(un->user_data_type == USER_DATA_PLAYER_UNIT) {
    Ogre::Bone *bone = un->ent[0]->getSkeleton()->getBone("sword");
    TR_particle_system_t *psys = &particle_system(st->ps);
    Ogre::Matrix4 mat = un->node->_getFullTransform() * bone->_getFullTransform();
    Ogre::Vector4 off = mat * Ogre::Vector4(0, 0, 1, 0);
    Ogre::Vector3 pos = mat.getTrans() + Ogre::Vector3(off.x, off.y, off.z);
    psys->x = pos.x * (1<<POS_SHIFT);
    psys->y = pos.z * (1<<POS_SHIFT);
    psys->z = pos.y;
    if(st->user_data != un->skill_marks[0]) {
      int x = psys->x,
          y = psys->y,
          z = psys->z;
      {START_PARTICLE("king_strike_hit", x, y, z, 0.025)}
      st->span = 0;
    }
  }
} break;

/* KING AURA */
case TR_status_effect_king_aura: {
  TR_get_unit_here_t data;
  UN_GetUnitsHere(
    &data, gi, u, un->team_id, 
    5, UNIT_X(un), UNIT_Y(un), true, true, TR_get_unit_here_mode_friends);
  DO_TIMES(data.count) {
    TR_unit_ptr u2 = data.result[f];
    TR_unit_t *un2 = &unit(u2);
    if(UN_GetDist(gi, u, u2) <= (1<<POS_SHIFT)) {
      TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u2, TR_status_effect_king_aura_buf);
      if(s2 == TR_null) {
        ST_New(gi, u2, TR_status_effect_king_aura_buf, 1000 + gi->game_time_step, false);
      } else {
        status_effect(s2).span = 1000 + gi->game_time_step;
      }
    }
  }
} break;

/* WOODWOLF PACK AURA */
case TR_status_effect_woodwolf_pack_aura: {
  TR_get_unit_here_t data;
  UN_GetUnitsHere(
    &data, gi, u, un->team_id, 
    8, UNIT_X(un), UNIT_Y(un), true, true, TR_get_unit_here_mode_friends);
  DO_TIMES(data.count) {
    TR_unit_ptr u2 = data.result[f];
    TR_unit_t *un2 = &unit(u2);
    if(CAN_BE_PUSHED(un2)
    && UN_GetDist(gi, u, u2) <= (4<<POS_SHIFT)) {
      TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u2, TR_status_effect_woodwolf_pack_aura_buf);
      if(s2 == TR_null) {
        ST_New(gi, u2, TR_status_effect_woodwolf_pack_aura_buf, 250 + gi->game_time_step, false);
      } else {
        status_effect(s2).span = 250 + gi->game_time_step;
      }
    }
  }
} break;

/* ICY STARE */
case TR_status_effect_icy_stare: {
  TR_get_unit_here_t data;
  UN_GetUnitsHere(
    &data, gi, u, un->team_id, 
    5, UNIT_X(un), UNIT_Y(un), true, true, TR_get_unit_here_mode_enemies);
  DO_TIMES(data.count) {
    TR_unit_ptr u2 = data.result[f];
    TR_unit_t *un2 = &unit(u2);
    if(CAN_BE_PUSHED(un2)
    && UN_GetDist(gi, u, u2) <= (2<<POS_SHIFT)) {
      TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u2, TR_status_effect_icy_stare_debuf);
      if(s2 == TR_null) {
        ST_New(gi, u2, TR_status_effect_icy_stare_debuf, 50 + gi->game_time_step, false);
      } else {
        status_effect(s2).span = 50 + gi->game_time_step;
      }
    }
  }
} break;

/* BRAVE AURA */
case TR_status_effect_brave_aura: {
  TR_unit_ptr u2 = pl->unit_head;
  while(u2 != TR_null) {
    TR_unit_t *un2 = &unit(u2);
    if(u2 != u) {
      TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u2, TR_status_effect_brave_aura_buf);
      if(s2 == TR_null) {
        ST_New(gi, u2, TR_status_effect_brave_aura_buf, 50 + gi->game_time_step, false);
      } else {
        status_effect(s2).span = 50 + gi->game_time_step;
      }
    }
    u2 = un2->pnext;
  }
} break;

/* ZOMBIE AURA */
case TR_status_effect_zombie_aura: {
  TR_unit_ptr u2 = pl->unit_head;
  while(u2 != TR_null) {
    TR_unit_t *un2 = &unit(u2);
    if(un2->user_data_type == USER_DATA_PLAYER_UNIT) {
      assert(un2->user_data != TR_null);
      TR_player_unit_t *pun = &player_unit(un2->user_data);
      if(pun->tags.find("undead") != std::string::npos) {
        TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u2, TR_status_effect_zombie_aura_buf);
        if(s2 == TR_null) {
          ST_New(gi, u2, TR_status_effect_zombie_aura_buf, 50 + gi->game_time_step, false);
        } else {
          status_effect(s2).span = 50 + gi->game_time_step;
        }
      }
    }
    u2 = un2->pnext;
  }
} break;

/* MUMMY AURA */
case TR_status_effect_mummy_aura: {
  TR_unit_ptr u2 = pl->unit_head;
  while(u2 != TR_null) {
    TR_unit_t *un2 = &unit(u2);
    if(un2->user_data_type == USER_DATA_PLAYER_UNIT) {
      assert(un2->user_data != TR_null);
      TR_player_unit_t *pun = &player_unit(un2->user_data);
      if(pun->tags.find("undead") != std::string::npos) {
        TR_status_effect_ptr s2 = UN_HasStatusEffect(gi, u2, TR_status_effect_mummy_aura_buf);
        if(s2 == TR_null) {
          ST_New(gi, u2, TR_status_effect_mummy_aura_buf, 50 + gi->game_time_step, false);
        } else {
          status_effect(s2).span = 50 + gi->game_time_step;
        }
      }
    }
    u2 = un2->pnext;
  }
} break;

