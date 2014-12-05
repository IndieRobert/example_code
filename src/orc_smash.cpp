#define ORC_SMASH_RADIUS 4
#define ORC_SMASH_DAMAGE 5

TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);

static int attack_penetrations[TR_armor_type_count] = {100, 100, 100, 100};

if(un->skill_state == 0
&& un->skill >= 0) {
  TR_get_unit_here_t data;
  UN_GetUnitsHere(&data, gi, u, un->team_id, ORC_SMASH_RADIUS, un->x, un->y, true, false, TR_get_unit_here_mode_enemies);
  DO_TIMES(data.count) {
    TR_unit_t *un2 = &unit(data.result[f]);
    if(un2->team_id != un->team_id)
      UN_DealInstantDamage(gi, p, data.result[f], ORC_SMASH_DAMAGE, attack_penetrations);
  }
  {START_PARTICLE("orc_woosh", x, y, 0.02, 1)}
  {START_PARTICLE("orc_smoke_bash", x, y, 0.02, 0.05)}
  {START_PARTICLE("orc_smoke", x, y, 0, 3)
    pun->ps2 = ps;}
  WISE_PLAY_SOUND(gp_gi.orc_smash_sound, "smash", x, y, 1000)
  un->skill_state++;
} else
if(un->skill_state == 1
&& un->skill >= skill_cast_speed/3) {
  TR_get_unit_here_t data;
  UN_GetUnitsHere(&data, gi, u, un->team_id, ORC_SMASH_RADIUS, un->x, un->y, true, false, TR_get_unit_here_mode_enemies);
  DO_TIMES(data.count) {
    TR_unit_t *un2 = &unit(data.result[f]);
    if(un2->team_id != un->team_id)
      UN_DealInstantDamage(gi, p, data.result[f], ORC_SMASH_DAMAGE, attack_penetrations);
  }
  {START_PARTICLE("orc_woosh", x, y, 0.02, 1)}
  {START_PARTICLE("orc_smoke_bash", x, y, 0.02, 0.05)}
  un->skill_state++;
} else
if(un->skill_state == 2
&& un->skill >= 2*skill_cast_speed/3) {
  TR_get_unit_here_t data;
  UN_GetUnitsHere(&data, gi, u, un->team_id, ORC_SMASH_RADIUS, un->x, un->y, true, false, TR_get_unit_here_mode_enemies);
  DO_TIMES(data.count) {
    TR_unit_t *un2 = &unit(data.result[f]);
    if(un2->team_id != un->team_id)
      UN_DealInstantDamage(gi, p, data.result[f], ORC_SMASH_DAMAGE, attack_penetrations);
  }
  {START_PARTICLE("orc_woosh", x, y, 0.02, 1)}
  {START_PARTICLE("orc_smoke_bash", x, y, 0.02, 0.05)}
  PS_Stop(gi, pun->ps2);
  un->skill_state++;
} else
if(un->skill_state == 3
&& un->skill >= skill_cast_speed) {
  un->skill_state++;
}
PLAY_UNIT_ANIM(u, SKILL_ANIM_NAME(un)[uo->skill_type].c_str(), false)
return un->skill < skill_cast_speed; // continue?