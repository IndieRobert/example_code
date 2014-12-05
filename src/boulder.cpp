
un->skills_cooldown[skill_id] = 0;
TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
TR_client_t *cl = &client(c);
if(cl->type != REPLAY_CLIENT) {
	CL_CmdSpawnUnit(gi, c, TR_null, x, y, TR_unit_type_boulder, TR_null, TR_null, false);
}
un->mana = MAX(0, un->mana-skill_mana_cost);

return false;