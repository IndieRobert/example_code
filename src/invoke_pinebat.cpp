
un->skills_cooldown[skill_id] = 0;

TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
TR_client_t *cl = &client(c);
if(cl->type == HUMAN_CLIENT) {
	DO_TIMES(skill_level)
  	CL_CmdSpawnUnit(gi, c, TR_null, x, y, TR_unit_type_pinebat);
}
PL_CooldownAll(gi, p, un->unit_type, skill_id);

return false;