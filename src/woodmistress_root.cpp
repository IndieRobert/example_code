
if(un->skill_state == 0
&& un->mana < WOODMISTRESS_ROOT_MANA_COST) 
	return false;
else {
	if(un->skill_state == 0) 
		un->skill_state++;
	bool start_anim = un->skill >= MAX(0, skill_cast_speed/2-SKILL_ANIM_TIME(un)[uo->skill_type]/2);
	if(start_anim)
		PLAY_UNIT_ANIM(u, SKILL_ANIM_NAME(un)[uo->skill_type].c_str(), false);
	if(un->skill_state == 1
	&& un->skill >= skill_cast_speed/2) {
		un->mana = MAX(0, un->mana-WOODMISTRESS_ROOT_MANA_COST);
		{START_PARTICLE("woodmistress_root", x+UN_GetSizeShift(gi, SIZE(un)), y+UN_GetSizeShift(gi, SIZE(un)), 0.02, 0.05)}
		TR_get_unit_here_t data;
	  UN_GetUnitsHere(
	    &data, gi, u, un->team_id, 
	    5, x, y, true, false, TR_get_unit_here_mode_enemies);
	  DO_TIMES(data.count) {
	    TR_unit_ptr u2 = data.result[f];
	    TR_unit_t *un2 = &unit(u2);
	    if(CAN_BE_PUSHED(un2)
	    && UN_GetDist(gi, u2, x, y) <= (2<<POS_SHIFT)
	    && UN_HasStatusEffect(gi, u, TR_status_effect_woodmistress_root) == TR_null) {
	      UN_PropagateAttack(gi, u2, u, false);
	      {START_PARTICLE("driade_attack", UNIT_X(un2), UNIT_Y(un2), (FLYING(un2) ? FLYING_HEIGHT : 0) + BAR_HEIGHT(un2)-0.25, 0.05)}
	      ST_New(gi, u2, TR_status_effect_woodmistress_root, 2500 + 500*(LEVEL(un)-1), false);
	    }
	  }
	  un->skill_state++;
	} 
	return un->skill < skill_cast_speed; // continue?
}