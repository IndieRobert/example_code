#define GOD_LIGHT_RADIUS 3

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

if(un->skill_state == 0
&& un->skill >= 0) {
	{START_PARTICLE("knight_god_light", UNIT_X(un), UNIT_Y(un), BAR_HEIGHT(un)-0.25, 0.03)}
	TR_get_unit_here_t data;
  UN_GetUnitsHere(&data, gi, u, un->team_id, GOD_LIGHT_RADIUS, un->x, un->y, true, true, TR_get_unit_here_mode_enemies);
  DO_TIMES(data.count) {
  	TR_unit_ptr u2 = data.result[f];
    TR_unit_t *un2 = &unit(u2);
    if(un2->team_id != un->team_id
    && UN_HasStatusEffect(gi, u2, TR_status_effect_god_light) == TR_null)
      ST_New(gi, u2, TR_status_effect_god_light, 20000, false);
  }
  un->skill_state++;
} 
return false; // continue?