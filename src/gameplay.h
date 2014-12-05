/*
* TownRush
* Copyright (c) 2014 Frederic Brachfogel
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Frederic Brachfogel may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY FREDERIC BRACHFOGEL ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL FREDERIC BRACHFOGEL BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define SAVE_VERSION                1

#define MAX_SELECTION_TYPE          4
#define START_BUILD_INFO_X          1//(HUD_INFO_CENTER-30)
#define START_BUILD_INFO_Y          (HUD_START_Y+3)
#define MAX_CASTLE_BUILD_COUNT      (5)
#define HUD_INFO_CENTER             (CON_RES_X/2)
#define START_HUD_INFO_X            (HUD_INFO_CENTER - 15-5)
#define START_HUD_INFO_X2           (HUD_INFO_CENTER + 5-5)
#define START_HUD_INFO_Y            (HUD_START_Y+3)
#define HUD_CENTRAL_INFO_COUNT      (HUD_HEIGHT-3)

#define USER_DATA_CREEP         0
#define USER_DATA_PLAYER_UNIT   1
#define USER_DATA_MOBS          2

/* this determines the selection order */
enum TR_gp_unit_type_t {
  #include "gen/unit_type.cpp"
  TR_unit_type_count
};

enum TR_gp_cmd_generic_t {
  TR_gp_generic_chose_class,
  TR_gp_generic_chose_skill,
};

#define BASE_ATTACK_TIME  1250
#define BASE_DAMAGE       10
#define BASE_HP           50
#define DAMAGE(X)         ((X) * 10)
#define HP(X)             ((X) * 50)
#define ARMOR_TIME(X)     ((X) * BASE_ATTACK_TIME * BASE_HP / BASE_DAMAGE)

#define INFINITE_SPAN             999999999
#define STRONG_TOWER_ARMOR_BONUS  100
#define INVOCATION_FATE_RATE      0.5

enum TR_status_effect_type_t {
  TR_status_effect_no_aggro,
  TR_status_effect_stun,
  TR_status_effect_invocation_fate,
  TR_status_effect_strong_tower,
  TR_status_effect_taunt,
  TR_status_effect_enrage,
  TR_status_effect_divine_fire,
  TR_status_effect_king_strike,
  TR_status_effect_god_light,
  TR_status_effect_cauldron_slow,
  TR_status_effect_woodmistress_root,
  TR_status_effect_wizard_wrath,
  TR_status_effect_invincibility,
  TR_status_effect_king_aura,
  TR_status_effect_king_aura_buf,
  TR_status_effect_woodwolf_pack_aura,
  TR_status_effect_woodwolf_pack_aura_buf,
  TR_status_effect_icy_stare,
  TR_status_effect_icy_stare_debuf,
  TR_status_effect_plate_armor,
  TR_status_effect_brave_aura,
  TR_status_effect_brave_aura_buf,
  TR_status_effect_zombie_aura,
  TR_status_effect_zombie_aura_buf,
  TR_status_effect_mummy_aura,
  TR_status_effect_mummy_aura_buf,
};

enum TR_game_ball_type_t {
  TR_ball_type_living_unit_only = 1<<0,
  TR_ball_type_moonshot         = 1<<1,
};

enum TR_castle_hud_state {
  TR_castle_hud_state_default,
  TR_castle_hud_state_build,
  TR_castle_hud_state_unlock,
  TR_castle_hud_state_learn,
  TR_castle_hud_state_count,
};

#define GENERIC_SKILL(VAR, XP) \
  case TR_gp_skill_##VAR: \
    /* one may spam-click */ \
    if(mpl->xp >= XP) { \
      mpl->has_##VAR = true; \
      assert(mpl->xp >= XP); \
      mpl->xp -= XP; \
    } break;

#define TRIGGER_ATTACK \
  { \
    bool start_anim = un->skill_state == 0; \
    if(start_anim) { \
      un->skill_state++; \
      PLAY_UNIT_ANIM(u, gi->get_skill_anim_name_cb(gi, uo->skill_type).c_str(), false) \
      GAME_OnAttackUnitSound(gi, un->unit_type, un->x, un->y); \
    } \
    if(un->skill_state == 1 \
    && ((un->skill >= gi->get_skill_anim_time_cb(gi, uo->skill_type) && !START_BALL_WITH_ATTACK_ANIM(un)) \
    || (start_anim && START_BALL_WITH_ATTACK_ANIM(un)))) { \
      un->skills_cooldown[skill_id] = 0; \
      TR_unit_t *target_un = &unit(uo->target); \
      un->skill_state++; \
      TR_ball_ptr b               = BA_New(gi); \
      assert(b != TR_null); \
      TR_ball_t *ba               = &ball(b); \
      ba->u                       = u; \
      ba->p                       = GI_GetPlayerPtr(gi, un->player_id); \
      ba->explosion_type          = EXPLOSION_TYPE(un); \
      ba->speed                   = BALL_SPEED(un); \
      ba->additive                = BALL_ADDITIVE(un); \
      ba->en_cloche               = BALL_EN_CLOCHE(un); \
      ba->attack_range            = ATTACK_RANGE(un); \
      ba->move                    = 0; \
      Ogre::Vector3 v             = un->node->getOrientation() * BALL_OFFSET(un); \
      ba->x                       = UNIT_X(un); \
      ba->y                       = UNIT_Y(un); \
      ba->z                       = ((FLYING(un) ? FLYING_HEIGHT : 0)<<POS_SHIFT) + BALL_HEIGHT(un) + (u+MAX_UNIT*2)*(1<<POS_SHIFT)/100; \
      assert(BALL_SPEED(un) != 0); \
      ba->target_cx               = UNIT_X(target_un) + target_un->ddx * 50 * 2 / 6; \
      ba->target_cy               = UNIT_Y(target_un) + target_un->ddy * 50 * 2 / 6; \
      ba->target_cz               = ((FLYING(target_un) ? FLYING_HEIGHT : 0)<<POS_SHIFT) + BALL_HEIGHT(target_un) + (uo->target+MAX_UNIT*2)*(1<<POS_SHIFT)/100; \
      ba->dx                      = ba->target_cx - ba->x; \
      ba->dy                      = ba->target_cy - ba->y; \
      ba->damage                  = ATTACK_DAMAGE(un); \
      ba->look_for_ground         = gi->skill_look_for_ground_cb(gi, skill_type); \
      ba->look_for_air            = gi->skill_look_for_air_cb(gi, skill_type); \
      ba->attack_type             = ATTACK_TYPE(un); \
      ba->target                  = uo->target; \
      ba->target_player_id        = uo->target_player_id; \
      ba->target_cmd_id           = uo->target_cmd_id; \
      if(BALL_PARTICLE(un) != "") { \
        START_PARTICLE(BALL_PARTICLE(un), ba->x, ba->y, ba->z, BALL_SIZE(un)) \
        ba->ps                    = ps; \
      } \
      if(BALL_MESH(un) != "") { \
        ba->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("ba " + i2a(b)); \
        ba->node->setPosition(Ogre::Vector3((float)ba->x/(1<<POS_SHIFT), (float)ba->z/(1<<POS_SHIFT), (float)ba->y/(1<<POS_SHIFT))); \
        ba->node->setScale(Ogre::Vector3(0.25*BALL_MESH_SIZE(un), 0.25*BALL_MESH_SIZE(un), 0.25*BALL_MESH_SIZE(un))); \
        ba->ball_mesh_cinetic_type = BALL_MESH_CINETIC_TYPE(un); \
        switch(ba->ball_mesh_cinetic_type) { \
          case TR_ball_mesh_cinetic_type_rotating: { \
            ba->ax = (float)(TR_Rand(&gp_gi.working_seed)%100)/100; \
            ba->ay = (float)(TR_Rand(&gp_gi.working_seed)%100)/100; \
            ba->az = (float)(TR_Rand(&gp_gi.working_seed)%100)/100; \
            ba->mesh_angle = (float)(TR_Rand(&gp_gi.working_seed)%100)/100; \
            ba->node->setOrientation(Ogre::Quaternion(Ogre::Degree(ba->mesh_angle), Ogre::Vector3(ba->ax, ba->ay, ba->az))); \
          } break; \
          default: \
            ba->ax = 0; \
            ba->ay = 1; \
            ba->az = 0; \
            ba->node->setOrientation( \
              Ogre::Quaternion( \
                Ogre::Radian(atan2(ba->target_cy - ba->y, ba->target_cy - ba->y)), \
                Ogre::Vector3(ba->ax, ba->ay, ba->az))); \
            break; \
        } \
        assert(ba->node); \
        ba->ent = gi->ogre_scene->createEntity("ball ent " + i2a(b), BALL_MESH(un) + ".mesh"); \
        assert(ba->ent); \
        ba->ent->setCastShadows(false); \
        ba->ent->setVisible(true); \
        ba->ent->setUserAny(Ogre::Any(u)); \
        ba->ent->setQueryFlags(0); \
        ba->node->attachObject(ba->ent); \
        DO_TIMES(ba->ent->getNumSubEntities()) { \
          if(!ba->ent-> \
                getSubEntity(f)-> \
                  getMaterial()-> \
                    getTechnique(0)-> \
                      getPass(0)-> \
                        getTextureUnitState("FovMapTex")) { \
            Ogre::TextureUnitState *tex = \
              ba->ent-> \
                getSubEntity(f)-> \
                  getMaterial()-> \
                    getTechnique(0)-> \
                      getPass(0)-> \
                        createTextureUnitState("FovMapTex"); \
            assert(tex); \
            tex->setName("FovMapTex"); \
            tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP); \
          } \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTexture", false); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setShadowCasterVertexProgram("Ogre/BasicVertexPrograms/AmbientShadowCasterUnified"); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setShadowCasterFragmentProgram("Ogre/BasicFragmentPrograms/ShadowCasterWithAlpha"); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setSceneBlending(Ogre::SBT_REPLACE); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                setDepthWriteEnabled(true); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                setDepthCheckEnabled(true); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                setTransparencyCastsShadows(false); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setDepthWriteEnabled(true); \
          ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128); \
          if(ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    getNumTextureUnitStates() > 0) \
            ba->ent-> \
              getSubEntity(f)-> \
                getMaterial()-> \
                  getTechnique(0)-> \
                    getPass(0)-> \
                      getTextureUnitState(0)-> \
                        setTextureFiltering(Ogre::TFO_NONE); \
          if(ba->ent-> \
            getSubEntity(f)-> \
              getMaterial()-> \
                getTechnique(0)-> \
                  getPass(0)-> \
                    getNumTextureUnitStates() > 1) \
            ba->ent-> \
              getSubEntity(f)-> \
                getMaterial()-> \
                  getTechnique(0)-> \
                    getPass(0)-> \
                      getTextureUnitState(1)-> \
                        setTextureFiltering(Ogre::TFO_NONE); \
        } \
      } \
    } \
  }

#define CLASS_COUNT 3
#define CLASS_CHOICE(CLASS, I, CLASS_NAME) \
  { \
    int top   = gi->ogre_window->getHeight()/2 - 60 + I*60, \
        left  = gi->ogre_window->getWidth()/2 - str(CLASS_NAME).size()/2*13; \
    bool is_inside = gi->mouse.x >= left \
                  && gi->mouse.x < left + str(CLASS_NAME).size()*13 \
                  && gi->mouse.y >= top \
                  && gi->mouse.y < top + 30; \
    Ogre::OverlayContainer *panel = gp_gi.chose_overlays[k]->getChild("chose " + i2a(k) + " panel"); \
    assert(panel); \
    Ogre::TextAreaOverlayElement *class_name_text = \
      (Ogre::TextAreaOverlayElement*)panel->getChild( \
        "chose " + i2a(k) + " text"); \
    class_name_text->setCaption(CLASS_NAME); \
    class_name_text->setLeft(left); \
    class_name_text->setTop(top); \
    gp_gi.choose_class_text->setCaption("Select a class: "); \
    gp_gi.choose_class_text->setLeft(gi->ogre_window->getWidth()/2 - str("Choose a Hero: ").size()/2*13); \
    gp_gi.choose_class_text->setTop(gi->ogre_window->getHeight()/2 - 120); \
    gp_gi.choose_class_text->show(); \
    class_name_text->setColourBottom(Ogre::ColourValue((float)TCOD_grey.r/255, (float)TCOD_grey.g/255, (float)TCOD_grey.b/255)); \
    class_name_text->setColourTop(Ogre::ColourValue((float)TCOD_grey.r/255, (float)TCOD_grey.g/255, (float)TCOD_grey.b/255)); \
    if(is_inside) { \
      class_name_text->setColourBottom(Ogre::ColourValue(52.f/255, 52.f/255, 200.f/255)); \
      class_name_text->setColourTop(Ogre::ColourValue(52.f/255, 52.f/255, 200.f/255)); \
      if(gi->lbtn_up) \
        mpl->player_class = CLASS; \
    } \
    if(mpl->player_class == CLASS) { \
      class_name_text->setColourBottom(Ogre::ColourValue((float)TCOD_dark_green.r/255, (float)TCOD_dark_green.g/255, (float)TCOD_dark_green.b/255)); \
      class_name_text->setColourTop(Ogre::ColourValue((float)TCOD_dark_green.r/255, (float)TCOD_dark_green.g/255, (float)TCOD_dark_green.b/255)); \
    } \
    gp_gi.chose_overlays[k]->show(); \
    k++; \
  }

#define SKILL_CHOICE(MAT, SKILL) \
  { \
    TR_unit_ptr C = GAME_GetCastle(gi, pl->id); \
    TR_unit_t *castle = &unit(C); \
    bool has_skill = false; \
    DO_TIMES(castle->skill_count) { \
      if(castle->skills[f] == SKILL) { \
        has_skill = true; \
        _finished = 1; \
      } \
    } \
    if(!has_skill) { \
      int X = k%12, \
          Y = k/12; \
      bool found = false; \
      int found_skill_index = -1; \
      DO_TIMES(mpl->choose_skill_count) { \
        if(mpl->choose_skills[f] == SKILL) { \
          found_skill_index = f; \
          found = true; \
          _finished = 1; \
        } \
      } \
      int top   = gi->ogre_window->getHeight()/2 - 600/2 + 100 + (Y)*(BTN_WIDTH+8), \
          left  = gi->ogre_window->getWidth()/2 - 800/2 + 20 + (X)*(BTN_WIDTH+8); \
      bool is_inside = gi->mouse.x >= left \
                    && gi->mouse.x < left + BTN_WIDTH \
                    && gi->mouse.y >= top \
                    && gi->mouse.y < top + BTN_WIDTH; \
      Ogre::PanelOverlayElement *panel = (Ogre::PanelOverlayElement*)gp_gi.chose_overlays[k]->getChild("chose " + i2a(k) + " panel"); \
      assert(panel); \
      Ogre::TextAreaOverlayElement *class_name_text = \
        (Ogre::TextAreaOverlayElement*)panel->getChild( \
          "chose " + i2a(k) + " text"); \
      class_name_text->hide(); \
      panel->setUV(0, 0, 1, 1); \
      panel->setMetricsMode(Ogre::GMM_PIXELS); \
      panel->setLeft(left); \
      panel->setTop(top); \
      panel->setWidth(BTN_WIDTH); \
      panel->setHeight(BTN_WIDTH); \
      panel->setMaterialName(MAT); \
      gp_gi.choose_skills_text->setCaption( \
        "You've gained in experience. Select new types of unit. \n" + i2a(mpl->level_to_assign-mpl->choose_skill_count) + " left: "); \
      gp_gi.choose_skills_text->setLeft(gi->ogre_window->getWidth()/2 - str( \
        "You've gained in experience. Select new types of unit. \n" + i2a(mpl->level_to_assign-mpl->choose_skill_count) + " left: ").size()/2*9); \
      gp_gi.choose_skills_text->setTop(gi->ogre_window->getHeight()/2-600/2+10); \
      gp_gi.choose_skills_text->show(); \
      Ogre::PanelOverlayElement *background =  \
            (Ogre::PanelOverlayElement*)gp_gi.chose_background_overlay-> \
              getChild("chose background panel"); \
      assert(background); \
      background->setMetricsMode(Ogre::GMM_PIXELS); \
      background->setPosition(gi->ogre_window->getWidth()/2-800/2, gi->ogre_window->getHeight()/2-600/2); \
      background->setDimensions(800, 600); \
      background->setUV(0, 0, (float)gi->ogre_window->getWidth()/48, (float)gi->ogre_window->getHeight()/48); \
      background->setMaterialName("background1_mat"); \
      Ogre::PanelOverlayElement *panel3 = (Ogre::PanelOverlayElement*)gp_gi.chose_overlays[k]->getChild("chose " + i2a(k) + " panel3"); \
      if(found) { \
        panel3->setUV(0, 0, 1, 1); \
        panel3->setMetricsMode(Ogre::GMM_PIXELS); \
        panel3->setLeft(left); \
        panel3->setTop(top); \
        panel3->setWidth(BTN_WIDTH); \
        panel3->setHeight(BTN_WIDTH); \
        panel3->setMaterialName("selection_mat_white"); \
        panel3->show(); \
      } else \
        panel3->hide(); \
      Ogre::PanelOverlayElement *panel2 = (Ogre::PanelOverlayElement*)gp_gi.chose_overlays[k]->getChild("chose " + i2a(k) + " panel2"); \
      if(is_inside) { \
        if(GAME_GetSkillName(gi, SKILL).find("invoke") != std::string::npos) \
          GAME_DisplayInvokeInfo( \
            gi, gp_gi.info_overlay, gp_gi.info_panel, gp_gi.info_unit_name, gp_gi.info_unit_carac, gp_gi.info_text_overlay, \
            SKILL, top + 75, left, 18, "Arial"); \
        else \
          GAME_DisplaySkillInfo( \
            gi, gp_gi.info_overlay, gp_gi.info_panel, gp_gi.info_unit_name, gp_gi.info_unit_carac, gp_gi.info_text_overlay, \
            SKILL, top + 75, left, 18, "Arial"); \
        assert(panel2); \
        panel2->setUV(0, 0, 1, 1); \
        panel2->setMetricsMode(Ogre::GMM_PIXELS); \
        panel2->setLeft(left); \
        panel2->setTop(top); \
        panel2->setWidth(BTN_WIDTH); \
        panel2->setHeight(BTN_WIDTH); \
        panel2->setMaterialName("selection_mat_green"); \
        panel2->show(); \
        if(gi->lbtn_up) { \
          if(!found) { \
            if(mpl->choose_skill_count < mpl->level_to_assign) \
              mpl->choose_skills[mpl->choose_skill_count++] = SKILL; \
            else { \
              CAM_SetFlash(gi, ca, "You can choose " + i2a(mpl->level_to_assign) + " skills max. Click on a selected skill to unselect it.", TCOD_red); \
              PLAY_SOUND(gp_gi.miss_sound, "miss") \
            } \
          } else { \
            for(int i = found_skill_index; i < mpl->choose_skill_count-1; i++) \
              mpl->choose_skills[i] = mpl->choose_skills[i+1]; \
            mpl->choose_skill_count--; \
          } \
        } \
      } else \
        panel2->hide(); \
      gp_gi.chose_overlays[k]->show(); \
      k++; \
    } \
  }

#define PLAY_SELECT_SOUND(type) \
  { \
    int i = rand()%SOUND_VARIATION_COUNT; \
    if(SO_WiseToPlay(gi, str("select_") + #type, pos_x, pos_y, -1)) { \
      PLAY_SOUND_I(gp_gi.select_##type, i, str("select_") + #type); \
      TR_sound_t *so = &sound(s); \
      so->x = pos_x; \
      so->y = pos_y; \
    } \
  }

#define PLAY_MOVE_SOUND(type) \
  if((rand()&3) == 0) { \
    int i = rand()%SOUND_VARIATION_COUNT; \
    if(SO_WiseToPlay(gi, str("move_") + #type, pos_x, pos_y, -1)) { \
      PLAY_SOUND_I(gp_gi.move_##type, i, str("move_") + #type); \
      TR_sound_t *so = &sound(s); \
      so->x = pos_x; \
      so->y = pos_y; \
    }  \
  }

#define PLAY_ATTACK_HERE_SOUND(S) \
  if((rand()&3) == 0) { \
    int i = rand()%SOUND_VARIATION_COUNT; \
    if(SO_WiseToPlay(gi, #S, pos_x, pos_y, -1)) { \
      PLAY_SOUND_I(gp_gi.S, i, #S); \
      TR_sound_t *so = &sound(s); \
      so->x = pos_x; \
      so->y = pos_y; \
    } \
  }

#define PLAY_ATTACK_SOUND(type) \
  { \
    int i = rand()%SOUND_VARIATION_COUNT; \
    if(SO_WiseToPlay(gi, str("attack_") + #type, pos_x, pos_y, -1)) { \
      PLAY_SOUND_I(gp_gi.attack_##type, i, str("attack_") + #type); \
      TR_sound_t *so = &sound(s); \
      so->x = pos_x; \
      so->y = pos_y; \
    } \
  }

#define PLAY_DEAD_SOUND_CASE(type) \
  { \
    int i = rand()%SOUND_VARIATION_COUNT; \
    if(SO_WiseToPlay(gi, str("dead_") + #type, un->x, un->y, -1)) { \
      PLAY_SOUND_I(gp_gi.dead_##type, i, str("dead_") + #type); \
      TR_sound_t *so = &sound(s); \
      so->x = un->x; \
      so->y = un->y; \
    } \
  }

#define PLAY_DEAD_SOUND(type) \
  { \
    int i = rand()%SOUND_VARIATION_COUNT; \
    if(SO_WiseToPlay(gi, str("dead_") + #type, un->x, un->y, -1)) { \
      PLAY_SOUND_I(gp_gi.dead_##type, i, str("dead_") + #type); \
      TR_sound_t *so = &sound(s); \
      so->x = un->x; \
      so->y = un->y; \
    } \
  }

#define DRAW_SIMPLE_ATTACK_CURSOR \
  case SKILL_ATTACK: \
    DRAW_SIMPLE_POINTER(TCOD_red, 1) \
    break;

#define GET_SIMPLE_SKILL_NAME(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return "(A) Attack"; \
    }

#define GET_SIMPLE_SKILL_SHORTCUT(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return 'A'; \
    }

#define GET_SIMPLE_SKILL_RANGE(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return ATTACK_RANGE(un); \
    }

#define GET_SIMPLE_SKILL_COOLDOWN(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return 0; \
    }

#define GET_SIMPLE_SKILL_ASCII(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return 0x1540; \
    }

#define GET_SIMPLE_SKILL_SPEED(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return ATTACK_SPEED(un); \
    }

#define GET_SIMPLE_SKILL_DIRECT_CAST(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        return false; \
    }

#define GET_SIMPLE_CAN_TRIGGER_SKILL(type) \
  case type: \
    switch(i) { \
      case SKILL_ATTACK: \
        result = ""; \
        break; \
    } break;

#define SIMPLE_TRIGGER_SKILL(type) \
  case type: \
    switch(uo->skill_type) { \
      case SKILL_ATTACK: { \
          TRIGGER_ATTACK \
          return true; \
        } break; \
    } break;

/*
 * players
 *  - this is unit user data
 */

#define MAX_GAME_PLAYER         MAX_PLAYER
#define gp_player(ptr)        (gp_gi.gp_players[(ptr)])
typedef short TR_gp_player_ptr;

#define PLAYER_STARTING_GOLD        0
#define PLAYER_STARTING_XP          0

#define LAWFUL_GOOD_LEVEL1_XP       200
#define CHAOTIC_GOOD_LEVEL1_XP      200
#define LAWFUL_NEUTRAL_LEVEL1_XP    200
#define CHAOTIC_NEUTRAL_LEVEL1_XP   200
#define LAWFUL_EVIL_LEVEL1_XP       200
#define CHAOTIC_EVIL_LEVEL1_XP      200
#define CHAOTIC_EVIL_LEVEL2_XP      300
#define SKELETON_HORDER_XP          250
#define EVIL_BOOTS_XP               100

#define DIVINE_FIRE                 1
#define KING_STRIKE                 2
#define GOD_LIGHT                   3

#define ORC_MAX_RAGE                10000
#define ORC_SMASH                   1
#define ORC_TAUNT                   2
#define ORC_ENRAGE                  3

#define EVIL_BOOTS_BOOST                4

#define SKILL_ATTACK                    0

#define WIZARD_WRATH                   1
#define WIZARD_WRATH_MANA_COST         TR_exp(LEVEL(un)-1, 80, 105)

#define TRAIN_KNIGHT                    0
#define TRAIN_WIZARD                    1
#define TRAIN_DRIADE                    2
#define TRAIN_WOODMISTRESS              3
#define TRAIN_DRAGON                    4
#define TRAIN_PINEBAT                   5
#define TRAIN_GRASSKELETON              6
#define TRAIN_KINGGUARD                 7
#define DRIADE_HEAL_MANA_COST           TR_exp(LEVEL(un)-1, 50, 105)
#define WOODMISTRESS_ROOT_MANA_COST     TR_exp(LEVEL(un)-1, 50, 105)
#define DRIADE_HEAL                     1
#define BAT_SLIME                       1
#define BAT_SLIME_BALL                  1
#define GRASS_SKELETON_INVINCIBILITY    1
#define KING_GUARD_KING_AURA            1
#define DRAGON_METEOR                   1

#define BAT_BALL_AOE                    5
#define BAT_ATTACK_DAMAGE               6

#define TRAIN_UNIT(UNIT_TYPE) \
  TR_client_ptr c = GI_GetClientPtr(gi, cam->player_id); \
  if(client(c).type == HUMAN_CLIENT) { \
    CL_CmdBuildOrder( \
      gi, \
      c, \
      un->cmd_id, \
      un->player_id, \
      cam->player_id, \
      UNIT_TYPE, \
      USER_DATA_PLAYER_UNIT, \
      un->x + (un->team_id == 0 ? 2 : -2) + TR_Rand(&gp_gi.working_seed)%200, \
      un->y + TR_Rand(&gp_gi.working_seed)%200, \
      true); \
  } \
  cam_mpl->gold -= gi->unit_dic[UNIT_TYPE].gold_price; \
  assert(cam_mpl->gold >= 0);

#define RAND(X) (TR_Rand(&gp_gi.working_seed)%X)
#define UNIT(X) (TR_unit_type_##X)
#define MAX_INVOKE_TYPE 8
#define TIER1_UNIT_COUNT 10
#define TIER2_UNIT_COUNT 4
#define TIER3_UNIT_COUNT 1
#define MPL_SKILL_COUNT 4
#define MPL_MANA_FACTOR         100
#define MPL_MAX_MANA            (mpl->level*100*MPL_MANA_FACTOR)
#define MPL_MANA_REGEN_BASE     150

#define INVOKE(UNIT_TYPE) \
  if(mpl->mana >= skill_mana_cost*MPL_MANA_FACTOR) { \
    int id = pl->id%game_desc.player_count_per_team, \
        pos_x, pos_y; \
    switch(id) { \
      case 0: \
        pos_x = 10; \
        pos_y = 10; \
        break; \
      case 1: \
        pos_x = 20-1; \
        pos_y = 10; \
        break; \
      case 2: \
        pos_x = 10; \
        pos_y = 20; \
        break; \
    } \
    mpl->mana -= skill_mana_cost*MPL_MANA_FACTOR; \
    un->skills_cooldown[skill_id] = 0; \
    TR_client_ptr c = GI_GetClientPtr(gi, un->player_id); \
    TR_client_t *cl = &client(c); \
    if(cl->type == HUMAN_CLIENT) { \
      TR_unit_ptr c_u = GAME_GetCastle(gi, un->player_id); \
      assert(c_u != TR_null); \
      TR_castle_ptr C = unit(c_u).user_data; \
      if(C != TR_null) \
        CL_CmdBuildOrder( \
          gi, \
          c, \
          unit(castle(C).u).cmd_id, \
          unit(castle(C).u).player_id, \
          un->player_id, \
          TR_unit_type_##UNIT_TYPE, \
          USER_DATA_PLAYER_UNIT, \
          (un->team_id == 0 ? pos_x : 128-1-pos_x)<<POS_SHIFT, \
          (un->team_id == 0 ? 128-1-pos_y : pos_y)<<POS_SHIFT, \
          true); \
    } \
  } \
  return false;

enum TR_player_class_t {
  TR_player_class_wizard,
  TR_player_class_druid,
  TR_player_class_necromencer,
};

struct TR_gp_player_t {
  TR_player_class_t     player_class;
  int                   gold,
                        xp,
                        kill,
                        loss,
                        level_to_assign,
                        level,
                        spawn_timer,
                        hit_blood,
                        mana,
                        skill_count,
                        choose_skill_count,
                        unit_counts[MAX_INVOKE_TYPE],
                        skill_levels[TR_skill_type_count],
                        skills_cooldown[TR_skill_type_count];
  TR_skill_type_t       skills[MAX_SKILLS_PER_UNIT];
  TR_skill_type_t       choose_skills[MAX_SKILLS_PER_UNIT];
  TR_time_t             mana_regen,
                        mana_regen_span;
  bool                  is_chosing_class,
                        assign_level;
  TR_gp_player_ptr      next,
                        previous,
                        fnext;
};

void                  MP_Init     (int argc, char** argv);
TR_gp_player_ptr      MP_New      ();
void                  MP_Free     (TR_gp_player_ptr l);
void                  MP_FreeAll  (TR_gp_player_ptr l);

/*
 * mobs
 *  - this is unit user data
 */

#define MAX_MOBS         MAX_UNIT
#define mobs(ptr)        (gp_gi.mobs[(ptr)])
typedef short TR_mobs_ptr;

struct TR_mobs_t {
  int           lane;
  TR_mobs_ptr   next,
                previous,
                fnext;
};

void          MOBS_Init     (int argc, char** argv);
TR_mobs_ptr   MOBS_New      ();
void          MOBS_Free     (TR_mobs_ptr l);
void          MOBS_FreeAll  (TR_mobs_ptr l);

/*
 * player_units
 *  - this is unit user data
 */

#define MAX_PLAYER_UNITS        MAX_UNIT
#define player_unit(ptr)        (gp_gi.player_units[(ptr)])
typedef short TR_player_unit_ptr;

struct TR_player_unit_t {
  str                     tags;
  TR_particle_system_ptr  ps,
                          ps2;
  bool                    resurect_it;
  int                     rage;
  Ogre::OverlayContainer  *custom_bar_panel,
                          *custom_bar_back_panel;
  Ogre::Overlay           *custom_bar_back_overlay,
                          *custom_bar_overlay;
  TR_player_unit_ptr      next,
                          previous,
                          fnext;
};

void                  PUN_Init     (int argc, char** argv);
TR_player_unit_ptr    PUN_New      ();
void                  PUN_Free     (TR_player_unit_ptr l);
void                  PUN_FreeAll  (TR_player_unit_ptr l);


/*
 * castles
 *  - this is unit user data
 */

#define MAX_CASTLE                 32
#define MAX_RESPAWN_UNIT_TEXT      16
#define castle(ptr)                (gp_gi.castles[(ptr)])
typedef short TR_castle_ptr;

struct TR_castle_t {
  Ogre::Overlay     *respawn_unit_overlay;
  Ogre::TextAreaOverlayElement *respawn_unit_texts[MAX_RESPAWN_UNIT_TEXT];
  TR_unit_ptr      u;
  TR_unit_ptr      rally_target;
  TR_id_t          rally_target_cmd_id,
                    rally_target_player_id;
  int               build_index,
                    skills_index;
  int               rally_cx,
                    rally_cy;
  TR_id_t          player_id,
                    team_id;
  TR_castle_ptr    next,
                    previous,
                    fnext;
};

void              CAS_Init                  (int argc, char** argv);
TR_castle_ptr    CAS_New                   ();
void              CAS_Free                  (TR_castle_ptr l);
void              CAS_FreeAll               (TR_castle_ptr l);

/*
 * mobs_ai_client
 */
#define MAX_MOBS_AI_CLIENT        MAX_CLIENT
#define MOBS_SPAWN_TIME           35000
#define mobs_ai_client(ptr)       (gp_gi.mobs_ai_clients[(ptr)])
typedef short TR_mobs_ai_client_ptr;

struct TR_mobs_ai_client_t {
  bool                     first_spawn;
  int                     spawn_id,
                          mobs_count;
  TR_time_t               mobs_spawn_timer;
  TR_mobs_ai_client_ptr   next,
                          previous,
                          fnext;
};

void                    MO_AI_CL_Init         (int argc, char** argv);
TR_mobs_ai_client_ptr  MO_AI_CL_New          ();
void                    MO_AI_CL_Free         (TR_mobs_ai_client_ptr l);
void                    MO_AI_CL_FreeAll      (TR_mobs_ai_client_ptr l);
bool                    MO_AI_CL_Update       (TR_mobs_ai_client_ptr c);

/*
 * creep_ai_client
 */
#define MAX_CREEP_AI_CLIENT         MAX_CLIENT
#define SOLO_END_SPAN               5000
#define creep_ai_client(ptr)        (gp_gi.creep_ai_clients[(ptr)])
#define CREEP_PLAYER_ID             ((game_desc.team_count-1) * game_desc.player_count_per_team)
typedef int TR_creep_ai_client_ptr;
typedef int TR_dead_creep_ptr;

struct TR_creep_ai_client_t {
  int                       generated_creep_count,
                            epic_center_x,
                            epic_center_y,
                            end_time;
  TR_dead_creep_ptr        creep_head;
  TR_creep_ai_client_ptr   next,
                            previous,
                            fnext;
};

void                    CR_AI_CL_Init         (int argc, char** argv);
TR_creep_ai_client_ptr CR_AI_CL_New          ();
void                    CR_AI_CL_Free         (TR_creep_ai_client_ptr l);
void                    CR_AI_CL_FreeAll      (TR_creep_ai_client_ptr l);
bool                    CR_AI_CL_Update       (TR_creep_ai_client_ptr c);

/*
 * creep
 */
#define MAX_CREEP                 MAX_UNIT
#define CREEP_RESPAWN_SPAN        (60000*450)
#define creep(ptr)                (gp_gi.creeps[(ptr)])
typedef int TR_creep_ptr;

struct TR_creep_t {
  TR_id_t        id;
  TR_time_t      respawn_span;
  int             x, 
                  y,
                  level;
  bool            dead;
  TR_unit_type_t unit_type;
  TR_creep_ptr   next,
                  previous,
                  fnext;
};

void               CR_Init         (int argc, char** argv);
TR_creep_ptr      CR_New          ();
void               CR_Free         (TR_creep_ptr l);
void               CR_FreeAll      (TR_creep_ptr l);
bool               CR_Update       (TR_creep_ptr c);

/*
 * game instance
 */
#define RIGHT_LANE              0
#define MIDDLE_LANE             1
#define LEFT_LANE               2

/* 1 is human */
#define PLAYER_AI_CLIENT        (1<<5)
#define MOBS_AI_CLIENT          (1<<6)
#define CREEP_AI_CLIENT         (1<<7)

#define DEMI_XP_RADIUS          15

#define MAX_CHOSE_OVERLAY       64

#if RELEASE && !RECORD_DEMO
#define START_BATTLE_TIME       60000
#else
#define START_BATTLE_TIME       5000
#endif

#define MANA_BAR_SIZEX          300
#define MANA_BAR_SIZEY          20

struct TR_gp_game_instance_t {
  int                           level,
                                mobs_count,
                                choose_counter,
                                day_night_time;
  TCOD_bsp_t                    *bsp;
  Ogre::Overlay                 *resource_overlay,
                                *kill_loss_overlay,
                                *info_overlay,
                                *info_overlay2,
                                *level_overlay,
                                *hit_blood_overlay,
                                *plus_overlay[MAX_SKILLS_PER_CAMERA],
                                *skill_level_overlay[MAX_SKILLS_PER_CAMERA],
                                *ok_button_overlay,
                                *assign_level_button,
                                *chose_background_overlay,
                                *chose_overlays[MAX_CHOSE_OVERLAY];
  Ogre::OverlayContainer        *info_panel,
                                *info_panel2,
                                *mana_bar,
                                *mana_bar_back;
  Ogre::TextAreaOverlayElement  *gold_overlay,
                                *xp_overlay,
                                *mana_overlay,
                                *mana_regen_overlay,
                                *kill_loss_text_overlay,
                                *choose_class_text,
                                *choose_skills_text,
                                *choose_counter_text,
                                *info_text_overlay,
                                *info_unit_name,
                                *info_unit_carac,
                                *info_text_overlay2,
                                *info_unit_name2,
                                *info_unit_carac2;
  TR_mobs_t                     mobs[MAX_MOBS];
  TR_mobs_ptr                   free_mobs_head;
  TR_mobs_ptr                   mobs_head;
  TR_player_unit_t              player_units[MAX_PLAYER_UNITS];
  TR_player_unit_ptr            free_player_unit_head;
  TR_player_unit_ptr            player_unit_head;
  TR_gp_player_t                gp_players[MAX_GAME_PLAYER];
  TR_gp_player_ptr              free_gp_player_head;
  TR_gp_player_ptr              gp_player_head;
  TR_castle_t                   castles[MAX_CASTLE];
  TR_castle_ptr                 free_castle_head;
  TR_castle_ptr                 castle_head;
  TR_mobs_ai_client_t           mobs_ai_clients[MAX_MOBS_AI_CLIENT];
  TR_mobs_ai_client_ptr         free_mobs_ai_client_head;
  TR_mobs_ai_client_ptr         mobs_ai_client_head;
  TR_creep_ai_client_t          creep_ai_clients[MAX_CREEP_AI_CLIENT];
  TR_creep_ai_client_ptr        free_creep_ai_client_head;
  TR_creep_ai_client_ptr        creep_ai_client_head;
  TR_creep_t                    creeps[MAX_CREEP];
  TR_creep_ptr                  free_creep_head;
  TR_btn_list_t                 central_hud_btns;
  TR_btn_t                      window_switch;
  unsigned int                   working_seed;
  /* audio */       
  ga_Handle                     *music_handle;
  gau_SampleSourceLoop          *music_loop;
  ga_Sound                      *coin_sound,
                                *xp_sound,
                                *torch_sound,
                                *foot_sound,
                                *miss_sound,
                                *basic_hit_sound,
                                *dark_skill_sound,
                                *orc_smash_sound,
                                *orc_enrage_sound,
                                *orc_smash_cry_sound,
                                *explosion_fireball,
                                *explosion_knife,
                                #include "gen/sound_decl.cpp"
                                ;
  bool                          wizards_taken,
                                knights_taken,
                                orcs_taken;
};

void            GAME_FreeAll                  (TR_game_instance_t *gi);
void            GAME_InitGame                 (TR_game_instance_t *gi, game_desc_t *gd);
void 			      GAME_CloneNode 				        (TR_game_instance_t *gi, Ogre::SceneNode *node);
void            GAME_MakeTrees                (TR_game_instance_t *gi, Ogre::SceneNode *node);
void            GAME_GenerateTrees            (TR_game_instance_t *gi);
void            GAME_StartGame                (TR_game_instance_t *gi);
void            GAME_StartMobsAIClient        (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_StartCreepAIClient       (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_StartPlayerClient        (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_StartHumanClient         (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_SpawnUnits               (TR_game_instance_t *gi, TR_client_ptr c, TR_user_data_type_t user_data_type);
unsigned char   GAME_GenerateCreep            (TCOD_bsp_t *node, void *user_data);
void            GAME_CreepAt                  (TR_creep_ai_client_ptr cr_ai_c, int i, int j, TR_unit_type_t unit_type, int count, int level);
void            GAME_UpdateClient             (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_UpdateMobsAI             (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_UpdateCreepAI            (TR_game_instance_t *gi, TR_client_ptr c);
void            GAME_UpdatePlayer             (TR_game_instance_t *gi, TR_player_ptr p);
void            GAME_MobsOrderCompletedCB     (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_order_ptr o);
void            GAME_MobsOrderFailedCB        (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_order_ptr o);
void            GAME_OnUpdateUnit             (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
bool            GAME_OnDeath                  (TR_game_instance_t *gi, TR_player_ptr killer_p, TR_player_ptr unit_p, TR_unit_ptr u);
void            GAME_GeneralDeath             (TR_game_instance_t *gi, TR_player_ptr killer_p, TR_player_ptr unit_p, TR_unit_ptr u);
short           GAME_GetSkillsCount           (TR_game_instance_t *gi, TR_unit_type_t unit_type);
str             GAME_GetSkillName             (TR_game_instance_t *gi, TR_skill_type_t skill_type);
str             GAME_GetSkillAnimName         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
int             GAME_GetSkillAnimTime         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
int             GAME_GetSkillMaxLevel         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
int             GAME_GetSkillEveryLevel       (TR_game_instance_t *gi, TR_skill_type_t skill_type);
TR_skill_type_t GAME_DefaultSkillType         (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target);
char            GAME_GetSkillShortcut         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
str             GAME_GetSkillInfo             (TR_game_instance_t *gi, TR_skill_type_t skill_type);
int             GAME_GetSkillRange            (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
int             GAME_GetSkillCoolDown         (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
int             GAME_GetSkillManaCost         (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
str             GAME_GetSkillMaterial         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
str             GAME_GetPSkillMaterial        (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_type_t unit_type);
int             GAME_GetSkillCastSpeed        (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
int             GAME_GetSkillCastSpeedProxy   (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
bool            GAME_GetSkillDirectCast       (TR_game_instance_t *gi, TR_skill_type_t skill_type);
TR_unit_type_t  GAME_GetSkillInvokeType       (TR_game_instance_t *gi, TR_skill_type_t skill_type);
int             GAME_GetSkillInvokeCount      (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillCanStop          (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillContinue         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillIsBlocking       (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillIsAvailable      (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillIsPassive        (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillIsUnique         (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillLookForAir       (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillLookForGround    (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_GetSkillCastOneByOne     (TR_game_instance_t *gi, TR_skill_type_t skill_type);
str             GAME_CanCastSkill             (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type, TR_unit_ptr target, short cx, short cy, bool dont_check_common_cond);
str             GAME_CanCastSkill2            (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type, TR_unit_ptr target, short cx, short cy);
bool            GAME_SkillLooksForUnit        (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_SkillLooksForEnemy       (TR_game_instance_t *gi, TR_skill_type_t skill_type);
bool            GAME_SkillLooksForTarget      (TR_game_instance_t *gi, TR_skill_type_t skill_type);
TR_time_t       GAME_SkillSpeed               (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
bool            GAME_CastSkill                (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_order_ptr o, int x, int y);
bool            GAME_HUDInfo                  (TR_game_instance_t *gi, TR_camera_ptr ca);
void            GAME_DisplayUnitInfo          (TR_game_instance_t *gi, Ogre::Overlay *overlay, Ogre::OverlayContainer *panel, Ogre::TextAreaOverlayElement *unit_name, Ogre::TextAreaOverlayElement *unit_carac, Ogre::TextAreaOverlayElement *info_text, TR_unit_type_t unit_type, int top, int left, int size, const char *font, bool display_info=true);
void            GAME_DisplayInvokeInfo        (TR_game_instance_t *gi, Ogre::Overlay *overlay, Ogre::OverlayContainer *panel, Ogre::TextAreaOverlayElement *unit_name, Ogre::TextAreaOverlayElement *unit_carac, Ogre::TextAreaOverlayElement *info_text, TR_skill_type_t skill_type, int top, int left, int size, const char *font, bool display_info=true);
void            GAME_DisplaySkillInfo         (TR_game_instance_t *gi, Ogre::Overlay *overlay, Ogre::OverlayContainer *panel, Ogre::TextAreaOverlayElement *unit_name, Ogre::TextAreaOverlayElement *unit_carac, Ogre::TextAreaOverlayElement *info_text, TR_skill_type_t skill_type, int top, int left, int size, const char *font, bool display_info=true);
void            GAME_UpdateUnitRender         (TR_game_instance_t *gi, TR_camera_ptr ca, TR_player_ptr p, TR_unit_ptr u);
str             GAME_GetUnitName              (TR_game_instance_t *gi, TR_unit_type_t unit_type);
void            GAME_OnSpawnUnit              (TR_game_instance_t *gi, TR_unit_ptr from_u, TR_unit_ptr u);
int             GAME_OnHitUnit                (TR_game_instance_t *gi, TR_ball_ptr b, TR_unit_ptr u, int damage, int attack_penetration, bool show_hp);
void            GAME_OnCancelBuild            (TR_game_instance_t *gi, TR_build_order_ptr b);
void            GAME_GenericCmd               (TR_game_instance_t *gi, TR_id_t cmd_type, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy);
void            GAME_OnUnitOrder              (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy, bool skill_here, TR_skill_type_t skill_id);
TR_unit_ptr     GAME_GetCastle                (TR_game_instance_t *gi, TR_id_t player_id);
TR_unit_ptr     GAME_GetHero                  (TR_game_instance_t *gi, TR_player_ptr p);
void            GAME_OpenWindow               (SDL_Surface *surface, TR_game_instance_t *gi, TR_client_ptr c, int size_x, int size_y);
bool            GAME_DrawUnitGoal             (TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u);
str             GAME_GetCursor                (TR_game_instance_t *gi, TR_camera_ptr ca, TR_skill_type_t skill_id, int cx, int cy);
void            GAME_OnSelectUnitSound        (TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y);
void            GAME_OnMoveUnitSound          (TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y);
void            GAME_OnSkillSound         	  (TR_game_instance_t *gi, TR_id_t skill_id, int pos_x, int pos_y, bool can_cast);
void            GAME_OnAttackUnitSound        (TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y);
TR_unit_ptr     GAME_GetUnitInRange           (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_ptr units[MAX_UNIT], int unit_count, int skill_range);
void            GAME_SaveGame                 (TR_game_instance_t *gi, int level);
void            GAME_LoadGameDesc             (TR_game_instance_t *gi);
void            GAME_LoadGame                 (TR_game_instance_t *gi);
int             GAME_GetSelectionSkills       (TR_game_instance_t *gi, TR_camera_ptr ca, int skill_ids[MAX_SKILLS_PER_CAMERA], TR_skill_type_t skill_types[MAX_SKILLS_PER_CAMERA], TR_unit_ptr skill_units[MAX_SKILLS_PER_CAMERA]);

extern TR_gp_game_instance_t gp_gi;

/*
 * end of file
 */    