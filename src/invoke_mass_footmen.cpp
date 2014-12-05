
un->skills_cooldown[skill_id] = 0;

TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
TR_client_t *cl = &client(c);
/* this is for the mobs so it's not replicated */
if(mpl->unit_counts[0] < 100)
	DO_TIMES(10)
	  CL_CmdSpawnUnit(gi, c, TR_null, UNIT_X(un), UNIT_Y(un), TR_unit_type_footmen, TR_null, TR_null, true);

return false;