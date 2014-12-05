

un->skills_cooldown[skill_id] = 0;

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

if(un->skill_state == 0
&& un->skill >= 0) {
  ST_New(gi, u, TR_status_effect_taunt, 10000, true);
  un->skill_state++;
} 
return un->skill < skill_cast_speed; // continue?