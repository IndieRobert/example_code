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

/*
 * mobs
 */

void MOBS_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_MOBS; i++) {
    if(i != MAX_MOBS-1)
      mobs(i).fnext = i+1;
    else
      mobs(i).fnext = TR_null;
  }
  gp_gi.free_mobs_head = 0;
  gp_gi.mobs_head      = TR_null;
  log("mobss initialized.");
}

TR_mobs_ptr MOBS_New() {
  if(gp_gi.free_mobs_head != TR_null) {
    TR_mobs_ptr r           = gp_gi.free_mobs_head;
    gp_gi.free_mobs_head  = mobs(gp_gi.free_mobs_head).fnext;
    mobs(r).previous        = TR_null;
    mobs(r).next            = gp_gi.mobs_head;
    if(gp_gi.mobs_head != TR_null)
      mobs(gp_gi.mobs_head).previous = r;
    gp_gi.mobs_head = r;
    return r;
  }
  log("No more mobs left.");
  return TR_null;
}

void MOBS_Free(TR_mobs_ptr l) {
  if(gp_gi.mobs_head == l)
    gp_gi.mobs_head = mobs(l).next;
  if(mobs(l).previous != TR_null)
    mobs(mobs(l).previous).next = mobs(l).next;
  if(mobs(l).next != TR_null)
    mobs(mobs(l).next).previous = mobs(l).previous;
  mobs(l).fnext = gp_gi.free_mobs_head;
  gp_gi.free_mobs_head = l;
}

void MOBS_FreeAll(TR_game_instance_t *gi) {
  while(gp_gi.mobs_head != TR_null)
    MOBS_Free(gp_gi.mobs_head);
  log("mobss freed.");
}

/*
 * player_unit
 */

void PUN_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_PLAYER_UNITS; i++) {
    if(i != MAX_PLAYER_UNITS-1)
      player_unit(i).fnext = i+1;
    else
      player_unit(i).fnext = TR_null;
  }
  gp_gi.free_player_unit_head = 0;
  gp_gi.player_unit_head      = TR_null;
  log("player_units initialized.");
}

TR_player_unit_ptr PUN_New() {
  if(gp_gi.free_player_unit_head != TR_null) {
    TR_player_unit_ptr r           = gp_gi.free_player_unit_head;
    gp_gi.free_player_unit_head  = player_unit(gp_gi.free_player_unit_head).fnext;
    player_unit(r).resurect_it     = false;
    player_unit(r).ps              = TR_null;
    player_unit(r).previous        = TR_null;
    player_unit(r).next            = gp_gi.player_unit_head;
    player_unit(r).custom_bar_overlay = null;
    if(gp_gi.player_unit_head != TR_null)
      player_unit(gp_gi.player_unit_head).previous = r;
    gp_gi.player_unit_head = r;
    return r;
  }
  log("No more player_unit left.");
  return TR_null;
}

void PUN_Free(TR_player_unit_ptr l) {
  TR_player_unit_t *pun = &player_unit(l);
  if(pun->custom_bar_overlay) {
    pun->custom_bar_overlay->hide();
    pun->custom_bar_back_overlay->hide();
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("rage " + i2a(l) + " panel");
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("rage back " + i2a(l) + " panel");
    Ogre::OverlayManager::getSingleton().destroy(pun->custom_bar_overlay);
    Ogre::OverlayManager::getSingleton().destroy(pun->custom_bar_back_overlay);
  }
  if(gp_gi.player_unit_head == l)
    gp_gi.player_unit_head = player_unit(l).next;
  if(player_unit(l).previous != TR_null)
    player_unit(player_unit(l).previous).next = player_unit(l).next;
  if(player_unit(l).next != TR_null)
    player_unit(player_unit(l).next).previous = player_unit(l).previous;
  player_unit(l).fnext = gp_gi.free_player_unit_head;
  gp_gi.free_player_unit_head = l;
}

void PUN_FreeAll(TR_game_instance_t *gi) {
  while(gp_gi.player_unit_head != TR_null)
    PUN_Free(gp_gi.player_unit_head);
  log("player_units freed.");
}

/*
 * gp_player
 */

void MP_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_GAME_PLAYER; i++) {
    if(i != MAX_GAME_PLAYER-1)
      gp_player(i).fnext = i+1;
    else
      gp_player(i).fnext = TR_null;
  }
  gp_gi.free_gp_player_head = 0;
  gp_gi.gp_player_head      = TR_null;
  log("gp_players initialized.");
}

TR_gp_player_ptr MP_New() {
  if(gp_gi.free_gp_player_head != TR_null) {
    TR_gp_player_ptr r                  = gp_gi.free_gp_player_head;
    gp_gi.free_gp_player_head           = gp_player(gp_gi.free_gp_player_head).fnext;
    gp_player(r).gold                   = PLAYER_STARTING_GOLD * (gp_gi.level+1);
    gp_player(r).xp                     = 0;
    gp_player(r).kill                   = 0;
    gp_player(r).loss                   = 0;
    gp_player(r).mana                   = 1;
    gp_player(r).mana_regen             = 150;
    gp_player(r).mana_regen_span        = 0;
    gp_player(r).level_to_assign        = 1;
    gp_player(r).spawn_timer            = 5000;
    gp_player(r).hit_blood              = 0;
    gp_player(r).is_chosing_class             = true;
    gp_player(r).assign_level           = false;
    gp_player(r).previous               = TR_null;
    gp_player(r).next                   = gp_gi.gp_player_head;
    gp_player(r).level                  = 1;
    gp_player(r).skill_count            = 0;
    gp_player(r).player_class           = (TR_player_class_t)TR_null;
    DO_TIMES(MAX_INVOKE_TYPE)
      gp_player(r).unit_counts[f] = 0;
    DO_TIMES(TR_skill_type_count) {
      gp_player(r).skill_levels[f] = 0;
      gp_player(r).skills_cooldown[f] = 99999999;
    }
    if(gp_gi.gp_player_head != TR_null)
      gp_player(gp_gi.gp_player_head).previous = r;
    gp_gi.gp_player_head = r;
    return r;
  }
  log("No more gp_player left.");
  return TR_null;
}

void MP_Free(TR_gp_player_ptr l) {
  if(gp_gi.gp_player_head == l)
    gp_gi.gp_player_head = gp_player(l).next;
  if(gp_player(l).previous != TR_null)
    gp_player(gp_player(l).previous).next = gp_player(l).next;
  if(gp_player(l).next != TR_null)
    gp_player(gp_player(l).next).previous = gp_player(l).previous;
  gp_player(l).fnext = gp_gi.free_gp_player_head;
  gp_gi.free_gp_player_head = l;
}

void MP_FreeAll(TR_game_instance_t *gi) {
  while(gp_gi.gp_player_head != TR_null)
    MP_Free(gp_gi.gp_player_head);
  log("gp_players freed.");
}

/*
 * castle
 */

void CAS_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_CASTLE; i++) {
    if(i != MAX_CASTLE-1)
      castle(i).fnext = i+1;
    else
      castle(i).fnext = TR_null;
  }
  gp_gi.free_castle_head = 0;
  gp_gi.castle_head      = TR_null;
  log("castles initialized.");
}

TR_castle_ptr CAS_New() {
  if(gp_gi.free_castle_head != TR_null) {
    TR_castle_ptr r                = gp_gi.free_castle_head;
    gp_gi.free_castle_head       = castle(gp_gi.free_castle_head).fnext;
    castle(r).previous             = TR_null;
    castle(r).next                 = gp_gi.castle_head;
    castle(r).rally_target         = TR_null;
    castle(r).rally_cx             = -1;
    castle(r).build_index          = 0;
    castle(r).skills_index         = 0;
    /* overlay */
    castle(r).respawn_unit_overlay   = Ogre::OverlayManager::getSingleton().create("CA build unit overlay " + i2a(r));
    Ogre::OverlayContainer *panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "CA build unit panel " + i2a(r));
    panel->setMetricsMode(Ogre::GMM_PIXELS);
    panel->setPosition(10, 10);
    panel->setDimensions(100, 100);
    DO_TIMES(MAX_RESPAWN_UNIT_TEXT) {
      castle(r).respawn_unit_texts[f] = 
        (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
          "TextArea", "CA build unit overlay text area " + i2a(r) + " " + i2a(f));
      castle(r).respawn_unit_texts[f]->setMetricsMode(Ogre::GMM_PIXELS);
      castle(r).respawn_unit_texts[f]->setLeft(0);
      castle(r).respawn_unit_texts[f]->setCaption("Yoooopi");
      castle(r).respawn_unit_texts[f]->setFontName("cuckoo");
      castle(r).respawn_unit_texts[f]->setCharHeight(22);
      castle(r).respawn_unit_texts[f]->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
      castle(r).respawn_unit_texts[f]->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
      castle(r).respawn_unit_texts[f]->hide();
      panel->addChild(castle(r).respawn_unit_texts[f]);
    }
    castle(r).respawn_unit_overlay->add2D(panel);
    castle(r).respawn_unit_overlay->show();
    if(gp_gi.castle_head != TR_null)
      castle(gp_gi.castle_head).previous = r;
    gp_gi.castle_head = r;
    return r;
  }
  log("No more castle left.");
  return TR_null;
}

void CAS_Free(TR_castle_ptr l) {
  if(gp_gi.castle_head == l)
    gp_gi.castle_head = castle(l).next;
  if(castle(l).previous != TR_null)
    castle(castle(l).previous).next = castle(l).next;
  if(castle(l).next != TR_null)
    castle(castle(l).next).previous = castle(l).previous;
  castle(l).fnext = gp_gi.free_castle_head;
  gp_gi.free_castle_head = l;
}

void CAS_FreeAll(TR_game_instance_t *gi) {
  while(gp_gi.castle_head != TR_null)
    CAS_Free(gp_gi.castle_head);
  log("castles freed.");
}

/*
 * mobs_ai_client
 */

void MO_AI_CL_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_MOBS_AI_CLIENT; i++) {
    if(i != MAX_MOBS_AI_CLIENT-1)
      mobs_ai_client(i).fnext = i+1;
    else
      mobs_ai_client(i).fnext = TR_null;
  }
  gp_gi.free_mobs_ai_client_head = 0;
  gp_gi.mobs_ai_client_head      = TR_null;
  log("mobs_ai_clients initialized.");
}

TR_mobs_ai_client_ptr MO_AI_CL_New() {
  if(gp_gi.free_mobs_ai_client_head != TR_null) {
    TR_mobs_ai_client_ptr r                = gp_gi.free_mobs_ai_client_head;
    gp_gi.free_mobs_ai_client_head       = mobs_ai_client(gp_gi.free_mobs_ai_client_head).fnext;
    mobs_ai_client(r).previous             = TR_null;
    mobs_ai_client(r).next                 = gp_gi.mobs_ai_client_head;
    mobs_ai_client(r).mobs_spawn_timer     = START_BATTLE_TIME;
    mobs_ai_client(r).first_spawn          = true;
    mobs_ai_client(r).spawn_id             = 0;
    mobs_ai_client(r).mobs_count           = 0;
    if(gp_gi.mobs_ai_client_head != TR_null)
      mobs_ai_client(gp_gi.mobs_ai_client_head).previous = r;
    gp_gi.mobs_ai_client_head = r;
    return r;
  }
  log("No more mobs_ai_client left.");
  return TR_null;
}

void MO_AI_CL_Free(TR_mobs_ai_client_ptr l) {
  if(gp_gi.mobs_ai_client_head == l)
    gp_gi.mobs_ai_client_head = mobs_ai_client(l).next;
  if(mobs_ai_client(l).previous != TR_null)
    mobs_ai_client(mobs_ai_client(l).previous).next = mobs_ai_client(l).next;
  if(mobs_ai_client(l).next != TR_null)
    mobs_ai_client(mobs_ai_client(l).next).previous = mobs_ai_client(l).previous;
  mobs_ai_client(l).fnext = gp_gi.free_mobs_ai_client_head;
  gp_gi.free_mobs_ai_client_head = l;
}

void MO_AI_CL_FreeAll(TR_game_instance_t *gi) {
  while(gp_gi.mobs_ai_client_head != TR_null)
    MO_AI_CL_Free(gp_gi.mobs_ai_client_head);
  log("mobs_ai_clients freed.");
}

/*
 * creep_ai_client
 */

void CR_AI_CL_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_CREEP_AI_CLIENT; i++) {
    if(i != MAX_CREEP_AI_CLIENT-1)
      creep_ai_client(i).fnext = i+1;
    else
      creep_ai_client(i).fnext = TR_null;
  }
  gp_gi.free_creep_ai_client_head = 0;
  gp_gi.creep_ai_client_head      = TR_null;
  log("creep_ai_clients initialized.");
}

TR_creep_ai_client_ptr CR_AI_CL_New() {
  if(gp_gi.free_creep_ai_client_head != TR_null) {
    TR_creep_ai_client_ptr r                = gp_gi.free_creep_ai_client_head;
    gp_gi.free_creep_ai_client_head       = creep_ai_client(gp_gi.free_creep_ai_client_head).fnext;
    creep_ai_client(r).previous             = TR_null;
    creep_ai_client(r).next                 = gp_gi.creep_ai_client_head;
    creep_ai_client(r).creep_head           = TR_null;
    creep_ai_client(r).end_time             = SOLO_END_SPAN;
    if(gp_gi.creep_ai_client_head != TR_null)
      creep_ai_client(gp_gi.creep_ai_client_head).previous = r;
    gp_gi.creep_ai_client_head = r;
    return r;
  }
  log("No more creep_ai_client left.");
  return TR_null;
}

void CR_AI_CL_Free(TR_creep_ai_client_ptr l) {
  CR_FreeAll(l);
  if(gp_gi.creep_ai_client_head == l)
    gp_gi.creep_ai_client_head = creep_ai_client(l).next;
  if(creep_ai_client(l).previous != TR_null)
    creep_ai_client(creep_ai_client(l).previous).next = creep_ai_client(l).next;
  if(creep_ai_client(l).next != TR_null)
    creep_ai_client(creep_ai_client(l).next).previous = creep_ai_client(l).previous;
  creep_ai_client(l).fnext = gp_gi.free_creep_ai_client_head;
  gp_gi.free_creep_ai_client_head = l;
}

void CR_AI_CL_FreeAll(TR_game_instance_t *gi) {
  while(gp_gi.creep_ai_client_head != TR_null)
    CR_AI_CL_Free(gp_gi.creep_ai_client_head);
  log("creep_ai_clients freed.");
}

/*
 * creep
 */

void CR_Init(int argc, char** argv) {
  for(int i = 0; i < MAX_CREEP; i++) {
    if(i != MAX_CREEP-1)
      creep(i).fnext = i+1;
    else
      creep(i).fnext = TR_null;
  }
  gp_gi.free_creep_head = 0;
  log("creeps initialized.");
}

TR_creep_ptr CR_New(TR_creep_ai_client_ptr cr_ai_c) {
  if(gp_gi.free_creep_head != TR_null) {
    TR_creep_ptr r                = gp_gi.free_creep_head;
    gp_gi.free_creep_head       = creep(gp_gi.free_creep_head).fnext;
    creep(r).previous             = TR_null;
    creep(r).next                 = creep_ai_client(cr_ai_c).creep_head;
    creep(r).level                = 1;
    if(creep_ai_client(cr_ai_c).creep_head != TR_null)
      creep(creep_ai_client(cr_ai_c).creep_head).previous = r;
    creep_ai_client(cr_ai_c).creep_head = r;
    return r;
  }
  log("No more creep left.");
  return TR_null;
}

void CR_Free(TR_creep_ai_client_ptr cr_ai_c, TR_creep_ptr l) {
  if(creep_ai_client(cr_ai_c).creep_head == l)
    creep_ai_client(cr_ai_c).creep_head = creep(l).next;
  if(creep(l).previous != TR_null)
    creep(creep(l).previous).next = creep(l).next;
  if(creep(l).next != TR_null)
    creep(creep(l).next).previous = creep(l).previous;
  creep(l).fnext = gp_gi.free_creep_head;
  gp_gi.free_creep_head = l;
}

void CR_FreeAll(TR_creep_ai_client_ptr cr_ai_c) {
  while(creep_ai_client(cr_ai_c).creep_head != TR_null)
    CR_Free(cr_ai_c, creep_ai_client(cr_ai_c).creep_head);
  log("creeps freed.");
}

/*
 * game instance
 */

void GAME_FreeAll(TR_game_instance_t *gi) {
  log("Deleting moba instance...");
  if(gp_gi.coin_sound) {
    ga_sound_release(gp_gi.coin_sound);
    ga_sound_release(gp_gi.xp_sound);
    ga_sound_release(gp_gi.torch_sound);
    ga_sound_release(gp_gi.foot_sound);
    ga_sound_release(gp_gi.miss_sound);
    ga_sound_release(gp_gi.basic_hit_sound);
    ga_sound_release(gp_gi.dark_skill_sound);
    ga_sound_release(gp_gi.orc_smash_sound);
    ga_sound_release(gp_gi.orc_enrage_sound);
    ga_sound_release(gp_gi.orc_smash_cry_sound);
    ga_sound_release(gp_gi.explosion_fireball);
    ga_sound_release(gp_gi.explosion_knife);
    #include "gen/sound_rel.cpp"
    gp_gi.coin_sound = null;
  }
  MP_FreeAll(gi);
  MOBS_FreeAll(gi);
  PUN_FreeAll(gi);
  CAS_FreeAll(gi);
  MO_AI_CL_FreeAll(gi);
  CR_AI_CL_FreeAll(gi);
  log("Deleting moba done.");
}

void GAME_InitGame(TR_game_instance_t *gi, game_desc_t *gd) {
  gp_gi.level = 0;
  gp_gi.mobs_count = 0;
  gp_gi.choose_counter = START_BATTLE_TIME;
  gp_gi.day_night_time = 0;
  /* music */
  gp_gi.music_handle = 
    gau_create_handle_buffered_file(gi->gau_mixer, gi->gau_stream_manager, 
      "./data/Level03.ogg", "ogg", &gau_on_finish_destroy, 0, &gp_gi.music_loop);
  gau_sample_source_loop_set(gp_gi.music_loop, -1, 0);
  assert(gp_gi.music_handle);
  ga_handle_setParamf(gp_gi.music_handle, GA_HANDLE_PARAM_GAIN, 0.10);
  ga_handle_play(gp_gi.music_handle);
  /* sounds */
  gp_gi.coin_sound                  = OPEN_SOUND("./data/coin.ogg", "ogg");
  gp_gi.xp_sound                    = OPEN_SOUND("./data/levelup.wav", "wav");
  gp_gi.torch_sound                 = OPEN_SOUND("./data/campfire.ogg", "ogg");
  gp_gi.foot_sound                  = OPEN_SOUND("./data/steps.ogg", "ogg");
  gp_gi.miss_sound                  = OPEN_SOUND("./data/miss.wav", "wav");
  gp_gi.basic_hit_sound             = OPEN_SOUND("./data/basic_hit_sound.wav", "wav");
  gp_gi.dark_skill_sound            = OPEN_SOUND("./data/dark_skill.wav", "wav");
  gp_gi.orc_smash_sound             = OPEN_SOUND("./data/orc_smash.wav", "wav");
  gp_gi.orc_enrage_sound            = OPEN_SOUND("./data/orc_enrage.wav", "wav");
  gp_gi.orc_smash_cry_sound         = OPEN_SOUND("./data/orc_smash_cry.wav", "wav");
  gp_gi.explosion_fireball          = OPEN_SOUND("./data/explosion_fireball.wav", "wav");
  gp_gi.explosion_knife             = OPEN_SOUND("./data/explosion_knife.wav", "wav");
  #include "gen/sound_ini.cpp"
  /* unit dictionnary */
  #include "gen/unit_def.cpp"
  /* init stuff */
  gp_gi.wizards_taken = false;
  gp_gi.knights_taken = false;
  gp_gi.orcs_taken = false;
  MP_Init(0, null);
  MOBS_Init(0, null);
  PUN_Init(0, null);
  CAS_Init(0, null);
  MO_AI_CL_Init(0, null);
  CR_AI_CL_Init(0, null);
  CR_Init(0, null);
  /* LOAD */
  if(game_desc.continue_game)
    GAME_LoadGameDesc(gi);
  /* load map */
  gi->working_seed = gp_gi.working_seed = game_desc.seed;
  GI_InitEnvMap(gi); 
  GI_InitClearanceMapBase(gi);
  gi->ogre_scene_loader.parseDotScene("./data/map/map.scene", "", gi->ogre_scene);
  gi->ogre_scene->setAmbientLight(Ogre::ColourValue(0, 0, 0));
  gi->ogre_scene->setShadowDirLightTextureOffset(0.6);
  #if MINIMAP_MODE
  gi->ogre_scene->setShadowFarDistance(250);
  #else
  gi->ogre_scene->setShadowFarDistance(21);
  #endif
  gi->ogre_scene->setShadowTextureSize(1024);
  gi->ogre_scene->getEntity("Plane02")->setVisible(false);
  log("Map loaded.");
  /* symetry */
  GAME_CloneNode(gi, gi->ogre_scene->getRootSceneNode());
  log("Symetry done.");
  GI_InitLightMap(gi);
  log("Lightmap done");
  /* trees */
  //GAME_GenerateTrees(gi);
  //GAME_MakeTrees(gi, gi->ogre_scene->getRootSceneNode());
  //GI_DrawTreesClearanceContribution(gi, 0, 0, (MAP_SIZE_X<<POS_SHIFT)-1, (MAP_SIZE_Y<<POS_SHIFT)-1);
  /* camera */
  CAM_New(gi, game_desc.local_player_id);
  log("Camera initialized");
  /* material setting */
  Ogre::SceneManager::MovableObjectIterator iter = 
    gi->ogre_scene->getMovableObjectIterator("Entity");
  while(iter.hasMoreElements()) {
    Ogre::Entity *ent = static_cast<Ogre::Entity*>(iter.getNext());
    if(ent->getName() != "Plane02")
      ent->setQueryFlags(0);
    if(ent->getName() == "march") {
      DO_TIMES(ent->getNumSubEntities()) {
        Ogre::TextureUnitState *tex = 
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(0);
        tex->setTextureName("MarchTex");
        tex->setName("MarchTex");
        tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
      }
    }
    DO_TIMES(ent->getNumSubEntities()) {
      if(ent->getName().find("banner") == std::string::npos) {
        bool has_alpha = ent->
            getSubEntity(f)->
              getMaterial()->
                getName().find("alpha") != std::string::npos;
        ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false);
        if(ent->getCastShadows()) {
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    setShadowCasterVertexProgram("Ogre/BasicVertexPrograms/AmbientShadowCasterUnified");
          if(has_alpha)
            ent->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      setShadowCasterFragmentProgram("Ogre/BasicFragmentPrograms/ShadowCasterWithAlpha");
          else
            ent->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      setShadowCasterFragmentProgram("Ogre/BasicFragmentPrograms/ShadowCaster");
        }
        if(has_alpha) {
          /*ent->
            getSubEntity(f)->
              getMaterial()->
                setReceiveShadows(false);*/
          ent->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    setSceneBlending(Ogre::SBT_REPLACE);
          ent->
            getSubEntity(f)->
              getMaterial()->
                setDepthWriteEnabled(true);
          ent->
            getSubEntity(f)->
              getMaterial()->
                setDepthCheckEnabled(true);
          ent->
            getSubEntity(f)->
              getMaterial()->
                setTransparencyCastsShadows(false);
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    setDepthWriteEnabled(true);
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
        }
        if(has_alpha)
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTexture", false);
        else
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    setFragmentProgram("Ogre/BasicFragmentPrograms/DiffuseOneTexture", false);
        #if !MINIMAP_MODE
        if(ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 0)
          ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(0)->
                      setTextureFiltering(Ogre::TFO_NONE);
        #endif
        if(!ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState("FovMapTex")) {
          Ogre::TextureUnitState *tex = 
            ent->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      createTextureUnitState("FovMapTex");
          tex->setName("FovMapTex");
          tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        }
      }
    }
  }
  /* CADAVER */
  //int k = 0;
  //DO_TIMES(TR_unit_type_count) {
  //  int j = f;
  //  DO_TIMES(gi->unit_dic[j].mesh_count)
  //    gi->ogre_scene->createEntity("dum " + i2a(k++), gi->unit_dic[j].mesh[f] + "cadaver.mesh");
  //}
  /* CREATE PLAYERS */
  for(int j = 0; j < game_desc.team_count-1; j++)
    for(int i = 0; i < game_desc.player_count_per_team; i++) {
      int player_id = i + (j*game_desc.player_count_per_team);
      int team_id   = j;
      TR_player_ptr p   = PL_New(gi, MP_New());
      TR_player_t *pl   = &player(p);
      pl->id            = player_id;
      pl->team_id       = team_id;
      /* human */
      if(player_id == game_desc.local_player_id) {
        pl->human = true;
        if(game_desc.replay == "") {
          /* human */
          TR_client_ptr c = CL_New(gi, CS_New(gi), HUMAN_CLIENT, player_id);
          pl->cs = client(c).cs;
          client(c).offline = !game_desc.online;
        } else {
          /* replay */
          TR_client_ptr c = CL_New(gi, CS_New(gi), REPLAY_CLIENT, player_id);
          pl->cs = client(c).cs;
          client(c).offline   = true;
        }
      } else 
      if(i == game_desc.player_count_per_team-1) {
        /* mobs ai */
        TR_client_ptr c = CL_New(gi, CS_New(gi), MOBS_AI_CLIENT, player_id);
        pl->cs = client(c).cs;
        client(c).user_data = (TR_ptr_t)MO_AI_CL_New();
        client(c).offline   = true;
      } else {
        pl->human = true;
        if(game_desc.replay == "") {
          if(!game_desc.PvE
          && i < game_desc.player_count_per_team - game_desc.offline_players) {
            /* remote */
            TR_remote_cmd_store_ptr r = RS_New(gi, CS_New(gi));
            pl->cs = remote_cmd_store(r).cs;
            remote_cmd_store(r).player_id = player_id;
          } else {
            /* player ai */
            TR_client_ptr c = CL_New(gi, CS_New(gi), PLAYER_AI_CLIENT, player_id);
            pl->cs = client(c).cs;
            client(c).offline   = true;
          }
        } else {
          /* replay */
          TR_client_ptr c = CL_New(gi, CS_New(gi), REPLAY_CLIENT, player_id);
          pl->cs = client(c).cs;
          client(c).offline   = true;
        }
      }
    }
  /* creep player */
  log("Creating creep player...");
  int player_id       = CREEP_PLAYER_ID;
  int team_id         = game_desc.team_count-1;
  TR_player_ptr p    = PL_New(gi, MP_New());
  TR_player_t *pl    = &player(p);
  pl->id              = player_id;
  pl->team_id         = team_id;
  TR_client_ptr c    = CL_New(gi, CS_New(gi), CREEP_AI_CLIENT, player_id);
  pl->cs              = client(c).cs;
  client(c).user_data = (TR_ptr_t)CR_AI_CL_New();
  client(c).offline   = true;
  log("Creating creep player done.");
  /* supposed to be in gp_camera */
  gp_gi.resource_overlay   = Ogre::OverlayManager::getSingleton().create("resource overlay ");
  gp_gi.resource_overlay->setZOrder(160);
  Ogre::OverlayContainer *panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "resource panel ");
  panel->setMetricsMode(Ogre::GMM_PIXELS);
  panel->setPosition(0, 0);
  panel->setDimensions(100, 100);
  gp_gi.gold_overlay = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "gold overlay ");
  gp_gi.gold_overlay->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.gold_overlay->setLeft(gi->ogre_window->getWidth()-20*10);
  gp_gi.gold_overlay->setTop(0);
  gp_gi.gold_overlay->setCaption("Yoooopi");
  gp_gi.gold_overlay->setFontName("cuckoo");
  gp_gi.gold_overlay->setCharHeight(20);
  gp_gi.gold_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.gold_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.gold_overlay->hide();
  panel->addChild(gp_gi.gold_overlay);
  gp_gi.xp_overlay = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "xp overlay ");
  gp_gi.xp_overlay->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.xp_overlay->setLeft(gi->ogre_window->getWidth()-20*10);
  gp_gi.xp_overlay->setTop(0);
  gp_gi.xp_overlay->setCaption("Yoooopi");
  gp_gi.xp_overlay->setFontName("cuckoo");
  gp_gi.xp_overlay->setCharHeight(20);
  gp_gi.xp_overlay->setColourBottom(Ogre::ColourValue(52.f/255, 200.f/255, 52.f/255));
  gp_gi.xp_overlay->setColourTop(Ogre::ColourValue(52.f/255, 200.f/255, 52.f/255));
  gp_gi.xp_overlay->hide();
  panel->addChild(gp_gi.xp_overlay);
  gp_gi.mana_bar_back = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "manabar_back");
  gp_gi.mana_bar_back->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.mana_bar_back->setLeft(gi->ogre_window->getWidth()/2-MANA_BAR_SIZEX/2-4);
  gp_gi.mana_bar_back->setTop(gi->ogre_window->getHeight() - 100);
  gp_gi.mana_bar_back->setMaterialName("bar_back_mat");
  gp_gi.mana_bar_back->setDimensions(MANA_BAR_SIZEX+8, MANA_BAR_SIZEY+8);
  panel->addChild(gp_gi.mana_bar_back);
  gp_gi.mana_bar = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "manabar");
  gp_gi.mana_bar->hide();
  gp_gi.mana_bar_back->addChild(gp_gi.mana_bar);
  gp_gi.mana_overlay = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "mana overlay ");
  gp_gi.mana_overlay->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.mana_overlay->setLeft(MANA_BAR_SIZEX/2 - 25*2);
  gp_gi.mana_overlay->setTop(6);
  gp_gi.mana_overlay->setCaption("Yoooopi");
  gp_gi.mana_overlay->setFontName("Arial");
  gp_gi.mana_overlay->setCharHeight(14);
  gp_gi.mana_overlay->setColourBottom(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.mana_overlay->setColourTop(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.mana_overlay->hide();
  gp_gi.mana_bar->addChild(gp_gi.mana_overlay);
  gp_gi.mana_regen_overlay = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "mana regen overlay ");
  gp_gi.mana_regen_overlay->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.mana_regen_overlay->setLeft(MANA_BAR_SIZEX/2 + 25*2);
  gp_gi.mana_regen_overlay->setTop(6);
  gp_gi.mana_regen_overlay->setCaption("Yoooopi");
  gp_gi.mana_regen_overlay->setFontName("Arial");
  gp_gi.mana_regen_overlay->setCharHeight(14);
  gp_gi.mana_regen_overlay->setColourBottom(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.mana_regen_overlay->setColourTop(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.mana_regen_overlay->hide();
  gp_gi.mana_bar->addChild(gp_gi.mana_regen_overlay);
  gp_gi.resource_overlay->add2D(panel);
  gp_gi.resource_overlay->show();
  /* supposed to be in gp_camera */
  gp_gi.kill_loss_overlay   = Ogre::OverlayManager::getSingleton().create("kill loss overlay ");
  Ogre::OverlayContainer *panel2 = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "kill loss panel ");
  panel2->setMetricsMode(Ogre::GMM_PIXELS);
  panel2->setPosition(0, 0);
  panel2->setDimensions(100, 100);
  gp_gi.kill_loss_text_overlay = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "kill loss overlay ");
  gp_gi.kill_loss_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.kill_loss_text_overlay->setLeft(gi->ogre_window->getWidth()/2 - 10*10);
  gp_gi.kill_loss_text_overlay->setTop(0);
  gp_gi.kill_loss_text_overlay->setCaption("Yoooopi");
  gp_gi.kill_loss_text_overlay->setFontName("cuckoo");
  gp_gi.kill_loss_text_overlay->setCharHeight(20);
  gp_gi.kill_loss_text_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.kill_loss_text_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.kill_loss_text_overlay->hide();
  panel2->addChild(gp_gi.kill_loss_text_overlay);
  gp_gi.resource_overlay->add2D(panel2);
  gp_gi.resource_overlay->show();
  /**/
  gp_gi.info_overlay   = Ogre::OverlayManager::getSingleton().create("info overlay");
  gp_gi.info_overlay->setZOrder(210);
  gp_gi.info_panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "info panel");
  gp_gi.info_panel->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_panel->setPosition(0, 0);
  gp_gi.info_panel->setDimensions(100, 100);
  gp_gi.info_panel->setMaterialName("winback_mat");
  gp_gi.info_overlay->add2D(gp_gi.info_panel);
  Ogre::OverlayContainer *panel3 = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "info overlay panel");
  panel3->setMetricsMode(Ogre::GMM_PIXELS);
  panel3->setPosition(0, 0);
  panel3->setDimensions(100, 100);
  gp_gi.info_text_overlay = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "info text overlay");
  gp_gi.info_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_text_overlay->setLeft(0);
  gp_gi.info_text_overlay->setTop(0);
  gp_gi.info_text_overlay->setCaption("Yoooopi");
  gp_gi.info_text_overlay->setFontName("Arial");
  gp_gi.info_text_overlay->setCharHeight(20);
  gp_gi.info_text_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.info_text_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  panel3->addChild(gp_gi.info_text_overlay);
  gp_gi.info_unit_name = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "info unit name");
  gp_gi.info_unit_name->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_unit_name->setLeft(0);
  gp_gi.info_unit_name->setTop(0);
  gp_gi.info_unit_name->setCaption("Yoooopi");
  gp_gi.info_unit_name->setFontName("Arial");
  gp_gi.info_unit_name->setCharHeight(20);
  gp_gi.info_unit_name->setColourBottom(Ogre::ColourValue(52.f/255, 52.f/255, 200.f/255));
  gp_gi.info_unit_name->setColourTop(Ogre::ColourValue(52.f/255, 52.f/255, 200.f/255));
  panel3->addChild(gp_gi.info_unit_name);
  gp_gi.info_unit_carac = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "info unit carac");
  gp_gi.info_unit_carac->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_unit_carac->setLeft(0);
  gp_gi.info_unit_carac->setTop(0);
  gp_gi.info_unit_carac->setCaption("Yoooopi");
  gp_gi.info_unit_carac->setFontName("Arial");
  gp_gi.info_unit_carac->setCharHeight(20);
  gp_gi.info_unit_carac->setColourBottom(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.info_unit_carac->setColourTop(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  panel3->addChild(gp_gi.info_unit_carac);
  gp_gi.info_overlay->add2D(panel3);
  gp_gi.info_overlay->hide();
  /**/
  gp_gi.info_overlay2   = Ogre::OverlayManager::getSingleton().create("info overlay 2");
  gp_gi.info_overlay2->setZOrder(180);
  gp_gi.info_panel2 = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "info panel 2");
  gp_gi.info_panel2->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_panel2->setPosition(0, 0);
  gp_gi.info_panel2->setDimensions(100, 100);
  gp_gi.info_panel2->setMaterialName("winback_mat");
  gp_gi.info_overlay2->add2D(gp_gi.info_panel2);
  Ogre::OverlayContainer *panel4 = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "info overlay panel 2");
  panel4->setMetricsMode(Ogre::GMM_PIXELS);
  panel4->setPosition(0, 0);
  panel4->setDimensions(100, 100);
  gp_gi.info_text_overlay2 = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "info text overlay 2");
  gp_gi.info_text_overlay2->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_text_overlay2->setLeft(0);
  gp_gi.info_text_overlay2->setTop(0);
  gp_gi.info_text_overlay2->setCaption("Yoooopi");
  gp_gi.info_text_overlay2->setFontName("Arial");
  gp_gi.info_text_overlay2->setCharHeight(20);
  gp_gi.info_text_overlay2->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.info_text_overlay2->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.info_text_overlay2->hide();
  panel4->addChild(gp_gi.info_text_overlay2);
  gp_gi.info_unit_name2 = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "info unit name 2");
  gp_gi.info_unit_name2->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_unit_name2->setLeft(0);
  gp_gi.info_unit_name2->setTop(0);
  gp_gi.info_unit_name2->setCaption("Yoooopi");
  gp_gi.info_unit_name2->setFontName("Arial");
  gp_gi.info_unit_name2->setCharHeight(20);
  gp_gi.info_unit_name2->setColourBottom(Ogre::ColourValue(52.f/255, 52.f/255, 200.f/255));
  gp_gi.info_unit_name2->setColourTop(Ogre::ColourValue(52.f/255, 52.f/255, 200.f/255));
  gp_gi.info_unit_name2->hide();
  panel4->addChild(gp_gi.info_unit_name2);
  gp_gi.info_unit_carac2 = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "info unit carac 2");
  gp_gi.info_unit_carac2->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.info_unit_carac2->setLeft(0);
  gp_gi.info_unit_carac2->setTop(0);
  gp_gi.info_unit_carac2->setCaption("Yoooopi");
  gp_gi.info_unit_carac2->setFontName("Arial");
  gp_gi.info_unit_carac2->setCharHeight(20);
  gp_gi.info_unit_carac2->setColourBottom(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.info_unit_carac2->setColourTop(Ogre::ColourValue(200.f/255, 200.f/255, 200.f/255));
  gp_gi.info_unit_carac2->hide();
  panel4->addChild(gp_gi.info_unit_carac2);
  gp_gi.info_overlay2->add2D(panel4);
  gp_gi.info_overlay2->hide();
  /**/ 
  DO_TIMES(MAX_SKILLS_PER_CAMERA) {
    {gp_gi.plus_overlay[f] = Ogre::OverlayManager::getSingleton().create("plus overlay " + i2a(f));
     Ogre::OverlayContainer *panel = 
       (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "plus overlay panel" + i2a(f));
     panel->setMetricsMode(Ogre::GMM_PIXELS);
     panel->setPosition(0, 0);
     panel->setDimensions(100, 100);
     panel->setMaterialName("plus_mat");
     gp_gi.plus_overlay[f]->add2D(panel);
     Ogre::OverlayContainer *panel2 = 
       (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "plus overlay panel 2 " + i2a(f));
     panel2->setMetricsMode(Ogre::GMM_PIXELS);
     panel2->setPosition(0, 0);
     panel2->setDimensions(100, 100);
     panel2->setMaterialName("alpha_blue_mat");
     gp_gi.plus_overlay[f]->add2D(panel2);
     gp_gi.plus_overlay[f]->hide();}
    {gp_gi.skill_level_overlay[f] = Ogre::OverlayManager::getSingleton().create("skill level overlay " + i2a(f));
     Ogre::OverlayContainer *panel = 
       (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "skill level overlay panel" + i2a(f));
     assert(panel);
     panel->setMetricsMode(Ogre::GMM_PIXELS);
     panel->setPosition(0, 0);
     panel->setDimensions(100, 100);
     gp_gi.skill_level_overlay[f]->add2D(panel);
     panel->addChild((Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "skill level text " + i2a(f)));
     gp_gi.skill_level_overlay[f]->setZOrder(210);
     gp_gi.skill_level_overlay[f]->hide();}
  }
  DO_TIMES(MAX_CHOSE_OVERLAY) {
    gp_gi.chose_overlays[f] = Ogre::OverlayManager::getSingleton().create("chose " + i2a(f));
    gp_gi.chose_overlays[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "chose " + i2a(f) + " panel"));
    gp_gi.chose_overlays[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "chose " + i2a(f) + " panel2"));
    gp_gi.chose_overlays[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "chose " + i2a(f) + " panel3"));
    gp_gi.chose_overlays[f]->hide();
    gp_gi.chose_overlays[f]->setZOrder(200);
    Ogre::OverlayContainer *panel = gp_gi.chose_overlays[f]->getChild("chose " + i2a(f) + " panel");
    assert(panel);
    Ogre::TextAreaOverlayElement *class_name_text = 
      (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
        "TextArea", "chose " + i2a(f) + " text");
    class_name_text->setMetricsMode(Ogre::GMM_PIXELS);
    class_name_text->setLeft(0);
    class_name_text->setTop(0);
    class_name_text->setCaption("Yoooopi");
    class_name_text->setFontName("cuckoo");
    class_name_text->setCharHeight(30);
    class_name_text->setColourBottom(Ogre::ColourValue((float)TCOD_grey.r/255, (float)TCOD_grey.g/255, (float)TCOD_grey.b/255));
    class_name_text->setColourTop(Ogre::ColourValue((float)TCOD_grey.r/255, (float)TCOD_grey.g/255, (float)TCOD_grey.b/255));
    panel->addChild(class_name_text);
  }
  gp_gi.ok_button_overlay = Ogre::OverlayManager::getSingleton().create("chose button");
  gp_gi.ok_button_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "ok button panel"));
  gp_gi.ok_button_overlay->hide();
  gp_gi.ok_button_overlay->setZOrder(210);
  Ogre::OverlayContainer *panel5 = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "chose hero text panel");
  panel5->setMetricsMode(Ogre::GMM_PIXELS);
  panel5->setPosition(0, 0);
  panel5->setDimensions(100, 100);
  gp_gi.ok_button_overlay->add2D(panel5);
  gp_gi.choose_class_text = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "choose_class_text");
  gp_gi.choose_class_text->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.choose_class_text->setLeft(0);
  gp_gi.choose_class_text->setTop(0);
  gp_gi.choose_class_text->setCaption("Yoooopi");
  gp_gi.choose_class_text->setFontName("cuckoo");
  gp_gi.choose_class_text->setCharHeight(30);
  gp_gi.choose_class_text->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.choose_class_text->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.choose_class_text->hide();
  panel5->addChild(gp_gi.choose_class_text);
  gp_gi.choose_skills_text = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "choose_skills_text");
  gp_gi.choose_skills_text->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.choose_skills_text->setLeft(0);
  gp_gi.choose_skills_text->setTop(0);
  gp_gi.choose_skills_text->setCaption("Yoooopi");
  gp_gi.choose_skills_text->setFontName("cuckoo");
  gp_gi.choose_skills_text->setCharHeight(25);
  gp_gi.choose_skills_text->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.choose_skills_text->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  gp_gi.choose_skills_text->hide();
  panel5->addChild(gp_gi.choose_skills_text);
  gp_gi.choose_counter_text = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "choose_counter_text");
  gp_gi.choose_counter_text->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.choose_counter_text->setLeft(0);
  gp_gi.choose_counter_text->setTop(0);
  gp_gi.choose_counter_text->setCaption("Yoooopi");
  gp_gi.choose_counter_text->setFontName("cuckoo");
  gp_gi.choose_counter_text->setCharHeight(35);
  gp_gi.choose_counter_text->setColourBottom(Ogre::ColourValue((float)TCOD_darker_red.r/255, (float)TCOD_darker_red.g/255, (float)TCOD_darker_red.b/255));
  gp_gi.choose_counter_text->setColourTop(Ogre::ColourValue((float)TCOD_darker_red.r/255, (float)TCOD_darker_red.g/255, (float)TCOD_darker_red.b/255));
  gp_gi.choose_counter_text->hide();
  panel5->addChild(gp_gi.choose_counter_text);
  gp_gi.chose_background_overlay = Ogre::OverlayManager::getSingleton().create("chose background");
  gp_gi.chose_background_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "chose background panel"));
  gp_gi.chose_background_overlay->show();
  gp_gi.chose_background_overlay->setZOrder(190);
  Ogre::PanelOverlayElement *background = 
        (Ogre::PanelOverlayElement*)gp_gi.chose_background_overlay->
          getChild("chose background panel");
  assert(background);
  background->setMetricsMode(Ogre::GMM_PIXELS);
  background->setPosition(0, 0);
  background->setDimensions(gi->ogre_window->getWidth(), gi->ogre_window->getHeight());
  background->setUV(0, 0, (float)gi->ogre_window->getWidth()/48, (float)gi->ogre_window->getHeight()/48);
  background->setMaterialName("background1_mat");
  gp_gi.assign_level_button = Ogre::OverlayManager::getSingleton().create("assign level button");
  gp_gi.assign_level_button->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "assign_level panel"));
  Ogre::TextAreaOverlayElement *level_to_assign_text = 
    (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
      "TextArea", "level_to_assign_text");
  level_to_assign_text->setMetricsMode(Ogre::GMM_PIXELS);
  level_to_assign_text->setLeft(0);
  level_to_assign_text->setTop(0);
  level_to_assign_text->setCaption("Yoooopi");
  level_to_assign_text->setFontName("Arial");
  level_to_assign_text->setCharHeight(25);
  level_to_assign_text->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  level_to_assign_text->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
  level_to_assign_text->show();
  gp_gi.assign_level_button->getChild("assign_level panel")->addChild(level_to_assign_text);
  gp_gi.assign_level_button->hide();
  gp_gi.assign_level_button->setZOrder(180);
  Ogre::OverlayContainer *panel6 = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "assign_level text panel");
  panel6->setMetricsMode(Ogre::GMM_PIXELS);
  panel6->setPosition(0, 0);
  panel6->setDimensions(100, 100);
  gp_gi.assign_level_button->add2D(panel6);
  /**/
  {gp_gi.hit_blood_overlay = Ogre::OverlayManager::getSingleton().create("hit blood overlay ");
   Ogre::OverlayContainer *panel = 
     (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "hit blood overlay panel");
   panel->setMetricsMode(Ogre::GMM_RELATIVE);
   panel->setPosition(0, 0);
   panel->setDimensions(1, 1);
   panel->setMaterialName("hit_blood_mat");
   panel->
    getMaterial()->
      getTechnique(0)->
        getPass(0)->
          setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
   gp_gi.hit_blood_overlay->add2D(panel);
   gp_gi.hit_blood_overlay->hide();}
  /* callbacks */
  gi->free_all_cb                   = GAME_FreeAll;
  gi->on_death_cb                   = GAME_OnDeath;
  gi->on_update_unit_cb             = GAME_OnUpdateUnit;
  gi->update_client_cb              = GAME_UpdateClient;
  gi->update_player_cb              = GAME_UpdatePlayer;
  gi->get_skill_name_cb             = GAME_GetSkillName;
  gi->get_skill_anim_name_cb        = GAME_GetSkillAnimName;
  gi->get_skill_anim_time_cb        = GAME_GetSkillAnimTime;
  gi->default_skill_type_cb         = GAME_DefaultSkillType;
  gi->get_skill_shortcut_cb         = GAME_GetSkillShortcut;
  gi->get_skill_info_cb             = GAME_GetSkillInfo;
  gi->get_skill_range_cb            = GAME_GetSkillRange;
  gi->get_skill_max_level_cb        = GAME_GetSkillMaxLevel;
  gi->get_skill_every_level_cb      = GAME_GetSkillEveryLevel;
  gi->get_skill_cooldown_cb         = GAME_GetSkillCoolDown;
  gi->get_skill_mana_cost_cb        = GAME_GetSkillManaCost;
  gi->get_skill_material_cb         = GAME_GetSkillMaterial;
  gi->get_skill_cast_speed_cb       = GAME_GetSkillCastSpeed;
  gi->get_skill_direct_cast_cb      = GAME_GetSkillDirectCast;
  gi->get_skill_invoke_type_cb      = GAME_GetSkillInvokeType;
  gi->get_skill_invoke_count_cb     = GAME_GetSkillInvokeCount;
  gi->get_skill_can_stop_cb         = GAME_GetSkillCanStop;
  gi->get_skill_continue_cb         = GAME_GetSkillContinue;
  gi->get_skill_is_blocking_cb      = GAME_GetSkillIsBlocking;
  gi->get_skill_is_available_cb     = GAME_GetSkillIsAvailable;
  gi->get_skill_is_passive_cb       = GAME_GetSkillIsPassive;
  gi->get_skill_is_unique_cb        = GAME_GetSkillIsUnique;
  gi->skill_look_for_air_cb         = GAME_GetSkillLookForAir;
  gi->skill_look_for_ground_cb      = GAME_GetSkillLookForGround;
  gi->skill_cast_one_by_one_cb      = GAME_GetSkillCastOneByOne;
  gi->can_cast_skill_cb             = GAME_CanCastSkill;
  gi->skill_looks_for_unit_cb       = GAME_SkillLooksForUnit;
  gi->skill_looks_for_enemy_cb      = GAME_SkillLooksForEnemy;
  gi->skill_looks_for_target_cb     = GAME_SkillLooksForTarget;
  gi->cast_skill_cb                 = GAME_CastSkill;
  gi->hud_info_cb                   = GAME_HUDInfo;
  gi->on_hit_unit_cb                = GAME_OnHitUnit;
  gi->on_update_unit_render_cb      = GAME_UpdateUnitRender;
  gi->on_spawn_unit_cb              = GAME_OnSpawnUnit;
  gi->on_cancel_build_cb            = GAME_OnCancelBuild;
  gi->on_generic_cmd_cb             = GAME_GenericCmd;
  gi->on_unit_order_cb              = GAME_OnUnitOrder;
  gi->draw_unit_goal_cb             = GAME_DrawUnitGoal;
  gi->get_cursor_cb                 = GAME_GetCursor;
  gi->on_select_unit_sound_cb       = GAME_OnSelectUnitSound;
  gi->on_move_unit_sound_cb         = GAME_OnMoveUnitSound;
  gi->on_skill_sound_cb             = GAME_OnSkillSound;
  gi->get_unit_in_range_cb          = GAME_GetUnitInRange;
  gi->get_selection_skills_cb       = GAME_GetSelectionSkills;
  log("Moba initialization done");
}

void GAME_CloneNode(TR_game_instance_t *gi, Ogre::SceneNode *node) {
  list<Ogre::SceneNode*> nodes, to_remove, to_clone;
  Ogre::SceneNode::ChildNodeIterator node_iter = node->getChildIterator();
  while(node_iter.hasMoreElements()) {
    Ogre::SceneNode *node = static_cast<Ogre::SceneNode*>(node_iter.getNext());
    nodes.push_back(node);
    if(ABS(node->_getDerivedPosition().x+(MAP_SIZE_Y-node->_getDerivedPosition().z)) < 128)
      to_clone.push_back(node);
    else
    if(node->getName().find("Plane") == std::string::npos
    && node->getName().find("march") == std::string::npos
    && node->getName().find("banner") == std::string::npos)
      to_remove.push_back(node);
  }
  for(list<Ogre::SceneNode*>::iterator it = nodes.begin(); it != nodes.end(); it++)
    GAME_CloneNode(gi, *it);
  for(list<Ogre::SceneNode*>::iterator it = to_remove.begin(); it != to_remove.end(); it++) {
    node->removeChild(*it);
    gi->ogre_scene->destroySceneNode(*it);
  }
  for(list<Ogre::SceneNode*>::iterator it = to_clone.begin(); it != to_clone.end(); it++) {
    Ogre::SceneNode *node = *it;
    if(node->numAttachedObjects() > 0) {
      Ogre::SceneNode *clone = gi->ogre_scene->getRootSceneNode()->createChildSceneNode(node->getName() + " sym");
      Ogre::Vector3 pos = node->_getDerivedPosition();
      Ogre::Quaternion ori = node->_getDerivedOrientation();
      //clone->setPosition(Ogre::Vector3(64-(pos.x-64), pos.y, 64-(pos.z-64)));
      clone->setPosition(Ogre::Vector3(MAP_SIZE_X-pos.x, pos.y, MAP_SIZE_Y-pos.z));
      //clone->setOrientation(Ogre::Quaternion(/*Ogre::Radian(3.14/2)-*/Ogre::Radian(3.14)+ori.getYaw(false), Ogre::Vector3(0, 1, 0)));
      clone->setScale(node->_getDerivedScale());
      if(node->getName().find("torchalpha") != std::string::npos) {
        {Ogre::Vector3 pos = node->getPosition();
         node->setPosition(Ogre::Vector3(floor(pos.x)+0.025, pos.y, floor(pos.z)+0.65f));}
        {Ogre::Vector3 pos = clone->_getDerivedPosition();
         clone->setPosition(Ogre::Vector3(floor(pos.x)+0.5f, pos.y, floor(pos.z)+0.425f));}
      } else 
      if(node->getName().find("flip") != std::string::npos)
        clone->setOrientation(Ogre::Quaternion(/*Ogre::Radian(3.14/2)-*/Ogre::Radian(3.14)+ori.getYaw(false), Ogre::Vector3(0, 1, 0)));
      DO_TIMES(node->numAttachedObjects()) {
        Ogre::MovableObject *obj = node->getAttachedObject(f);
        if(obj->getMovableType() == "Entity") {
          Ogre::Entity *ent = (Ogre::Entity*)obj;
          if(ent->getName().find("Plane") == std::string::npos
          && ent->getName().find("march") == std::string::npos
          && ent->getName().find("banner") == std::string::npos)
            clone->attachObject(ent->clone(ent->getName() + " sym"));
        } else
        if(obj->getMovableType() == "Light") {
          Ogre::Light *light = (Ogre::Light*)obj,
                      *new_light;
          if(light->getName().find("Direct") == std::string::npos) {
            switch(light->getType()) {
              case Ogre::Light::LT_POINT:
                //new_light = gi->ogre_scene->createLight("pointLight");
                //new_light->setType(Ogre::Light::LT_POINT);
                //new_light->setPosition(light->getPosition());
                //new_light->setDirection(light->getDirection());
                //new_light->setDiffuseColour(light->getDiffuseColour());
                //clone->attachObject(new_light);
                {
                  TR_light_ptr l = LI_New(gi);
                  if(l != TR_null) {
                    TR_light_t *li = &light(l);
                    li->x = pos.x;
                    li->y = pos.z;
                    li->c = TCOD_color_RGB((float)light->getDiffuseColour().r*255, (float)light->getDiffuseColour().g*255, (float)light->getDiffuseColour().b*255);
                    li->r = light->getAttenuationRange()*(1<<POS_SHIFT);
                    li->i = 0.20;//light->getPowerScale();
                  }
                }
                if(light->getName().find("middle") == std::string::npos)
                {
                  TR_light_ptr l = LI_New(gi);
                  if(l != TR_null) {
                    TR_light_t *li = &light(l);
                    li->x = MAP_SIZE_X-pos.x;
                    li->y = MAP_SIZE_Y-pos.z;
                    li->c = TCOD_color_RGB((float)light->getDiffuseColour().r*255, (float)light->getDiffuseColour().g*255, (float)light->getDiffuseColour().b*255);
                    li->r = (float)light->getAttenuationRange()*(1<<POS_SHIFT);
                    li->i = 0.20;//light->getPowerScale();
                  }
                }
                break;
            }
          }
        }
      }
    }
  }
}

void GAME_GenerateTrees(TR_game_instance_t *gi) {
  #define TREES_COUNT MAX_TREE
  #define TREE_MAP_SIZE 64
  TCOD_image_t walls = TCOD_image_load("./data/map walls.png");
  static bool tree_map[TREE_MAP_SIZE*TREE_MAP_SIZE];
  unsigned int seed = game_desc.seed;
  DO_TIMES(TREE_MAP_SIZE*TREE_MAP_SIZE)
    tree_map[f] = false;
  TCOD_random_t randm = TCOD_random_new_from_seed(TCOD_RNG_CMWC, game_desc.seed);
  DO_TIMES(TREES_COUNT) {
    int x = TCOD_random_get_int(randm, 0, MAP_SIZE_X<<POS_SHIFT),
        y = TCOD_random_get_int(randm, 0, MAP_SIZE_Y<<POS_SHIFT);
    TCOD_color_t c = TCOD_image_get_pixel(walls, x>>POS_SHIFT, y>>POS_SHIFT);
    if(IS_COLOR(255, 255, 255)
    && !AT2(tree_map, x>>(POS_SHIFT+1), y>>(POS_SHIFT+1))) {
      AT2(tree_map, x>>(POS_SHIFT+1), y>>(POS_SHIFT+1)) = true;
      TR_tree_ptr t = TR_New(gi);
      assert(t != TR_null);
      TR_tree_t *tree = &tree(t);
      tree->x = x;
      tree->y = y;
      tree->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("tree " + i2a(t));
      tree->node->setPosition(Ogre::Vector3((float)tree->x/(1<<POS_SHIFT), 0, (float)tree->y/(1<<POS_SHIFT)));
      tree->node->setOrientation(Ogre::Quaternion(Ogre::Radian((float)3.14*2*(TR_Rand(&seed)%1000)/1000), Ogre::Vector3(0, 1, 0)) * Ogre::Quaternion(Ogre::Radian(3.14/2), Ogre::Vector3(1, 0, 0)));
      tree->node->setScale(Ogre::Vector3(0.40, 0.40, 0.40));
      int k = (TR_Rand(&seed)%6)+5;
      str n = i2a(k);
      if(k < 10)
        n = "0" + n;
      switch(k) {
        default:
          tree->size = 1;
      }
      tree->ent[0] = gi->ogre_scene->createEntity("tree ent " + i2a(t) + " trunk", "tree_trunk_" + n + ".mesh");
      tree->ent[1] = gi->ogre_scene->createEntity("tree ent " + i2a(t) + " leaf", "tree_" + n + "_leaf" + ".mesh");
      DO_TIMES(2) {
        tree->ent[f]->setCastShadows(false);
        tree->ent[f]->setUserAny(Ogre::Any(t));
        tree->ent[f]->setQueryFlags(0);
        tree->node->attachObject(tree->ent[f]);
      }
    }
  }
  log("Trees generated");
}

void GAME_MakeTrees(TR_game_instance_t *gi, Ogre::SceneNode *node) {
  list<Ogre::SceneNode*> nodes, trees;
  Ogre::SceneNode::ChildNodeIterator node_iter = node->getChildIterator();
  while(node_iter.hasMoreElements()) {
    Ogre::SceneNode *node = static_cast<Ogre::SceneNode*>(node_iter.getNext());
    nodes.push_back(node);
    if(node->getName().find("trunk") != std::string::npos
    && node->getName().find("04") == std::string::npos)
      trees.push_back(node);
  }
  for(list<Ogre::SceneNode*>::iterator it = nodes.begin(); it != nodes.end(); it++)
    GAME_MakeTrees(gi, *it);
  for(list<Ogre::SceneNode*>::iterator it = trees.begin(); it != trees.end(); it++) {
    Ogre::SceneNode *node = *it;
    TR_tree_ptr t = TR_New(gi);
    assert(t != TR_null);
    TR_tree_t *tree = &tree(t);
    Ogre::Vector3 pos = node->_getDerivedPosition();
    tree->x = (float)pos.x*(1<<POS_SHIFT);
    tree->y = (float)pos.z*(1<<POS_SHIFT);
    tree->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("tree " + i2a(t));
    tree->node->setPosition(node->_getDerivedPosition());
    tree->node->setOrientation(node->_getDerivedOrientation());
    tree->node->setScale(Ogre::Vector3(0.25, 0.25, 0.25));
    str n = "";
    n.append(1, node->getName()[11]);
    n.append(1, node->getName()[12]);
    int k = atoi(n.c_str());
    switch(k) {
      default:
        tree->size = 1;
    }
    tree->ent[0] = gi->ogre_scene->createEntity("tree ent " + i2a(t) + " trunk", "tree_trunk_" + n + ".mesh");
    tree->ent[1] = gi->ogre_scene->createEntity("tree ent " + i2a(t) + " leaf", "tree_" + n + "_leaf" + ".mesh");
    DO_TIMES(2) {
      tree->ent[f]->setCastShadows(false);
      tree->ent[f]->setUserAny(Ogre::Any(t));
      tree->ent[f]->setQueryFlags(0);
      tree->node->attachObject(tree->ent[f]);
    }
    node->removeChild(*it);
  }
}

void GAME_StartGame(TR_game_instance_t *gi) {
  /* INIT CLIENTS */
  TR_client_ptr c = gi->client_head;
  while(c != TR_null) {
    TR_client_t *cl = &client(c);
    #if !MINIMAP_MODE
    if(cl->type == MOBS_AI_CLIENT) {
      GAME_StartMobsAIClient(gi, c);
    } else 
    if(cl->type == CREEP_AI_CLIENT) {
      GAME_StartCreepAIClient(gi, c);
    } else
    if(cl->type == PLAYER_AI_CLIENT) {
      GAME_StartPlayerClient(gi, c);
    } else
    if(cl->type == HUMAN_CLIENT) {
      GAME_StartPlayerClient(gi, c);
      GAME_StartHumanClient(gi, c);
    }
    #endif
    c = cl->next;
  }
}

void GAME_StartMobsAIClient(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl  = &client(c);
  TR_id_t team_id  = TEAM_ID(cl->player_id);
  CL_CmdSpawnUnit(
    gi, 
    c, 
    TR_null,
    (team_id == 0 ? 20 : 128-2-20)<<POS_SHIFT,
    (team_id == 0 ? 128-1-20 : 20-1)<<POS_SHIFT,
    TR_unit_type_inhibitor,
    USER_DATA_MOBS);
  CL_CmdSpawnUnit(
    gi, 
    c, 
    TR_null,
    (team_id == 0 ? 35 : 128-2-35)<<POS_SHIFT,
    (team_id == 0 ? 128-1-35 : 35-1)<<POS_SHIFT,
    team_id == 0 ? TR_unit_type_tower : TR_unit_type_tower,
    USER_DATA_MOBS);
  CL_CmdSpawnUnit(
    gi, 
    c, 
    TR_null,
    (team_id == 0 ? 35 : 128-2-35)<<POS_SHIFT,
    (team_id == 0 ? 128-1-15 : 15-1)<<POS_SHIFT,
    team_id == 0 ? TR_unit_type_tower : TR_unit_type_tower,
    USER_DATA_MOBS);
  CL_CmdSpawnUnit(
    gi, 
    c, 
    TR_null,
    (team_id == 0 ? 15 : 128-2-15)<<POS_SHIFT,
    (team_id == 0 ? 128-1-35 : 35-1)<<POS_SHIFT,
    team_id == 0 ? TR_unit_type_tower : TR_unit_type_tower,
    USER_DATA_MOBS);
}

void GAME_StartCreepAIClient(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl                 = &client(c);
  TR_player_ptr p                 = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl                 = &player(p);
  TR_creep_ai_client_ptr cr_ai_c  = (TR_creep_ai_client_ptr)cl->user_data;
  TR_creep_ai_client_t *creep_cl  = &creep_ai_client(cr_ai_c);
  creep_cl->generated_creep_count = 0;
  /* EPIC CENTER */
  //do {
  //  creep_cl->epic_center_x = TR_Rand(&gp_gi.working_seed)%MAP_SIZE_X;
  //  creep_cl->epic_center_y = TR_Rand(&gp_gi.working_seed)%MAP_SIZE_Y;
  //} while(creep_cl->epic_center_x + (MAP_SIZE_Y-creep_cl->epic_center_y) < 160);
  creep_cl->epic_center_x = 100;
  creep_cl->epic_center_y = 20;
  log("Epic center: " + i2a(creep_cl->epic_center_x) + ", " + i2a(creep_cl->epic_center_y));
  /* TRAVERSE BSP */
  //TCOD_bsp_traverse_inverted_level_order(gp_gi.bsp, GAME_GenerateCreep, (void*)cr_ai_c);
  /* SYMETRY */
  if(!game_desc.PvE) {
    TR_creep_ptr cr = creep_cl->creep_head;
    while(cr != TR_null) {
      TR_creep_ptr cr2          = CR_New(cr_ai_c);
      if(cr2 != TR_null) {
        creep(cr2).id            = 0;
        creep(cr2).dead          = true;
        creep(cr2).respawn_span  = 0;  
        creep(cr2).x             = creep(cr).y;
        creep(cr2).y             = creep(cr).x;
        creep(cr2).unit_type     = creep(cr).unit_type;
        creep_cl->generated_creep_count++;
      }
      cr = creep(cr).next;
    }
  }
  log("" + i2a(creep_cl->generated_creep_count) + " creeps.");
}

unsigned char GAME_GenerateCreep(TCOD_bsp_t *node, void *user_data) {
  TR_creep_ai_client_ptr cr_ai_c  = (TR_creep_ai_client_ptr)user_data;
  TR_creep_ai_client_t *creep_cl  = &creep_ai_client(cr_ai_c);
  if(TCOD_bsp_is_leaf(node)) {
    int w    = node->w*MAP_SIZE_X / MAP_GENERATOR_SIZE_X,
        h    = node->h*MAP_SIZE_Y / MAP_GENERATOR_SIZE_Y,
        minx = (node->x+1)*MAP_SIZE_X / MAP_GENERATOR_SIZE_X,
        maxx = (node->x+node->w-1)*MAP_SIZE_X / MAP_GENERATOR_SIZE_X,
        miny = (node->y+1)*MAP_SIZE_Y / MAP_GENERATOR_SIZE_Y,
        maxy = (node->y+node->h-1)*MAP_SIZE_Y / MAP_GENERATOR_SIZE_Y,
        i, j;
    i = minx + w/2;
    j = miny + h/2;
    int dist = TR_ISqrt(i*i + (MAP_SIZE_Y-j)*(MAP_SIZE_Y-j));
    if((game_desc.PvE
    || i+(MAP_SIZE_Y-j) < 128)
    && dist > 30) {
      int diff_dist = TR_ISqrt((i-creep_cl->epic_center_x)*(i-creep_cl->epic_center_x) 
                              + (j-creep_cl->epic_center_y)*(j-creep_cl->epic_center_y)),
          max_dist  = 1414 * MAP_SIZE_X / 1000,
          sx = i + RAND(20)-10,
          sy = j + RAND(20)-10;
      int difficulty = MAX(0, max_dist - diff_dist);
      #include "generate_creep.cpp"
    }
  }
  return 1;//creep_cl->generated_creep_count < 50;
}

void GAME_CreepAt(TR_creep_ai_client_ptr cr_ai_c, int i, int j, TR_unit_type_t unit_type, int count, int level) {
  TR_creep_ai_client_t *creep_cl  = &creep_ai_client(cr_ai_c);
  DO_TIMES(count) {
    TR_creep_ptr cr          = CR_New(cr_ai_c);
    if(cr != TR_null) {
      creep(cr).id            = 0;
      creep(cr).dead          = true;
      creep(cr).respawn_span  = 0;
      creep(cr).x             = (i<<POS_SHIFT) + TR_Rand(&gp_gi.working_seed)%200;
      creep(cr).y             = (j<<POS_SHIFT) + TR_Rand(&gp_gi.working_seed)%200;
      creep(cr).unit_type     = unit_type;
      creep(cr).level         = level;
      creep_cl->generated_creep_count++;
    }
  }
}

void GAME_StartPlayerClient(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl  = &client(c);
  TR_id_t team_id  = TEAM_ID(cl->player_id);
  TR_player_ptr p  = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl  = &player(p);
  int id = pl->id%game_desc.player_count_per_team,
      pos_x, pos_y;
  switch(id) {
    case 0:
      pos_x = 10;
      pos_y = 10;
      break;
    case 1:
      pos_x = 20-1;
      pos_y = 10;
      break;
    case 2:
      pos_x = 10;
      pos_y = 20;
      break;
  }
  CL_CmdSpawnUnit(
    gi, 
    c, 
    TR_null,
    (team_id == 0 ? pos_x : 128-2-pos_x)<<POS_SHIFT,
    (team_id == 0 ? 128-1-pos_y : pos_y-1)<<POS_SHIFT,
    team_id == 0 ? TR_unit_type_human_cauldron : TR_unit_type_human_cauldron,
    USER_DATA_PLAYER_UNIT);
}

void GAME_StartHumanClient(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl                 = &client(c);
  TR_id_t team_id                 = TEAM_ID(cl->player_id);
  TR_player_ptr p                 = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl                 = &player(p);
  TR_gp_player_ptr mp           = pl->user_data;
  TR_gp_player_t   *mpl         = &gp_player(mp);
  int id = pl->id%game_desc.player_count_per_team,
      pos_x, pos_y;
  /* camera */
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  switch(id) {
    case 0:
      pos_x = 10;
      pos_y = 10;
      break;
    case 1:
      pos_x = 20-1;
      pos_y = 10;
      break;
    case 2:
      pos_x = 10;
      pos_y = 20;
      break;
  }
  CAM_LookAt(
    gi, ca,
    (team_id == 0 ? pos_x : 128-1-pos_x)<<POS_SHIFT,
    (team_id == 0 ? 128-1-pos_y : pos_y)<<POS_SHIFT);
  BOUND_VIEWPORT
  /* LEVEL TO ASSIGN */
  mpl->level_to_assign = 1;
}

void GAME_UpdateMobsAI(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl = &player(p);
  TR_mobs_ai_client_ptr mo_ai_c = (TR_mobs_ai_client_ptr)cl->user_data;
  TR_mobs_ai_client_t *mobs_cl = &mobs_ai_client(mo_ai_c);
  TR_id_t team_id = TEAM_ID(cl->player_id);
  /* MOBS SPAWN */
  gp_gi.day_night_time += gi->game_time_step/2;
  if(gp_gi.day_night_time > 2*60000+30000)
    gp_gi.day_night_time = -1*40000;
  mobs_cl->mobs_spawn_timer -= gi->game_time_step;
  if(mobs_cl->mobs_spawn_timer <= 0
  /*&& gp_gi.day_night_time > 0*/) {
    mobs_cl->mobs_spawn_timer = 30000;
    /*if(gp_gi.mobs_count <= 60)*/ {
      #if !MINIMAP_MODE
      DO_TIMES(3) {
        DO_TIMES(25) {
          CL_CmdSpawnUnit(
            gi, 
            c, 
            TR_null,
            (team_id == 0 ? 18+2 : 128-1-18-2)<<POS_SHIFT,
            (team_id == 0 ? 128-1-18-2 : 18+2)<<POS_SHIFT,
            team_id == 0 ? TR_unit_type_crypt_cow : TR_unit_type_crypt_cow,
            USER_DATA_MOBS);
        }
      }
      #endif
    }
  }
}

void GAME_UpdateCreepAI(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl                 = &client(c);
  TR_player_ptr p                 = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl                 = &player(p);
  TR_creep_ai_client_ptr cr_ai_c  = (TR_creep_ai_client_ptr)cl->user_data;
  TR_creep_ai_client_t *creep_cl  = &creep_ai_client(cr_ai_c);
  TR_creep_ptr cr                 = creep_cl->creep_head;
  TR_id_t team_id                 = TEAM_ID(cl->player_id);
  bool end                         = true;
  while(cr != TR_null) {
    TR_creep_t *cree = &creep(cr);
    if(cree->dead) {
      cree->respawn_span -= gi->game_time_step;
      if(cree->respawn_span <= 0) {
        end = false;
        cree->dead = false;
        switch(cree->id) {
          case 0:
            CL_CmdSpawnUnit(
              gi, 
              c, 
              TR_null,
              cree->x,
              cree->y,
              cree->unit_type,
              USER_DATA_CREEP,
              (TR_ptr_t)cr);
            break;
          default:
            assert(0);
        }
      }
    } else
      end = false;
    cr = cree->next;
  }
  /*if(game_desc.PvE
  && end) {
    TR_camera_ptr ca = GI_GetCamera(gi);
    TR_camera_t *cam = &camera(ca);
    TR_player_t *pl = &player(GI_GetPlayerPtr(gi, cam->player_id));
    pl->win = true;
    creep_cl->end_time -= gi->game_time_step;
    if(creep_cl->end_time <= 0) {
      GAME_SaveGame(gi, gp_gi.level+1);
      TR_Quit(999);
    }
  }*/
}

void GAME_UpdatePlayer(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  TR_player_t *pl = &player(p);
  TR_id_t team_id = pl->team_id;
  TR_gp_player_ptr mp = player(p).user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  int id = pl->id%game_desc.player_count_per_team;
  /* LEVEL TO ASSIGN */
  while(mpl->xp >= 120*mpl->level) {
    mpl->xp -= 120*mpl->level;
    mpl->level_to_assign++;
    mpl->level++;
    if(pl->id == cam->player_id)
      WISE_PLAY_SOUND(gp_gi.xp_sound, "level up", -1, -1, 3000)
  }
  /* MANA */
  mpl->mana_regen = mpl->level*MPL_MANA_REGEN_BASE;
  mpl->mana_regen_span += gi->game_time_step;
  while(mpl->mana_regen_span > 1000/20) {
    mpl->mana_regen_span -= 1000/20;
    mpl->mana += mpl->mana_regen/20;
    if(mpl->mana > MPL_MAX_MANA) {
      mpl->mana = MPL_MAX_MANA;
      mpl->mana_regen_span = 0;
    }
  }
}

void GAME_UpdateClient(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl = &player(p);
  TR_gp_player_ptr mp = player(p).user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  if(gi->game_time_step != 0
  && !pl->lose
  && !pl->win) {
    /* music */
    //#if RELEASE
    ga_handle_setParamf(gp_gi.music_handle, GA_HANDLE_PARAM_GAIN, 0.10);
    //#endif
    if(cl->type == MOBS_AI_CLIENT)
      GAME_UpdateMobsAI(gi, c);
    else
    if(cl->type == CREEP_AI_CLIENT)
      GAME_UpdateCreepAI(gi, c);
    else
    if(cl->type == PLAYER_AI_CLIENT) {
    } else
    if(cl->type == HUMAN_CLIENT) {
    }
  }
}

void GAME_MobsOrderCompletedCB(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_order_ptr o) {
}

void GAME_MobsOrderFailedCB(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_order_ptr o) {
}

void GAME_OnUpdateUnit(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_player_t *pl = &player(p);
  if(un->user_data_type == USER_DATA_PLAYER_UNIT) {
    assert(un->user_data != TR_null);
    TR_player_unit_ptr pu = un->user_data;
    TR_player_unit_t *pun = &player_unit(pu);
    switch(un->unit_type) {
    }
  }
}

bool GAME_OnDeath(TR_game_instance_t *gi, TR_player_ptr killer_p, TR_player_ptr unit_p, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_player_t *pl = &player(unit_p);
  TR_gp_player_ptr killer_mp = player(killer_p).user_data;
  TR_gp_player_t *killer_mpl = &gp_player(killer_mp);
  TR_gp_player_ptr mp = player(unit_p).user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  TR_camera_t *cam = &camera(GI_GetCamera(gi));
  assert(mp != TR_null);
  bool with_cadaver = true;
  /* general death */
  GAME_GeneralDeath(gi, killer_p, unit_p, u);
  /* miscs */
  switch(un->unit_type) {
    case TR_unit_type_human_cauldron: {
        TR_build_explosion_ptr b = BE_New(gi);;
        TR_build_explosion_t *be = &build_explosion(b);
        be->x = un->x + UN_GetSizeShift(gi, SIZE(un));
        be->y = un->y + UN_GetSizeShift(gi, SIZE(un));
        be->mesh_count = MESH_COUNT(un);
        DO_TIMES(be->mesh_count)
          be->ent_name[f] = MESH(un)[f];
        be->size  = SIZE(un);
        be->model_scale = MODEL_SCALE(un);
        TR_player_ptr p2 = gi->player_head;
        while(p2 != TR_null) {
          TR_player_t *pl2 = &player(p2);
          if(pl2->team_id == pl->team_id)
            pl2->lose = true;
          else
            pl2->win  = true;
          p2 = pl2->next;
        }
        if(unit(u).user_data != TR_null)
          CAS_Free((TR_castle_ptr)unit(u).user_data);
        /* dead sound */
        /*DO_TIMES(3) {
          PLAY_SOUND_I(gp_gi.dead_tower, rand()%SOUND_VARIATION_COUNT, "dead_tower");
          TR_sound_t *so = &sound(s);
          so->x = un->x;
          so->y = un->y;
        }*/
      } break;
    default: {
        switch(un->unit_type) {
          #include "gen/play_dead_sound.cpp"
        }
        switch(un->user_data_type) {
          case USER_DATA_PLAYER_UNIT: {
          } break;
          case USER_DATA_MOBS: {
            gp_gi.mobs_count--;
          } break;
          case USER_DATA_CREEP: {
          } break;
        }
        /* RESURECT */
        if(HERO(un)) {
          DO_TIMES(un->skill_count)
            if(gi->get_skill_is_unique_cb(gi, un->skills[f]))
              mpl->skills_cooldown[un->skills[f]] = un->skills_cooldown[f];
          if(HERO(un)) {
            mpl->skill_count = un->skill_count;
            DO_TIMES(un->skill_count)
              mpl->skills[f] = un->skills[f];
          }
          TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
          if(client(c).type == HUMAN_CLIENT) {
            TR_unit_ptr c_u = GAME_GetCastle(gi, pl->id);
            assert(c_u != TR_null);
            TR_castle_ptr C = unit(c_u).user_data;
            if(C != TR_null) {
              CL_CmdBuildOrder(
                gi, 
                c, 
                unit(castle(C).u).cmd_id, 
                unit(castle(C).u).player_id,
                un->player_id,
                un->unit_type, 
                USER_DATA_PLAYER_UNIT,
                (pl->team_id == 0 ? 22 : 128-1-22)<<POS_SHIFT,
                (pl->team_id == 0 ? 128-1-22 : 22)<<POS_SHIFT,
                true);
            }
          }
        }
        switch(un->unit_type) {
          case TR_unit_type_magma_golem_of_lacdragrath: {
            {START_PARTICLE("orc_death_woosh2", UNIT_X(un), UNIT_Y(un), 0.2, 0.05f)}
            {START_PARTICLE("house_explosion", UNIT_X(un), UNIT_Y(un), 1, 0.06)}
          } break;
          /* RED BLOOD */
          case TR_unit_type_moon_gnome:
          case TR_unit_type_guard_of_the_misty_keep:
          case TR_unit_type_cursed_chest:
          case TR_unit_type_raven_of_krul:
          case TR_unit_type_beholder_of_doom:
          case TR_unit_type_savage_of_ridgar:
          case TR_unit_type_thief_of_tavratt:
          case TR_unit_type_warrior_of_mushof:
          case TR_unit_type_woodsman_of_vale:
          case TR_unit_type_mask_of_vaaror:
          case TR_unit_type_archer_of_thelmeril:
          case TR_unit_type_skeleton_mage: {
            {START_PARTICLE("unit_blood", UNIT_X(un), UNIT_Y(un), 0, 0.05f*SIZE(un))
             psys->ogre_particle->setScaleTime(0.75);}
            {START_PARTICLE("unit_blood2", UNIT_X(un), UNIT_Y(un), 0, 0.0065f*SIZE(un))
             /*psys->ogre_particle->setScaleTime(0.75);*/}
          } break;
          case TR_unit_type_crypt_cow: {
            {START_PARTICLE("unit_blood", UNIT_X(un), UNIT_Y(un), 0, 0.05f*SIZE(un))
             psys->ogre_particle->setScaleTime(0.75);}
            {START_PARTICLE("cow_blood", UNIT_X(un), UNIT_Y(un), 0, 0.0065f*SIZE(un))
             /*psys->ogre_particle->setScaleTime(0.75);*/}
          } break;
          /* GREEN BLOOD */
          case TR_unit_type_vorguul:
          case TR_unit_type_dark_bat_of_ida:
          case TR_unit_type_widow_of_krulah:
          case TR_unit_type_archer_of_yssath: {
            {START_PARTICLE("green_blood", UNIT_X(un), UNIT_Y(un), 0, 0.05f*SIZE(un))
             psys->ogre_particle->setScaleTime(0.75);}
            {START_PARTICLE("green_blood2", UNIT_X(un), UNIT_Y(un), 0, 0.0065f*SIZE(un))
             /*psys->ogre_particle->setScaleTime(0.75);*/}
          } break;
          /* bLUE BLOOD */
          case TR_unit_type_witch_of_the_lake:
          case TR_unit_type_halfling_of_the_hills: 
          case TR_unit_type_disciple_of_waja: {
            {START_PARTICLE("blue_blood", UNIT_X(un), UNIT_Y(un), 0, 0.05f*SIZE(un))
             psys->ogre_particle->setScaleTime(0.75);}
            {START_PARTICLE("blue_blood2", UNIT_X(un), UNIT_Y(un), 0, 0.0065f*SIZE(un))
             /*psys->ogre_particle->setScaleTime(0.75);*/}
          } break;
          /* BUILD EXPLOSION */
          case TR_unit_type_inhibitor:
          case TR_unit_type_tower:
          case TR_unit_type_human_cauldron: {
            //TR_burn_trace_ptr b = BT_New(gi);
            //if(b != TR_null) {
            //  TR_burn_trace_t *bt = &burn_trace(b);
            //  bt->x = UNIT_X(un);
            //  bt->y = UNIT_Y(un);
            //}
            {START_PARTICLE("orc_death_woosh2", UNIT_X(un), UNIT_Y(un), 0.2, 0.05f)}
            {START_PARTICLE("house_explosion", UNIT_X(un), UNIT_Y(un), 1, 0.06)}
          } break;
        }
        if(un->player_id == CREEP_PLAYER_ID) {
          TR_creep_ptr cr        = (TR_creep_ptr)un->user_data;
          creep(cr).dead          = true;
          creep(cr).respawn_span  = game_desc.PvE ? 999999999 : 60000*12;
        }
      } break;
  }
  /* WITH PUN */
  switch(un->user_data_type) {
    case USER_DATA_PLAYER_UNIT: {
      if(un->unit_type != TR_unit_type_human_cauldron) {
        assert(un->user_data != TR_null);
        PUN_Free(un->user_data);
      }
    } break;
    case USER_DATA_MOBS: {
      assert(un->user_data == TR_null);
    } break;
  }
  return with_cadaver && TIME_TO_LIVE(un) == 0;
}

void GAME_GeneralDeath(TR_game_instance_t *gi, TR_player_ptr killer_p, TR_player_ptr unit_p, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_player_t *pl = &player(unit_p);
  TR_gp_player_ptr mp = pl->user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  TR_gp_player_ptr killer_mp = player(killer_p).user_data;
  TR_gp_player_t *killer_mpl = &gp_player(killer_mp);
  TR_camera_t *cam = &camera(GI_GetCamera(gi));
  assert(mp != TR_null);
  if(true) {
    mpl->loss++;
    killer_mpl->kill++;
  }
  /* XP */
  if(killer_p != unit_p) {
    killer_mpl->xp += XP(un);
    TR_player_ptr p = gi->player_head;
    while(p != TR_null) {
      TR_player_t *pl = &player(p);
      if(p != killer_p
      && pl->team_id == player(killer_p).team_id) {
        TR_gp_player_ptr mp = pl->user_data;
        TR_gp_player_t *mpl = &gp_player(mp);
        int xp_amount = 25*XP(un)/100;
        mpl->xp += xp_amount;
        if(pl->id == cam->player_id) {
          if(xp_amount > 0) {
            TR_floating_text_ptr ft = FT_New(gi);
            if(ft != TR_null) {
              TR_floating_text_t *flt = &floating_text(ft);
              flt->x = un->x;
              flt->y = un->y;
              flt->offset_y = 0;
              flt->s = "+" + i2a(xp_amount) + "XP";
              flt->c = TCOD_green;
              flt->char_size = 18 + (xp_amount>>3);
            }
          }
        }
      }
      p = pl->next;
    }
    int gold_amount = 90*GOLD(un)/100;
    killer_mpl->gold += gold_amount;
    if(player(killer_p).id == cam->player_id) {
      if(XP(un) > 0) {
        TR_floating_text_ptr ft = FT_New(gi);
        if(ft != TR_null) {
          TR_floating_text_t *flt = &floating_text(ft);
          flt->x = un->x;
          flt->y = un->y;
          flt->offset_y = gold_amount > 0 ? 22 : 0;
          flt->s = "+" + i2a(XP(un)) + "XP";
          flt->c = TCOD_green;
          flt->char_size = 18 + (XP(un)>>3);
        }
      }
      if(gold_amount > 0) {
        TR_floating_text_ptr ft_gold = FT_New(gi);
        if(ft_gold != TR_null) {
          TR_floating_text_t *flt_gold = &floating_text(ft_gold);
          flt_gold->x = un->x;
          flt_gold->y = un->y;
          flt_gold->offset_y = 0;
          flt_gold->s = "+" + i2a(gold_amount) + "Gold";
          flt_gold->c = TCOD_color_RGB(255, 174, 52);
          flt_gold->char_size = 18 + (gold_amount>>3);
        }
        WISE_PLAY_SOUND(gp_gi.coin_sound, "coin", -1, -1, 3000)
      }
    }
  }
}

str GAME_GetSkillName(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_name.cpp"
  assert(false);
  return "";
}

str GAME_GetSkillAnimName(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_anim_name.cpp"
  assert(false);
  return "";
}

int GAME_GetSkillAnimTime(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_anim_time.cpp"
  assert(false);
  return 0;
}

int GAME_GetSkillMaxLevel(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_max_level.cpp"
  assert(false);
  return 0;
}

int GAME_GetSkillEveryLevel(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_every_level.cpp"
  assert(false);
  return 0;
}

TR_skill_type_t GAME_DefaultSkillType(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target) {
  TR_unit_t *un = &unit(u);
  TR_unit_t *target_un = &unit(target);
  switch(un->unit_type) {
    #include "gen/default_skill_type.cpp"
  }
  assert(false);
  return (TR_skill_type_t)-1;
}

char GAME_GetSkillShortcut(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_shortcut.cpp"
  log(skill_type);
  assert(false);
  return 0;
}

str GAME_GetSkillInfo(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_info.cpp"
  log(skill_type);
  assert(false);
  return "";
}

int GAME_GetSkillRange(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  #include "gen/get_skill_range.cpp"
  assert(false);
  return 0;
}

int GAME_GetSkillCoolDown(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  int skill_level = un->skill_levels[skill_type];
  #include "gen/get_skill_cooldown.cpp"
  assert(false);
  return 0;
}

int GAME_GetSkillManaCost(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  #include "gen/get_skill_mana_cost.cpp"
  assert(false);
  return 0;
}

str GAME_GetSkillMaterial(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_material.cpp"
  assert(false);
  return 0;
}

int GAME_GetSkillCastSpeed(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  int skill_id = UN_SkillId(gi, u, skill_type);
  if(skill_id == -1)
    log(un->unit_type);
  assert(skill_id != -1);
  return GAME_GetSkillCastSpeedProxy(gi, p, u, skill_type)*un->skill_speed_factors[skill_id]/100;
}

int GAME_GetSkillCastSpeedProxy(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  #include "gen/get_skill_cast_speed.cpp"
  assert(false);
  return 0;
}

bool GAME_GetSkillDirectCast(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_direct_cast.cpp"
  log(skill_type);
  assert(false);
  return false;
}

TR_unit_type_t GAME_GetSkillInvokeType(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_invoke_type.cpp"
  log(skill_type);
  assert(false);
  return false;
}

int GAME_GetSkillInvokeCount(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_invoke_count.cpp"
  log(skill_type);
  assert(false);
  return false;
}

bool GAME_GetSkillCanStop(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_can_stop.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillContinue(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_continue.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillIsBlocking(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_is_blocking.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillIsAvailable(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_is_available.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillIsPassive(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_is_passive.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillIsUnique(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/get_skill_is_unique.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillLookForAir(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/skill_look_for_air.cpp"
  assert(false);
  return false;
}

bool GAME_GetSkillLookForGround(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/skill_look_for_ground.cpp"
  log(skill_type);
  assert(false);
  return false;
}

bool GAME_GetSkillCastOneByOne(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/skill_cast_one_by_one.cpp"
  log(skill_type);
  assert(false);
  return false;
}

str GAME_CanCastSkill(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type, TR_unit_ptr target, short cx, short cy, bool dont_check_common_cond) {
  TR_unit_t *un            = &unit(u),
            *target_un     = &unit(target);
  TR_gp_player_ptr mp      = player(p).user_data;
  TR_gp_player_t *mpl      = &gp_player(mp);
  TR_camera_t *cam         = &camera(GI_GetCamera(gi));
  TR_player_ptr cam_p      = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cam_pl      = &player(cam_p);
  TR_gp_player_ptr cam_mp  = cam_pl->user_data;
  TR_gp_player_t *cam_mpl  = &gp_player(cam_mp);
  str result               = GAME_CanCastSkill2(gi, p, u, skill_type, target, cx, cy);
  bool looks_for_target    = gi->skill_looks_for_target_cb(gi, skill_type),
       looks_for_enemy     = gi->skill_looks_for_enemy_cb(gi, skill_type);
  if(dont_check_common_cond)
    return result;
  return 
    //!gi->get_skill_is_available_cb(gi, skill_type) && un->skill_levels[skill_type] == 0 ? "You must learn this skill first" :
    mpl->mana < gi->get_skill_mana_cost_cb(gi, p, u, skill_type) ? "Not enough mana" :
    looks_for_target && target == TR_null ? "Target a unit." : 
    looks_for_target && looks_for_enemy  && unit(target).team_id == un->team_id ? "Target an enemy." :
    looks_for_target && !looks_for_enemy && unit(target).team_id != un->team_id ? "Target a friendly unit." : result;
}

str GAME_CanCastSkill2(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type, TR_unit_ptr target, short cx, short cy) {
  TR_unit_t *un         = &unit(u),
            *target_un  = &unit(target);
  TR_gp_player_ptr mp = player(p).user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  TR_camera_t *cam       = &camera(GI_GetCamera(gi));
  TR_player_ptr cam_p    = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cam_pl    = &player(cam_p);
  TR_gp_player_ptr cam_mp  = cam_pl->user_data;
  TR_gp_player_t *cam_mpl  = &gp_player(cam_mp);
  int skill_cast_speed = gi->get_skill_cast_speed_cb(gi, p, u, skill_type),
      skill_id         = UN_SkillId(gi, u, skill_type),
      skill_level      = un->skill_levels[skill_type],
      skill_mana_cost  = gi->get_skill_mana_cost_cb(gi, p, u, skill_type);
  #include "gen/can_cast_skill.cpp"
  assert(false);
  return "";
}

bool GAME_SkillLooksForUnit(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/skill_looks_for_unit.cpp"
  log(skill_type);
  assert(false);
  return false;
}

bool GAME_SkillLooksForEnemy(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/skill_looks_for_enemy.cpp"
  log(skill_type);
  assert(false);
  return false;
}

bool GAME_SkillLooksForTarget(TR_game_instance_t *gi, TR_skill_type_t skill_type) {
  #include "gen/skill_looks_for_target.cpp"
  assert(false);
  return false;
}

bool GAME_HUDInfo(TR_game_instance_t *gi, TR_camera_ptr ca) {
  bool result = true;
  TR_camera_t *cam     = &camera(ca);
  TR_player_ptr p      = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *pl     = &player(p);
  TR_gp_player_ptr mp = player(GI_GetPlayerPtr(gi, cam->player_id)).user_data;
  assert(mp != TR_null);
  TR_gp_player_t *mpl = &gp_player(mp);
  /* resources */
  gp_gi.gold_overlay->setCaption(str(i2a(mpl->gold) + " Gold"));
  gp_gi.gold_overlay->setLeft(3*gi->ogre_window->getWidth()/4-str(i2a(mpl->gold) + " Gold").size()/2*10);
  gp_gi.gold_overlay->show();
  gp_gi.xp_overlay->setCaption(str("XP " + i2a(mpl->xp) + "/" + i2a(120*mpl->level)));
  gp_gi.xp_overlay->setLeft(3*gi->ogre_window->getWidth()/4-str("XP " + i2a(mpl->xp) + "/" + i2a(120*mpl->level)).size()/2*10 + 100);
  gp_gi.xp_overlay->show();
  gp_gi.kill_loss_text_overlay->setCaption(str("Kills: " + i2a(mpl->kill) + " Losses: " + i2a(mpl->loss)));
  gp_gi.kill_loss_text_overlay->setLeft(gi->ogre_window->getWidth()/4-str("Kills: " + i2a(mpl->kill) + " Losses: " + i2a(mpl->loss)).size()/2*10);
  gp_gi.kill_loss_text_overlay->show();
  /* MANA */
  #if !MINIMAP_MODE
  gp_gi.mana_bar->setMetricsMode(Ogre::GMM_PIXELS);
  gp_gi.mana_bar->setLeft(4);
  gp_gi.mana_bar->setTop(4);
  gp_gi.mana_bar->setDimensions(mpl->mana * MANA_BAR_SIZEX / MPL_MAX_MANA, MANA_BAR_SIZEY);
  gp_gi.mana_bar->setMaterialName("mn_mat2");
  gp_gi.mana_bar->show();
  gp_gi.mana_overlay->setLeft(MANA_BAR_SIZEX/2 - str(i2a(mpl->mana/MPL_MANA_FACTOR) + "/" + i2a(MPL_MAX_MANA/MPL_MANA_FACTOR)).size()/2*10);
  gp_gi.mana_overlay->setCaption(str(i2a(mpl->mana/MPL_MANA_FACTOR) + "/" + i2a(MPL_MAX_MANA/MPL_MANA_FACTOR)));
  gp_gi.mana_overlay->show();
  gp_gi.mana_regen_overlay->setLeft(MANA_BAR_SIZEX - str(f2a((float)mpl->mana_regen/100) + "/s").size()*10);
  gp_gi.mana_regen_overlay->setCaption(f2a((float)mpl->mana_regen/100) + "/s");
  gp_gi.mana_regen_overlay->show();
  #else
  gp_gi.mana_bar_back->hide();
  #endif
  /* HIT BLOOD OVERLAY */
  if(mpl->hit_blood > 0) {
    mpl->hit_blood -= gi->game_time_step*3;
    Ogre::OverlayContainer *panel = gp_gi.hit_blood_overlay->getChild("hit blood overlay panel");
    panel->
      getMaterial()->
        getTechnique(0)->
          getPass(0)->
            getTextureUnitState(0)->
              setAlphaOperation(
                Ogre::LBX_MODULATE, 
                Ogre::LBS_TEXTURE, 
                Ogre::LBS_MANUAL,
                1.0, 
                MIN(1.f, (float)mpl->hit_blood/10000));
    gp_gi.hit_blood_overlay->show();
  } else
    gp_gi.hit_blood_overlay->hide();
  /* CHOSE OVERLAY */
  gp_gi.info_overlay->hide();
  TR_client_ptr c = GI_GetClientPtr(gi, cam->player_id);
  if(client(c).type == HUMAN_CLIENT) {
    int k = 0,
        skill_count = MPL_SKILL_COUNT;
    if(mpl->is_chosing_class) {
      gp_gi.chose_background_overlay->show();
      CLASS_CHOICE(TR_player_class_druid,       k, "Druid")
      CLASS_CHOICE(TR_player_class_wizard,      k, "Wizard")
      CLASS_CHOICE(TR_player_class_necromencer, k, "Necromancer")
      int top   = gi->ogre_window->getHeight()-80,
          left  = gi->ogre_window->getWidth()/2-96/2,
          sizex = 96,
          sizey = 48;
      bool is_inside = gi->mouse.x >= left
                    && gi->mouse.x < left + sizex
                    && gi->mouse.y >= top
                    && gi->mouse.y < top + sizey;
      Ogre::PanelOverlayElement *button = 
        (Ogre::PanelOverlayElement*)gp_gi.ok_button_overlay->
          getChild("ok button panel");
      assert(button);
      button->setMetricsMode(Ogre::GMM_PIXELS);
      button->setPosition(left, top);
      button->setDimensions(sizex, sizey);
      button->setMaterialName(is_inside ? "button_ok_hover_mat" : "button_ok_up_mat");
      if(is_inside && gi->lbtn_down)
        button->setMaterialName("button_ok_down_mat");
      gp_gi.ok_button_overlay->show();
      if(is_inside && gi->lbtn_up) {
        if(mpl->player_class != (TR_player_class_t)TR_null) {
          CL_CmdGeneric(
            gi, c, TR_gp_generic_chose_class,
            gi->cmd_id++, cam->player_id, mpl->player_class, -1, -1, -1);
          CAM_SetFlash(gi, ca, "Click on the '+' button", TCOD_dark_green);
          mpl->is_chosing_class = false;
          gp_gi.chose_background_overlay->hide();
        } else {
          CAM_SetFlash(gi, ca, "Please choose a class.", TCOD_red);
          PLAY_SOUND(gp_gi.miss_sound, "miss")
        }
      }
      result = false;
    } else {
      gp_gi.choose_class_text->hide();
      gp_gi.choose_skills_text->hide();
      gp_gi.info_overlay->hide();
      /* ASSIGN LEVEL BUTTON */
      if(mpl->assign_level) {
        int k = 0;
        if(mpl->player_class == TR_player_class_druid) {
          SKILL_CHOICE("skeleton_mages_mat",  TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("train_amazon_mat",    TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("train_driade_mat",    TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("driade_heal_mat",     TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("train_skeleton_mat",  TR_skill_type_invoke_skeleton_mages)  
        } else 
        if(mpl->player_class == TR_player_class_necromencer) {
          SKILL_CHOICE("multishot_mat",       TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("train_wizard_mat",    TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("train_zombie_mat",    TR_skill_type_invoke_skeleton_mages) 
          SKILL_CHOICE("train_knight_mat",    TR_skill_type_invoke_skeleton_mages) 
        }
        DO_TIMES(MAX_CHOSE_OVERLAY-k)
          gp_gi.chose_overlays[k+f]->hide();
        int top   = gi->ogre_window->getHeight()/2 - 600/2 + 600 - 55,
            left  = gi->ogre_window->getWidth()/2-96/2,
            sizex = 96,
            sizey = 48;
        bool is_inside = gi->mouse.x >= left
                      && gi->mouse.x < left + sizex
                      && gi->mouse.y >= top
                      && gi->mouse.y < top + sizey;
        Ogre::PanelOverlayElement *button = 
          (Ogre::PanelOverlayElement*)gp_gi.ok_button_overlay->
            getChild("ok button panel");
        assert(button);
        button->setMetricsMode(Ogre::GMM_PIXELS);
        button->setPosition(left, top);
        button->setDimensions(sizex, sizey);
        button->setMaterialName(is_inside ? "button_ok_hover_mat" : "button_ok_up_mat");
        button->show();
        if(is_inside && gi->lbtn_down)
          button->setMaterialName("button_ok_down_mat");
        gp_gi.ok_button_overlay->show();
        if(is_inside && gi->lbtn_up) {
          DO_TIMES(mpl->choose_skill_count)
            CL_CmdGeneric(
              gi, c, TR_gp_generic_chose_skill,
              mpl->choose_skills[f], cam->player_id, TR_null, -1, -1, -1);
          CAM_SetFlash(gi, ca, "Select your main building to train units.", TCOD_dark_green);
          mpl->assign_level = false;
          gp_gi.chose_background_overlay->hide();
        }
        result = false;
      } else {
        Ogre::PanelOverlayElement *button = 
          (Ogre::PanelOverlayElement*)gp_gi.ok_button_overlay->
            getChild("ok button panel");
        assert(button);
        button->hide();
        DO_TIMES(MAX_CHOSE_OVERLAY)
          gp_gi.chose_overlays[f]->hide();
        if(mpl->level_to_assign > 0) {
          int top   = gi->ogre_window->getHeight()-60,
              left  = 3*gi->ogre_window->getWidth()/4 + 50,
              sizex = 48,
              sizey = 48;
          bool is_inside = gi->mouse.x >= left
                        && gi->mouse.x < left + sizex
                        && gi->mouse.y >= top
                        && gi->mouse.y < top + sizey;
          Ogre::PanelOverlayElement *button = 
            (Ogre::PanelOverlayElement*)gp_gi.assign_level_button->
              getChild("assign_level panel");
          assert(button);
          button->setMetricsMode(Ogre::GMM_PIXELS);
          button->setPosition(left, top);
          button->setDimensions(sizex, sizey);
          button->setMaterialName(is_inside ? "button_level_up_hover_mat" : "button_level_up_up_mat");
          button->show();
          Ogre::TextAreaOverlayElement *level_to_assign_text = 
            (Ogre::TextAreaOverlayElement*)button->getChild("level_to_assign_text");
          assert(level_to_assign_text);
          level_to_assign_text->setCaption("(" + i2a(mpl->level_to_assign) + ")");
          level_to_assign_text->setLeft(50);
          level_to_assign_text->setTop(15);
          level_to_assign_text->show();
          if(is_inside && gi->lbtn_down)
            button->setMaterialName("button_level_up_down_mat");
          gp_gi.assign_level_button->show();
          if(is_inside && gi->lbtn_up) {
            mpl->choose_skill_count = 0;
            mpl->assign_level = true;
            Ogre::PanelOverlayElement *background =
              (Ogre::PanelOverlayElement*)gp_gi.chose_background_overlay->
                getChild("chose background panel");
            assert(background);
            background->setMetricsMode(Ogre::GMM_PIXELS);
            background->setPosition(gi->ogre_window->getWidth()/2-800/2, gi->ogre_window->getHeight()/2-600/2);
            background->setDimensions(800, 600);
            gp_gi.chose_background_overlay->show();
          }
        } else {
          Ogre::PanelOverlayElement *button = 
            (Ogre::PanelOverlayElement*)gp_gi.assign_level_button->
              getChild("assign_level panel");
          assert(button);
          button->hide();
        }
      }
    }
  } else {
    gp_gi.chose_background_overlay->hide();
  }
  gp_gi.choose_counter -= gi->game_time_step;
  gp_gi.choose_counter_text->setCaption("Battle starts in " + i2a(gp_gi.choose_counter/1000) + "s");
  gp_gi.choose_counter_text->setLeft(gi->ogre_window->getWidth()/2 - str("Game start in " + i2a(gp_gi.choose_counter/1000) + "s").size()/2*(20-3));
  gp_gi.choose_counter_text->setTop(20);
  gp_gi.choose_counter_text->show();
  if(gp_gi.choose_counter <= 0) {
    gp_gi.choose_counter = 0;
    if(client(c).type == HUMAN_CLIENT
    && mpl->is_chosing_class)
      gp_gi.choose_counter_text->setCaption("Quickly! (The game has started.)");
    else
      gp_gi.choose_counter_text->hide();
  }
  /* LEVEL TO ASSIGN */
  int             skill_ids[MAX_SKILLS_PER_CAMERA];
  TR_skill_type_t skill_types[MAX_SKILLS_PER_CAMERA];
  TR_unit_ptr skill_units[MAX_SKILLS_PER_CAMERA];
  int   skill_count = gi->get_selection_skills_cb(gi, ca, skill_ids, skill_types, skill_units),
        count = 0,
        i = 0;
  DO_TIMES(skill_count)
    count++;
  DO_TIMES(skill_count) {
    TR_unit_t *un = &unit(skill_units[f]);
    if(gi->default_skill_type_cb(gi, skill_units[f], TR_null) != skill_types[f]) {
      int left = gi->ogre_window->getWidth()/2 - (count+1)*(BTN_WIDTH+8)/2 + i*(BTN_WIDTH+8) + BTN_WIDTH/2,
          top  = gi->ogre_window->getHeight()-12-BTN_WIDTH;
      bool is_inside = gi->mouse.x >= left
                    && gi->mouse.x < left + BTN_WIDTH
                    && gi->mouse.y >= top 
                    && gi->mouse.y < top + BTN_WIDTH;
      gp_gi.plus_overlay[f]->hide();
      Ogre::OverlayContainer *panel = gp_gi.skill_level_overlay[f]->getChild("skill level overlay panel" + i2a(f));
      assert(panel);
      panel->setMetricsMode(Ogre::GMM_PIXELS);
      panel->setLeft(left);
      panel->setTop(top);
      panel->setWidth(100);
      panel->setHeight(100);
      Ogre::TextAreaOverlayElement *text_level = (Ogre::TextAreaOverlayElement*)panel->getChild("skill level text " + i2a(f));
      assert(text_level);
      text_level->setMetricsMode(Ogre::GMM_PIXELS);
      text_level->setLeft(5);
      text_level->setTop(30);
      text_level->setCaption(i2a(gi->get_skill_mana_cost_cb(gi, GI_GetPlayerPtr(gi, un->player_id), skill_units[f], skill_types[f])));
      text_level->setFontName("cuckoo");
      text_level->setCharHeight(21);
      text_level->setColourBottom(Ogre::ColourValue(0.f/255, 26.f/255, 217.f/255));
      text_level->setColourTop(Ogre::ColourValue(0.f/255, 75.f/255, 217.f/255));
      gp_gi.skill_level_overlay[f]->show();
      if(is_inside) {
        if(GAME_GetSkillName(gi, skill_types[f]).find("invoke") != std::string::npos)
          GAME_DisplayInvokeInfo(
            gi, gp_gi.info_overlay, gp_gi.info_panel, gp_gi.info_unit_name, gp_gi.info_unit_carac, gp_gi.info_text_overlay, 
            skill_types[f], gi->ogre_window->getHeight() - 300, gi->ogre_window->getWidth()/2-100, 14, "Arial");
        else
          GAME_DisplaySkillInfo(
            gi, gp_gi.info_overlay, gp_gi.info_panel, gp_gi.info_unit_name, gp_gi.info_unit_carac, gp_gi.info_text_overlay, 
            skill_types[f], gi->ogre_window->getHeight() - 300, gi->ogre_window->getWidth()/2-100, 14, "Arial");
      }
    } else
      gp_gi.skill_level_overlay[f]->hide();
    #if RECORD_DEMO
    gp_gi.skill_level_overlay[f]->hide();
    #endif
    i++;
  }
  DO_TIMES(TR_unit_type_count-skill_count) {
    gp_gi.skill_level_overlay[skill_count+f]->hide();
    gp_gi.plus_overlay[skill_count+f]->hide();
  }
  /* selection */
  if(cam->selected_units_head != TR_null)
    GAME_DisplayUnitInfo(
      gi, gp_gi.info_overlay2, gp_gi.info_panel2, gp_gi.info_unit_name2, gp_gi.info_unit_carac2, gp_gi.info_text_overlay2,
      cam->selected_sub_group, gi->ogre_window->getHeight() - 170, 20, 12, "Arial");
  else {
    if(!mpl->is_chosing_class) {
      gp_gi.info_panel2->setLeft(20-15);
      gp_gi.info_panel2->setTop(gi->ogre_window->getHeight() - 170-15);
      gp_gi.info_panel2->setDimensions(12*20, 12*15);
      gp_gi.info_panel2->show();
      gp_gi.info_overlay2->show();
    }
  }
  /* resurecting units */
  TR_unit_ptr C = GAME_GetCastle(gi, pl->id);
  if(C != TR_null) {
    TR_unit_t *un = &unit(C);
    TR_castle_t *ca = &castle(un->user_data);
    /* build */
    TR_build_order_ptr b = un->build_order_head[cam->player_id];
    DO_TIMES(MAX_RESPAWN_UNIT_TEXT)
      ca->respawn_unit_texts[f]->hide();
    int f = 0;
    while(b != TR_null
       && f < MAX_RESPAWN_UNIT_TEXT) {
      TR_build_order_t *bo = &build_order(b);
      int completion = (pl->unit_dic[bo->unit_type].build_time - bo->progress) / 1000;
      ca->respawn_unit_texts[f]->setCaption(str(GAME_GetUnitName(gi, bo->unit_type) + " " + i2a(completion) + "s"));
      ca->respawn_unit_texts[f]->setTop(100 + f*20);
      ca->respawn_unit_texts[f]->show();
      f++;
      b = bo->next;
    }
    if(f == MAX_RESPAWN_UNIT_TEXT)
      ca->respawn_unit_texts[MAX_RESPAWN_UNIT_TEXT-1]->setCaption("And more...");
  }
  #if RECORD_DEMO
  mpl->is_chosing_class = false;
  gp_gi.mana_bar->hide();
  gp_gi.mana_overlay->hide();
  gp_gi.mana_regen_overlay->hide();
  gp_gi.mana_bar_back->hide();
  gp_gi.kill_loss_text_overlay->hide();
  gp_gi.xp_overlay->hide();
  gp_gi.gold_overlay->hide();
  gp_gi.info_overlay->hide();
  gp_gi.info_overlay2->hide();
  gp_gi.assign_level_button->hide();
  gp_gi.choose_counter_text->hide();
  gp_gi.chose_background_overlay->hide();
  gp_gi.info_panel2->hide();
  #endif
  return result; /* process generic inputs? */
}

int GAME_GetSelectionSkills(TR_game_instance_t *gi, TR_camera_ptr ca, int skill_ids[MAX_SKILLS_PER_CAMERA], TR_skill_type_t skill_types[MAX_SKILLS_PER_CAMERA], TR_unit_ptr skill_units[MAX_SKILLS_PER_CAMERA]) {
  TR_camera_t *cam = &camera(ca);
  int skill_count = 0,
      team_id = TEAM_ID(cam->player_id);
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null
     && skill_count < MAX_SKILLS_PER_CAMERA) {
    TR_unit_t *un = &unit(u);
    if(un->team_id == team_id) {
      DO_TIMES(un->skill_count) {
        bool found = false;
        for(int i = 0; i < skill_count; i++)
          if(skill_types[i] == un->skills[f]
          && unit(skill_units[i]).unit_type == un->unit_type) {
            found = true;
            break;
          }
        if(!found) {
          skill_ids[skill_count] = f;
          skill_types[skill_count] = un->skills[f];
          skill_units[skill_count] = u;
          skill_count = MIN(skill_count+1, MAX_SKILLS_PER_CAMERA-1);
        }
      }
    }
    u = un->snext[ca];
  }
  return skill_count;
}

void GAME_DisplayUnitInfo(TR_game_instance_t *gi, Ogre::Overlay *overlay, Ogre::OverlayContainer *panel, Ogre::TextAreaOverlayElement *unit_name, Ogre::TextAreaOverlayElement *unit_carac, Ogre::TextAreaOverlayElement *info_text, TR_unit_type_t unit_type, int top, int left, int size, const char *font, bool display_info) {
  unit_name->setFontName(font);
  unit_name->setCharHeight(size);
  unit_name->setCaption(GAME_GetUnitName(gi, unit_type));
  unit_name->setLeft(left);
  unit_name->setTop(top);
  unit_name->show();
  str info;
  str attack_type;
  switch(gi->unit_dic[unit_type].attack_type) {
    case TR_attack_type_normal:
      attack_type = "Normal (*)";
      info += "(*) Do more damage against light armor.\n";
      break;
    case TR_attack_type_piercing:
      attack_type = "Piercing (*)";
      info += "(*) Do more damage against medium armor.\n";
      break;
    case TR_attack_type_magic:
      attack_type = "Magic (*)";
      info += "(*) Do more damage against heavy armor.\n";
      break;
    case TR_attack_type_crashing:
      attack_type = "Crashing (*)";
      info += "(*) Do more damage against heavy and fortified armor.\n";
      break;
    case TR_attack_type_chaos:
      attack_type = "Chaos (*)";
      info += "(*) Do more damage against all type of armor.\n";
      break;
  }
  str armor_type;
  switch(gi->unit_dic[unit_type].armor_type) {
    case TR_armor_type_light:
      armor_type = "Light (**)";
      info += "(**) Take more damage from normal attacks.\nTake less damage from piercing attacks.\n";
      break;
    case TR_armor_type_medium:
      armor_type = "Medium (**)";
      info += "(**) Take more damage from piercing attacks.\nTake less damage from normal attacks.\n";
      break;
    case TR_armor_type_heavy:
      armor_type = "Heavy (**)";
      info += "(**) Take more damage from magic attacks.\nTake less damage from normal and piercing \nattacks.\n";
      break;
    case TR_armor_type_fortified:
      armor_type = "Fortified (**)";
      info += "(**) Take more damage from crashing attacks.\nTake less damage from the others.\n";
      break;
  }
  unit_carac->setFontName(font);
  unit_carac->setCharHeight(size);
  unit_carac->setCaption(str("HP: " + i2a(gi->unit_dic[unit_type].max_hp) + "\n" +
                                        (gi->unit_dic[unit_type].is_hero ? "HP Per Level: " + f2a((float)gi->unit_dic[unit_type].hp_per_level) + "\n" : "") +
                                        "Damage: " + i2a(gi->unit_dic[unit_type].attack_damage) + "\n" + 
                                        (gi->unit_dic[unit_type].is_hero ? "Damage Per Level: " + i2a(gi->unit_dic[unit_type].attack_damage_per_level) + "\n" : "") + 
                                        "Attack Speed: " + f2a(1.f/(MAX(1, (float)gi->unit_dic[unit_type].attack_speed/1000))) + " per second\n" + 
                                        "Attack Type: " + attack_type + "\n" +
                                        "Armor Type: " + armor_type + "\n" + info));
  unit_carac->setLeft(left);
  unit_carac->setTop(top + size);
  unit_carac->show();
  if(display_info) {
    info_text->setFontName(font);
    info_text->setCharHeight(size);
    info_text->setCaption(gi->unit_dic[unit_type].info);
    info_text->setLeft(left);
    info_text->setTop(top + size*(11 + (gi->unit_dic[unit_type].is_hero ? 2 : 0)));
    info_text->show();
  } else
    info_text->hide();
  panel->setLeft(left-15);
  panel->setTop(top-15);
  panel->setDimensions(size*20, size*15);
  panel->show();
  overlay->show();
}

void GAME_DisplayInvokeInfo(TR_game_instance_t *gi, Ogre::Overlay *overlay, Ogre::OverlayContainer *panel, Ogre::TextAreaOverlayElement *unit_name, Ogre::TextAreaOverlayElement *unit_carac, Ogre::TextAreaOverlayElement *info_text, TR_skill_type_t skill_type, int top, int left, int size, const char *font, bool display_info) {
  TR_unit_type_t unit_type = gi->get_skill_invoke_type_cb(gi, skill_type);
  int unit_count = gi->get_skill_invoke_count_cb(gi, skill_type);
  GAME_DisplayUnitInfo(gi, overlay, panel, unit_name, unit_carac, info_text, unit_type, top, left, size, font, display_info);
  unit_name->setFontName(font);
  unit_name->setCharHeight(size);
  unit_name->setCaption("Summon " + i2a(unit_count) + " " + GAME_GetUnitName(gi, unit_type));
  unit_name->setLeft(left);
  unit_name->setTop(top);
}

void GAME_DisplaySkillInfo(TR_game_instance_t *gi, Ogre::Overlay *overlay, Ogre::OverlayContainer *panel, Ogre::TextAreaOverlayElement *unit_name, Ogre::TextAreaOverlayElement *unit_carac, Ogre::TextAreaOverlayElement *info_text, TR_skill_type_t skill_type, int top, int left, int size, const char *font, bool display_info) {
  unit_name->setFontName(font);
  unit_name->setCharHeight(size);
  unit_name->setCaption(GAME_GetSkillName(gi, skill_type));
  unit_name->setLeft(left);
  unit_name->setTop(top);
  unit_carac->setFontName(font);
  unit_carac->setCharHeight(size);
  unit_carac->setCaption("");
  unit_carac->setLeft(left);
  unit_carac->setTop(top + size);
  info_text->setFontName(font);
  info_text->setCharHeight(size);
  info_text->setCaption(gi->get_skill_info_cb(gi, skill_type));
  info_text->setLeft(left);
  info_text->setTop(top + size);
  panel->setLeft(left-15);
  panel->setTop(top-15);
  panel->setDimensions(size*20, size*15);
  overlay->show();
}

void GAME_UpdateUnitRender(TR_game_instance_t *gi, TR_camera_ptr ca, TR_player_ptr cam_p, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_camera_t *cam = &camera(ca);
  TR_player_t *cam_pl = &player(cam_p);
  switch(un->unit_type) {
  }
}

str GAME_GetUnitName(TR_game_instance_t *gi, TR_unit_type_t unit_type) {
  switch(unit_type) {
    #include "gen/get_unit_name.cpp"
  }
  assert(false);
  return "";
}

void GAME_OnSpawnUnit(TR_game_instance_t *gi, TR_unit_ptr from_u, TR_unit_ptr u) {
  DTRACE("GAME_OnSpawnUnit")
  TR_unit_t *un = &unit(u);
  TR_player_ptr p = GI_GetPlayerPtr(gi, un->player_id);
  TR_player_t *pl = &player(p);
  TR_gp_player_ptr mp = pl->user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  TR_player_unit_ptr pu = TR_null;
  TR_player_unit_t *pun = null;
  DO_TIMES(TR_skill_type_count)
    un->skill_levels[f] = mpl->skill_levels[f];
  LEVEL(un) = mpl->level;
  DO_TIMES(un->skill_count)
    if(gi->get_skill_is_unique_cb(gi, un->skills[f]))
      un->skills_cooldown[f] = mpl->skills_cooldown[un->skills[f]];
  if(TIME_TO_LIVE(un) > 0) {
    TR_status_effect_ptr s = ST_New(gi, u, TR_status_effect_invocation_fate, INFINITE_SPAN, false);
    status_effect(s).amount = TIME_TO_LIVE(un);
  }
  if(HERO(un)
  && mpl->skill_count > 0) {
    un->skill_count = mpl->skill_count;
    DO_TIMES(mpl->skill_count)
      un->skills[f] = mpl->skills[f];
  }
  switch(un->user_data_type) {
    case USER_DATA_PLAYER_UNIT: {
      pu = PUN_New();
      assert(pu != TR_null);
      player_unit(pu).resurect_it = true;
      un->user_data = pu;
      pun = &player_unit(pu);
      TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
      if(c != TR_null) {
        TR_client_t *cl = &client(c);
      }
    } break;
    case USER_DATA_MOBS: {
      if(un->unit_type == TR_unit_type_crypt_cow) {
        static int spots_x[3] = {64, 26, 128-26},
                   spots_y[3] = {64, 26, 128-26};
        DTRACEI(u)
        int lane = u%3;
        TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
        assert(c != TR_null);
        TR_client_t *cl = &client(c);
        /* offline client */
        if(cl->type != REPLAY_CLIENT) {
          TR_mobs_ai_client_ptr mo_ai_c = (TR_mobs_ai_client_ptr)cl->user_data;
          TR_mobs_ai_client_t *mobs_cl = &mobs_ai_client(mo_ai_c);
          gp_gi.mobs_count++;
          //START_PARTICLE("woodwolf_spawn", UNIT_X(un), UNIT_Y(un), 0.8, 0.03*MODEL_SCALE(un))
          CL_CmdUnitOrder(
            gi, c, u, 
            spots_x[lane]<<POS_SHIFT, 
            spots_y[lane]<<POS_SHIFT, 
            TR_null, false, true, un->skills[0], 
            /* ground only? */ true, -1, FOLLOW_R_SQUARED);
          CL_CmdUnitOrder(
            gi, c, u, 
            (un->team_id == 0 ? MAP_SIZE_X : 0)<<POS_SHIFT, 
            (un->team_id == 0 ? 0 : MAP_SIZE_Y)<<POS_SHIFT, 
            TR_null, true /* push back*/, true, un->skills[0], /* ground only? */ true);
        }
      } else {
      }
    } break;
    case USER_DATA_CREEP: {
      TR_creep_ptr cr = un->user_data;
      TR_creep_t *cree = &creep(cr);
      LEVEL(un) = cree->level;
      UN_AddHP(gi, u, 99999999);
      UN_SetMana(gi, u, MAX_MANA(un));
    } break;
  }
  switch(un->unit_type) {
    /* WARNING: only castle in this switch, look the default case */
    case TR_unit_type_human_cauldron: {
        DTRACE("Castle spawn");
        TR_castle_ptr C = CAS_New();
        assert(C != TR_null);
        castle(C).u         = u;
        castle(C).player_id = un->player_id;
        castle(C).team_id   = un->team_id;
        castle(C).rally_cx  = UNIT_X(un);
        castle(C).rally_cy  = UNIT_Y(un);
        un->user_data       = (TR_ptr_t)C;
        /*if(un->unit_type == TR_unit_type_human_cauldron) {
          {START_PARTICLE("campfire", 0, 0, 1.0, 0.05)
           UN_AttachParticleSystem(gi, u, ps);}
          {START_PARTICLE("glowing_smoke", 0, 0, 2.5, 0.05)
           UN_AttachParticleSystem(gi, u, ps);}
        } else {
          {START_PARTICLE("campfire", 0, 0, 1.0, 0.05)
           UN_AttachParticleSystem(gi, u, ps);}
          {START_PARTICLE("orc_glowing_smoke", 0, 0, 2.5, 0.05)
           UN_AttachParticleSystem(gi, u, ps);}
        }*/
        /* LOAD */
        if(game_desc.continue_game)
          GAME_LoadGame(gi);
      } break;
    default: {
      if(CAN_BE_PUSHED(un)) {
        START_PARTICLE("woodwolf_spawn", UNIT_X(un), UNIT_Y(un), MAX_UNIT*2*0.01, 0.03*MODEL_SCALE(un))
      }
      switch(un->unit_type) {
        case TR_unit_type_tower: {
          un->lookat_x = un->team_id == 0 ? 1 : -1;
        } break;
        case TR_unit_type_skeleton_mage: {
          assert(pun);
          pun->tags += "undead, ";
          //TR_status_effect_ptr s = ST_New(gi, u, TR_status_effect_invocation_fate, INFINITE_SPAN, false);
          //status_effect(s).amount = 3;
        } break;
        case TR_unit_type_crypt_cow: {
          mpl->unit_counts[0]++;
        } break;
      }
      if(from_u != TR_null) {
        TR_unit_t *from_un = &unit(from_u);
        /* from who? */
        switch(from_un->unit_type) {
          case TR_unit_type_human_cauldron: {
            TR_castle_ptr C = (TR_castle_ptr)from_un->user_data;
            TR_castle_t *ca = &castle(C);
            if(ca->rally_target != TR_null
            && !PL_IsUnitAlive(gi, GI_GetPlayerPtr(gi, ca->rally_target_player_id), ca->rally_target_cmd_id))
              ca->rally_target = TR_null;
            if(ca->rally_target != TR_null
            || ca->rally_cx != -1) {
              TR_client_ptr c = GI_GetClientPtr(gi, un->player_id);
              TR_client_t *cl = &client(c);
              if(cl->type == HUMAN_CLIENT) {
                int pos_x = (ca->rally_target != TR_null ? unit(ca->rally_target).x : ca->rally_cx),
                    pos_y = (ca->rally_target != TR_null ? unit(ca->rally_target).y : ca->rally_cy);
                CL_CmdUnitOrder(gi, c, u, pos_x, pos_y, ca->rally_target, false, false, (TR_skill_type_t)-1, ca->rally_target == TR_null);
              }
            }
          } break;
        }
      }
    } break;
  }
}

int GAME_OnHitUnit(TR_game_instance_t *gi, TR_ball_ptr b, TR_unit_ptr target, int damage, int attack_penetration, bool show_hp) {
  /* don't kill the target here */
  TR_unit_t *target_un = &unit(target);
  TR_player_unit_t *target_pun = null;
  if(b != TR_null) {
    TR_ball_t *ba = &ball(b);
    if(ba->type & TR_ball_type_living_unit_only) {
      if(!CAN_BE_PUSHED(&unit(target)))
        damage = 0;
    }
    if(ba->type & TR_ball_type_moonshot) {
      TR_get_unit_here_t data;
      UN_GetUnitsHere(&data, gi, target, target_un->team_id, 12, target_un->x, target_un->y, true, true, TR_get_unit_here_mode_friends);
      DO_TIMES(data.count) {
        TR_unit_ptr u2 = data.result[f];
        TR_unit_t *un2 = &unit(u2);
        if(UN_GetDist(gi, target, u2) <= (4<<POS_SHIFT)) {
          UN_DealInstantDamage(gi, GI_GetPlayerPtr(gi, un2->player_id), u2, 20, TR_attack_type_magic, false);
          ST_New(gi, u2, TR_status_effect_stun, 2000, false);
          TR_explosion_ptr e = EX_New(gi);
          if(e != TR_null) {
            TR_explosion_t *ex  = &explosion(e);
            ex->x               = UNIT_X(un2);
            ex->y               = UNIT_Y(un2);
            ex->total_frame     = 2;
            ex->size            = 1;
            ex->mesh_name       = "blue_fireball_explosion";
            ex->span            = -EXPLOSION_FRAME_SPAN;
          }
        }
      }
      {START_PARTICLE("moonshot_explosion", UNIT_X(target_un), UNIT_Y(target_un), 2*MAX_UNIT*0.01, 1.f)}
      {START_PARTICLE("pixel_explosion", UNIT_X(target_un), UNIT_Y(target_un), 2*MAX_UNIT*0.01, 0.25f)}
      TR_explosion_ptr e = EX_New(gi);
      if(e != TR_null) {
        TR_explosion_t *ex  = &explosion(e);
        ex->x               = UNIT_X(target_un);
        ex->y               = UNIT_Y(target_un);
        ex->total_frame     = 2;
        ex->size            = 4;
        ex->mesh_name       = "blue_fireball_explosion";
        ex->span            = -EXPLOSION_FRAME_SPAN;
      }
    }
  }
  if(target_un->user_data_type == USER_DATA_PLAYER_UNIT) {
    assert(target_un->user_data != TR_null);
    target_pun = &player_unit(target_un->user_data);
  }
  switch(target_un->unit_type) {
  }
  if(target_un->user_data_type == USER_DATA_PLAYER_UNIT
  && HERO(target_un)) {
    TR_player_ptr p = GI_GetPlayerPtr(gi, target_un->player_id);
    TR_player_t *pl = &player(p);
    TR_gp_player_ptr mp = pl->user_data;
    TR_gp_player_t *mpl = &gp_player(mp);
    mpl->hit_blood += 3000;
  }
  if(show_hp
  && damage > 0) {
    TR_floating_text_ptr ft = FT_New(gi);
    assert(ft != TR_null);
    if(ft != TR_null) {
      TR_floating_text_t *flt = &floating_text(ft);
      flt->char_size = 20 + (attack_penetration < 100 ? 0 : attack_penetration == 100 ? 4 : 8);
      flt->x = UNIT_X(target_un)-flt->char_size/2 /* IDUWINW */ -155;
      flt->y = UNIT_Y(target_un);
      flt->offset_y = -18; 
      flt->s = i2a(damage);
      flt->c = TCOD_red;
    }
  }
  return damage;
}

void GAME_OnCancelBuild(TR_game_instance_t *gi, TR_build_order_ptr b) {
  TR_build_order_t *bo = &build_order(b);
  TR_player_ptr p      = GI_GetPlayerPtr(gi, bo->unit_player_id);
  TR_player_t *pl     = &player(p);
  TR_gp_player_ptr mp = pl->user_data;
  assert(mp != TR_null);
  TR_gp_player_t *mpl = &gp_player(mp);
  mpl->gold += pl->unit_dic[bo->unit_type].gold_price;
}

void GAME_GenericCmd(TR_game_instance_t *gi, TR_id_t cmd_type, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy) {
  TR_player_ptr p = GI_GetPlayerPtr(gi, player_id);
  TR_player_t *pl = &player(p);
  TR_gp_player_ptr mp = pl->user_data;
  assert(mp != TR_null);
  TR_gp_player_t *mpl = &gp_player(mp);
  switch(cmd_type) {
    case TR_gp_generic_chose_class: {
      mpl->player_class = (TR_player_class_t)cmd_mask;
    } break;
    case TR_gp_generic_chose_skill: {
      mpl->level_to_assign--;
      TR_unit_ptr C = GAME_GetCastle(gi, pl->id);
      assert(C != TR_null);
      TR_unit_t *castle = &unit(C);
      castle->skills[castle->skill_count++] = (TR_skill_type_t)id;
    } break;
    default:
      assert(false);
  }
}

void GAME_OnUnitOrder(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy, bool skill_here, TR_skill_type_t skill_type) {
  TR_player_ptr p = GI_GetPlayerPtr(gi, player_id);
  TR_player_t *pl = &player(p);
  switch(unit_type) {
    case TR_unit_type_human_cauldron: {
      if(!skill_here
      || !gi->get_skill_direct_cast_cb(gi, skill_type)) {
        TR_castle_ptr c    = (TR_castle_ptr)unit(u).user_data;
        TR_castle_t *ca    = &castle(c);
        ca->rally_target    = target;
        if(target != TR_null) {
          ca->rally_target_player_id = unit(target).player_id;
          ca->rally_target_cmd_id    = unit(target).cmd_id;
        }
        ca->rally_cx = target_cx;
        ca->rally_cy = target_cy;
      }
    } break;
  }
}

TR_unit_ptr GAME_GetCastle(TR_game_instance_t *gi, TR_id_t player_id) {
  TR_castle_ptr cas = gp_gi.castle_head;
  while(cas != TR_null) {
    if(castle(cas).player_id == player_id)
      return castle(cas).u;
    cas = castle(cas).next;
  }
  return TR_null;
}

TR_unit_ptr GAME_GetHero(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  TR_unit_ptr u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(HERO(un))
      return u;
    u = un->pnext;
  }
  return TR_null;
}

void GAME_OpenWindow(SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca, int size_x, int size_y) {
  TR_camera_t *cam       = &camera(ca);
  cam->window_opened     = true;
  cam->window_size_x     = size_x;
  cam->window_size_y     = size_y;
  cam->window_start_x    = (CON_RES_X-cam->window_size_x)/2;
  cam->window_start_y    = ((HUD_START_Y-cam->window_size_y)/2);
  cam->window_end_x      = (cam->window_start_x+cam->window_size_x);
  cam->window_end_y      = (cam->window_start_y+cam->window_size_y); 
  CAM_UpdateWindowCloseButton(gi, ca);
  //CAM_UpdateAndRenderWindow(surface, gi, ca);
  GI_StopClicksPropagation(gi);
}

bool GAME_DrawUnitGoal(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u) {
  TR_camera_t *cam = &camera(ca);
  TR_unit_t *un = &unit(u);
  switch(un->unit_type) {
  }
  return true; /* can hide? */
}

str GAME_GetCursor(TR_game_instance_t *gi, TR_camera_ptr ca, TR_skill_type_t skill_type, int cx, int cy) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_gp_player_ptr mp = player(p).user_data;
  TR_gp_player_t *mpl = &gp_player(mp);
  #include "gen/get_cursor.cpp"
  assert(false);
}

void GAME_OnSelectUnitSound(TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y) {
  switch(unit_type) {
    #include "gen/on_select_unit_sound.cpp"
    default:
      assert(false);
  }
}

void GAME_OnMoveUnitSound(TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y) {
  switch(unit_type) {
    #include "gen/on_move_unit_sound.cpp"
    default:
      assert(false);
  }
}

void GAME_OnSkillSound(TR_game_instance_t *gi, TR_id_t skill_type, int pos_x, int pos_y, bool can_cast) {
  if(!can_cast) {
    PLAY_SOUND(gp_gi.miss_sound, "miss")
  } else {
    #include "gen/on_skill_sound.cpp"
  }
}

void GAME_OnAttackUnitSound(TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y) {
  switch(unit_type) {
    #include "gen/on_attack_unit_sound.cpp"
  }
}

TR_unit_ptr GAME_GetUnitInRange(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_ptr units[MAX_UNIT], int unit_count, int skill_range) {
  TR_unit_t *un = &unit(u);
  TR_unit_ptr result = TR_null;
  int min_dist        = 0x7fffffff;
  DO_TIMES(unit_count) {
    TR_unit_ptr u2 = units[f];
    TR_unit_t *un2 = &unit(u2);
    if(ATTACK_DAMAGE(un2) > 0) {
      int dist = UN_GetDist(gi, u, u2);
      if(un->user_data_type == USER_DATA_MOBS
      && un2->user_data_type == USER_DATA_MOBS)
        dist >>= 2;
      if(dist < min_dist
      && un2->virtual_hp > 0) {
        result = u2;
        min_dist = dist;
      }
    }
  }
  if(result == TR_null) {
    if(result == TR_null) {
      DO_TIMES(unit_count) {
        TR_unit_ptr u2 = units[f];
        TR_unit_t *un2 = &unit(u2);
        int dist = UN_GetDist(gi, u, u2);
        if(dist < min_dist
        && un2->virtual_hp > 0) {
          result = u2;
          min_dist = dist;
        }
      }
    }
  }
  return result;
}

bool GAME_CastSkill(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_order_ptr o, int x, int y) {
  TR_player_t *pl        = &player(p);
  TR_unit_t *un          = &unit(u);
  TR_camera_t *cam       = &camera(GI_GetCamera(gi));
  TR_player_ptr cam_p    = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cam_pl    = &player(cam_p);
  TR_gp_player_ptr cam_mp  = cam_pl->user_data;
  TR_gp_player_t *cam_mpl  = &gp_player(cam_mp);
  TR_unit_order_t *uo    = &order(o);
  TR_gp_player_ptr mp  = pl->user_data;
  TR_gp_player_t *mpl  = &gp_player(mp);
  TR_player_unit_t *pun  = null;
  if(un->user_data_type == USER_DATA_PLAYER_UNIT) {
    assert(un->user_data != TR_null);
    TR_player_unit_ptr pu = un->user_data;
    pun = &player_unit(pu);
  }
  int skill_cast_speed = gi->get_skill_cast_speed_cb(gi, p, u, uo->skill_type);
  TR_skill_type_t skill_type = uo->skill_type;
  int skill_id   = UN_SkillId(gi, u, skill_type),
      skill_level = un->skill_levels[skill_type],
      skill_mana_cost = gi->get_skill_mana_cost_cb(gi, p, u, skill_type);
  assert(skill_id != -1);
  #include "gen/cast_skill.cpp"
  assert(false);
  return true;  // continue?
}

void GAME_SaveGame(TR_game_instance_t *gi, int level) {
  str dir = str("./save/" + i2a(game_desc.team_count) + i2a(game_desc.player_count_per_team) + "/");
  mkdir(dir.c_str());
  FILE *desc_file = fopen(str(dir + "desc").c_str(), "w+b");
  int version = SAVE_VERSION;
  fwrite(&version, sizeof(version), 1, desc_file);
  fwrite(&level, sizeof(level), 1, desc_file);
  fwrite(&game_desc.seed, sizeof(game_desc.seed), 1, desc_file);
  fclose(desc_file);
  TR_player_ptr p = gi->player_head;
  while(p != TR_null) {
    TR_player_t *pl          = &player(p);
    TR_gp_player_ptr mp    = pl->user_data;
    TR_gp_player_t   *mpl  = &gp_player(mp);
    FILE *file = fopen(str(dir + "p" + i2a(pl->id)).c_str(), "w+b");
    int unit_count = 0;
    TR_unit_ptr u = pl->unit_head;
    while(u != TR_null) {
      TR_unit_t *un = &unit(u);
      unit_count++;
      u = un->pnext;
    }
    fwrite(&unit_count, sizeof(unit_count), 1, file);
    fwrite(&mpl->kill, sizeof(mpl->kill), 1, file);
    fwrite(&mpl->loss, sizeof(mpl->loss), 1, file);
    u = pl->unit_head;
    while(u != TR_null) {
      TR_unit_t *un = &unit(u);
      fwrite(&un->unit_type, sizeof(un->unit_type), 1, file);
      u = un->pnext;
    }
    fclose(file);
    p = pl->next;
  }
}

void GAME_LoadGameDesc(TR_game_instance_t *gi) {
  str dir = str("./save/" + i2a(game_desc.team_count) + i2a(game_desc.player_count_per_team) + "/");
  FILE *desc_file = fopen(str(dir + "desc").c_str(), "r+b");
  int version;
  fread(&version, sizeof(version), 1, desc_file);
  assert(version == SAVE_VERSION);
  fread(&gp_gi.level, sizeof(gp_gi.level), 1, desc_file);
  fread(&game_desc.seed, sizeof(game_desc.seed), 1, desc_file);
  game_desc.seed += gp_gi.level;
  fclose(desc_file);
}

void GAME_LoadGame(TR_game_instance_t *gi) {
  /*str dir = str("./save/" + i2a(game_desc.team_count) + i2a(game_desc.player_count_per_team) + "/");
  TR_player_ptr p = gi->player_head;
  while(p != TR_null) {
    TR_player_t *pl          = &player(p);
    TR_gp_player_ptr mp    = pl->user_data;
    TR_gp_player_t   *mpl  = &gp_player(mp);
    FILE *file = fopen(str(dir + "p" + i2a(pl->id)).c_str(), "r+b");
    int unit_count;
    fread(&unit_count, sizeof(unit_count), 1, file);
    fread(&mpl->kill, sizeof(mpl->kill), 1, file);
    fread(&mpl->loss, sizeof(mpl->loss), 1, file);
    DO_TIMES(unit_count) {
      TR_unit_type_t unit_type;
      fread(&unit_type, sizeof(unit_type), 1, file);
      TR_unit_ptr cas = GAME_GetCastle(gi, pl->team_id);
      if(cas != TR_null) {
        TR_unit_t *castle = &unit(cas);
        if(unit_type != TR_unit_type_castle
        && unit_type != TR_unit_type_tower)
          CL_CmdSpawnUnit(
            gi, 
            GI_GetClientPtr(gi, pl->id), 
            TR_null,
            castle->x + (castle->team_id == 0 ? 2 : -2) + TR_Rand(&gp_gi.working_seed)%200,
            castle->y + TR_Rand(&gp_gi.working_seed)%200,
            unit_type);
      }
    }
    fclose(file);
    p = pl->next;
  }*/
}

/*
 *
 */

TR_gp_game_instance_t gp_gi;
  

/*
 * end of file
 */  