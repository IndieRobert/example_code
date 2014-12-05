

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

static int attack_penetrations[TR_armor_type_count] = {100, 100, 100, 100};

if(un->skill_state == 0) {
	un->skill_state++;
  PLAY_UNIT_ANIM(u, gi->get_skill_anim_name_cb(gi, uo->skill_type).c_str(), true)
}

if(un->skill_state > 0
&& un->skill >= 0) {
	if((un->skill/500) >= (un->skill_state-1)) {
		un->skill_state++;
		TR_get_unit_here_t data;
	  UN_GetUnitsHere(&data, gi, u, un->team_id, 8, UNIT_X(un), UNIT_Y(un), true, true, TR_get_unit_here_mode_enemies);
	  DO_TIMES(data.count) {
	    TR_unit_ptr u2 = data.result[f];
	    TR_unit_t *un2 = &unit(u2);
	    if(UN_GetDist(gi, u, u2) <= (1<<POS_SHIFT)) {
	    	if(UN_HasStatusEffect(gi, u2, TR_status_effect_stun) == TR_null) {
		      {START_PARTICLE("divine_fire_woosh", UNIT_X(un2), UNIT_Y(un2), 0.02, 0.025*MODEL_SCALE(un2))}
		      ST_New(gi, u2, TR_status_effect_stun, 1500 + 500*(LEVEL(un)-1), false);
		    }
	      UN_DealInstantDamage(gi, GI_GetPlayerPtr(gi, un->player_id), u2, 20 + 5*(LEVEL(un)-1), attack_penetrations);
	    }
	  }
	}
} 

return un->skill < skill_cast_speed; // continue?