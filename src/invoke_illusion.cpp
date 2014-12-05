
if(un->skill_state == 0
&& un->mana < skill_mana_cost) 
	return false;
else {
	if(un->skill_state == 0) 
		un->skill_state++;
	un->skills_cooldown[skill_id] = 0;
	un->mana = MAX(0, un->mana-skill_mana_cost);
	TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
	TR_client_t *cl = &client(c);
	if(cl->type == HUMAN_CLIENT) {
		DO_TIMES(skill_level)
	  		CL_CmdSpawnUnit(gi, c, TR_null, x, y, TR_unit_type_illusion);
	}
}

return false;