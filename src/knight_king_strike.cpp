

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

if(un->skill_state == 0
&& un->skill >= 0) {
	ST_New(gi, u, TR_status_effect_king_strike, 10000, false);
  WISE_PLAY_SOUND(gp_gi.move_knight[0], "king_strike", x, y, 500)
  un->skill_state++;
} else {

}
return un->skill < skill_cast_speed; // continue?