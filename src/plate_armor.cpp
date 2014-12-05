
if(un->skill_state == 0
&& mpl->mana < skill_mana_cost) 
	return false;
else {
	if(un->skill_state == 0)
		un->skill_state++;
	bool start_anim = un->skill >= MAX(0, skill_cast_speed/2-gi->get_skill_anim_time_cb(gi, uo->skill_type));
	if(start_anim)
		PLAY_UNIT_ANIM(u, gi->get_skill_anim_name_cb(gi, uo->skill_type).c_str(), false);
	if(un->skill_state == 1
	&& un->skill >= skill_cast_speed/2) {
		un->skills_cooldown[skill_id] = 0;
		mpl->mana = MAX(0, mpl->mana-skill_mana_cost);
		TR_unit_t *target_un = &unit(uo->target);
		ST_New(gi, uo->target, TR_status_effect_plate_armor, INFINITE_SPAN, false);
	  un->skill_state++;
	} 
	return un->skill < skill_cast_speed; // continue?
}