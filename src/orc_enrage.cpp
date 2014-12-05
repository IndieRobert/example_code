

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

if((un->skill_state2&7) == 0
&& un->skill <= 2*skill_cast_speed/3)
	{START_PARTICLE("orc_red_woosh", x, y, 0.02, 1)}
un->skill_state2++;

if(un->skill_state == 0) {
	PLAY_UNIT_ANIM(u, SKILL_ANIM_NAME(un)[uo->skill_type].c_str(), false)
  WISE_PLAY_SOUND(gp_gi.orc_enrage_sound, "orc_enrage", x, y, 500)
  {START_PARTICLE("orc_smoke_bash", x, y, 0.02, 0.05)}
  un->skill_state++;
} 

if(un->skill >= skill_cast_speed)
	ST_New(gi, u, TR_status_effect_enrage, pun->rage, true);

return un->skill < skill_cast_speed; // continue?