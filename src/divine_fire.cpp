

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

if(un->skill_state == 0
&& un->skill >= 0) {
	un->skills_cooldown[skill_id] = 0;
	UN_Cleanse(gi, u);
  ST_New(gi, u, TR_status_effect_divine_fire, 1500 + 500*(LEVEL(un)-1), false);
  WISE_PLAY_SOUND(gp_gi.move_footmen[0], "divine_fire", x, y, 500)
  un->skill_state++;
} 
return false; // continue?