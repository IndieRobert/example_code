
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
		TR_ball_ptr b               = BA_New(gi);
    assert(b != TR_null);
    TR_ball_t *ba              = &ball(b);
    ba->ball_type 							= TR_ball_type_projectile_one_hit;
    ba->u                       = u;
    ba->p                       = GI_GetPlayerPtr(gi, un->player_id);
    ba->explosion_type          = EXPLOSION_TYPE(un);
    ba->speed                   = 2;
    ba->additive                = BALL_ADDITIVE(un);
    ba->move                    = 0;
    Ogre::Vector3 v             = un->node->getOrientation() * BALL_OFFSET(un);
    ba->x                       = UNIT_X(un);
    ba->y                       = UNIT_Y(un);
    ba->z                       = ((FLYING(un) ? FLYING_HEIGHT : 0)<<POS_SHIFT) + BALL_HEIGHT(un) + (u+MAX_UNIT*2)*(1<<POS_SHIFT)/100;
    assert(BALL_SPEED(un) != 0);
    ba->target_cx               = xs[0];
    ba->target_cy               = ys[0];
    ba->target_cz               = ((FLYING(un) ? FLYING_HEIGHT : 0)<<POS_SHIFT) + BALL_HEIGHT(un) + (u+MAX_UNIT*2)*(1<<POS_SHIFT)/100;
    ba->damage                  = HP(2);
    ba->look_for_ground         = true;
    ba->look_for_air            = false;
    ba->dx 											= ba->target_cx - ba->x;
		ba->dy 											= ba->target_cy - ba->y;
    ba->type                    = TR_ball_type_moonshot | TR_ball_type_living_unit_only;
    ba->attack_type             = TR_attack_type_magic;
    ba->target                  = TR_null;
    ba->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("ba " + i2a(b));
    ba->node->setPosition(Ogre::Vector3((float)ba->x/(1<<POS_SHIFT), (float)ba->z/(1<<POS_SHIFT), (float)ba->y/(1<<POS_SHIFT)));
    int size = 1;
    ba->node->setScale(Ogre::Vector3(0.25*size, 0.25*size, 0.25*size));
    ba->ball_mesh_cinetic_type = BALL_MESH_CINETIC_TYPE(un);
    switch(ba->ball_mesh_cinetic_type) {
      case TR_ball_mesh_cinetic_type_rotating: {
        ba->ax = (float)(TR_Rand(&gp_gi.working_seed)%100)/100;
        ba->ay = (float)(TR_Rand(&gp_gi.working_seed)%100)/100;
        ba->az = (float)(TR_Rand(&gp_gi.working_seed)%100)/100;
        ba->mesh_angle = (float)(TR_Rand(&gp_gi.working_seed)%100)/100;
        ba->node->setOrientation(Ogre::Quaternion(Ogre::Degree(ba->mesh_angle), Ogre::Vector3(ba->ax, ba->ay, ba->az)));
      } break;
      default:
        ba->ax = 0;
        ba->ay = 1;
        ba->az = 0;
        ba->node->setOrientation(
          Ogre::Quaternion(
            Ogre::Radian(atan2(ba->target_cy - ba->y, ba->target_cy - ba->y)),
            Ogre::Vector3(ba->ax, ba->ay, ba->az)));
        break;
    }
    assert(ba->node);
    ba->ent = gi->ogre_scene->createEntity("ball ent " + i2a(b), "big_liteball.mesh");
    assert(ba->ent);
    ba->ent->setCastShadows(false);
    ba->ent->setVisible(true);
    ba->ent->setUserAny(Ogre::Any(u));
    ba->ent->setQueryFlags(0);
    ba->node->attachObject(ba->ent);
    DO_TIMES(ba->ent->getNumSubEntities()) {
      if(!ba->ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState("FovMapTex")) {
        Ogre::TextureUnitState *tex =
          ba->ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    createTextureUnitState("FovMapTex");
        assert(tex);
        tex->setName("FovMapTex");
        tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
      }
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTexture", false);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setShadowCasterVertexProgram("Ogre/BasicVertexPrograms/AmbientShadowCasterUnified");
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setShadowCasterFragmentProgram("Ogre/BasicFragmentPrograms/ShadowCasterWithAlpha");
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setSceneBlending(Ogre::SBT_REPLACE);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            setDepthWriteEnabled(true);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            setDepthCheckEnabled(true);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            setTransparencyCastsShadows(false);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setDepthWriteEnabled(true);
      ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
      if(ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                getNumTextureUnitStates() > 0)
        ba->ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getTextureUnitState(0)->
                    setTextureFiltering(Ogre::TFO_NONE);
      if(ba->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                getNumTextureUnitStates() > 1)
        ba->ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getTextureUnitState(1)->
                    setTextureFiltering(Ogre::TFO_NONE);
    }
	  un->skill_state++;
	} 
	return un->skill < skill_cast_speed; // continue?
}