
static int attack_penetrations[TR_armor_type_count] = {100, 100, 150, 50};

if(un->skill_state == 0
&& un->mana < skill_mana_cost) 
	return false;
else {
	un->skills_cooldown[skill_id] = 0;
	if(un->skill_state == 0) 
		un->skill_state++;
	bool start_anim = un->skill >= MAX(0, skill_cast_speed/2-gi->get_skill_anim_time_cb(gi, uo->skill_type)/2);
	if(start_anim)
		PLAY_UNIT_ANIM(u, gi->get_skill_anim_name_cb(gi, uo->skill_type).c_str(), false);
	if(un->skill_state == 1) {
		un->skills_cooldown[skill_id] = 0;
		un->mana = MAX(0, un->mana-skill_mana_cost);
		{START_PARTICLE("woodmistress_root", x+UN_GetSizeShift(gi, SIZE(un)), y+UN_GetSizeShift(gi, SIZE(un)), 0.02, 0.05)}
		TR_get_unit_here_t data;
	  UN_GetUnitsHere(
	    &data, gi, u, un->team_id, 
	    5, x, y, true, false, TR_get_unit_here_mode_enemies);
	  DO_TIMES(data.count) {
	    TR_unit_ptr u2 = data.result[f];
	    TR_unit_t *un2 = &unit(u2);
	    if(CAN_BE_PUSHED(un2)
	    && UN_GetDist(gi, u2, x, y) <= (2<<POS_SHIFT)) {
	    	if(UN_HasStatusEffect(gi, u, TR_status_effect_woodmistress_root) == TR_null) {
		      UN_PropagateAttack(gi, u2, u, false);
		      {START_PARTICLE("driade_attack", UNIT_X(un2), UNIT_Y(un2), (FLYING(un2) ? FLYING_HEIGHT : 0) + BAR_HEIGHT(un2)-0.25, 0.05)}
		      ST_New(gi, u2, TR_status_effect_woodmistress_root, 2500 + 500*(LEVEL(un)-1), false);
		    }
		  }
	  }
	  un->skill_state++;
	} 
	if(un->skill_state > 1) {
		un->skill_state++;
		if((un->skill_state&7) == 0) {
			TR_get_unit_here_t data;
		  UN_GetUnitsHere(
		    &data, gi, u, un->team_id, 
		    12, x, y, true, false, TR_get_unit_here_mode_enemies);
		  DO_TIMES(data.count) {
		    TR_unit_ptr u2 = data.result[f];
		    TR_unit_t *un2 = &unit(u2);
		    if(CAN_BE_PUSHED(un2)
		    && UN_GetDist(gi, u2, x, y) <= (3<<POS_SHIFT)) {
		    	UN_DealInstantDamage(gi, p, u2, 10+4*skill_level, TR_attack_type_magic);
		    	if(MAX_MANA(un2) > 0) {
		    		int mana_amount = MIN(4+2*skill_level, un2->mana);
		    		un2->mana -= mana_amount;
		    		UN_SetMana(gi, u, un->mana+mana_amount);
		    		assert(un2->mana >= 0);
		    		TR_floating_text_ptr ft = FT_New(gi);
				    assert(ft != TR_null);
				    if(ft != TR_null) {
				      TR_floating_text_t *flt = &floating_text(ft);
				      flt->x = UNIT_X(un2);
				      flt->y = UNIT_Y(un2);
				      flt->offset_y = -18*2;
				      flt->s = i2a(mana_amount);
				      flt->c = TCOD_purple;
				      flt->char_size = 16 + (mana_amount>>5);
				    }
		    	}
		    }
		  }
	  }
	}
	PL_CooldownAll(gi, p, un->unit_type, skill_id);
	return un->skill < skill_cast_speed; // continue?
}