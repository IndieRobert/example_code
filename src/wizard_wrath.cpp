
if(un->skill_state == 0
&& un->mana < WIZARD_WRATH_MANA_COST) 
	return false;
else {
	UN_SetMana(gi, u, un->mana-WIZARD_WRATH_MANA_COST);
	ST_New(gi, u, TR_status_effect_wizard_wrath, 3000 + 250*(LEVEL(un)-1), false);
	return false; // continue?
}