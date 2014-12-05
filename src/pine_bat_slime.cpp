if(un->skill >= MAX(0, skill_cast_speed/2-SKILL_ANIM_TIME(un)[uo->skill_type]/2)) {
  PLAY_UNIT_ANIM(u, SKILL_ANIM_NAME(un)[uo->skill_type].c_str(), false)
  GAME_OnAttackUnitSound(gi, un->unit_type, un->x, un->y);
}

if(un->skill_state == 0
&& un->skill >= skill_cast_speed/2) {
  un->skill_state++;
  TR_ball_ptr b               = BA_New(gi);
  TR_ball_t *ba               = &ball(b);
  ba->u                       = u;
  ba->p                       = GI_GetPlayerPtr(gi, un->player_id);
  ba->explosion_type          = EXPLOSION_TYPE(un);
  START_PARTICLE("plain_bat_ball", UNIT_X(un), UNIT_Y(un), (FLYING(un) ? FLYING_HEIGHT : 0) + BALL_OFFSET(un).y, 0.03)
  ba->ps                      = ps;
  ba->speed                   = BALL_SPEED(un);
  ba->additive                = BALL_ADDITIVE(un);
  ba->move                    = 0;
  Ogre::Vector3 v             = un->node->getOrientation() * BALL_OFFSET(un);
  ba->x                       = UNIT_X(un);
  ba->y                       = UNIT_Y(un);
  ba->start_z = ba->z         = (FLYING(un) ? FLYING_HEIGHT : 0) + BALL_OFFSET(un).y;
  ba->target_cx               = unit(uo->target).x + UN_GetSizeShift(gi, SIZE(&unit(uo->target)));
  ba->target_cy               = unit(uo->target).y + UN_GetSizeShift(gi, SIZE(&unit(uo->target)));
  ba->damage                  = 5 + 1*(LEVEL(un)-1);
  ba->attack_type             = TR_attack_type_normal;
  ba->target                  = uo->target;
  ba->target_player_id        = uo->target_player_id;
  ba->target_cmd_id           = uo->target_cmd_id;
  ba->type                    = BAT_SLIME_BALL;
}

return un->skill < skill_cast_speed; // continue?