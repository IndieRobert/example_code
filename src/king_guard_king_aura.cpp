
if(un->skill_state == 0
&& un->skill >= 0) {
  ST_New(gi, u, TR_status_effect_king_aura, 3000 + 1000*(LEVEL(un)-1), false);
  un->skill_state++;
} 
return false; // continue?