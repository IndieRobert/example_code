
if(un->skill_state == 0
&& mpl->mana < skill_mana_cost) 
	return false;
else {
	un->skills_cooldown[skill_id] = 0;
	if(un->skill_state == 0)
		un->skill_state++;
	bool start_anim = un->skill_state == 1;
	if(start_anim)
		PLAY_UNIT_ANIM(u, gi->get_skill_anim_name_cb(gi, uo->skill_type).c_str(), false);
	if(un->skill_state == 1
	&& ((un->skill >= gi->get_skill_anim_time_cb(gi, uo->skill_type) && !START_BALL_WITH_ATTACK_ANIM(un)) \
  || (start_anim && START_BALL_WITH_ATTACK_ANIM(un)))) {
    un->skills_cooldown[skill_id] = 0;
		mpl->mana = MAX(0, mpl->mana-skill_mana_cost);
    int dx = (x+UN_GetSizeShift(gi, SIZE(un))) - UNIT_X(un),
        dy = (y+UN_GetSizeShift(gi, SIZE(un))) - UNIT_Y(un),
        nx = -dy / 5,
        ny = dx / 5;
    int xs[5],
        ys[5];
    xs[0] = x+UN_GetSizeShift(gi, SIZE(un));
    ys[0] = y+UN_GetSizeShift(gi, SIZE(un));
    xs[1] = xs[0] - 2*nx;
    ys[1] = ys[0] - 2*ny;
    xs[2] = xs[0] + 2*nx;
    ys[2] = ys[0] + 2*ny;
    xs[3] = xs[0] -   nx;
    ys[3] = ys[0] -   ny;
    xs[4] = xs[0] +   nx;
    ys[4] = ys[0] +   ny;
    DO_TIMES(5) {
  		TR_ball_ptr b               = BA_New(gi);
      assert(b != TR_null);
      TR_ball_t *ba              = &ball(b);
      ba->ball_type 							= TR_ball_type_projectile_one_hit;
      ba->u                       = u;
      ba->p                       = GI_GetPlayerPtr(gi, un->player_id);
      ba->explosion_type          = EXPLOSION_TYPE(un);
      ba->speed                   = 4;
      ba->additive                = BALL_ADDITIVE(un);
      ba->move                    = 0;
      Ogre::Vector3 v             = un->node->getOrientation() * BALL_OFFSET(un);
      ba->x                       = UNIT_X(un);
      ba->y                       = UNIT_Y(un);
      ba->z                       = ((FLYING(un) ? FLYING_HEIGHT : 0)<<POS_SHIFT) + BALL_HEIGHT(un) + (u+MAX_UNIT*2)*(1<<POS_SHIFT)/100;
      assert(BALL_SPEED(un) != 0);
      ba->target_cx               = xs[f];
      ba->target_cy               = ys[f];
      ba->target_cz               = ((FLYING(un) ? FLYING_HEIGHT : 0)<<POS_SHIFT) + BALL_HEIGHT(un) + (u+MAX_UNIT*2)*(1<<POS_SHIFT)/100;
      ba->damage                  = HP(2);
      ba->look_for_ground         = gi->skill_look_for_ground_cb(gi, skill_type);
      ba->look_for_air            = gi->skill_look_for_air_cb(gi, skill_type);
      ba->dx 											= ba->target_cx - ba->x;
  		ba->dy 											= ba->target_cy - ba->y;
      ba->type                    = TR_ball_type_living_unit_only;
      ba->attack_type             = TR_attack_type_magic;
      ba->target                  = TR_null;
      START_PARTICLE("shot_ball", ba->x, ba->y, ba->z, 0.035)
      ba->ps                    = ps;
    }
	  un->skill_state++;
	} 
	return un->skill < skill_cast_speed; // continue?
}