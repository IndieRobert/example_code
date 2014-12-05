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

void BTN_UpdateEntry(TR_game_instance_t *gi, TR_btn_t *b) {
}

void BTN_Render(SDL_Surface *surface, TR_game_instance_t *gi, TR_btn_t *b, bool left) {
}

void BTN_Render(SDL_Surface *surface, TR_game_instance_t *gi, TR_btn_list_t *l, bool center_x, short step, bool left) {
}

bool BTN_IsMouseOver(TR_game_instance_t *gi, TR_btn_t *b, bool left) {
}

bool BTN_IsClicked(TR_game_instance_t *gi, TR_btn_t *b, bool left) {
  return false;
}

bool BTN_GonnaBeClicked(TR_game_instance_t *gi, TR_btn_t *b, bool left) {
  return false;
}

void BTN_CenterX(TR_game_instance_t *gi, TR_btn_t *b, short axe) {
}

void BTN_CenterY(TR_game_instance_t *gi, TR_btn_t *b) {
}

void BTN_CenterY(TR_game_instance_t *gi, TR_btn_list_t *l) {
}

/*
 *
 */

void UO_Init(TR_game_instance_t *gi, int argc, char** argv) {
  for(int i = 0; i < MAX_UNIT_ORDER; i++) {
    order(i).order = TR_unit_order_none;
    if(i != MAX_UNIT_ORDER-1)
      order(i).fnext = i+1;
    else
      order(i).fnext = TR_null;
    order(i).next     = TR_null;
  }
  gi->free_unit_order_head = 0;
  log("Unit orders initialized.");
}

TR_unit_order_ptr UO_New(TR_game_instance_t *gi) {
  if(gi->free_unit_order_head != TR_null) {
    TR_unit_order_ptr o        = gi->free_unit_order_head;
    TR_unit_order_t   *uo      = &order(o);
    gi->free_unit_order_head    = uo->fnext;
    uo->order                   = TR_unit_order_none;
    uo->started                 = false;
    uo->starting                = false;
    uo->started_proxy           = false;
    uo->done                    = false;
    uo->skill_here              = false;
    uo->precise_target          = true;
    uo->with_counter            = false;
    uo->following_friend        = false;
    uo->forced_action           = false;
    uo->generated               = true; /* most are */
    uo->last_target_cx          = -1;
    uo->counter                 = 0;
    uo->task_id                 = -1;
    uo->target                  = TR_null;
    uo->next                    = TR_null;
    uo->user_data               = TR_null;
    uo->unit_order_completed_cb = null;
    uo->unit_order_failed_cb    = null;
    return o;
  }
  log("No more unit order left.");
  return TR_null;
}

TR_unit_order_ptr UO_PushFront(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_order_ptr o = UO_New(gi);
  if(o != TR_null) {
    TR_unit_order_t   *uo     = &order(o);
    uo->next                  = unit(u).order_head;
    unit(u).order_head        = o;
  }
  return o;
}

TR_unit_order_ptr UO_PushBack(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_order_ptr o = UO_New(gi);
  if(o != TR_null) {
    if(unit(u).order_head == TR_null)
      unit(u).order_head = o;
    else {
      TR_unit_order_ptr last = unit(u).order_head;
      while(order(last).next != TR_null)
        last = order(last).next;
      order(last).next = o;
    }
  }
  return o;
}

void UO_FreeFront(TR_game_instance_t *gi, TR_unit_ptr u, bool completed) {
  TR_unit_t *un             = &unit(u);
  TR_unit_order_ptr o       = un->order_head;
  TR_unit_order_t *uo       = &order(o);
  if(o != TR_null) {
    un->order_head             = uo->next;
    if(completed) {
      if(uo->unit_order_completed_cb)
        uo->unit_order_completed_cb(gi, u, o);
    } else {
      if(uo->unit_order_failed_cb)
        uo->unit_order_failed_cb(gi, u, o);
    }
    uo->task_id                = gi->task_mark++;
    uo->fnext                  = gi->free_unit_order_head;
    gi->free_unit_order_head   = o;
    if(uo->skill_here) {
      int skill_id = UN_SkillId(gi, u, uo->skill_type);
      assert(skill_id != -1);
      un->skill_is_cooling_down[skill_id] = true;
      if(uo->target != TR_null
      && uo->skill_type == gi->default_skill_type_cb(gi, u, uo->target)) {
        TR_unit_t *target_un    = &unit(uo->target);
        TR_player_ptr target_p  = GI_GetPlayerPtr(gi, uo->target_player_id);  
        int skill_id            = UN_SkillId(gi, u, uo->skill_type);
        if(target_p != TR_null
        && un->skill_marks[skill_id] == un->virtual_hp_cache
        && PL_IsUnitAlive(gi, target_p, uo->target_cmd_id))
          target_un->virtual_hp = MIN(target_un->hp, target_un->virtual_hp+un->virtual_hp_cache2);
      }
    }
  }
}

void UO_FreeAll(TR_game_instance_t *gi, TR_unit_ptr u) {
  while(unit(u).order_head != TR_null)
    UO_FreeFront(gi, u);
}

TR_unit_order_type_t UO_GetFrontType(TR_game_instance_t *gi, TR_unit_ptr u) {
  if(unit(u).order_head == TR_null)
    return TR_unit_order_none;
  return order(unit(u).order_head).order;
}

TR_unit_order_t* UO_GetFront(TR_game_instance_t *gi, TR_unit_ptr u) {
  if(unit(u).order_head == TR_null)
    return null;
  return &order(unit(u).order_head); 
}

bool UO_IsTarget(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, TR_unit_order_ptr o) {
  if(o == TR_null)
    o = unit(u).order_head;
  if(o != TR_null) {
    TR_unit_order_t *uo = &order(o);
    if(uo->order == TR_unit_order_follow_goal
    && uo->target == target)
      return true;
    if(uo->next != TR_null)
      return UO_IsTarget(gi, u, target, uo->next);
  }
  return false;
}

/*
 * particle_systems
 */

void PS_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create particle_systems.
  for(int i = 0; i < MAX_PARTICLE_SYSTEM; i++) {
    if(i != MAX_PARTICLE_SYSTEM-1)
      particle_system(i).fnext = i+1;
    else
      particle_system(i).fnext = TR_null;
  }
  gi->free_particle_system_head = 0;
  gi->particle_system_head      = TR_null;
  log("particle_systems initialized.");
}

TR_particle_system_ptr PS_New(TR_game_instance_t *gi) {
  if(gi->free_particle_system_head != TR_null) {
    TR_particle_system_ptr r            = gi->free_particle_system_head;
    gi->free_particle_system_head       = particle_system(gi->free_particle_system_head).fnext;
    particle_system(r).previous         = TR_null;
    particle_system(r).next             = gi->particle_system_head;
    particle_system(r).ogre_node        = null;
    particle_system(r).parent_node      = null;
    particle_system(r).ogre_particle    = null;
    particle_system(r).stopped          = false;
    particle_system(r).free_it          = true;
    particle_system(r).scale            = 0.3f;
    particle_system(r).unext            = TR_null;
    if(gi->particle_system_head != TR_null)
      particle_system(gi->particle_system_head).previous = r;
    gi->particle_system_head = r;
    return r;
  }
  log("No more particle_system left.");
  return TR_null;
}

void PS_Free(TR_game_instance_t *gi, TR_particle_system_ptr b) {
  TR_particle_system_t *psys = &particle_system(b);
  psys->ogre_node->detachObject(psys->ogre_particle);
  //if(!psys->parent_node)
    gi->ogre_scene->getRootSceneNode()->removeChild(psys->ogre_node);
  //else
  //  psys->parent_node->removeChild(psys->ogre_node);
  ParticleUniverse::ParticleSystemManager::getSingletonPtr()->
    destroyParticleSystem(psys->ogre_particle, gi->ogre_scene);
  gi->ogre_scene->destroySceneNode(psys->ogre_node);
  if(gi->particle_system_head == b)
    gi->particle_system_head = particle_system(b).next;
  if(particle_system(b).previous != TR_null)
    particle_system(particle_system(b).previous).next = particle_system(b).next;
  if(particle_system(b).next != TR_null)
    particle_system(particle_system(b).next).previous = particle_system(b).previous;
  particle_system(b).fnext = gi->free_particle_system_head;
  gi->free_particle_system_head = b;
}

void PS_FreeAll(TR_game_instance_t *gi) {
  while(gi->particle_system_head != TR_null)
    PS_Free(gi, gi->particle_system_head);
  log("particle_systems freed.");
}

bool PS_Update(TR_game_instance_t *gi, TR_player_ptr cam_p, TR_particle_system_ptr ps) {
  TR_particle_system_t *psys = &particle_system(ps);
  psys->ogre_node->setPosition((float)psys->x/(1<<POS_SHIFT), psys->z, (float)psys->y/(1<<POS_SHIFT));
  Ogre::Vector3 pos = psys->ogre_node->_getDerivedPosition();
  if(!PL_IsInFov(gi, cam_p, (int)floor(pos.x), (int)floor(pos.z)))
    psys->ogre_node->setPosition((float)9999999, psys->z, (float)9999999);
  if(psys->ogre_particle->getState() 
  == ParticleUniverse::ParticleSystem::PSS_PREPARED)
    psys->ogre_particle->start();
  return psys->free_it
      && PS_IsStopped(gi, ps); /* free? */
}

void PS_Start(TR_game_instance_t *gi, TR_particle_system_ptr ps) {
  TR_particle_system_t *psys = &particle_system(ps);
  psys->ogre_node = 
    gi->ogre_scene->
      getRootSceneNode()->
        createChildSceneNode("particle node " + i2a(ps));
  psys->ogre_particle =
    ParticleUniverse::ParticleSystemManager::getSingletonPtr()->
        createParticleSystem("particle " + i2a(ps), psys->name, gi->ogre_scene);
  psys->ogre_particle->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
  psys->ogre_node->attachObject(psys->ogre_particle);
  psys->ogre_particle->setQueryFlags(0);
  psys->ogre_particle->setScaleVelocity(psys->scale);
  psys->ogre_particle->setScale(Ogre::Vector3(psys->scale, psys->scale, psys->scale));
  psys->ogre_particle->prepare();
}

void PS_Stop(TR_game_instance_t *gi, TR_particle_system_ptr ps, bool force) {
  TR_particle_system_t *psys = &particle_system(ps);
  if(force) {
    psys->ogre_particle->stop();
    psys->ogre_particle->setEnabled(false);
  } else
    psys->ogre_particle->stopFade();
  psys->stopped = true;
}

bool PS_IsStopped(TR_game_instance_t *gi, TR_particle_system_ptr ps) {
  TR_particle_system_t *psys = &particle_system(ps);
  return /*psys->stopped 
      &&*/ psys->ogre_particle->getState() 
      == ParticleUniverse::ParticleSystem::PSS_STOPPED;
}

/*
 * balls
 */

void BA_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create balls.
  for(int i = 0; i < MAX_BALL; i++) {
    if(i != MAX_BALL-1)
      ball(i).fnext = i+1;
    else
      ball(i).fnext = TR_null;
  }
  gi->free_ball_head = 0;
  gi->ball_head      = TR_null;
  log("Balls initialized.");
}

TR_ball_ptr BA_New(TR_game_instance_t *gi) {
  if(gi->free_ball_head != TR_null) {
    TR_ball_ptr r            = gi->free_ball_head;
    gi->free_ball_head       = ball(gi->free_ball_head).fnext;
    ball(r).previous         = TR_null;
    ball(r).next             = gi->ball_head;
    ball(r).ps               = TR_null;
    ball(r).node             = null;
    ball(r).mesh_angle       = 0;
    ball(r).speed            = 10;
    ball(r).type             = 0;
    ball(r).radius           = 1<<(POS_SHIFT-3);
    ball(r).ball_type        = TR_ball_type_normal;
    ball(r).en_cloche        = false;
    ball(r).distance         = 0;
    if(gi->ball_head != TR_null)
      ball(gi->ball_head).previous = r;
    gi->ball_head = r;
    return r;
  }
  log("No more ball left.");
  return TR_null;
}

void BA_Free(TR_game_instance_t *gi, TR_ball_ptr b) {
  TR_ball_t *ba = &ball(b);
  if(gi->ball_head == b)
    gi->ball_head = ball(b).next;
  if(ball(b).previous != TR_null)
    ball(ball(b).previous).next = ball(b).next;
  if(ball(b).next != TR_null)
    ball(ball(b).next).previous = ball(b).previous;
  ball(b).fnext = gi->free_ball_head;
  gi->free_ball_head = b;
}

void BA_FreeAll(TR_game_instance_t *gi) {
  while(gi->ball_head != TR_null)
    BA_Free(gi, gi->ball_head);
  log("Balls freed.");
}

bool BA_Update(TR_game_instance_t *gi, TR_ball_ptr b) {
  TR_ball_t *ba = &ball(b);
  //DTRACE("BA_Update")
  switch(ba->ball_type) {
    case TR_ball_type_normal: {
      TR_player_ptr target_p  = GI_GetPlayerPtr(gi, ba->target_player_id);
      bool target_is_alive = PL_IsUnitAlive(gi, target_p, ba->target_cmd_id);
      //DTRACEI(ba->target_player_id)
      //DTRACEI(ba->target_cmd_id)
      TR_unit_t *target_un = null;
      if(target_is_alive)
        target_un = &unit(ba->target);
      if(target_is_alive) {
        assert(target_un);
        //ba->target_cx = target_un->x+UN_GetSizeShift(gi, SIZE(target_un));
        //ba->target_cy = target_un->y+UN_GetSizeShift(gi, SIZE(target_un));
        //ba->target_cz = (FLYING(target_un) ? FLYING_HEIGHT : 0) + BALL_HEIGHT(target_un);
      }
      TR_particle_system_t *psys;
      if(ba->ps != TR_null)
        psys = &particle_system(ba->ps);
      ba->move += gi->game_time_step * ba->speed;
      //DTRACEI(ba->move)
      int   dx = ba->target_cx - ba->x,
            dy = ba->target_cy - ba->y,
            dz = ba->target_cz - ba->z;
      while(ABS(dx) > 1000 || ABS(dy) > 1000) {
        dx >>= 1;
        dy >>= 1;
        dz >>= 1;
      }
      int d = dx*dx + dy*dy + dz*dz;
      if(d > 0) {
        int dist = TR_ISqrt(d),
            ddx = dx * ba->move / dist,
            ddy = dy * ba->move / dist,
            ddz = dz * ba->move / dist;
        if(ddx*ddx + ddy*ddy + ddz*ddz >= d) {
          ba->x += dx;
          ba->y += dy;
          ba->z += dz;
        } else {
          ba->x += ddx;
          ba->y += ddy;
          ba->z += ddz;
        }
        int arot = ba->attack_range<<(POS_SHIFT-1);
        if(ba->ps != TR_null) {
          psys->x = ba->x;
          psys->y = ba->y;
          psys->z = (float)(ba->z + (ba->en_cloche ? (1.f-pow((float)ABS((float)ba->distance*0.9-arot)/arot, 2))*(1<<POS_SHIFT) : 0))/(1<<POS_SHIFT);
        }
        if(ba->node) {
          ba->node->setPosition(
            Ogre::Vector3(
              (float)ba->x/(1<<POS_SHIFT), 
              (float)(ba->z + (ba->en_cloche ? (1.f-pow((float)ABS((float)ba->distance*0.9-arot)/arot, 2))*(1<<POS_SHIFT) : 0))/(1<<POS_SHIFT),
              (float)ba->y/(1<<POS_SHIFT)));
          ba->ent->setVisible(PL_IsInFov(gi, ba->p, ba->x>>POS_SHIFT, ba->y>>POS_SHIFT));
          switch(ba->ball_mesh_cinetic_type) {
            case TR_ball_mesh_cinetic_type_rotating: {
              ba->node->setOrientation(Ogre::Quaternion(Ogre::Degree(ba->mesh_angle), Ogre::Vector3(ba->ax, ba->ay, ba->az)));
              ba->mesh_angle += (float)gi->game_time_step;
            } break;
            default:
              ba->node->setOrientation(
                Ogre::Quaternion(
                  Ogre::Radian(3.14/2-atan2(ba->target_cy - ba->y, ba->target_cx - ba->x)), 
                  Ogre::Vector3(ba->ax, ba->ay, ba->az)));
              break;
          }
        }
        //DTRACEI(ba->x)
        //DTRACEI(ba->y)
        ba->distance += ba->move;
        ba->move = 0;
        dx = ba->target_cx - ba->x;
        dy = ba->target_cy - ba->y;
        dz = ba->target_cz - ba->z;
        while(ABS(dx) > 1000 || ABS(dy) > 1000) {
          dx >>= 1;
          dy >>= 1;
          dz >>= 1;
        }
        d = dx*dx + dy*dy + dz*dz;
      }
      if(ba->speed >= MAX_BALL_SPEED
      || d < (1<<(POS_SHIFT-4))) {
        //DTRACE("here");
        if(target_is_alive) {
          //DTRACE("over here");
          assert(target_un);
          assert(target_un->hp > 0);
          int damage = UN_GetDamage(gi, ba->target, ba->damage, ba->attack_type);
          damage = gi->on_hit_unit_cb(gi, b, ba->target, damage, attack_penetrations[ba->attack_type][ARMOR_TYPE(target_un)], true);
          target_un->hp -= damage;
          if(target_un->hp <= 0) {
            TR_cadaver_ptr c = PL_KillUnit(gi, ba->p, target_p, ba->target, true);
            assert(c != TR_null);
            TR_cadaver_t *ca = &cadaver(c);
            d = ba->dx*ba->dx + ba->dy*ba->dy;
            if(CAN_BE_PUSHED(target_un)
            && d != 0) {
              d = TR_ISqrt(d);
              ca->force_x = ba->dx * damage * 4 / d;
              ca->force_y = ba->dy * damage * 4 / d;
            }
          }
        }
        if(ba->ps != TR_null)
          PS_Stop(gi, ba->ps);
        if(ba->node) {
          ba->node->detachObject(ba->ent);
          gi->ogre_scene->getRootSceneNode()->removeChild(ba->node);
          gi->ogre_scene->destroyEntity(ba->ent);
          gi->ogre_scene->destroySceneNode(ba->node);
        }
        TR_explosion_ptr e = EX_New(gi);
        if(e != TR_null) {
          TR_explosion_t *ex  = &explosion(e);
          ex->x               = target_is_alive ? UNIT_X(target_un) : ba->x;
          ex->y               = target_is_alive ? UNIT_Y(target_un) : ba->y;
          ex->total_frame     = ba->explosion_type.total_frame;
          ex->size            = ba->explosion_type.size;
          ex->mesh_name       = ba->explosion_type.mesh_name;
          if(ba->explosion_type.particle != "") {
            START_PARTICLE(ba->explosion_type.particle, ba->x, ba->y, (float)ba->z/(1<<POS_SHIFT), ba->explosion_type.size)
            ex->ps = ps;
          }
          ex->additive        = ba->explosion_type.additive;
        }
        if(ba->explosion_type.explosion_sound
        && SO_WiseToPlay(gi, "explosion", ba->x, ba->y, 3000)) {
          PLAY_SOUND(ba->explosion_type.explosion_sound, "explosion")
          TR_sound_t *so = &sound(s);
          so->x = ba->x;
          so->y = ba->y;
        }
        return true; /* free? */
      }
      return false; /* free? */
    } break;
    case TR_ball_type_projectile_one_hit: {
      TR_player_t *pl = &player(ba->p);
      TR_particle_system_t *psys;
      if(ba->ps != TR_null)
        psys = &particle_system(ba->ps);
      ba->move += gi->game_time_step * ba->speed;
      //DTRACEI(ba->move)
      int   dx = ba->dx,
            dy = ba->dy;
      while(ABS(dx) > 1000 || ABS(dy) > 1000) {
        dx >>= 1;
        dy >>= 1;
      }
      int d = dx*dx + dy*dy;
      if(d > 0) {
        int   dist = TR_ISqrt(d),
              ddx = dx * ba->move / (dist),
              ddy = dy * ba->move / (dist);
        if(ddx*ddx + ddy*ddy >= d) {
          ba->x += dx;
          ba->y += dy;
        } else {
          ba->x += ddx;
          ba->y += ddy;
        }
        if(ba->ps != TR_null) {
          psys->x = ba->x;
          psys->y = ba->y;
          psys->z = (float)ba->z/(1<<POS_SHIFT);
        }
        if(ba->node) {
          ba->node->setPosition(
            Ogre::Vector3(
              (float)ba->x/(1<<POS_SHIFT), 
              (float)ba->z/(1<<POS_SHIFT),
              (float)ba->y/(1<<POS_SHIFT)));
          ba->ent->setVisible(PL_IsInFov(gi, ba->p, ba->x>>POS_SHIFT, ba->y>>POS_SHIFT));
          switch(ba->ball_mesh_cinetic_type) {
            case TR_ball_mesh_cinetic_type_rotating: {
              ba->node->setOrientation(Ogre::Quaternion(Ogre::Degree(ba->mesh_angle), Ogre::Vector3(ba->ax, ba->ay, ba->az)));
              ba->mesh_angle += (float)gi->game_time_step;
            } break;
          }
        }
        //DTRACEI(ba->x)
        //DTRACEI(ba->y)
        ba->move = 0;
      }
      if(INSIDE_MAP(ba->x>>POS_SHIFT, ba->y>>POS_SHIFT)) {
        TR_get_unit_here_t data;
        UN_GetUnitsHere(
          &data, gi, TR_null, pl->team_id, 
          5, ba->x, ba->y, ba->look_for_ground, ba->look_for_air, TR_get_unit_here_mode_enemies);
        DO_TIMES(data.count) {
          TR_unit_ptr u2 = data.result[f];
          TR_unit_t *un2 = &unit(u2);
          if(UN_GetDist(gi, u2, ba->x, ba->y) < (SIZE(un2)<<(POS_SHIFT-1)) + ba->radius) {
            int damage = UN_GetDamage(gi, u2, ba->damage, ba->attack_type);
            damage = gi->on_hit_unit_cb(gi, b, u2, damage, attack_penetrations[ba->attack_type][ARMOR_TYPE(un2)], true);
            un2->hp -= damage;
            if(un2->hp <= 0) {
              TR_cadaver_ptr c = PL_KillUnit(gi, ba->p, GI_GetPlayerPtr(gi, un2->player_id), u2, true);
              assert(c != TR_null);
              TR_cadaver_t *ca = &cadaver(c);
              d = ba->dx*ba->dx + ba->dy*ba->dy;
              if(CAN_BE_PUSHED(un2)
              && d != 0) {
                d = TR_ISqrt(d);
                ca->force_x = ba->dx * damage * 4 / d;
                ca->force_y = ba->dy * damage * 4 / d;
              }
            }
            if(ba->ps != TR_null)
              PS_Stop(gi, ba->ps);
            if(ba->node) {
              ba->node->detachObject(ba->ent);
              gi->ogre_scene->getRootSceneNode()->removeChild(ba->node);
              gi->ogre_scene->destroyEntity(ba->ent);
              gi->ogre_scene->destroySceneNode(ba->node);
            }
            if(ba->explosion_type.explosion_sound
            && SO_WiseToPlay(gi, "explosion", ba->x, ba->y, 3000)) {
              PLAY_SOUND(ba->explosion_type.explosion_sound, "explosion")
              TR_sound_t *so = &sound(s);
              so->x = ba->x;
              so->y = ba->y;
            }
            return true; /* free? */
          }
        }
        return false; /* free? */
      } else {
        if(ba->ps != TR_null)
          PS_Stop(gi, ba->ps);
        if(ba->node) {
          ba->node->detachObject(ba->ent);
          gi->ogre_scene->getRootSceneNode()->removeChild(ba->node);
          gi->ogre_scene->destroyEntity(ba->ent);
          gi->ogre_scene->destroySceneNode(ba->node);
        }
        if(ba->explosion_type.explosion_sound
        && SO_WiseToPlay(gi, "explosion", ba->x, ba->y, 3000)) {
          PLAY_SOUND(ba->explosion_type.explosion_sound, "explosion")
          TR_sound_t *so = &sound(s);
          so->x = ba->x;
          so->y = ba->y;
        }
      }
      return true;  /* free? */
    } break;
    case TR_ball_type_projectile_distance: {

    } break;
  }
  assert(false);  /* free? */
  return false;
}

/*
 * burn_traces
 */

void BT_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create burn_traces.
  for(int i = 0; i < MAX_BURN_TRACE; i++) {
    if(i != MAX_BURN_TRACE-1)
      burn_trace(i).fnext = i+1;
    else
      burn_trace(i).fnext = TR_null;
  }
  gi->free_burn_trace_head = 0;
  gi->burn_trace_head      = TR_null;
  log("burn_traces initialized.");
}

TR_burn_trace_ptr BT_New(TR_game_instance_t *gi) {
  if(gi->free_burn_trace_head != TR_null) {
    TR_burn_trace_ptr r            = gi->free_burn_trace_head;
    gi->free_burn_trace_head       = burn_trace(gi->free_burn_trace_head).fnext;
    burn_trace(r).span             = BURN_TRACE_LIFE_SPAN;
    burn_trace(r).previous         = TR_null;
    burn_trace(r).next             = gi->burn_trace_head;
    burn_trace(r).c                = TCOD_color_multiply_scalar(BURN_TRACE_COLOR, (float)(128+(rand()&127))/255);
    burn_trace(r).node             = null;
    if(gi->burn_trace_head != TR_null)
      burn_trace(gi->burn_trace_head).previous = r;
    gi->burn_trace_head = r;
    return r;
  }
  log("No more burn_trace left.");
  return TR_null;
}

void BT_Free(TR_game_instance_t *gi, TR_burn_trace_ptr b) {
  if(gi->burn_trace_head == b)
    gi->burn_trace_head = burn_trace(b).next;
  if(burn_trace(b).previous != TR_null)
    burn_trace(burn_trace(b).previous).next = burn_trace(b).next;
  if(burn_trace(b).next != TR_null)
    burn_trace(burn_trace(b).next).previous = burn_trace(b).previous;
  burn_trace(b).fnext = gi->free_burn_trace_head;
  gi->free_burn_trace_head = b;
}

void BT_FreeAll(TR_game_instance_t *gi) {
  while(gi->burn_trace_head != TR_null)
    BT_Free(gi, gi->burn_trace_head);
  log("burn_traces freed.");
}

bool BT_Update(TR_game_instance_t *gi, TR_burn_trace_ptr b) {
  TR_burn_trace_t *bt = &burn_trace(b);
  if(bt->node == null) {
    bt->ent = gi->ogre_scene->createEntity("bt " + i2a(b), "burntrace.mesh");
    bt->ent->setCastShadows(false);
    bt->ent->setQueryFlags(0);
    bt->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("bt node " + i2a(b));
    bt->node->attachObject(bt->ent);
    bt->node->setPosition(Ogre::Vector3((float)bt->x/(1<<POS_SHIFT), 0.01f*(b+1), (float)bt->y/(1<<POS_SHIFT)));
    DO_TIMES(bt->ent->getNumSubEntities()) {
      bt->ent->
        getSubEntity(f)->
          getMaterial()->
            setDepthWriteEnabled(false);
      if(!bt->ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getTextureUnitState("FovMapTex")) {
        Ogre::TextureUnitState *tex = 
          bt->ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    createTextureUnitState("FovMapTex");
        tex->setName("FovMapTex");
        tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
      }
      Ogre::MaterialPtr mat = 
        bt->ent->
          getSubEntity(f)->
            getMaterial()->
              clone("bt " + i2a(b) + " mat");
      mat->compile();
      bt->ent->
        getSubEntity(f)->
          setMaterialName("bt " + i2a(b) + " mat");
      Ogre::GpuProgramParametersSharedPtr params = 
        bt->ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getFragmentProgramParameters();
      params->setNamedConstant("alpha", 0.f);
    }
  }
  DO_TIMES(bt->ent->getNumSubEntities()) {
    Ogre::GpuProgramParametersSharedPtr params = 
      bt->ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                getFragmentProgramParameters();
    params->setNamedConstant("alpha", MIN(1.f, 1.f - (float)bt->span/BURN_TRACE_LIFE_SPAN));
  }
  bt->span -= gi->game_time_step;
  //if(bt->span <= 0)
  //  return true; /* free the blood */
  return false; /* don't free the blood */
}

/*
 * floating_texts
 */

void FT_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create floating_texts.
  for(int i = 0; i < MAX_FLOATING_TEXT; i++) {
    if(i != MAX_FLOATING_TEXT-1)
      floating_text(i).fnext = i+1;
    else
      floating_text(i).fnext = TR_null;
  }
  gi->free_floating_text_head = 0;
  gi->floating_text_head      = TR_null;
  log("floating_texts initialized.");
}

TR_floating_text_ptr FT_New(TR_game_instance_t *gi) {
  if(gi->free_floating_text_head != TR_null) {
    static int mat_id = 0;
    TR_floating_text_ptr r            = gi->free_floating_text_head;
    gi->free_floating_text_head       = floating_text(gi->free_floating_text_head).fnext;
    floating_text(r).span             = FLOATING_TEXT_LIFE_SPAN;
    floating_text(r).previous         = TR_null;
    floating_text(r).next             = gi->floating_text_head;
    floating_text(r).c                = TCOD_white;
    floating_text(r).s                = "unset";
    floating_text(r).offset_y         = 0;
    floating_text(r).z                = 0;
    floating_text(r).char_size        = 19;
    floating_text(r).overlay   = Ogre::OverlayManager::getSingleton().create("ft overlay " + i2a(r));
    floating_text(r).panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "ft panel " + i2a(r));
    floating_text(r).panel->setMetricsMode(Ogre::GMM_PIXELS);
    floating_text(r).panel->setPosition(10, 10);
    floating_text(r).panel->setDimensions(100, 100);
    floating_text(r).text_overlay = 
      (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
        "TextArea", "ft text " + i2a(r));
    floating_text(r).text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
    floating_text(r).text_overlay->setLeft(800);
    floating_text(r).text_overlay->setTop(0);
    floating_text(r).text_overlay->setCaption("Yoooopi");
    floating_text(r).text_overlay->setFontName("cuckoo");
    floating_text(r).text_overlay->setCharHeight(floating_text(r).char_size);
    floating_text(r).text_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
    floating_text(r).text_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
    floating_text(r).text_overlay->hide();
    floating_text(r).panel->addChild(floating_text(r).text_overlay);
    floating_text(r).overlay->add2D(floating_text(r).panel);
    floating_text(r).overlay->show();
    Ogre::MaterialPtr mat = 
      floating_text(r).text_overlay->
        getMaterial()->
          clone("ft " + i2a(mat_id) + " mat");
    mat->compile();
    floating_text(r).text_overlay->
      setMaterialName("ft " + i2a(mat_id) + " mat");
    mat_id++;
    if(gi->floating_text_head != TR_null)
      floating_text(gi->floating_text_head).previous = r;
    gi->floating_text_head = r;
    return r;
  }
  log("No more floating_text left.");
  return TR_null;
}

void FT_Free(TR_game_instance_t *gi, TR_floating_text_ptr ft) {
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("ft panel " + i2a(ft));
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("ft text " + i2a(ft));
  Ogre::OverlayManager::getSingleton().destroy(floating_text(ft).overlay);
  if(gi->floating_text_head == ft)
    gi->floating_text_head = floating_text(ft).next;
  if(floating_text(ft).previous != TR_null)
    floating_text(floating_text(ft).previous).next = floating_text(ft).next;
  if(floating_text(ft).next != TR_null)
    floating_text(floating_text(ft).next).previous = floating_text(ft).previous;
  floating_text(ft).fnext = gi->free_floating_text_head;
  gi->free_floating_text_head = ft;
}

void FT_FreeAll(TR_game_instance_t *gi) {
  while(gi->floating_text_head != TR_null)
    FT_Free(gi, gi->floating_text_head);
  log("floating_texts freed.");
}

bool FT_Update(TR_game_instance_t *gi, TR_floating_text_ptr ft) {
  TR_floating_text_t *flt = &floating_text(ft);
  flt->span -= gi->game_time_step;
  Ogre::Vector2 pos = 
    CAM_WorldToScreen(
      gi, GI_GetCamera(gi), 
      (float)flt->x/(1<<POS_SHIFT), 
      (float)flt->y/(1<<POS_SHIFT), 
      0);
  pos.x *= gi->ogre_window->getWidth();
  pos.y *= gi->ogre_window->getHeight();
  flt->z += gi->game_time_step*flt->span/(6*FLOATING_TEXT_LIFE_SPAN);
  flt->text_overlay->setLeft(pos.x);
  flt->text_overlay->setTop(pos.y+flt->offset_y-flt->z);
  flt->text_overlay->setCaption(flt->s);
  flt->text_overlay->setCharHeight(flt->char_size);
  flt->text_overlay->setColourBottom(Ogre::ColourValue((float)flt->c.r/255, (float)flt->c.g/255, (float)flt->c.b/255));
  flt->text_overlay->setColourTop(Ogre::ColourValue((float)flt->c.r/255, (float)flt->c.g/255, (float)flt->c.b/255));
  flt->text_overlay->show();
  flt->text_overlay->
    getMaterial()->
      getTechnique(0)->
        getPass(0)->
          setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  flt->text_overlay->getMaterial()->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
  flt->text_overlay->
    getMaterial()->
      getTechnique(0)->
        getPass(0)->
          getTextureUnitState(0)->
            setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_MANUAL, 1.0, (float)flt->span/FLOATING_TEXT_LIFE_SPAN);
  if(flt->span <= 0)
    return true; /* free the floating text */
  return false; /* don't free the floating text */
}

/*
 * lights
 */

void LI_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create lights.
  gi->tcod_light_map = TCOD_map_new(RANGE_MAP_SIZE_X, RANGE_MAP_SIZE_Y);
  gi->torch_noise    = TCOD_noise_new(1, 1.f, 1.f, null);
  for(int i = 0; i < MAX_LIGHT; i++) {
    if(i != MAX_LIGHT-1)
      light(i).fnext = i+1;
    else
      light(i).fnext = TR_null;
  }
  gi->free_light_head = 0;
  gi->light_head      = TR_null;
  log("lights initialized.");
}

TR_light_ptr LI_New(TR_game_instance_t *gi) {
  if(gi->free_light_head != TR_null) {
    TR_light_ptr r            = gi->free_light_head;
    gi->free_light_head       = light(gi->free_light_head).fnext;
    light(r).previous         = TR_null;
    light(r).next             = gi->light_head;
    if(gi->light_head != TR_null)
      light(gi->light_head).previous = r;
    gi->light_head = r;
    return r;
  }
  log("No more light left.");
  return TR_null;
}

void LI_Free(TR_game_instance_t *gi, TR_light_ptr l) {
  if(gi->light_head == l)
    gi->light_head = light(l).next;
  if(light(l).previous != TR_null)
    light(light(l).previous).next = light(l).next;
  if(light(l).next != TR_null)
    light(light(l).next).previous = light(l).previous;
  light(l).fnext = gi->free_light_head;
  gi->free_light_head = l;
}

void LI_FreeAll(TR_game_instance_t *gi) {
  while(gi->light_head != TR_null)
    LI_Free(gi, gi->light_head);
  if(gi->torch_noise) {
    TCOD_noise_delete(gi->torch_noise);
    gi->torch_noise = null;
  }
  log("lights freed.");
}

void LI_Update(TR_game_instance_t *gi) {
}

/*
 * sounds
 */

void SO_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create sounds.
  for(int i = 0; i < MAX_SOUND; i++) {
    if(i != MAX_SOUND-1)
      sound(i).fnext = i+1;
    else
      sound(i).fnext = TR_null;
  }
  gi->free_sound_head = 0;
  gi->sound_head      = TR_null;
  log("sounds initialized.");
}

TR_sound_ptr SO_New(TR_game_instance_t *gi) {
  if(gi->free_sound_head != TR_null) {
    TR_sound_ptr r            = gi->free_sound_head;
    gi->free_sound_head       = sound(gi->free_sound_head).fnext;
    sound(r).u                = TR_null;
    sound(r).volume           = 0.2f;
    sound(r).x                = -1;
    sound(r).previous         = TR_null;
    sound(r).next             = gi->sound_head;
    sound(r).handle           = null;
    sound(r).time             = gi->game_time;
    if(gi->sound_head != TR_null)
      sound(gi->sound_head).previous = r;
    gi->sound_head = r;
    return r;
  }
  log("No more sound left.");
  return TR_null;
}

void SO_Free(TR_game_instance_t *gi, TR_sound_ptr l) {
  if(gi->sound_head == l)
    gi->sound_head = sound(l).next;
  if(sound(l).previous != TR_null)
    sound(sound(l).previous).next = sound(l).next;
  if(sound(l).next != TR_null)
    sound(sound(l).next).previous = sound(l).previous;
  sound(l).fnext = gi->free_sound_head;
  gi->free_sound_head = l;
}

void SO_FreeAll(TR_game_instance_t *gi) {
  while(gi->sound_head != TR_null)
    SO_Free(gi, gi->sound_head);
  log("sounds freed.");
}

void SO_Update(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_sound_ptr s = gi->sound_head;
  while(s != TR_null) {
    TR_sound_t *so = &sound(s);
    int dx = 0, dy = 0;
    int x = cam->viewport_x>>POS_SHIFT,
        y = cam->viewport_y>>POS_SHIFT;
    if(so->u != TR_null) {
      TR_unit_t *un = &unit(so->u);
      dx = ((un->x + UN_GetSizeShift(gi, SIZE(un)))>>POS_SHIFT) - x,
      dy = ((un->y + UN_GetSizeShift(gi, SIZE(un)))>>POS_SHIFT) - y;
    } else 
    if(so->x != -1) {
      dx = (so->x>>POS_SHIFT) - x,
      dy = (so->y>>POS_SHIFT) - y;
    }
    ga_handle_setParamf(so->handle, GA_HANDLE_PARAM_GAIN, 
      so->volume*MAX(0, 1.f-sqrt((float)dx*dx + dy*dy)/25));
    ga_handle_setParamf(so->handle, GA_HANDLE_PARAM_PAN, MIN(1, MAX(-1, (float)dx/25)));
    if(so->handle
    && ga_handle_tell(so->handle, GA_TELL_PARAM_CURRENT)
    == ga_handle_tell(so->handle, GA_TELL_PARAM_TOTAL)) {
      TR_sound_ptr s2 = s;
      s = so->next;
      SO_Free(gi, s2);
    } else
      s = so->next;
  }
}

bool SO_WiseToPlay(TR_game_instance_t *gi, const str &tag, int pos_x, int pos_y, int time_diff) {
  TR_sound_ptr s2 = gi->sound_head;
  while(s2 != TR_null) {
    TR_sound_t *so2 = &sound(s2);
    if(so2->tag == tag
    && (time_diff == -1 || ABS(so2->time-gi->game_time) < time_diff)
    && so2->x >= pos_x-SOUND_WISE_RADIUS
    && so2->x <= pos_x+SOUND_WISE_RADIUS
    && so2->y >= pos_y-SOUND_WISE_RADIUS
    && so2->y <= pos_y+SOUND_WISE_RADIUS)
      return false;
    s2 = so2->next;
  }
  return true;
}

/*
 * fluids
 */

void FL_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create fluids.
  for(int i = 0; i < MAX_FLUID; i++) {
    if(i != MAX_FLUID-1)
      fluid(i).fnext = i+1;
    else
      fluid(i).fnext = TR_null;
  }
  gi->free_fluid_head = 0;
  gi->fluid_head      = TR_null;
  log("fluids initialized.");
}

TR_fluid_ptr FL_New(TR_game_instance_t *gi) {
  if(gi->free_fluid_head != TR_null) {
    TR_fluid_ptr r            = gi->free_fluid_head;
    gi->free_fluid_head       = fluid(gi->free_fluid_head).fnext;
    fluid(r).counter          = FLUID_MAX_COUNTER;
    fluid(r).span             = FLUID_SPAN;
    fluid(r).handle           = null;
    fluid(r).diffusion        = FLUID_DIFFUSION;
    fluid(r).color            = FLUID_COLOR;
    DO_TIMES(FLUID_BOX_SIZE_SQUARE) {
      fluid(r).fluid_s[f]        = 0;
      fluid(r).fluid_density[f]  = 0;
      fluid(r).fluid_Vx[f]       = 0;
      fluid(r).fluid_Vy[f]       = 0;
      fluid(r).fluid_Vx0[f]      = 0;
      fluid(r).fluid_Vy0[f]      = 0;
    }
    fluid(r).previous         = TR_null;
    fluid(r).next             = gi->fluid_head;
    if(gi->fluid_head != TR_null)
      fluid(gi->fluid_head).previous = r;
    gi->fluid_head = r;
    return r;
  }
  log("No more fluid left.");
  return TR_null;
}

void FL_Free(TR_game_instance_t *gi, TR_fluid_ptr l) {
  if(fluid(l).handle) 
    CloseHandle(fluid(l).handle);
  if(gi->fluid_head == l)
    gi->fluid_head = fluid(l).next;
  if(fluid(l).previous != TR_null)
    fluid(fluid(l).previous).next = fluid(l).next;
  if(fluid(l).next != TR_null)
    fluid(fluid(l).next).previous = fluid(l).previous;
  fluid(l).fnext = gi->free_fluid_head;
  gi->free_fluid_head = l;
}

void FL_FreeAll(TR_game_instance_t *gi) {
  while(gi->fluid_head != TR_null)
    FL_Free(gi, gi->fluid_head);
  log("fluids freed.");
}

bool FL_Update(TR_game_instance_t *gi, TR_fluid_ptr ff) {
  TR_fluid_t *fl = &fluid(ff);
#if THREADS
  if(!fl->handle
  || WaitForSingleObject(fl->handle, 0) 
  == WAIT_OBJECT_0) {
    if(fl->handle)
      CloseHandle(fl->handle);
    DO_TIMES(FLUID_BOX_SIZE_SQUARE)
      fl->fluid_final_density[f] = fl->fluid_density[f];
    if(fl->span > 0) {
      fl->worker_param.gi = gi;
      fl->worker_param.f = ff;
      fl->worker_param.game_time_step = gi->game_time_step;
      fl->handle = CreateThread(null, 0, FL_WorkerFun, (void*)&fl->worker_param, 0, &fl->id); 
      assert(fl->handle);
      return false;
    } else
      return true;
  }
  return false;
#endif
  return true;
}

DWORD WINAPI FL_WorkerFun(void *param) {
  TR_worker_fluid_param_t *params = (TR_worker_fluid_param_t*)param;
  TR_game_instance_t *gi  = params->gi;
  TR_fluid_ptr f          = params->f;
  TR_fluid_t *fl          = &fluid(f);
  fl->counter--;
  fl->span -= params->game_time_step;
  if(fl->counter > 0) {
    short socle = 10;
    unsigned int seed = TR_GetTime();
    for(int j = -socle/2; j < socle/2; j++)
      for(int i = -socle/2; i < socle/2; i++) {
        int dist = i*i + j*j;
        if(dist < (socle/2)*(socle/2)) {
          short env_x = fl->pos_x+i,
                env_y = fl->pos_y+j;
          if(INSIDE_FLUID(env_x, env_y)) {
            short fl_x = env_x - (fl->pos_x - FLUID_BOX_SIZE/2),
                  fl_y = env_y - (fl->pos_y - FLUID_BOX_SIZE/2);
            if(INSIDE_FLUID2(fl_x, fl_y)) {
              AT_FL(fluid_density, fl_x, fl_y) += 0.3;
              float dis = pow(dist, 2);
              dis = dis == 0 ? 1 : dis;
              float ii = i;
              float jj = j;
              AT_FL(fluid_Vx, fl_x, fl_y) += (float)ii*(16 + (TR_Rand(&seed)&15))*3*pow((FLUID_MAX_COUNTER-fl->counter)*0.1, 1)/dis;
              AT_FL(fluid_Vy, fl_x, fl_y) += (float)jj*(16 + (TR_Rand(&seed)&15))*3*pow((FLUID_MAX_COUNTER-fl->counter)*0.1, 1)/dis;
            }
          }
        } 
      }
  }
  FL_Step(gi, f);
}

void FL_Step(TR_game_instance_t *gi, TR_fluid_ptr f) {
  TR_fluid_t *fl = &fluid(f);
  FL_Diffuse(gi, 1, fl->fluid_Vx0, fl->fluid_Vx, FLUID_VISCOSITY, 1);
  FL_Diffuse(gi, 2, fl->fluid_Vy0, fl->fluid_Vy, FLUID_VISCOSITY, 1);
  
  FL_Project(gi, fl->fluid_Vx0, fl->fluid_Vy0, fl->fluid_Vx, fl->fluid_Vy, 1);
  
  FL_Advect(gi, 1, fl->fluid_Vx, fl->fluid_Vx0, fl->fluid_Vx0, fl->fluid_Vy0);
  FL_Advect(gi, 2, fl->fluid_Vy, fl->fluid_Vy0, fl->fluid_Vx0, fl->fluid_Vy0);
  
  FL_Project(gi, fl->fluid_Vx, fl->fluid_Vy, fl->fluid_Vx0, fl->fluid_Vy0, 1);
  
  FL_Diffuse(gi, 0, fl->fluid_s, fl->fluid_density, fl->diffusion, 1);
  FL_Advect(gi, 0, fl->fluid_density, fl->fluid_s, fl->fluid_Vx, fl->fluid_Vy);
}

void FL_SetBnd(TR_game_instance_t *gi, int b, float *x) {
 for(int i = 1; i < FLUID_BOX_SIZE - 1; i++) {
    AT_FL2(x, i, 0)                 = b == 2 ? -AT_FL2(x, i, 1) : AT_FL2(x, i, 1);
    AT_FL2(x, i, FLUID_BOX_SIZE-1)  = b == 2 ? -AT_FL2(x, i, FLUID_BOX_SIZE-2) : AT_FL2(x, i, FLUID_BOX_SIZE-2);
  }
  for(int j = 1; j < FLUID_BOX_SIZE - 1; j++) {
    AT_FL2(x, 0  , j)           = b == 1 ? -AT_FL2(x, 1, j) : AT_FL2(x, 1, j);
    AT_FL2(x, FLUID_BOX_SIZE-1, j)  = b == 1 ? -AT_FL2(x, FLUID_BOX_SIZE-2, j) : AT_FL2(x, FLUID_BOX_SIZE-2, j);
  }
  AT_FL2(x, 0, 0)                         = 0.5f * (AT_FL2(x, 1, 0)                       + AT_FL2(x, 0, 1));
  AT_FL2(x, 0, FLUID_BOX_SIZE-1)              = 0.5f * (AT_FL2(x, 1, FLUID_BOX_SIZE-1)            + AT_FL2(x, 0, FLUID_BOX_SIZE-2));
  AT_FL2(x, 0, 0)                         = 0.5f * (AT_FL2(x, 1, 0)                       + AT_FL2(x, 0, 1));
  AT_FL2(x, 0, FLUID_BOX_SIZE-1)              = 0.5f * (AT_FL2(x, 1, FLUID_BOX_SIZE-1)            + AT_FL2(x, 0, FLUID_BOX_SIZE-2));
  AT_FL2(x, FLUID_BOX_SIZE-1, 0)              = 0.5f * (AT_FL2(x, FLUID_BOX_SIZE-2, 0)            + AT_FL2(x, FLUID_BOX_SIZE-1, 1));
  AT_FL2(x, FLUID_BOX_SIZE-1, FLUID_BOX_SIZE-1)   = 0.5f * (AT_FL2(x, FLUID_BOX_SIZE-2, FLUID_BOX_SIZE-1) + AT_FL2(x, FLUID_BOX_SIZE-1, FLUID_BOX_SIZE-2));
  AT_FL2(x, FLUID_BOX_SIZE-1, 0)              = 0.5f * (AT_FL2(x, FLUID_BOX_SIZE-2, 0)            + AT_FL2(x, FLUID_BOX_SIZE-1, 1));
  AT_FL2(x, FLUID_BOX_SIZE-1, FLUID_BOX_SIZE-1)   = 0.5f * (AT_FL2(x, FLUID_BOX_SIZE-2, FLUID_BOX_SIZE-1) + AT_FL2(x, FLUID_BOX_SIZE-1, FLUID_BOX_SIZE-2));
}

void FL_LinSolve(TR_game_instance_t *gi, int b, float *x, float *x0, float a, float c, int iter) {
  float cRecip = 1.0 / c;
  float *xx   = x,
        *xx0  = x0;
  for (int k = 0; k < iter; k++) {
    for (int j = 1; j < FLUID_BOX_SIZE - 1; j++)
      for (int i = 1; i < FLUID_BOX_SIZE - 1; i++) {
        AT_FL2(xx, i, j) = 
          (AT_FL2(xx0, i, j) + 
          a * 
          (AT_FL2(xx0, i+1, j+0) +
           AT_FL2(xx0, i-1, j+0) +
           AT_FL2(xx0, i+0, j+1) +
           AT_FL2(xx0, i+0, j-1))) * 
          cRecip;
      }
    FL_SetBnd(gi, b, xx);
    SWAP(float*, xx, xx0);
  }
}

void FL_Diffuse(TR_game_instance_t *gi, int b, float *x, float *x0, float diff, int iter) {
  float a = 0.015 * diff * (FLUID_BOX_SIZE - 2) * (FLUID_BOX_SIZE - 2);
  FL_LinSolve(gi, b, x, x0, a, 1 + 6 * a, iter);
}

void FL_Project(TR_game_instance_t *gi, float *velocX, float *velocY, float *p, float *div, int iter) {
  for (int j = 1; j < FLUID_BOX_SIZE - 1; j++) 
    for (int i = 1; i < FLUID_BOX_SIZE - 1; i++) {
      AT_FL2(div, i, j) = -0.5f*(
              +AT_FL2(velocX, i+1, j)
              -AT_FL2(velocX, i-1, j)
              +AT_FL2(velocY, i, j+1)
              -AT_FL2(velocY, i, j-1)
          )/FLUID_BOX_SIZE;
      AT_FL2(p, i, j) = 0;
    }
  FL_SetBnd(gi, 0, div); 
  FL_SetBnd(gi, 0, p);
  FL_LinSolve(gi, 0, p, div, 1, 6, iter);
  
  for (int j = 1; j < FLUID_BOX_SIZE - 1; j++) 
    for (int i = 1; i < FLUID_BOX_SIZE - 1; i++) {
      AT_FL2(velocX, i, j) -= 0.5f * (AT_FL2(p, i+1, j) - AT_FL2(p, i-1, j)) * FLUID_BOX_SIZE;
      AT_FL2(velocY, i, j) -= 0.5f * (AT_FL2(p, i, j+1) - AT_FL2(p, i, j-1)) * FLUID_BOX_SIZE;
    }
  FL_SetBnd(gi, 1, velocX);
  FL_SetBnd(gi, 2, velocY);
}

void FL_Advect(TR_game_instance_t *gi, int b, float *d, float *d0,  float *velocX, float *velocY) {
  float i0, i1, j0, j1, k0, k1;
  
  float dtx = 0.015 * (FLUID_BOX_SIZE - 2);
  float dty = 0.015 * (FLUID_BOX_SIZE - 2);
  
  float s0, s1, t0, t1, u0, u1;
  float tmp1, tmp2, x, y;
  
  float Nfloat = FLUID_BOX_SIZE-2;
  float ifloat, jfloat;
  int i, j;
  
  for(j = 1, jfloat = 1; j < FLUID_BOX_SIZE - 1; j++, jfloat++)
    for(i = 1, ifloat = 1; i < FLUID_BOX_SIZE - 1; i++, ifloat++) {
      tmp1 = dtx * AT_FL2(velocX, i, j);
      tmp2 = dty * AT_FL2(velocY, i, j);
      x    = ifloat - tmp1; 
      y    = jfloat - tmp2;
      if(x < 0.5f) x = 0.5f; 
      if(x > Nfloat + 0.5f) x = Nfloat + 0.5f; 
      i0 = floorf(x); 
      i1 = i0 + 1.0f;
      if(y < 0.5f) y = 0.5f; 
      if(y > Nfloat + 0.5f) y = Nfloat + 0.5f; 
      j0 = floorf(y);
      j1 = j0 + 1.0f; 
      s1 = x - i0; 
      s0 = 1.0f - s1; 
      t1 = y - j0; 
      t0 = 1.0f - t1;
      int i0i = i0;
      int i1i = i1;
      int j0i = j0;
      int j1i = j1;
      AT_FL2(d, i, j) = 
        s0 * (t0 * AT_FL2(d0, i0i, j0i) + t1 * AT_FL2(d0, i0i, j1i)) +
        s1 * (t0 * AT_FL2(d0, i1i, j0i) + t1 * AT_FL2(d0, i1i, j1i));
    }
  FL_SetBnd(gi, b, d);
}

/*
 * explosions
 */

void EX_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create explosions.
  for(int i = 0; i < MAX_EXPLOSION; i++) {
    if(i != MAX_EXPLOSION-1)
      explosion(i).fnext = i+1;
    else
      explosion(i).fnext = TR_null;
  }
  gi->free_explosion_head = 0;
  gi->explosion_head      = TR_null;
  log("explosions initialized.");
}

TR_explosion_ptr EX_New(TR_game_instance_t *gi) {
  if(gi->free_explosion_head != TR_null) {
    TR_explosion_ptr r           = gi->free_explosion_head;
    gi->free_explosion_head       = explosion(gi->free_explosion_head).fnext;
    if(gi->explosion_head == TR_null) {
      explosion(r).previous = TR_null;
      explosion(r).next     = TR_null;
      gi->explosion_head    = r;
    } else {
      TR_explosion_ptr e = gi->explosion_head;
      while(explosion(e).next != TR_null)
        e = explosion(e).next;
      explosion(r).previous = e;
      explosion(r).next     = TR_null;
      explosion(e).next     = r;
    }
    explosion(r).span             = 0;
    explosion(r).current_frame    = 0;
    explosion(r).additive         = false;
    explosion(r).ps               = TR_null;
    explosion(r).node             = null;
    return r;
  }
  log("No more explosion left.");
  return TR_null;
}

void EX_Free(TR_game_instance_t *gi, TR_explosion_ptr l) {
  TR_explosion_t *ex = &explosion(l);
  if(ex->mesh_name != "") {
    assert(ex->node);
    DO_TIMES(ex->total_frame)
      ex->node->detachObject(ex->ent[f]);
    gi->ogre_scene->getRootSceneNode()->removeChild(ex->node);
    DO_TIMES(ex->total_frame)
      gi->ogre_scene->destroyEntity(ex->ent[f]);
    gi->ogre_scene->destroySceneNode(ex->node);
  }
  if(gi->explosion_head == l)
    gi->explosion_head = explosion(l).next;
  if(explosion(l).previous != TR_null)
    explosion(explosion(l).previous).next = explosion(l).next;
  if(explosion(l).next != TR_null)
    explosion(explosion(l).next).previous = explosion(l).previous;
  explosion(l).fnext = gi->free_explosion_head;
  gi->free_explosion_head = l;
}

void EX_FreeAll(TR_game_instance_t *gi) {
  while(gi->explosion_head != TR_null)
    EX_Free(gi, gi->explosion_head);
  log("explosions freed.");
}

bool EX_Update(TR_game_instance_t *gi, TR_explosion_ptr b) {
  TR_explosion_t *ex = &explosion(b);
  if(!ex->node
  && ex->mesh_name != "") {
    static int mat_id = 0;
    ex->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("ex node " + i2a(b));
    assert(ex->size > 0);
    ex->node->setPosition(Ogre::Vector3((float)ex->x/(1<<POS_SHIFT), 0.01f*(MAX_UNIT*3+b), (float)ex->y/(1<<POS_SHIFT)));
    ex->node->setScale(Ogre::Vector3(0.25*ex->size, 0.25*ex->size, 0.25*ex->size));
    DO_TIMES(ex->total_frame) {
      int i = f;
      ex->ent[i] = gi->ogre_scene->createEntity("ex " + i2a(b) + " " + i2a(f), ex->mesh_name + (f > 0 ? i2a(f+1) : "") + ".mesh");
      ex->ent[i]->setVisible(i == 0);
      ex->ent[i]->setQueryFlags(0);
      ex->ent[i]->setCastShadows(false);
      ex->ent[i]->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
      ex->node->attachObject(ex->ent[i]);
      DO_TIMES(ex->ent[i]->getNumSubEntities()) {
        if(!ex->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState("FovMapTex")) {
          Ogre::TextureUnitState *tex = 
            ex->ent[i]->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      createTextureUnitState("FovMapTex");
          tex->setName("FovMapTex");
          tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        }
        ex->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false);
        ex->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTextureCadaver", false);
        if(ex->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 0)
          ex->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(0)->
                      setTextureFiltering(Ogre::TFO_NONE);
        if(ex->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 1)
          ex->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(1)->
                      setTextureFiltering(Ogre::TFO_NONE);
        Ogre::MaterialPtr mat = 
          ex->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                clone("ex " + i2a(mat_id) + " mat");
        assert(mat.get());
        mat->compile();
        ex->ent[i]->
          getSubEntity(f)->
            setMaterialName("ex " + i2a(mat_id) + " mat");
        mat_id++;
        Ogre::GpuProgramParametersSharedPtr params = 
          ex->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getFragmentProgramParameters();
        assert(params.get());
        params->setNamedConstant("alpha", 1.f);
      }
    }
  }
  ex->span += gi->game_time_step;
  if(ex->span >= EXPLOSION_FRAME_SPAN) {
    ex->span -= EXPLOSION_FRAME_SPAN;
    if(ex->node)
      ex->ent[ex->current_frame]->setVisible(false);
    ex->current_frame++;
  }
  if(ex->current_frame >= ex->total_frame) {
    if(ex->ps != TR_null)
      PS_Stop(gi, ex->ps);
  } else {
    if(ex->node)
      ex->ent[ex->current_frame]->setVisible(true);
  }
  return ex->current_frame >= ex->total_frame; // done?
}

/*
 * build_explosions
 */

void BE_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create build_explosions.
  for(int i = 0; i < MAX_BUILD_EXPLOSION; i++) {
    if(i != MAX_BUILD_EXPLOSION-1)
      build_explosion(i).fnext = i+1;
    else
      build_explosion(i).fnext = TR_null;
  }
  gi->free_build_explosion_head = 0;
  gi->build_explosion_head      = TR_null;
  log("build_explosions initialized.");
}

TR_build_explosion_ptr BE_New(TR_game_instance_t *gi) {
  if(gi->free_build_explosion_head != TR_null) {
    TR_build_explosion_ptr r            = gi->free_build_explosion_head;
    gi->free_build_explosion_head       = build_explosion(gi->free_build_explosion_head).fnext;
    build_explosion(r).previous         = TR_null;
    build_explosion(r).next             = gi->build_explosion_head;
    build_explosion(r).span             = BUILD_EXPLOSION_SPAN;
    build_explosion(r).state            = 0;
    build_explosion(r).size             = 3;
    build_explosion(r).c                = TR_null;
    build_explosion(r).mesh_count       = 0;
    if(gi->build_explosion_head != TR_null)
      build_explosion(gi->build_explosion_head).previous = r;
    gi->build_explosion_head = r;
    return r;
  }
  log("No more build_explosion left.");
  return TR_null;
}

void BE_Free(TR_game_instance_t *gi, TR_build_explosion_ptr l) {
  if(gi->build_explosion_head == l)
    gi->build_explosion_head = build_explosion(l).next;
  if(build_explosion(l).previous != TR_null)
    build_explosion(build_explosion(l).previous).next = build_explosion(l).next;
  if(build_explosion(l).next != TR_null)
    build_explosion(build_explosion(l).next).previous = build_explosion(l).previous;
  build_explosion(l).fnext = gi->free_build_explosion_head;
  gi->free_build_explosion_head = l;
}

void BE_FreeAll(TR_game_instance_t *gi) {
  while(gi->build_explosion_head != TR_null)
    BE_Free(gi, gi->build_explosion_head);
  log("build_explosions freed.");
}

bool BE_Update(TR_game_instance_t *gi, TR_build_explosion_ptr b) {
  unsigned int seed = TR_GetTime();
  TR_build_explosion_t *be = &build_explosion(b);
  assert(be->mesh_count > 0);
  if(be->span == BUILD_EXPLOSION_SPAN) {
    TR_cadaver_ptr c = CA_New(gi);
    if(c != TR_null) {
      TR_cadaver_t *ca = &cadaver(c);
      ca->mesh_count = be->mesh_count;
      DO_TIMES(ca->mesh_count)
        ca->ent_name[f] = be->ent_name[f];
      ca->x = be->x - UN_GetSizeShiftFloat(gi, be->size);
      ca->y = be->y - UN_GetSizeShiftFloat(gi, be->size);
      ca->size = be->size;
      ca->model_scale = be->model_scale;
      be->c = c;
    }
    //TR_explosion_ptr e = EX_New(gi);
    //if(e != TR_null) {
    //  TR_explosion_t *ex = &explosion(e);
    //  ex->x = be->x;
    //  ex->y = be->y;
    //  START_PARTICLE("beblackhole", be->x, be->y, 1, 0.3)
    //  ex->ps = ps;
    //  ex->span = 500;
    //}
    be->state++;
  } else 
  if(be->span <= 2*BUILD_EXPLOSION_SPAN/3
  && be->state == 1) {
    /* fade cadaver */
    TR_cadaver_t *ca = &cadaver(be->c);
    ca->fade = true;
    /* explosion */
    TR_explosion_ptr e = EX_New(gi);
    if(e != TR_null) {
      TR_explosion_t *ex = &explosion(e);
      ex->x = be->x;
      ex->y = be->y;
      START_PARTICLE("besmoke", be->x, be->y, 1, 1)
      ex->ps = ps;
      ex->span = 500;
    }
    //e = EX_New(gi);
    //if(e != TR_null) {
    //  TR_explosion_t *ex = &explosion(e);
    //  ex->x = be->x;
    //  ex->y = be->y;
    //  //START_PARTICLE("beexplosion", be->x, be->y, 1, 1)
    //  START_PARTICLE("house_explosion", be->x, be->y, 1, 0.06)
    //  ex->ps = ps;
    //  ex->span = 500;
    //}
    //e = EX_New(gi);
    //if(e != TR_null) {
    //  TR_explosion_t *ex = &explosion(e);
    //  ex->x = be->x;
    //  ex->y = be->y;
    //  START_PARTICLE("bemagy", be->x, be->y, 1, 2)
    //  particle_system(ps).ogre_particle->setScaleTime(2);
    //  ex->ps = ps;
    //  ex->span = 500;
    //}
    //TR_burn_trace_ptr b = BT_New(gi);
    //TR_burn_trace_t *bt = &burn_trace(b);
    //bt->x = be->x;
    //bt->y = be->y;
    be->state++;
  } else 
  if(be->span <= BUILD_EXPLOSION_SPAN/3
  && be->state == 2) {
    be->state++;
  } else 
  if(be->span <= BUILD_EXPLOSION_SPAN/12
  && be->state == 3) {
    be->state++;
  }
  be->span -= gi->game_time_step;
  return be->span <= 0;
}

/*
 * cadavers
 */

void CA_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create cadavers.
  for(int i = 0; i < MAX_CADAVER; i++) {
    if(i != MAX_CADAVER-1)
      cadaver(i).fnext = i+1;
    else
      cadaver(i).fnext = TR_null;
  }
  gi->free_cadaver_head = 0;
  gi->cadaver_head      = TR_null;
  log("cadavers initialized.");
}

TR_cadaver_ptr CA_New(TR_game_instance_t *gi) {
  if(gi->free_cadaver_head != TR_null) {
    TR_cadaver_ptr r            = gi->free_cadaver_head;
    gi->free_cadaver_head       = cadaver(gi->free_cadaver_head).fnext;
    cadaver(r).previous         = TR_null;
    cadaver(r).next             = gi->cadaver_head;
    cadaver(r).span             = CADAVER_FADE_SPAN;
    cadaver(r).node             = null;
    cadaver(r).anim_name        = "";
    cadaver(r).fade             = false;
    cadaver(r).size             = 1;
    cadaver(r).lookat_x         = -1;
    cadaver(r).lookat_y         = 0;
    cadaver(r).mesh_count       = 0;
    cadaver(r).flying           = false;
    cadaver(r).force_x          = 0;
    cadaver(r).force_y          = 0;
    DO_TIMES(MAX_UNIT_ENTITY)
      cadaver(r).anim_state[f] = null;
    if(gi->cadaver_head != TR_null)
      cadaver(gi->cadaver_head).previous = r;
    gi->cadaver_head = r;
    return r;
  }
  log("No more cadaver left.");
  return TR_null;
}

void CA_Free(TR_game_instance_t *gi, TR_cadaver_ptr l) {
  TR_cadaver_t *ca = &cadaver(l);
  DO_TIMES(1)
    ca->node->detachObject(ca->ent[f]);
  gi->ogre_scene->getRootSceneNode()->removeChild(ca->node);
  DO_TIMES(1)
    gi->ogre_scene->destroyEntity(ca->ent[f]);
  gi->ogre_scene->destroySceneNode(ca->node);
  if(gi->cadaver_head == l)
    gi->cadaver_head = cadaver(l).next;
  if(cadaver(l).previous != TR_null)
    cadaver(cadaver(l).previous).next = cadaver(l).next;
  if(cadaver(l).next != TR_null)
    cadaver(cadaver(l).next).previous = cadaver(l).previous;
  cadaver(l).fnext = gi->free_cadaver_head;
  gi->free_cadaver_head = l;
}

void CA_FreeAll(TR_game_instance_t *gi) {
  while(gi->cadaver_head != TR_null)
    CA_Free(gi, gi->cadaver_head);
  log("cadavers freed.");
}

bool CA_Update(TR_game_instance_t *gi, TR_cadaver_ptr b) {
  TR_cadaver_t *ca = &cadaver(b);
  assert(ca->mesh_count > 0);
  if(ca->node == null) {
    static int mat_id = 0;
    ca->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("ca node " + i2a(b));
    ca->node->setPosition(Ogre::Vector3((float)ca->x/(1<<POS_SHIFT), ca->y_offset + b*0.01, (float)ca->y/(1<<POS_SHIFT)));
    ca->node->setScale(Ogre::Vector3((ca->lookat_x > 0 ? -1.f : 1.f)*0.25*ca->size*ca->model_scale, 0.25*ca->size*ca->model_scale, 0.25*ca->size*ca->model_scale));
    ca->node->setOrientation(ca->ori * Ogre::Quaternion(Ogre::Degree(ca->y_angle), Ogre::Vector3(0, 1, 0)));
    /*if(ca->lookat_x != -1)
      ca->node->lookAt(
        Ogre::Vector3(ca->lookat_x, ca->y_offset + (ca->flying ? FLYING_HEIGHT : 0.01f), ca->lookat_y),
        Ogre::Node::TS_WORLD,
        Ogre::Vector3::UNIT_Z);*/
    DO_TIMES(1) {
      int i = f;
      ca->ent[i] = gi->ogre_scene->createEntity("ca " + i2a(b) + " " + i2a(f), ca->ent_name[f] + "cadaver.mesh");
      ca->ent[i]->setQueryFlags(0);
      ca->ent[i]->setCastShadows(false);
      ca->ent[i]->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
      if(ca->anim_name != "")
        PLAY_ANIM(ca->ent[i], ca->anim_state[f], ca->anim_name, false)
      ca->node->attachObject(ca->ent[i]);
      DO_TIMES(ca->ent[i]->getNumSubEntities()) {
        if(!ca->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState("FovMapTex")) {
          Ogre::TextureUnitState *tex = 
            ca->ent[i]->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      createTextureUnitState("FovMapTex");
          tex->setName("FovMapTex");
          tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        }
        ca->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false);
        ca->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTextureCadaver", false);
        if(ca->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 0)
          ca->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(0)->
                      setTextureFiltering(Ogre::TFO_NONE);
        if(ca->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 1)
          ca->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(1)->
                      setTextureFiltering(Ogre::TFO_NONE);
        Ogre::MaterialPtr mat = 
          ca->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                clone("ca " + i2a(mat_id) + " mat");
        assert(mat.get());
        mat->compile();
        ca->ent[i]->
          getSubEntity(f)->
            setMaterialName("ca " + i2a(mat_id) + " mat");
        mat_id++;
        Ogre::GpuProgramParametersSharedPtr params = 
          ca->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getFragmentProgramParameters();
        assert(params.get());
        params->setNamedConstant("alpha", 1.f);
      }
    }
  }
  ca->x += ca->force_x;
  ca->y += ca->force_y;
  ca->force_x = 90 * ca->force_x / 100;
  ca->force_y = 90 * ca->force_y / 100;
  ca->node->setPosition(
    Ogre::Vector3(
      (float)ca->x/(1<<POS_SHIFT), 
      ca->y_offset + b*0.01, 
      (float)ca->y/(1<<POS_SHIFT)));
  DO_TIMES(1)
    if(ca->anim_name != ""
    && ca->ent[f]->hasSkeleton()
    && ca->anim_state[f])
      ca->anim_state[f]->addTime((float)gi->game_time_step*0.001f*ANIMATION_SPEED_FACTOR);
  if(ca->fade) {
    ca->span -= gi->game_time_step;
    DO_TIMES(1) {
      int i = f;
      DO_TIMES(ca->ent[i]->getNumSubEntities()) {
        Ogre::GpuProgramParametersSharedPtr params = 
          ca->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getFragmentProgramParameters();
        assert(params.get());
        params->setNamedConstant("alpha", MIN(1.f, (float)ca->span/1000));
      }
    }
  }
  return ca->span <= 0;
}

/*
 * lightnings
 */

void LG_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create lightnings.
  for(int i = 0; i < MAX_LIGHTNING; i++) {
    if(i != MAX_LIGHTNING-1)
      lightning(i).fnext = i+1;
    else
      lightning(i).fnext = TR_null;
  }
  gi->free_lightning_head = 0;
  gi->lightning_head      = TR_null;
  log("lightnings initialized.");
}

TR_lightning_ptr LG_New(TR_game_instance_t *gi) {
  if(gi->free_lightning_head != TR_null) {
    TR_lightning_ptr r            = gi->free_lightning_head;
    gi->free_lightning_head       = lightning(gi->free_lightning_head).fnext;
    lightning(r).previous         = TR_null;
    lightning(r).next             = gi->lightning_head;
    lightning(r).span             = LIGHTNING_SPAN;
    lightning(r).color            = LIGHTNING_COLOR;
    lightning(r).opacity          = 1;
    lightning(r).seed             = TR_GetTime();
    if(gi->lightning_head != TR_null)
      lightning(gi->lightning_head).previous = r;
    gi->lightning_head = r;
    return r;
  }
  log("No more lightning left.");
  return TR_null;
}

void LG_Free(TR_game_instance_t *gi, TR_lightning_ptr l) {
  if(gi->lightning_head == l)
    gi->lightning_head = lightning(l).next;
  if(lightning(l).previous != TR_null)
    lightning(lightning(l).previous).next = lightning(l).next;
  if(lightning(l).next != TR_null)
    lightning(lightning(l).next).previous = lightning(l).previous;
  lightning(l).fnext = gi->free_lightning_head;
  gi->free_lightning_head = l;
}

void LG_FreeAll(TR_game_instance_t *gi) {
  while(gi->lightning_head != TR_null)
    LG_Free(gi, gi->lightning_head);
  log("lightnings freed.");
}

bool LG_Update(TR_game_instance_t *gi, TR_lightning_ptr b) {
  TR_lightning_t *lg = &lightning(b);
  lg->span -= gi->game_time_step;
  lg->opacity = SATURATE((float)lg->span / LIGHTNING_SPAN);
  return lg->span <= 0;
}

void LG_Compute(TR_game_instance_t *gi, TR_lightning_ptr l) {
  TR_lightning_t *lg = &lightning(l);
  LG_BuildPoints(gi, l, lg->start_x, lg->start_y, lg->end_x, lg->end_y, 0, MAX_LIGHTNING_POINT-1, 1);
  lg->point_count = 0;
  DO_TIMES(MAX_LIGHTNING_POINT-1) {
    int x0 = lg->pos_x[f+0],
        y0 = lg->pos_y[f+0],
        x1 = lg->pos_x[f+1],
        y1 = lg->pos_y[f+1];
    TCOD_line_init(x0, y0, x1, y1);
    while(!TCOD_line_step(&x0, &y0)) {
      lg->point_count++;
    }
  }
}

void LG_BuildPoints(TR_game_instance_t *gi, TR_lightning_ptr l, int x0, int y0, int x1, int y1, int i, int j, int level) {
  TR_lightning_t *lg = &lightning(l);
  if(j-i > 1) {
    int count = j-i-2 >= LIGHTNING_STEP_COUNT ? LIGHTNING_STEP_COUNT : 1+TR_Rand(&lg->seed)%(j-i-1),
        step = (j-i)/(count+1),
        dx  = x1-x0,
        dy  = y1-y0,
        nx  = -dy,
        ny  = dx;
    DO_TIMES(count) {
      int k = i+(f+1)*step;
      lg->pos_x[k] = x0 + dx*(k-i)/(j-i) + ((TR_Rand(&lg->seed)&1023)-512)*(nx>>3)/512;
      lg->pos_y[k] = y0 + dy*(k-i)/(j-i) + ((TR_Rand(&lg->seed)&1023)-512)*(ny>>3)/512;
    }
    lg->pos_x[i] = x0;
    lg->pos_y[i] = y0;
    lg->pos_x[j] = x1;
    lg->pos_y[j] = y1;
    LG_BuildPoints(gi, l, lg->pos_x[i], lg->pos_y[i], lg->pos_x[i+step], lg->pos_y[i+step], i, i+step, level);
    LG_BuildPoints(gi, l, lg->pos_x[i+count*step], lg->pos_y[i+count*step], lg->pos_x[j], lg->pos_y[j], i+count*step, j, level);
    DO_TIMES(count-1) 
      LG_BuildPoints(gi, l, lg->pos_x[i+(f+1)*step], lg->pos_y[i+(f+1)*step], lg->pos_x[i+(f+2)*step], lg->pos_y[i+(f+2)*step], i+(f+1)*step, i+(f+2)*step, level);
  }
}

/*
 * status_effects
 */

void ST_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create status_effects.
  for(int i = 0; i < MAX_STATUS_EFFECT; i++) {
    if(i != MAX_STATUS_EFFECT-1)
      status_effect(i).fnext = i+1;
    else
      status_effect(i).fnext = TR_null;
  }
  gi->free_status_effect_head = 0;
  log("status_effects initialized.");
}

TR_status_effect_ptr ST_New(TR_game_instance_t *gi, TR_unit_ptr u, int type, int span, bool evil) {
  if(gi->free_status_effect_head != TR_null) {
    TR_unit_t *un                     = &unit(u);
    TR_status_effect_ptr r            = gi->free_status_effect_head;
    gi->free_status_effect_head       = status_effect(gi->free_status_effect_head).fnext;
    TR_status_effect_t *st            = &status_effect(r);
    st->previous                      = TR_null;
    st->next                          = un->status_effect_head;
    st->type                          = type;
    st->span                          = span;
    st->evil                          = evil;
    st->ps                            = TR_null;
    st->buf                           = false;
    st->buf_overlay = Ogre::OverlayManager::getSingleton().create("buf " + i2a(r));
    st->buf_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "buf " + i2a(r) + " panel"));
    st->buf_overlay->hide();
    if(un->status_effect_head != TR_null)
      status_effect(un->status_effect_head).previous = r;
    un->status_effect_head = r;
    switch(st->type) {
      #include "start_status_effect.cpp"
    }
    return r;
  }
  log("No more status_effect left.");
  return TR_null;
}

void ST_Free(TR_game_instance_t *gi, TR_unit_ptr u, TR_status_effect_ptr l) {
  TR_unit_t *un = &unit(u);
  TR_status_effect_t *st = &status_effect(l);
  if(st->ps != TR_null)
    PS_Stop(gi, st->ps);
  switch(st->type) {
    #include "leave_status_effect.cpp"
  }
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("buf " + i2a(l) + " panel");
  Ogre::OverlayManager::getSingleton().destroy(st->buf_overlay);
  if(un->status_effect_head == l)
    un->status_effect_head = status_effect(l).next;
  if(status_effect(l).previous != TR_null)
    status_effect(status_effect(l).previous).next = status_effect(l).next;
  if(status_effect(l).next != TR_null)
    status_effect(status_effect(l).next).previous = status_effect(l).previous;
  status_effect(l).fnext = gi->free_status_effect_head;
  gi->free_status_effect_head = l;
}

void ST_FreeAll(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  while(un->status_effect_head != TR_null)
    ST_Free(gi, u, un->status_effect_head);
}

void ST_Update(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_player_t *pl = &player(p);
  if(un->hp > 0) {
    TR_status_effect_ptr s = un->status_effect_head;
    while(s != TR_null) {
      TR_status_effect_t *st = &status_effect(s);
      switch(st->type) {
        #include "status_effect.cpp"
      }
      st->span -= gi->game_time_step;
      if(st->span <= 0) {
        TR_status_effect_ptr s2 = s;
        s = st->next;
        ST_Free(gi, u, s2);
      } else
        s = st->next;
    }
  }
}

void ST_Remove(TR_game_instance_t *gi, TR_unit_ptr u, int type) {
  TR_unit_t *un = &unit(u);
  TR_status_effect_ptr s = un->status_effect_head;
  while(s != TR_null) {
    TR_status_effect_t *st = &status_effect(s);
    if(st->type == type) {
      ST_Free(gi, u, s);
      break;
    }
    s = st->next;
  }
}

int ST_GetBufCount(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  int result = 0;
  TR_status_effect_ptr s = un->status_effect_head;
  while(s != TR_null) {
    TR_status_effect_t *st = &status_effect(s);
    if(st->buf)
      result++;
    s = st->next;
  }
  return result;
}

void ST_SetBufPos(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, Ogre::Vector2 pos, bool vis) {
  TR_unit_t *un = &unit(u);
  int count = ST_GetBufCount(gi, u),
      offset = 0;
  TR_status_effect_ptr s = un->status_effect_head;
  while(s != TR_null) {
    TR_status_effect_t *st = &status_effect(s);
    if(vis
    && st->buf
    && (pos.x != -1 || pos.y != -1)) {
      Ogre::Vector2 pos2 = pos;
      pos2.x *= gi->ogre_window->getWidth();
      pos2.y *= gi->ogre_window->getHeight();
      offset++;
      Ogre::OverlayContainer *panel = st->buf_overlay->getChild("buf " + i2a(s) + " panel");
      assert(panel);
      panel->setLeft(pos2.x - count*32/2 + offset*32 - 32);
      panel->setTop(pos2.y - 32);
      if(INSIDE_SCREEN(panel->getLeft(), panel->getTop())
      || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop())
      || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop()+panel->getHeight())
      || INSIDE_SCREEN(panel->getLeft(), panel->getTop()+panel->getHeight())) {
        panel->setMetricsMode(Ogre::GMM_PIXELS);
        panel->setWidth(32);
        panel->setHeight(32);
        panel->setMaterialName(st->buf_material);
        st->buf_overlay->setZOrder(99);
        #if !MINIMAP_MODE
        st->buf_overlay->show();
        #else
        st->buf_overlay->hide();
        #endif
      } else 
        st->buf_overlay->hide();
    } else
      st->buf_overlay->hide();
    s = st->next;
  }
}

/*
 * build order
 */

void BO_Init(TR_game_instance_t *gi,int argc, char** argv) {
  for(int i = 0; i < MAX_BUILD_ORDER; i++) {
    if(i != MAX_BUILD_ORDER-1)
      build_order(i).fnext = i+1;
    else
      build_order(i).fnext = TR_null;
  }
  gi->free_build_order_head = 0;
  log("build_orders initialized.");
}

TR_build_order_ptr BO_New(TR_game_instance_t *gi) {
  if(gi->free_build_order_head != TR_null) {
    TR_build_order_ptr r           = gi->free_build_order_head;
    gi->free_build_order_head  = build_order(gi->free_build_order_head).fnext;
    build_order(r).previous    = TR_null;
    build_order(r).next        = TR_null;
    build_order(r).progress    = 0;
    build_order(r).started     = false;
    return r;
  }
  log("No more build_order left.");
  return TR_null;
}

void BO_Free(TR_game_instance_t *gi, TR_unit_ptr u, TR_build_order_ptr b) {
  TR_unit_t *un = &unit(u);
  TR_build_order_t *bo = &build_order(b);
  if(un->build_order_head[bo->unit_player_id] == b)
    un->build_order_head[bo->unit_player_id] = build_order(b).next;
  if(build_order(b).previous != TR_null)
    build_order(build_order(b).previous).next = build_order(b).next;
  if(build_order(b).next != TR_null)
    build_order(build_order(b).next).previous = build_order(b).previous;
  build_order(b).fnext = gi->free_build_order_head;
  gi->free_build_order_head = b;
}

void BO_FreeAll(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  DO_TIMES(MAX_PLAYER)
    while(un->build_order_head[f] != TR_null)
      BO_Free(gi, u, un->build_order_head[f]);
}

void BO_PushFrontBuildOrder(TR_game_instance_t *gi, TR_unit_ptr u, TR_build_order_ptr b) {
  TR_unit_t *un           = &unit(u);
  TR_build_order_t *bo = &build_order(b);
  build_order(b).next     = un->build_order_head[bo->unit_player_id];
  if(un->build_order_head[bo->unit_player_id] != TR_null)
    build_order(un->build_order_head[bo->unit_player_id]).previous = b;
  un->build_order_head[bo->unit_player_id] = b;
}

void BO_PushBackBuildOrder(TR_game_instance_t *gi, TR_unit_ptr u, TR_build_order_ptr b) {
  TR_unit_t *un = &unit(u);
  TR_build_order_t *bo = &build_order(b);
  if(un->build_order_head[bo->unit_player_id] == TR_null) {
    un->build_order_head[bo->unit_player_id] = b;
  } else {
    TR_build_order_ptr b2 = un->build_order_head[bo->unit_player_id];
    while(build_order(b2).next != TR_null)
      b2 = build_order(b2).next;
    build_order(b2).next = b;
    build_order(b).previous = b2;
  }
}

short BO_GetBuildOrderCount(TR_game_instance_t *gi, TR_id_t player_id, TR_client_ptr c, TR_unit_ptr u) {
  TR_client_t *cl = &client(c);
  TR_unit_t *un = &unit(u);
  TR_build_order_ptr b = un->build_order_head[player_id];
  int i = 0;
  while(b != TR_null) {
    i++;
    b = build_order(b).next;
  }
  TR_cmd_store_t *cs = &cmd_store(cl->cs);
  TR_cmd_ptr cm = cs->cmd_head;
  while(cm != TR_null) {
    TR_cmd_t *cmd = &cmd(cm);
    if(cmd->id == un->cmd_id
    && cmd->type == TR_cmd_type_build_unit
    && !cmd->cancel_it)
      i++;
    cm = cmd->cmd_next;
  }
  return i;
}

/*
 * trees
 */

void TR_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create trees.
  for(int i = 0; i < MAX_TREE; i++) {
    if(i != MAX_TREE-1)
      tree(i).fnext = i+1;
    else
      tree(i).fnext = TR_null;
  }
  gi->free_tree_head = 0;
  gi->tree_head      = TR_null;
  log("trees initialized.");
}

TR_tree_ptr TR_New(TR_game_instance_t *gi) {
  if(gi->free_tree_head != TR_null) {
    TR_tree_ptr r            = gi->free_tree_head;
    gi->free_tree_head       = tree(gi->free_tree_head).fnext;
    tree(r).previous         = TR_null;
    tree(r).next             = gi->tree_head;
    if(gi->tree_head != TR_null)
      tree(gi->tree_head).previous = r;
    gi->tree_head = r;
    return r;
  }
  log("No more tree left.");
  return TR_null;
}

void TR_Free(TR_game_instance_t *gi, TR_tree_ptr l) {
  if(gi->tree_head == l)
    gi->tree_head = tree(l).next;
  if(tree(l).previous != TR_null)
    tree(tree(l).previous).next = tree(l).next;
  if(tree(l).next != TR_null)
    tree(tree(l).next).previous = tree(l).previous;
  tree(l).fnext = gi->free_tree_head;
  gi->free_tree_head = l;
}

void TR_FreeAll(TR_game_instance_t *gi) {
  while(gi->tree_head != TR_null)
    TR_Free(gi, gi->tree_head);
  log("trees freed.");
}

/*
 * unit_nodes
 */

void NO_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create unit_nodes.
  for(int i = 0; i < MAX_UNIT_NODE; i++) {
    if(i != MAX_UNIT_NODE-1)
      unit_node(i).fnext = i+1;
    else
      unit_node(i).fnext = TR_null;
  }
  gi->free_unit_node_head = 0;
  gi->unit_node_head      = TR_null;
  log("unit_nodes initialized.");
}

TR_unit_node_ptr NO_New(TR_game_instance_t *gi) {
  if(gi->free_unit_node_head != TR_null) {
    TR_unit_node_ptr r            = gi->free_unit_node_head;
    gi->free_unit_node_head       = unit_node(gi->free_unit_node_head).fnext;
    unit_node(r).previous         = TR_null;
    unit_node(r).next             = gi->unit_node_head;
    unit_node(r).mnext            = TR_null;
    unit_node(r).u                = TR_null;
    if(gi->unit_node_head != TR_null)
      unit_node(gi->unit_node_head).previous = r;
    gi->unit_node_head = r;
    return r;
  }
  log("No more unit_node left.");
  return TR_null;
}

void NO_Free(TR_game_instance_t *gi, TR_unit_node_ptr l) {
  if(gi->unit_node_head == l)
    gi->unit_node_head = unit_node(l).next;
  if(unit_node(l).previous != TR_null)
    unit_node(unit_node(l).previous).next = unit_node(l).next;
  if(unit_node(l).next != TR_null)
    unit_node(unit_node(l).next).previous = unit_node(l).previous;
  unit_node(l).fnext = gi->free_unit_node_head;
  gi->free_unit_node_head = l;
}

void NO_FreeAll(TR_game_instance_t *gi) {
  while(gi->unit_node_head != TR_null)
    NO_Free(gi, gi->unit_node_head);
  log("unit_nodes freed.");
}

/*
 * animations
 */

void AN_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // create animations.
  for(int i = 0; i < MAX_ANIMATION; i++) {
    if(i != MAX_ANIMATION-1)
      animation(i).fnext = i+1;
    else
      animation(i).fnext = TR_null;
  }
  gi->free_animation_head = 0;
  log("animations initialized.");
}

TR_animation_ptr AN_New(TR_game_instance_t *gi, TR_unit_ptr u) {
  if(gi->free_animation_head != TR_null) {
    TR_unit_t *un                = &unit(u);
    TR_animation_ptr r           = gi->free_animation_head;
    gi->free_animation_head       = animation(gi->free_animation_head).fnext;
    animation(r).previous         = TR_null;
    animation(r).next             = un->animation_head;
    DO_TIMES(MESH_COUNT(un))
      animation(r).anim_state[f] = null;
    animation(r).fade_span        = -1;
    animation(r).weight           = 0.f;
    animation(r).speed            = 1.f;
    if(un->animation_head != TR_null)
      animation(un->animation_head).previous = r;
    un->animation_head = r;
    return r;
  }
  log("No more animation left.");
  return TR_null;
}

void AN_Free(TR_game_instance_t *gi, TR_unit_ptr u, TR_animation_ptr l) {
  TR_unit_t *un = &unit(u);
  TR_animation_t *an = &animation(l);
  DO_TIMES(MESH_COUNT(un))
    an->anim_state[f]->setEnabled(false);
  if(un->animation_head == l)
    un->animation_head = animation(l).next;
  if(animation(l).previous != TR_null)
    animation(animation(l).previous).next = animation(l).next;
  if(animation(l).next != TR_null)
    animation(animation(l).next).previous = animation(l).previous;
  animation(l).fnext = gi->free_animation_head;
  gi->free_animation_head = l;
}

void AN_FreeAll(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  while(un->animation_head != TR_null)
    AN_Free(gi, u, un->animation_head);
}

void AN_Update(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_animation_ptr a = un->animation_head;
  while(a != TR_null) {
    TR_animation_t *an = &animation(a);
    bool free_it = false;
    if(an->fade_span != -1) {
      an->fade_span -= gi->game_time_step;
      if(an->fade_span <= 0)
        free_it = true;
      else
        an->weight = (float)an->fade_span/ANIMATION_FADE_OUT_SPAN;
    } else {
      an->weight += (float)ANIMATION_FADE_IN_SPEED * gi->game_time_step / NORMAL_TIME_STEP;
      if(an->weight > 1.f)
        an->weight = 1.f;
    }
    bool has_ended = an->anim_state[0]->hasEnded();
    free_it = free_it || has_ended;
    if(free_it) {
      DO_TIMES(MESH_COUNT(un)) {
        an->anim_state[f]->setTimePosition(0);
        an->anim_state[f]->setEnabled(false);
      }
      TR_animation_ptr a2 = a;
      a = an->next;
      AN_Free(gi, u, a2);
      /* we don't want 'un' to blend with bind pose */
      if(un->animation_head == TR_null) {
        PLAY_UNIT_ANIM(u, "Idle", true)
        un->anim_span = IDLE_ANIM_SPAN;
        TR_animation_ptr a3 = un->animation_head;
        assert(a3 != TR_null);
        TR_animation_t *an = &animation(a3);
        if(BLEND_WITH_POSE(un))
          an->weight = 0.f;
        else
          an->weight = 1.f;
        DO_TIMES(MESH_COUNT(un))
          an->anim_state[f]->setWeight(an->weight);
      }
    } else {
      DO_TIMES(MESH_COUNT(un)) {
        an->anim_state[f]->setWeight(an->weight);
        an->anim_state[f]->addTime((float)gi->game_time_step*0.001f*ANIMATION_SPEED_FACTOR*an->speed);
      }
      a = an->next;
    }
  }
}

void AN_FadeOut(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_animation_ptr a = un->animation_head;
  while(a != TR_null) {
    TR_animation_t *an = &animation(a);
    an->fade_span = an->weight * ANIMATION_FADE_OUT_SPAN;
    a = an->next;
  }
}

TR_animation_ptr AN_AlreadyPlaying(TR_game_instance_t *gi, TR_unit_ptr u, const char *name, bool fade_in) {
  TR_unit_t *un = &unit(u);
  TR_animation_ptr a = un->animation_head;
  while(a != TR_null) {
    TR_animation_t *an = &animation(a);
    if(strcmp(name, an->anim_state[0]->getAnimationName().c_str()) == 0) {
      if(fade_in)
        an->fade_span = -1;
      return a;
    }
    a = an->next;  
  }
  return TR_null;
}

void AN_Reset(TR_game_instance_t *gi, TR_unit_ptr u, const char *name) {
  TR_unit_t *un = &unit(u);
  TR_animation_ptr a = un->animation_head;
  while(a != TR_null) {
    TR_animation_t *an = &animation(a);
    if(strcmp(name, an->anim_state[0]->getAnimationName().c_str()) == 0) {
      DO_TIMES(MESH_COUNT(un))
        an->anim_state[f]->setTimePosition(0);
    }
    a = an->next;  
  }
}

/*
 *
 */

void UN_Init(TR_game_instance_t *gi, int argc, char** argv) {
  // clear unit map.
  for(int i = 0; i < MAP_SIZE; i++)
    gi->unit_map[i] = TR_null;
  // make units.
  for(int i = 0; i < MAX_UNIT; i++) {
    if(i != MAX_UNIT-1)
      unit(i).fnext = i+1;
    else
      unit(i).fnext = TR_null;
    unit(i).visibility_map = 
      TCOD_map_new(RANGE_MAP_SIZE_X, RANGE_MAP_SIZE_Y);
    unit(i).previous = TR_null;
    unit(i).next     = TR_null;
  }
  gi->free_unit_head = 0;
  gi->unit_head = TR_null;
  log("Units initialized.");
}

TR_unit_ptr UN_New(TR_game_instance_t *gi) {
  if(gi->free_unit_head != TR_null) {
    TR_unit_ptr u       = gi->free_unit_head;
    TR_unit_t   *un     = &unit(u);
    FOR_RECT(0, RANGE_MAP_SIZE_X-1, 0, RANGE_MAP_SIZE_Y-1) {
      bool vis = true;//FLYING(un) || TCOD_map_is_walkable(gi->wall_map, i, j);
      TCOD_map_set_properties(un->visibility_map, i, j, vis, vis);
    }
    un->colision_mark   = -1;
    un->penetration_mark   = -1;
    un->user_data       = TR_null;
    un->order_head      = TR_null;
    un->animation_head  = TR_null;
    DO_TIMES(MAX_PLAYER)
      un->build_order_head[f] = TR_null;
    DO_TIMES(MAX_SKILLS_PER_UNIT) {
      un->skills[f] = (TR_skill_type_t)-1;
      un->skill_is_cooling_down[f] = true;
      un->skill_speed_factors[f] = 100;
      un->skills_cooldown[f] = 99999999;
      un->skill_marks[f] = 1;
    }
    DO_TIMES(TR_skill_type_count) {
      un->skill_levels[f] = 0;
    }
    un->hp_regen_remaining = 0;
    un->mana_regen_remaining = 0;
    un->skill_count       = 0;
    un->path_len          = 0;
    un->path_index        = 0;
    un->blocked_time      = 0;
    un->think             = 0; /* rand */
    un->move              = 0;  
    un->wait              = 0;
    un->skill             = 0;
    un->skill_state       = 0;
    un->skill_state2      = 0;
    un->virtual_hp_cache  = -1;
    un->virtual_hp_cache2 = -1;
    un->silenced          = 0;
    un->rooted            = 0;
    un->stun              = 0;
    un->uncontrolable     = 0;
    un->blind             = 0;
    un->invincibility     = 0;
    un->no_aggro          = 0;
    un->forced_action_marker = 0;
    un->forced_action_ps  = TR_null;
    un->ps_head           = TR_null;
    un->lookat_x          = 0;
    un->lookat_y          = 1;
    un->stun_force_x      = 0;
    un->stun_force_y      = 0;
    un->dx                = 0;
    un->dy                = 0;
    un->old_lookat_x      = 0;
    un->old_lookat_y      = 1;
    un->anim_time         = 0;
    un->anim_step         = 0;
    DO_TIMES(MAX_CLIENT) {
      un->sprevious[f]  = TR_null;
      un->snext[f]      = TR_null;
    }
    DO_TIMES(MAX_GROUP_SELECTION) {
      un->gsprevious[f] = TR_null;
      un->gsnext[f] = TR_null;
    } 
    un->status_effect_head = TR_null;
    un->pprevious       = TR_null;
    un->pnext           = TR_null;
    un->previous        = TR_null;
    un->next            = gi->unit_head;
    if(gi->unit_head != TR_null)
      unit(gi->unit_head).previous = u;
    gi->unit_head = u;
    gi->free_unit_head  = unit(gi->free_unit_head).fnext;
    return u;
  }
  log("No more units left.");
  return TR_null;
}

void UN_Free(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  UN_RemoveFromMap(gi, u);
  BO_FreeAll(gi, u);
  UO_FreeAll(gi, u);
  ST_FreeAll(gi, u);
  /* must be after UO_FreeAll which start an anim */
  AN_FreeAll(gi, u);
  if(gi->unit_head == u)
    gi->unit_head = unit(u).next;
  if(un->previous != TR_null)
    unit(un->previous).next = un->next;
  if(un->next != TR_null)
    unit(un->next).previous = un->previous;
  un->fnext = gi->free_unit_head;
  gi->free_unit_head = u;
}

void UN_FreeAll(TR_game_instance_t *gi) {
  while(gi->unit_head != TR_null)
    UN_Free(gi, gi->unit_head);
  log("Units freed.");
}

bool UN_CanBeHereWallOnly(TR_game_instance_t *gi, int size, int x, int y) {
  REAL_SOCLE(size, x, y) {
    if(!INSIDE_MAP(i, j)
    || AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT) == TR_WALL)
      return false;
  }
  return true;
}


bool UN_CanBeHere(TR_game_instance_t *gi, int size, int x, int y, bool cannot_be_pushed_only) {
  REAL_SOCLE(size, x, y) {
    if(!cannot_be_pushed_only) {
      if(!INSIDE_MAP(i, j)
      || AT(unit_map, i>>RANGE_SHIFT, j>>RANGE_SHIFT) != TR_null
      || AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT) == TR_WALL)
        return false;
    } else {
      if(!INSIDE_MAP(i, j)
      || AT_ENV(i, j) == TR_WALL)
        return false;
      bool cannot_be_pushed_here = false;
      TR_unit_node_ptr n = AT(unit_map, i, j);
      while(!cannot_be_pushed_here
         && n != TR_null) {
        TR_unit_ptr u = unit_node(n).u;
        TR_unit_t *un = &unit(u);
        if(!CAN_BE_PUSHED(un))
          cannot_be_pushed_here = true;
        n = unit_node(n).mnext;
      }
      if(cannot_be_pushed_here)
        return false;
    }
  }
  return true;
}

int UN_DistFromEdge(TR_game_instance_t *gi, TR_edge_t *edge, int x, int y) {
  int dex = edge->x1 - edge->x0,
      dey = edge->y1 - edge->y0,
      de  = TR_ISqrt(dex * dex + dey * dey),
      dx  = x - edge->x0,
      dy  = y - edge->y0,
      result;
  if(de == 0)
    result = TR_ISqrt(dx * dx + dy * dy);
  else {
    int dot = (dx * dex + dy * dey) / de;
    if(dot <= 0)
      result = TR_ISqrt(dx * dx + dy * dy);
    else
    if(dot >= de)
      result = TR_ISqrt((x - edge->x1) * (x - edge->x1) + (y - edge->y1) * (y - edge->y1));
    else {
      result = ABS(-dey * dx + dex * dy) / de;
      //result *= result;
    }
  }
  assert(result >= 0);
  return result;
}

void UN_SnapToEdge(TR_game_instance_t *gi, TR_unit_ptr u, TR_edge_t *edge) {
  TR_unit_t *un = &unit(u);
  UN_RemoveFromMap(gi, u);
  int x   = un->x + UN_GetSizeShift(gi, SIZE(un)),
      y   = un->y + UN_GetSizeShift(gi, SIZE(un)),
      dex = edge->x1 - edge->x0,
      dey = edge->y1 - edge->y0,
      de  = TR_ISqrt(dex * dex + dey * dey),
      dx  = x - edge->x0,
      dy  = y - edge->y0;
  if(de == 0) {
    un->x = edge->x0 - UN_GetSizeShift(gi, SIZE(un));
    un->y = edge->y0 - UN_GetSizeShift(gi, SIZE(un));
  } else {
    int dot = (dx * dex + dy * dey) / de;
    if(dot <= 0) {
      un->x = edge->x0 - UN_GetSizeShift(gi, SIZE(un));
      un->y = edge->y0 - UN_GetSizeShift(gi, SIZE(un));
    } else
    if(dot >= de) {
      un->x = edge->x1 - UN_GetSizeShift(gi, SIZE(un));
      un->y = edge->y1 - UN_GetSizeShift(gi, SIZE(un));
    } else {
      un->x = edge->x0 + (edge->x1 - edge->x0) * dot / de - UN_GetSizeShift(gi, SIZE(un));
      un->y = edge->y0 + (edge->y1 - edge->y0) * dot / de - UN_GetSizeShift(gi, SIZE(un));
    }
  }
  UN_PutInMap(gi, u);
}

void UN_PutInMap(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  float x = (float)un->x/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)),
        z = (float)un->y/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un));
  un->node->setPosition(
    Ogre::Vector3(
      x, 
      Y_OFFSET(un) + (FLYING(un) ? FLYING_HEIGHT : 0) + 0.01f*(MAX_UNIT+u), 
      z));
  REAL_SOCLE(SIZE(un), un->x, un->y) {
    if(INSIDE_MAP(i, j)) {
      TR_unit_node_ptr n = NO_New(gi);
      assert(n != TR_null);
      TR_unit_node_t *no = &unit_node(n);
      no->u = u;
      no->mnext = AT(unit_map, i, j);
      AT(unit_map, i, j) = n;
    }
  }
}

void UN_RemoveFromMap(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  REAL_SOCLE(SIZE(un), un->x, un->y) {
    if(INSIDE_MAP(i, j)) {
      TR_unit_node_ptr n = AT(unit_map, i, j);
      if(n != TR_null) {
        if(unit_node(n).u == u) {
          AT(unit_map, i, j) = unit_node(n).mnext;
          NO_Free(gi, n);
        } else {
          TR_unit_node_ptr nprev;
          while(n != TR_null) {
            if(unit_node(n).u == u) {
              unit_node(nprev).mnext = unit_node(n).mnext;
              NO_Free(gi, n);
              n = nprev;
            } 
            nprev = n;
            n = unit_node(n).mnext;
          }
        }
      }
    }
  }
}

bool UN_Raycast(TR_game_instance_t *gi, int x0, int y0, int x1, int y1, bool wall_only) {
  int ray_dir_x     = x1-x0,
      ray_dir_y     = y1-y0;
  while(ABS(ABS(ray_dir_x)-ABS(ray_dir_y)) > 50) {
    ray_dir_x >>= 1;
    ray_dir_y >>= 1;
  }
  int start_x       = x0>>POS_SHIFT,
      start_y       = y0>>POS_SHIFT,  
      end_x         = x1>>POS_SHIFT,
      end_y         = y1>>POS_SHIFT,  
      x             = start_x,
      y             = start_y,
      side_size     = 1<<POS_SHIFT,
      slope_x       = ray_dir_x == 0 ? 0 : side_size * ray_dir_y / ray_dir_x,
      slope_y       = ray_dir_y == 0 ? 0 : side_size * ray_dir_x / ray_dir_y,
      delta_dist_x  = TR_ISqrt(side_size * side_size + slope_x * slope_x),
      delta_dist_y  = TR_ISqrt(side_size * side_size + slope_y * slope_y),
      side_dist_x, side_dist_y, step_x, step_y;
  if(ray_dir_x < 0) {
    step_x = -1;
    side_dist_x = (x0 - (x<<POS_SHIFT)) * delta_dist_x / side_size;
  } else {
    step_x = 1;
    side_dist_x = (side_size + (x<<POS_SHIFT) - x0) * delta_dist_x / side_size;
  }
  if(ray_dir_y < 0) {
    step_y = -1;
    side_dist_y = (y0 - (y<<POS_SHIFT)) * delta_dist_y / side_size;
  } else {
    step_y = 1;
    side_dist_y = (side_size + (y<<POS_SHIFT) - y0) * delta_dist_y / side_size;
  }
  while((end_x - x) * step_x >= 0
     && (end_y - y) * step_y >= 0) {
    if(AT_ENV(x>>RANGE_SHIFT, y>>RANGE_SHIFT) == TR_WALL
    || (!wall_only && AT(heavy_map, x, y))
    || AT(obstacle_map, x, y))
      return true;
    if(ray_dir_x == 0) {
      side_dist_y += delta_dist_y;
      y += step_y;
    } else
    if(ray_dir_y == 0) {
      side_dist_x += delta_dist_x;
      x += step_x;
    } else
    if(side_dist_x == side_dist_y) {
      side_dist_x += delta_dist_x;
      x += step_x;
      side_dist_y += delta_dist_y;
      y += step_y;
    } else
    if(side_dist_x < side_dist_y) {
      side_dist_x += delta_dist_x;
      x += step_x;
    } else {
      side_dist_y += delta_dist_y;
      y += step_y;
    }
  }
  return false;
}

bool UN_Raycast(TR_game_instance_t *gi, int size, int x0, int y0, int x1, int y1, bool wall_only) {
  int cx[4], cy[4];
  int half     = (halfes[size-1])<<POS_SHIFT,
      start_i  = (x0 - half),
      end_i    = (x0 - half + (size<<POS_SHIFT)-1),
      start_j  = (y0 - half),
      end_j    = (y0 - half + (size<<POS_SHIFT)-1);
  cx[0] = start_i;
  cx[1] =   end_i;
  cx[2] = start_i;
  cx[3] =   end_i;
  cy[0] = start_j;
  cy[1] = start_j;
  cy[2] =   end_j;
  cy[3] =   end_j;
  DO_TIMES(4)
    if(UN_Raycast(gi, cx[f], cy[f], cx[f]+(x1-x0), cy[f]+(y1-y0), wall_only))
      return true;
  return false;
}

void UN_GetUnitsHere(TR_get_unit_here_t *data, TR_game_instance_t *gi, TR_unit_ptr not_this_one, TR_id_t team_id, int size, int x, int y, bool ground, bool air, TR_get_unit_here_mode_t mode) {
  data->count = 0;
  REAL_SOCLE(size, x, y) {
    if(INSIDE_MAP(i, j)) {
      TR_unit_node_ptr n = AT(unit_map, i, j);
      while(n != TR_null) {
        TR_unit_ptr u = unit_node(n).u;
        TR_unit_t *un = &unit(u);
        if(u != not_this_one
        && ((!FLYING(un) && ground) || (FLYING(un) && air))
        && (mode == TR_get_unit_here_mode_all
        || (mode == TR_get_unit_here_mode_enemies
        && un->team_id != team_id)
        || (mode == TR_get_unit_here_mode_friends
        && un->team_id == team_id))) {
          bool already = false;
          for(int f = 0; !already && f < data->count; f++)
            already = data->result[f] == u;
          if(!already)
            data->result[data->count++] = u;
        }
        n = unit_node(n).mnext;
      }
    }
  }
}

int UN_Overlap(TR_game_instance_t *gi, TR_unit_ptr u, int size, int x, int y) {
  TR_unit_t *un = &unit(u);
  int dist = TR_ISqrt((un->x - x) * (un->x - x) + (un->y - y) * (un->y - y));
  return MAX(0, ((SIZE(un)+size)<<(POS_SHIFT-1))-dist);
}

bool UN_HasOrder(TR_game_instance_t *gi, TR_unit_ptr u, TR_id_t cmd_mask) {
  TR_unit_t *un       = &unit(u);
  TR_unit_order_ptr o = un->order_head;
  while(o != TR_null) {
    TR_unit_order_t *uo = &order(o);
    if(uo->cmd_mask == cmd_mask)
      return true;
    o = uo->next;
  }
  return false;
}

TR_status_effect_ptr UN_HasStatusEffect(TR_game_instance_t *gi, TR_unit_ptr u, int type) {
  TR_unit_t *un       = &unit(u);
  TR_status_effect_ptr o = un->status_effect_head;
  while(o != TR_null) {
    TR_status_effect_t *uo = &status_effect(o);
    if(uo->type == type)
      return o;
    o = uo->next;
  }
  return TR_null;
}

void UN_PrintOrders(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un       = &unit(u);
  TR_unit_order_ptr o = un->order_head;
  log("printorder start");
  while(o != TR_null) {
    TR_unit_order_t *uo = &order(o);
    log(uo->order);
    o = uo->next;
  }
  log("printorder end");
}

bool UN_CheckTargetMove(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un        = &unit(u);
  TR_unit_order_ptr o = unit(u).order_head;
  TR_unit_order_t *uo = &order(o);
  while(uo->order != TR_unit_order_follow_goal
     || uo->target == TR_null
     || !uo->precise_target
     /* it is following a friend, so no need 
        to check if the precise target is 
        friendly */) {
    o = order(o).next;
    uo = &order(o);
  }
  assert(o != TR_null);
  TR_unit_order_t *follow_uo = &order(o);
  assert(follow_uo->order == TR_unit_order_follow_goal);
  TR_unit_t *target_un = &unit(follow_uo->target);
  /* no need to check if target is alive, the order processing will check everything */
  return follow_uo->last_target_cx != target_un->x+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un))
      || follow_uo->last_target_cy != target_un->y+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un));
}

bool UN_IsFollowingThisTarget(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target) {
  TR_unit_order_ptr o = unit(u).order_head;
  while(o != TR_null) {
    if(order(o).order == TR_unit_order_follow_goal)
      if(order(o).target == target
      && !order(o).generated)
        return true;
      else
        return false;
    o = order(o).next;
  }
  return false;
}

bool UN_CheckFollowTarget(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target) {
  TR_unit_order_ptr o = unit(u).order_head;
  while(o != TR_null) {
    if(order(o).order == TR_unit_order_follow_goal
    && order(o).target == target)
      return true;
    o = order(o).next;
  }
  return false;
}

void UN_FindClosestPoint(TR_game_instance_t *gi, int size, int *to_x, int *to_y, bool cannot_be_pushed_only) {
  short       off         = 0;
  bool        found       = false;
  int         min_dist    = 0x7fffffff, dist;
  int         x, y, i, j, endi, endj,
              dx = *to_x, dy = *to_y;
  while(!found 
     && off < MAP_SIZE_X) {
    endi = 0+off;
    for(i = 0-off; i <= endi; i++) {
      j = 0-off;
      if(UN_CanBeHere(gi, size, dx+(i<<POS_SHIFT), dy+(j<<POS_SHIFT), cannot_be_pushed_only)) {
        dist = i * i + j * j;
        if(dist < min_dist) {
          min_dist = dist;
          x = dx+(i<<POS_SHIFT);
          y = dy+(j<<POS_SHIFT);
          found = true;
        }
      }
    }
    endi = 0+off;
    for(i = 0-off; i <= endi; i++) {
      j = 0+off;
      if(UN_CanBeHere(gi, size, dx+(i<<POS_SHIFT), dy+(j<<POS_SHIFT), cannot_be_pushed_only)) {
        dist = i * i + j * j;
        if(dist < min_dist) {
          min_dist = dist;
          x = dx+(i<<POS_SHIFT);
          y = dy+(j<<POS_SHIFT);
          found = true;
        }
      }
    }
    endj = 0+off;
    for(j = 0-off; j <= endj; j++) {
      i = 0-off;
      if(UN_CanBeHere(gi, size, dx+(i<<POS_SHIFT), dy+(j<<POS_SHIFT), cannot_be_pushed_only)) {
        dist = i * i + j * j;
        if(dist < min_dist) {
          min_dist = dist;
          x = dx+(i<<POS_SHIFT);
          y = dy+(j<<POS_SHIFT);
          found = true;
        }
      }
    }
    endj = 0+off;
    for(j = 0-off; j <= endj; j++) {
      i = 0+off;
      if(UN_CanBeHere(gi, size, dx+(i<<POS_SHIFT), dy+(j<<POS_SHIFT), cannot_be_pushed_only)) {
        dist = i * i + j * j;
        if(dist < min_dist) {
          min_dist = dist;
          x = dx+(i<<POS_SHIFT);
          y = dy+(j<<POS_SHIFT);
          found = true;
        }
      }
    }
    if(found) {
      *to_x = x;
      *to_y = y;
    }
    off++;
  }
}

int UN_GetDist(TR_game_instance_t *gi, TR_unit_ptr u, int x, int y) {
  TR_unit_t *un       = &unit(u);
  int dx = un->x - x,
      dy = un->y - y,
      result = dx * dx + dy * dy;
  if(result < 0 || dx > MAX_DIST || dy > MAX_DIST)
    result = 0x7fffffff;
  return TR_ISqrt(result);
}

int UN_GetDist(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr u2) {
  TR_unit_t *un       = &unit(u);
  TR_unit_t *un2      = &unit(u2);
  int dx      = (un->x + UN_GetSizeShift(gi, SIZE(un))) - (un2->x + UN_GetSizeShift(gi, SIZE(un2))),
      dy      = (un->y + UN_GetSizeShift(gi, SIZE(un))) - (un2->y + UN_GetSizeShift(gi, SIZE(un2))),
      result  = dx * dx + dy * dy;
  if(result < 0 || dx > MAX_DIST || dy > MAX_DIST)
    result = 0x7fffffff;
  return MAX(0, TR_ISqrt(result) - ((SIZE(un)+SIZE(un2))<<(POS_SHIFT-1)));
}

inline
int UN_GetLight(TR_game_instance_t *gi, TR_unit_ptr u, short pos_x, short pos_y, short u_pos_x, short u_pos_y, int opt_fact) {
  TR_unit_t *un = &unit(u);
  /* this fun is bottleneck */
  bool is_vis = TCOD_map_is_in_fov(un->visibility_map, pos_x, pos_y);
  if(is_vis) {
    int dx = u_pos_x - pos_x,
        dy = u_pos_y - pos_y,
        dist  = dx * dx + dy * dy;
    return MAX(0, 255 - ((dist * opt_fact)>>8));
  }
  return 0;
}

int UN_GetDamage(TR_game_instance_t *gi, TR_unit_ptr u, int attack_damage, TR_attack_type_t attack_type) {
  TR_unit_t *un = &unit(u);
  if(un->invincibility > 0)
    return 0;
  attack_damage = attack_penetrations[attack_type][ARMOR_TYPE(un)] * attack_damage / 100;
  return MIN(un->hp, MAX(0, attack_damage*MAX(0, 1000-ARMOR(un))/1000));
}

void UN_DealInstantDamage(TR_game_instance_t *gi, TR_player_ptr killer_p, TR_unit_ptr target_u, int attack_damage, TR_attack_type_t attack_type, bool show_hp) {
  TR_unit_t *target_un = &unit(target_u);
  if(target_un->hp > 0) {
    int damage = UN_GetDamage(gi, target_u, attack_damage, attack_type);
    damage = gi->on_hit_unit_cb(gi, TR_null, target_u, damage, attack_penetrations[attack_type][ARMOR_TYPE(target_un)], show_hp);
    target_un->hp -= damage;
    if(target_un->hp <= 0)
      PL_KillUnit(gi, killer_p, GI_GetPlayerPtr(gi, target_un->player_id), target_u, true);
  }
}

bool UN_Intersect(TR_game_instance_t *gi, TR_unit_ptr u, int x1, int y1, int x2, int y2) {
  TR_unit_t *un = &unit(u);
  int size     = SIZE(un)-1,
      half     = halfes[size]<<POS_SHIFT,
      start_i  = un->x - half,
      end_i    = un->x - half + ((SIZE(un))<<POS_SHIFT),
      start_j  = un->y - half,
      end_j    = un->y - half + ((SIZE(un))<<POS_SHIFT);
  bool infront[4];
  infront[0] = x1 >= start_i || x2 >= start_i;
  infront[1] = x1 <= end_i || x2 <= end_i;
  infront[2] = y1 >= start_j || y2 >= start_j;
  infront[3] = y1 <= end_j || y2 <= end_j;
  return infront[0] && infront[1] && infront[2] && infront[3];
}

TR_unit_ptr UN_GetUnitHere(TR_game_instance_t *gi, int x, int y) {
  TR_unit_node_ptr n = AT(unit_map, x>>POS_SHIFT, y>>POS_SHIFT);
  while(n != TR_null) {
    if(UN_Intersect(gi, unit_node(n).u, x, y, x, y)) 
      return unit_node(n).u;
    n = unit_node(n).mnext;
  }
  return TR_null;
}

int UN_GetSizeShift(TR_game_instance_t *gi, int size) {
  return 1<<((size&1) == 0 ? POS_SHIFT : POS_SHIFT-1);
}

float UN_GetSizeShiftFloat(TR_game_instance_t *gi, int size) {
  return (size&1) == 0 ? 1.f : 0.5f;
}

void UN_LookAt(TR_game_instance_t *gi, TR_unit_ptr u, int lookat_x, int lookat_y) {
  TR_unit_t *un = &unit(u);
  if(CAN_BE_PUSHED(un)) {
    un->lookat_x = lookat_x - un->x;
    un->lookat_y = lookat_y - un->y;
  }
}

void UN_Cleanse(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  TR_status_effect_ptr s = un->status_effect_head;
  while(s != TR_null) {
    TR_status_effect_t *st = &status_effect(s);
    if(st->evil) {
      TR_status_effect_ptr s2 = s;
      s = st->next;
      ST_Free(gi, u, s2);
    } else
      s = st->next;
  }
  un->blind = 0;
  un->uncontrolable = 0;
  un->silenced = 0;
  un->rooted = 0;
  un->stun = 0;
}

void UN_AttachParticleSystem(TR_game_instance_t *gi, TR_unit_ptr u, TR_particle_system_ptr ps) {
  TR_unit_t *un = &unit(u);
  TR_particle_system_t *psys = &particle_system(ps);
  gi->ogre_scene->getRootSceneNode()->removeChild(psys->ogre_node);
  un->node->addChild(psys->ogre_node);
  psys->x = 0;
  psys->y = 0;
  psys->z /= 0.25*SIZE(un)*MODEL_SCALE(un);
  psys->free_it = false;
  psys->unext = un->ps_head;
  un->ps_head = ps;
}

void UN_PropagateAttack(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, bool around_only) {
  assert(target != TR_null);
  TR_unit_t *un = &unit(u);
  if(around_only
  || UO_GetFrontType(gi, u) == TR_unit_order_none
  || (UO_GetFrontType(gi, u) == TR_unit_order_follow_goal 
  && UO_GetFront(gi, u)->done)) {
    bool gen = !around_only
            && gi->default_skill_type_cb(gi, u, target) != -1
            && ((!FLYING(&unit(target)) && gi->skill_look_for_ground_cb(gi, gi->default_skill_type_cb(gi, u, target)))
            || ((FLYING(&unit(target)) && gi->skill_look_for_air_cb(gi, gi->default_skill_type_cb(gi, u, target)))));
    if(gen)
      PL_GenFollowTarget(
        gi, GI_GetPlayerPtr(gi, un->player_id), u, target, 0, gi->default_skill_type_cb(gi, u, target), 
        UO_GetFrontType(gi, u) != TR_unit_order_none && UO_GetFront(gi, u)->following_friend);
    if(around_only || gen) {
      TR_get_unit_here_t data;
      UN_GetUnitsHere(
        &data, gi, u, un->team_id, ATTACK_PROPAGATION_RADIUS, 
        un->x, un->y, true, true, TR_get_unit_here_mode_friends);
      DO_TIMES(data.count)
        UN_PropagateAttack(gi, data.result[f], target, false);
    }
  }
}

void UN_AddHP(TR_game_instance_t *gi, TR_unit_ptr u, int hp) {
  TR_unit_t *un = &unit(u);
  un->hp += hp;
  un->virtual_hp += hp;
  if(un->hp > MAX_HP(un)) {
    un->virtual_hp -= MAX_HP(un) - un->hp;
    un->hp = MAX_HP(un);
  }
  if(un->hp < 0)
    un->hp = un->virtual_hp = 0;
}

void UN_SetMana(TR_game_instance_t *gi, TR_unit_ptr u, int mn) {
  TR_unit_t *un = &unit(u);
  un->mana = mn;
  if(un->mana > MAX_MANA(un))
    un->mana = MAX_MANA(un);
  if(un->mana < 0)
    un->mana = 0;
}

void UN_AddSkill(TR_game_instance_t *gi, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  un->skills[un->skill_count++] = skill_type;
}

int UN_SkillId(TR_game_instance_t *gi, TR_unit_ptr u, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  DO_TIMES(un->skill_count)
    if(un->skills[f] == skill_type)
      return f;
  return -1;
}

/*
 * cmd
 */

void CMD_Init(TR_game_instance_t *gi, int argc, char** argv) {
  for(int i = 0; i < MAX_CMD; i++) {
    if(i != MAX_CMD-1)
      cmd(i).fnext = i+1;
    else
      cmd(i).fnext = TR_null;
  }
  gi->free_cmd_head = 0;
  gi->cmd_head      = TR_null;
  log("Cmds initialized.");
}

TR_cmd_ptr CMD_New(TR_game_instance_t *gi, TR_id_t id) {
  if(gi->free_cmd_head != TR_null) {
    TR_cmd_ptr r                    = gi->free_cmd_head;
    gi->free_cmd_head               = cmd(gi->free_cmd_head).fnext;
    cmd(r).user_data                = TR_null;
    cmd(r).id                       = id;
    cmd(r).sent                     = false;
    cmd(r).cmd_next                 = TR_null;
    cmd(r).previous                 = TR_null;
    cmd(r).next                     = gi->cmd_head;
    cmd(r).unit_order_completed_cb  = null;
    cmd(r).unit_order_failed_cb     = null;
    cmd(r).start_it                 = false;
    if(gi->cmd_head != TR_null)
      cmd(gi->cmd_head).previous = r;
    gi->cmd_head = r;
    return r;
  }
  log("No more cmd left.");
  return TR_null;
}

void CMD_Free(TR_game_instance_t *gi, TR_cmd_ptr cmd) {
  if(gi->cmd_head == cmd)
    gi->cmd_head = cmd(cmd).next;
  if(cmd(cmd).previous != TR_null)
    cmd(cmd(cmd).previous).next = cmd(cmd).next;
  if(cmd(cmd).next != TR_null)
    cmd(cmd(cmd).next).previous = cmd(cmd).previous;
  cmd(cmd).fnext = gi->free_cmd_head;
  gi->free_cmd_head = cmd;
}

void CMD_FreeAll(TR_game_instance_t *gi) {
  while(gi->cmd_head != TR_null)
    CMD_Free(gi, gi->cmd_head);
  log("Cmds freed.");
}

/*
 * cmd store
 */

void CS_Init(TR_game_instance_t *gi, int argc, char** argv) {
  for(int i = 0; i < MAX_PLAYER; i++) {
    if(i != MAX_PLAYER-1)
      cmd_store(i).fnext = i+1;
    else
      cmd_store(i).fnext = TR_null;
  }
  gi->free_cmd_store_head = 0;
  gi->cmd_store_head      = TR_null;
  log("Cmd stores initialized.");
}

TR_cmd_store_ptr CS_New(TR_game_instance_t *gi) {
  if(gi->free_cmd_store_head != TR_null) {
    TR_cmd_store_ptr r = gi->free_cmd_store_head;
    gi->free_cmd_store_head = cmd_store(gi->free_cmd_store_head).fnext;
    TR_cmd_store_t *cs = &cmd_store(r);
    cmd_store(r).cmd_head   = TR_null;
    cmd_store(r).previous   = TR_null;
    cmd_store(r).next       = gi->cmd_store_head;
    if(gi->cmd_store_head != TR_null)
      cmd_store(gi->cmd_store_head).previous = r;
    gi->cmd_store_head = r;
    return r;
  }
  log("No more cmd_store left.");
  return TR_null;
}

void CS_Free(TR_game_instance_t *gi, TR_cmd_store_ptr cs) {
  if(gi->cmd_store_head == cs)
    gi->cmd_store_head = cmd_store(cs).next;
  if(cmd_store(cs).previous != TR_null)
    cmd_store(cmd_store(cs).previous).next 
      = cmd_store(cs).next;
  if(cmd_store(cs).next != TR_null)
    cmd_store(cmd_store(cs).next).previous 
      = cmd_store(cs).previous;
  cmd_store(cs).fnext = gi->free_cmd_store_head;
  gi->free_cmd_store_head = cs;
}

void CS_FreeAll(TR_game_instance_t *gi) {
  while(gi->cmd_store_head != TR_null)
    CS_Free(gi, gi->cmd_store_head);
  log("Cmd stores freed.");
}

void CS_ClearCmdStore(TR_game_instance_t *gi, TR_cmd_store_ptr c) {
  TR_cmd_store_t *cs    = &cmd_store(c);
  TR_cmd_ptr  r         = cs->cmd_head,
              previous  = r;
  while(r != TR_null) {
    if(cmd(r).turn == gi->turn) {
      if(r == cs->cmd_head) {
        cs->cmd_head = cmd(r).cmd_next;
        CMD_Free(gi, r);
        previous = r = cs->cmd_head;
      } else {
        cmd(previous).cmd_next = cmd(r).cmd_next;
        CMD_Free(gi, r);
        r = cmd(previous).cmd_next;
      }
    } else {
      previous = r;
      r = cmd(r).cmd_next;
    }
  }
}

void CS_InsertCmd(TR_game_instance_t *gi, TR_cmd_store_ptr r, TR_cmd_ptr c) {
  TR_cmd_store_t *cs    = &cmd_store(r);
  TR_cmd_ptr  d         = cs->cmd_head,
              previous  = d;
  while(d != TR_null) {
    previous = d;
    d = cmd(d).cmd_next;
  }
  if(previous != TR_null)
    cmd(previous).cmd_next = c;
  else
    cs->cmd_head = c;
}

int CS_LeechCompressedBuffer(TR_game_instance_t *gi, TR_cmd_store_ptr cs, int data_length, unsigned char *data) {
  int max_turn = 0;
  z_stream strm;
  strm.zalloc   = Z_NULL;
  strm.zfree    = Z_NULL;
  strm.opaque   = Z_NULL;
  strm.avail_in = 0;
  strm.next_in  = Z_NULL;
  TR_ZErr(inflateInit(&strm));
  strm.avail_in   = data_length;
  strm.next_in    = data;
  strm.avail_out  = ZLIB_CHUNK;
  strm.next_out   = zlib_out;
  int ret = inflate(&strm, Z_FINISH);
  TR_ZErr(ret);
  assert(ret == Z_STREAM_END);
  inflateEnd(&strm);
  //log("" + i2a(packet->dataLength) + " " + i2a((int)(ZLIB_CHUNK - strm.avail_out)));
  unsigned char *pack_data = zlib_out;
  TR_cmd_type_t type = ((TR_cmd_type_t*)pack_data)[0];
  while(type != TR_cmd_type_none) {
    TR_id_t cmd_id      = ((TR_end_of_turn_packet_t*)pack_data)->id;
    TR_cmd_ptr r        = CMD_New(gi, cmd_id);
    TR_cmd_t *cmd       = &cmd(r);
    switch(type) {
      case TR_cmd_type_unit_order: {
          TR_unit_order_packet_t *data = 
            (TR_unit_order_packet_t*)pack_data;
          cmd->type             = data->type;
          cmd->id               = data->id;
          cmd->turn             = data->turn;
          cmd->cmd_mask         = data->cmd_mask;
          cmd->r_squared        = data->r_squared;
          cmd->target_cx        = data->target_cx;
          cmd->target_cy        = data->target_cy;
          cmd->click_cx         = data->click_cx;
          cmd->click_cy         = data->click_cy;
          cmd->target           = data->target;
          cmd->target_player_id = data->target_player_id;
          cmd->target_cmd_id    = data->target_cmd_id;
          cmd->skill_here       = data->skill_here;
          cmd->skill_type       = data->skill_type;
          cmd->push_back        = data->push_back;
          max_turn = MAX(max_turn, cmd->turn);
          CS_InsertCmd(gi, cs, r);
          pack_data += sizeof(TR_unit_order_packet_t);
        }
        break;
      case TR_cmd_type_spawn_unit: {
          TR_spawn_unit_packet_t *data = 
            (TR_spawn_unit_packet_t*)pack_data;
          cmd->type           = data->type;
          cmd->id             = data->id;
          cmd->unit_type      = data->unit_type;
          cmd->turn           = data->turn;
          cmd->cmd_mask       = data->cmd_mask;
          cmd->target_cx      = data->target_cx;
          cmd->target_cy      = data->target_cy;
          cmd->user_data_type = data->user_data_type;
          cmd->find_pos       = data->find_pos;
          max_turn = MAX(max_turn, cmd->turn);
          CS_InsertCmd(gi, cs, r);
          pack_data += sizeof(TR_spawn_unit_packet_t);
        }
        break;
      case TR_cmd_type_stop_unit: {
          TR_stop_unit_packet_t *data = 
            (TR_stop_unit_packet_t*)pack_data;
          cmd->type           = data->type;
          cmd->id             = data->id;
          cmd->turn           = data->turn;
          max_turn = MAX(max_turn, cmd->turn);
          CS_InsertCmd(gi, cs, r);
          pack_data += sizeof(TR_stop_unit_packet_t);
        }
        break;
      case TR_cmd_type_build_unit: {
          TR_build_unit_packet_t *data = 
            (TR_build_unit_packet_t*)pack_data;
          cmd->type               = data->type;
          cmd->id                 = data->id;
          cmd->turn               = data->turn;
          cmd->cmd_mask           = data->cmd_mask;
          cmd->unit_type          = data->unit_type;
          cmd->player_id          = data->player_id;
          cmd->target_player_id   = data->target_player_id;
          cmd->target_cx          = data->target_cx;
          cmd->target_cy          = data->target_cy;
          cmd->start_it           = data->start_it;
          cmd->cancel_it          = data->cancel_it;
          cmd->user_data_type     = data->user_data_type;
          max_turn = MAX(max_turn, cmd->turn);
          CS_InsertCmd(gi, cs, r);
          pack_data += sizeof(TR_build_unit_packet_t);
        }
        break;
      case TR_cmd_type_generic: {
          TR_generic_packet_t *data = 
            (TR_generic_packet_t*)pack_data;
          cmd->type               = data->type;
          cmd->id                 = data->id;
          cmd->turn               = data->turn;
          cmd->cmd_type           = data->cmd_type;
          cmd->cmd_mask           = data->cmd_mask;
          cmd->unit_type          = data->unit_type;
          cmd->player_id          = data->player_id;
          cmd->target_cx          = data->target_cx;
          cmd->target_cy          = data->target_cy;
          max_turn = MAX(max_turn, cmd->turn);
          CS_InsertCmd(gi, cs, r);
          pack_data += sizeof(TR_generic_packet_t);
        }
        break;
      case TR_cmd_type_end_of_turn: {
        TR_end_of_turn_packet_t *data = 
          (TR_end_of_turn_packet_t*)pack_data;
        cmd->type               = data->type;
        cmd->id                 = data->id;
        cmd->turn               = data->turn;
        cmd->turn_frame_count   = data->turn_frame_count;
        cmd->frame_time_step    = data->frame_time_step;
        cmd->hash               = data->hash;
        max_turn = MAX(max_turn, cmd->turn);
        CS_InsertCmd(gi, cs, r);
        pack_data += sizeof(TR_end_of_turn_packet_t);
        //log("EOT: " + i2a(data->turn));
      }
      break;
    }
    type = *(TR_cmd_type_t*)pack_data;
  }
  return max_turn;
}

/*
 *
 */

void RS_Init(TR_game_instance_t *gi, int argc, char** argv) {
  for(int i = 0; i < MAX_PLAYER; i++) {
    if(i != MAX_PLAYER-1)
      remote_cmd_store(i).fnext = i+1;
    else
      remote_cmd_store(i).fnext = TR_null;
  }
  gi->free_remote_cmd_store_head = 0;
  gi->remote_cmd_store_head      = TR_null;
  log("Remote cmd stores initialized.");
}

TR_remote_cmd_store_ptr RS_New(TR_game_instance_t *gi, TR_cmd_store_ptr cmd_store) {
  if(gi->free_remote_cmd_store_head != TR_null) {
    TR_remote_cmd_store_ptr r 
      = gi->free_remote_cmd_store_head;
    gi->free_remote_cmd_store_head 
      = remote_cmd_store(gi->free_remote_cmd_store_head).fnext;
    TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
    rs->cs                  = cmd_store;
    rs->previous            = TR_null;
    rs->peer                = null;
    rs->host                = null;
    rs->connected           = false;
    rs->next                = gi->remote_cmd_store_head;
    if(gi->remote_cmd_store_head != TR_null)
      remote_cmd_store(gi->remote_cmd_store_head).previous = r;
    gi->remote_cmd_store_head = r;
    return r;
  }
  log("No more remote_cmd_store left.");
  return TR_null;
}

void RS_Connect(TR_game_instance_t *gi, TR_remote_cmd_store_ptr r) {
  TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
  rs->address.host          = ENET_HOST_ANY;
  rs->address.port          = 0;
  rs->host                  = enet_host_create(&rs->address, MAX_PEER, 1, 0, 0);
  rs->peer                  = enet_host_connect(rs->host, &gi->master_server_addr, 1, 0);
  assert(rs->peer);
  enet_host_flush(rs->host);
}

void RS_WaitForConnection(TR_game_instance_t *gi, TR_remote_cmd_store_ptr r) {
  TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
  ENetEvent e;
  if(!rs->connected) {
    if(enet_host_service(rs->host, &e, 50) != ENET_EVENT_TYPE_NONE) {
      switch(e.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            log("RS: Connection to master server succeed.");
            ENetPacket *packet =
            enet_packet_create(null, 0, ENET_PACKET_FLAG_RELIABLE);
            TR_leech_game_packet_t data;
            data.type         = TR_cmd_type_leech_game;
            data.player_id    = rs->player_id;
            log("rs player id: " + i2a(rs->player_id));
            enet_packet_resize(packet, sizeof(data));
            memcpy(packet->data, &data, sizeof(data));
            enet_peer_send(rs->peer, 0, packet);
            enet_host_flush(rs->host);
            rs->connected = true;
          } break;
        case ENET_EVENT_TYPE_DISCONNECT:
          log("RS: cannot connect to master server.");
          TR_Quit(888);
          break;
        case ENET_EVENT_TYPE_RECEIVE:
          assert(false);
          break;
      }
    }
  } else
    enet_host_service(rs->host, null, 0);
}

void RS_Free(TR_game_instance_t *gi, TR_remote_cmd_store_ptr r) {
  TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
  if(gi->remote_cmd_store_head == r)
    gi->remote_cmd_store_head = rs->next;
  if(rs->previous != TR_null)
    remote_cmd_store(rs->previous).next = rs->next;
  if(rs->next != TR_null)
    remote_cmd_store(rs->next).previous = rs->previous;
  if(rs->peer) {
    enet_peer_disconnect(rs->peer, 0);
    ENetEvent e;
    enet_host_service(rs->host, &e, 500);
    enet_peer_reset(rs->peer);
  }
  if(rs->host)
    enet_host_destroy(rs->host);
  log(gi->name + " rs: disconnection.");
  rs->fnext = gi->free_remote_cmd_store_head;
  gi->free_remote_cmd_store_head = r;
}

void RS_FreeAll(TR_game_instance_t *gi) {
  while(gi->remote_cmd_store_head != TR_null)
    RS_Free(gi, gi->remote_cmd_store_head);
  log("Remote cmd stores freed.");
}

void RS_Update(TR_game_instance_t *gi, TR_remote_cmd_store_ptr r) {
  TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
  rs->ping = gi->ping;
  RS_Leech(gi, r);
}

void RS_Leech(TR_game_instance_t *gi, TR_remote_cmd_store_ptr r) {
  TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
  ENetEvent e;
  while(enet_host_service(rs->host, &e, 0) != ENET_EVENT_TYPE_NONE) {
    ENetPacket *packet = e.packet;
    switch(e.type) {
      case ENET_EVENT_TYPE_CONNECT:
        log(gi->name + " rs: connection succeed.");
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        e.peer->data = NULL;
        log(gi->name + " rs: disconnection.");
        TR_Quit(888);
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        if(packet->dataLength > 0) {
          CS_LeechCompressedBuffer(gi, rs->cs, packet->dataLength, packet->data);
          /* replay */
          TR_buffer_t *buf = new TR_buffer_t;
          buf->data_length = packet->dataLength;
          buf->data = new char[buf->data_length];
          memcpy(buf->data, packet->data, buf->data_length);
          cmd_store(rs->cs).cmd_buffers.push_front(buf);
        }
        enet_packet_destroy(packet);
        break;
    }
  }
}

/*
 * camera
 */

void CAM_Init(TR_game_instance_t *gi, int argc, char** argv) {
  for(int i = 0; i < MAX_CAMERA; i++) {
    if(i != MAX_CAMERA-1)
      camera(i).fnext = i+1;
    else
      camera(i).fnext = TR_null;
  }
  gi->free_camera_head = 0;
  gi->camera_head      = TR_null;
  log("cameras initialized.");
}

TR_camera_ptr CAM_New(TR_game_instance_t *gi, TR_id_t player_id) {
  if(gi->free_camera_head != TR_null) {
    TR_camera_ptr r     = gi->free_camera_head;
    TR_camera_t *cam     = &camera(r);
    gi->free_camera_head    = camera(gi->free_camera_head).fnext;
    cam->selected_units_head  = TR_null;
    cam->skill_here          = false;
    cam->skill_type_count    = 0;
    cam->previous            = TR_null;
    cam->viewport_x          = 0;
    cam->viewport_y          = 0;
    cam->selected_sub_group  = -1;
    cam->hud_state           = 0;
    cam->window_opened       = false;
    cam->window_locked       = false;
    cam->selected_gs         = -1;
    cam->typing              = false;
    cam->last_hover          = null;
    cam->banner_node         = null;
    cam->player_id           = player_id;
    cam->flash               = "Welcome";
    cam->banner_anim_state   = null;
    DO_TIMES(MAX_GROUP_SELECTION) {
      cam->gs_head[f] = TR_null;
      cam->gs_sub_group[f] = -1;
    }
    cam->next                = gi->camera_head;
    /* manual textures */
    cam->fovmap_tex = Ogre::TextureManager::getSingleton().createManual(
      "FovMapTex",
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D,
      RANGE_MAP_SIZE_X, RANGE_MAP_SIZE_Y,
      0,
      Ogre::PF_R8G8B8A8,
      Ogre::TU_AUTOMIPMAP | Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    cam->minimap_camera_tex = Ogre::TextureManager::getSingleton().createManual(
      "MinimapCameraTex",
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D,
      MINIMAP_SIZE_X, MINIMAP_SIZE_X,
      1,
      Ogre::PF_R8G8B8A8,
      Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    cam->lagometer_tex = Ogre::TextureManager::getSingleton().createManual(
      "LagometerTex",
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D,
      LAGOMETER_SIZE, LAGOMETER_SIZE,
      1,
      Ogre::PF_R8G8B8A8,
      Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    cam->march_tex = Ogre::TextureManager::getSingleton().createManual(
      "MarchTex",
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D,
      MAP_SIZE_X*8, MAP_SIZE_Y*8,
      1,
      Ogre::PF_R8G8B8A8,
      Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    if(((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().
        getByName("lagometer_overlay_mat"))->
          getTechnique(0)->
            getPass(0)->getNumTextureUnitStates()==0) {
      ((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().
        getByName("lagometer_overlay_mat"))->
          getTechnique(0)->
            getPass(0)->
                createTextureUnitState("LagometerTex");
    }
    /* manage many human cameras */
    cam->camera = gi->ogre_scene->createCamera("camera " + i2a(r));
    cam->camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    #if MINIMAP_MODE
    cam->camera->setPosition(Ogre::Vector3(64, 200, 64));
    cam->camera->pitch(Ogre::Degree(-90));
    cam->camera->setOrthoWindow(150, 150);
    //cam->camera->setFOVy(Ogre::Degree(15));
    #else
    cam->camera->setPosition(Ogre::Vector3(0, CAMERA_HEIGHT*5, 0));
    cam->camera->pitch(Ogre::Degree(-CAMERA_ANGLE));
    cam->camera->setOrthoWindow(CAMERA_HEIGHT, CAMERA_HEIGHT);
    #endif
    cam->camera->setNearClipDistance(1);
    cam->viewport = gi->ogre_window->addViewport(cam->camera);
    cam->viewport->setBackgroundColour(Ogre::ColourValue(0.8f, 0.7f, 0.6f, 1.0f));
    cam->camera->setAspectRatio(
      Ogre::Real(cam->viewport->getActualWidth()) / Ogre::Real(cam->viewport->getActualHeight()));
    cam->viewport->setCamera(cam->camera);
    cam->selection = Ogre::OverlayManager::getSingleton().getByName("selection_overlay");
    assert(cam->selection);
    /* lagometer */
    cam->lagometer = Ogre::OverlayManager::getSingleton().getByName("lagometer_overlay");
    assert(cam->lagometer);
    /* minimap */
    cam->minimap = Ogre::OverlayManager::getSingleton().getByName("minimap_overlay");
    assert(cam->minimap);
    Ogre::OverlayContainer *panel = cam->minimap->getChild("minimap_overlay_panel");
    assert(panel);
    panel->setLeft(MINIMAP_START_X);
    panel->setTop(MINIMAP_START_Y);
    panel->setWidth(MINIMAP_SIZE_X);
    panel->setHeight(MINIMAP_SIZE_Y);
    if(!panel->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                getTextureUnitState("FovMapTex")) {
      Ogre::TextureUnitState *tex = 
        panel->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                createTextureUnitState("FovMapTex");
      tex->setName("FovMapTex");
      tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }
    cam->minimap->setZOrder(170);
    cam->minimap->show();
    /* minimap camera */
    cam->minimap_camera = Ogre::OverlayManager::getSingleton().getByName("minimap_camera_overlay");
    assert(cam->minimap_camera);
    cam->minimap_camera->show();
    Ogre::OverlayContainer *panel2 = cam->minimap_camera->getChild("minimap_camera_overlay_panel");
    assert(panel2);
    panel2->setLeft(MINIMAP_START_X);
    panel2->setTop(MINIMAP_START_Y);
    panel2->setWidth(MINIMAP_SIZE_X);
    panel2->setHeight(MINIMAP_SIZE_Y);
    if(!panel2->
        getMaterial()->
          getTechnique(0)->
            getPass(0)->
              getTextureUnitState("MinimapCameraTex")) {
      panel2->
        getMaterial()->
          getTechnique(0)->
            getPass(0)->
              createTextureUnitState("MinimapCameraTex");
      panel2->
        getMaterial()->
          getTechnique(0)->
            getPass(0)->
              getTextureUnitState(0)->
                setTextureAddressingMode(
                  Ogre::TextureUnitState::TAM_CLAMP);
    }
    cam->minimap_camera->setZOrder(180);
    /* skill overlays */
    DO_TIMES(MAX_SKILLS_PER_CAMERA) {
      cam->skill_overlays[f]           = Ogre::OverlayManager::getSingleton().create("skill " + i2a(r) + " " + i2a(f));
      cam->skill_overlays[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "skill " + i2a(r) + " " + i2a(f) + " panel"));
      cam->skill_overlays[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "skill " + i2a(r) + " " + i2a(f) + " panel 2"));
      cam->skill_overlays[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "skill " + i2a(r) + " " + i2a(f) + " panel 3"));
      cam->skill_overlays[f]->getChild("skill " + i2a(r) + " " + i2a(f) + " panel")->addChild(Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "skill " + i2a(r) + " " + i2a(f) + " shortcut"));
      cam->skill_overlays[f]->hide();
      cam->skill_overlays[f]->setZOrder(150);
      cam->skill_overlay_tex_panels[f] = Ogre::OverlayManager::getSingleton().create("skill " + i2a(r) + " " + i2a(f) + " texture panel");
      cam->skill_overlay_tex_panels[f]->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "skill " + i2a(r) + " " + i2a(f) + " panel tex"));
      cam->skill_overlay_tex_panels[f]->hide();
      cam->skill_overlay_tex_panels[f]->setZOrder(151);
      cam->skill_overlay_texs[f] = Ogre::TextureManager::getSingleton().createManual(
        "skill " + i2a(r) + " " + i2a(f) + " texture",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D,
        BTN_WIDTH, BTN_WIDTH,
        1,
        Ogre::PF_R8G8B8A8,
        Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
      Ogre::OverlayContainer *panel_tex = (Ogre::OverlayContainer*)
        cam->skill_overlay_tex_panels[f]->
            getChild("skill " + i2a(r) + " " + i2a(f) + " panel tex");
      assert(panel_tex);
      Ogre::MaterialPtr m = Ogre::MaterialManager::getSingleton().create(
        "skill " + i2a(r) + " " + i2a(f) + " material",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      panel_tex->setMaterialName("skill " + i2a(r) + " " + i2a(f) + " material");
      panel_tex->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
      panel_tex->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                createTextureUnitState("skill " + i2a(r) + " " + i2a(f) + " texture");
      panel_tex->
        getMaterial()->
          getTechnique(0)->
            getPass(0)->
              getTextureUnitState(0)->
                setTextureAddressingMode(
                  Ogre::TextureUnitState::TAM_CLAMP);
    }
    /* SKILL INFO */
    {cam->skill_info_overlay = Ogre::OverlayManager::getSingleton().create("skill info overlay " + i2a(r));
     Ogre::OverlayContainer *panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "skill info " + i2a(r) + " panel");
     panel->setLeft(0);
     panel->setTop(0);
     panel->setWidth(100);
     panel->setHeight(100);
     cam->skill_info_text_overlay = 
       (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
       "TextArea", "skill info text overlay " + i2a(r));
     panel->addChild(cam->skill_info_text_overlay);
     cam->skill_info_overlay->add2D(panel);
     cam->skill_info_overlay->hide();}
    /* RAY QUERY */
    cam->ray_query = gi->ogre_scene->createRayQuery(Ogre::Ray());
    /* BANNER */
    cam->banner_ent = gi->ogre_scene->createEntity("cam banner " + i2a(r), TEAM_ID(cam->player_id) == 0 ? "banner.mesh" : "banner.mesh");
    assert(cam->banner_ent);
    cam->banner_ent->setCastShadows(false);
    cam->banner_ent->setQueryFlags(0);
    cam->banner_node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("cam banner " + i2a(r));
    cam->banner_node->attachObject(cam->banner_ent);
    cam->banner_node->setPosition(Ogre::Vector3(0, 0.01, 0));
    cam->banner_ent->setVisible(false);
    cam->banner_node->setScale(Ogre::Vector3(0.25, 0.25, 0.25));
    assert(cam->banner_node);
    DO_TIMES(cam->banner_ent->getNumSubEntities()) {
      if(!cam->banner_ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getTextureUnitState("FovMapTex")) {
        Ogre::TextureUnitState *tex = 
          cam->banner_ent->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    createTextureUnitState("FovMapTex");
        tex->setName("FovMapTex");
        tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
      }
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTexture", false);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setShadowCasterVertexProgram("Ogre/BasicVertexPrograms/AmbientShadowCasterUnified");
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setShadowCasterFragmentProgram("Ogre/BasicFragmentPrograms/ShadowCasterWithAlpha");
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setSceneBlending(Ogre::SBT_REPLACE);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            setDepthWriteEnabled(true);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            setDepthCheckEnabled(true);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            setTransparencyCastsShadows(false);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setDepthWriteEnabled(true);
      cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
      if(cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                getNumTextureUnitStates() > 0)
        cam->banner_ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getTextureUnitState(0)->
                    setTextureFiltering(Ogre::TFO_NONE);
      if(cam->banner_ent->
        getSubEntity(f)->
          getMaterial()->
            getTechnique(0)->
              getPass(0)->
                getNumTextureUnitStates() > 1)
        cam->banner_ent->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getTextureUnitState(1)->
                    setTextureFiltering(Ogre::TFO_NONE);
    }
    PLAY_ANIM(cam->banner_ent, cam->banner_anim_state, "Idle", true)
    /* FLASH */
    static int mat_id = 0;
    {cam->flash_overlay   = Ogre::OverlayManager::getSingleton().create("flash overlay ");
     Ogre::OverlayContainer *panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "flash panel ");
     panel->setMetricsMode(Ogre::GMM_PIXELS);
     panel->setPosition(0, 0);
     panel->setDimensions(100, 100);
     cam->flash_overlay->show();
     cam->flash_overlay->setZOrder(170);
     cam->flash_overlay->add2D(panel);
     cam->flash_text_overlay = 
       (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
         "TextArea", "flash text overlay ");
     cam->flash_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
     cam->flash_text_overlay->setLeft(0);
     cam->flash_text_overlay->setTop(0);
     cam->flash_text_overlay->setCaption("Yoooopi");
     cam->flash_text_overlay->setFontName("cuckoo");
     cam->flash_text_overlay->setCharHeight(22);
     cam->flash_text_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
     cam->flash_text_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
     cam->flash_text_overlay->hide();
     panel->addChild(cam->flash_text_overlay);
     Ogre::MaterialPtr mat = 
        cam->flash_text_overlay->
          getMaterial()->
            clone("flash text overlay " + i2a(mat_id) + " mat");
      mat->compile();
      cam->flash_text_overlay->
        setMaterialName("flash text overlay " + i2a(mat_id) + " mat");
      mat_id++;}
    /* WIN LOSE OVERLAY */
    {cam->win_lose_overlay   = Ogre::OverlayManager::getSingleton().create("win_lose overlay ");
     Ogre::OverlayContainer *panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "win_lose panel ");
     panel->setMetricsMode(Ogre::GMM_PIXELS);
     panel->setPosition(0, 0);
     panel->setDimensions(100, 100);
     cam->win_lose_overlay->hide();
     cam->win_lose_overlay->add2D(panel);
     cam->win_lose_text_overlay = 
       (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
         "TextArea", "win_lose text overlay ");
     cam->win_lose_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
     cam->win_lose_text_overlay->setLeft(0);
     cam->win_lose_text_overlay->setTop(0);
     cam->win_lose_text_overlay->setCaption("Yoooopi");
     cam->win_lose_text_overlay->setFontName("cuckoo");
     cam->win_lose_text_overlay->setCharHeight(35);
     panel->addChild(cam->win_lose_text_overlay);}
    if(gi->camera_head != TR_null)
      camera(gi->camera_head).previous = r;
    gi->camera_head         = r;
    return r;
  } 
  log("No more camera left.");
  return TR_null;
}

void CAM_Free(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  cam->fovmap_tex->unload();
  cam->fovmap_tex.setNull();
  cam->lagometer_tex->unload();
  cam->lagometer_tex.setNull();
  cam->march_tex->unload();
  cam->march_tex.setNull();
  cam->minimap_camera_tex->unload();
  cam->minimap_camera_tex.setNull();
  DO_TIMES(MAX_SKILLS_PER_CAMERA) {
    cam->skill_overlay_texs[f]->unload();
    cam->skill_overlay_texs[f].setNull();
  }
  if(gi->camera_head == ca)
    gi->camera_head = cam->next;
  if(cam->previous != TR_null)
    camera(cam->previous).next = cam->next;
  if(cam->next != TR_null)
    camera(cam->next).previous = cam->previous;
  cam->fnext = gi->free_camera_head;
  gi->free_camera_head = ca;
}

void CAM_FreeAll(TR_game_instance_t *gi) {
  while(gi->camera_head != TR_null)
    CAM_Free(gi, gi->camera_head);
  log("cameras freed.");
}

Ogre::Vector3 CAM_GetWorldMousePosition(TR_game_instance_t *gi, TR_camera_ptr ca) {
  return CAM_RayToVector(gi, ca, CAM_GetMouseRay(gi, ca));
}

void CAM_Update(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  CAM_UpdateBanner(gi, ca);
  CAM_UpdateInputs(gi, ca);
  CAM_UpdateFovMapTex(gi, ca);
  //CAM_UpdateMarchTex(gi, ca);
  CAM_UpdateMinimapTex(gi, ca);
  CAM_UpdateLagometer(gi, ca);
  CAM_UpdateSelection(gi, ca);
  CAM_UpdateMouseHover(gi, ca);
  CAM_UpdateUnitRender(gi, ca);
  CAM_UpdateFlash(gi, ca);
  CAM_UpdateWinLose(gi, ca);
  SO_Update(gi, ca);
}

void CAM_UpdateInputs(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_client_ptr c = GI_GetClientPtr(gi, cam->player_id);
  TR_client_t *cl = &client(c);
  /* UPDATE HUD */
  bool process_inputs = CAM_UpdateHUD(gi, ca);
  if(process_inputs) {
    Ogre::Vector3 mouse_p = CAM_GetWorldMousePosition(gi, ca);
    BOUND_MINMAX(1, MAP_SIZE_X-2, mouse_p.x) 
    BOUND_MINMAX(1, MAP_SIZE_Y-2, mouse_p.z)
    if(cam->skill_here) {
      assert(cam->skill_type_count > 0);
      gi->cursor_tex = gi->get_cursor_cb(gi, ca, cam->skill_types[0], gi->mouse.x, gi->mouse.y);
    } else 
      gi->cursor_tex = "cursor2";
    if(
    #if RELEASE
      cl->type == REPLAY_CLIENT && 
    #endif
      GI_IsKeyPressed(gi, OIS::KC_F11)) {
      cam->player_id++;
      if(cam->player_id >= GI_GetPlayerCount(gi))
        cam->player_id = 0;
    }
    if(gi->lbtn_down
    && !cam->skill_here
    && !cam->selection_started
    && INSIDE_MINI(gi->mouse.x, gi->mouse.y))
      cam->pointing_minimap = true;
    if(gi->lbtn_down
    && !cam->skill_here
    && !cam->selection_started
    && cam->pointing_minimap) {
      CAM_LookAt(gi, ca,
        ((gi->mouse.x-MINIMAP_START_X)*MINIMAP_INV_RATIO_X)<<POS_SHIFT,
        ((gi->mouse.y-MINIMAP_START_Y)*MINIMAP_INV_RATIO_Y)<<POS_SHIFT);
    }
    if(GI_IsKeyPressed(gi, OIS::KC_F12)) {
      gi->ogre_window->writeContentsToTimestampedFile("./screenshot/screenshot", ".png");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_TAB))
      CAM_SelectNextSubGroup(gi, ca);
    if(GI_IsKeyPressed(gi, OIS::KC_RETURN)) {
      if(!cam->typing) {
        TCOD_console_set_keyboard_repeat(500, 50);
        cam->typing = true;
        cam->send_to_all = GI_IsKeyDown(gi, OIS::KC_LCONTROL);
        cam->chat_btn.s = "";
        cam->chat_btn.color = TCOD_white;
        cam->chat_btn.highlight = false;
        cam->chat_btn.entry = true;
        cam->chat_btn.shortcut = 0;
        cam->chat_btn.shortcut_special = (TCOD_keycode_t)-1;
      } else {
        if(cam->chat_btn.s.length() > 0) {
          gi->send_message = cam->chat_btn.s;
          gi->send_to_all = cam->send_to_all;
        }
        cam->typing = false;
        TCOD_console_set_keyboard_repeat(1, 0);
      }
      GI_StopKeyPropagation(gi, OIS::KC_RETURN);
    }
    if(GI_IsKeyPressed(gi, OIS::KC_ESCAPE)
    && cam->typing) {
      cam->typing = false;
      GI_StopKeyPropagation(gi, OIS::KC_ESCAPE);
    }
    if(gi->lbtn_up)
      cam->pointing_minimap = false;
    if(!cam->selection_started) {
      if(gi->mouse.x <= 10
      || GI_IsKeyDown(gi, OIS::KC_LEFT))
        cam->viewport_x -= ((int)MAX(1, ((float)10.f * gi->frame_time)));
      if(gi->mouse.x >= resx-10
      || GI_IsKeyDown(gi, OIS::KC_RIGHT))
        cam->viewport_x += ((int)MAX(1, ((float)10.f * gi->frame_time)));
      if(gi->mouse.y <= 10
      || GI_IsKeyDown(gi, OIS::KC_UP))
        cam->viewport_y -= ((int)MAX(1, ((float)10.f * gi->frame_time)));
      if(gi->mouse.y >= resy-10
      || GI_IsKeyDown(gi, OIS::KC_DOWN))
        cam->viewport_y += ((int)MAX(1, ((float)10.f * gi->frame_time)));
      BOUND_VIEWPORT
      if(GI_IsKeyDown(gi, OIS::KC_SPACE))
        CAM_FocusOnUnits(gi, ca);
    }
    /* restore selection */
    bool shift_down = !GI_IsKeyDown(gi, OIS::KC_LSHIFT)
                   && !GI_IsKeyDown(gi, OIS::KC_RSHIFT);
    if(GI_IsKeyPressed(gi, OIS::KC_1)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 0, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_2)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 1, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_3)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 2, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_4)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 3, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_5)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 4, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_6)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 5, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_7)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 6, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_8)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 7, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_9)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 8, shift_down);
    if(GI_IsKeyPressed(gi, OIS::KC_0)
    && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    && !GI_IsKeyDown(gi, OIS::KC_RCONTROL))
      CAM_RestoreGroupSelection(gi, ca, 9, shift_down);
    if(GI_IsKeyDown(gi, OIS::KC_1)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 0);
    if(GI_IsKeyDown(gi, OIS::KC_2)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 1);
    if(GI_IsKeyDown(gi, OIS::KC_3)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 2);
    if(GI_IsKeyDown(gi, OIS::KC_4)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 3);
    if(GI_IsKeyDown(gi, OIS::KC_5)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 4);
    if(GI_IsKeyDown(gi, OIS::KC_6)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 5);
    if(GI_IsKeyDown(gi, OIS::KC_7)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 6);
    if(GI_IsKeyDown(gi, OIS::KC_8)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 7);
    if(GI_IsKeyDown(gi, OIS::KC_9)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 8);
    if(GI_IsKeyDown(gi, OIS::KC_0)
    && (GI_IsKeyDown(gi, OIS::KC_LCONTROL)
    || GI_IsKeyDown(gi, OIS::KC_RCONTROL)))
      CAM_CopySelectionTo(gi, ca, 9);
    /* MISC */
    if(gi->lbtn_down 
    && !cam->selection_started
    && !cam->skill_here
    && !cam->pointing_minimap
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      cam->mp_lb_x = gi->mouse.x;
      cam->mp_lb_y = gi->mouse.y;
      cam->selection_started = true;
      cam->selection->show();
    }
    if(gi->lbtn_up
    && cam->selection_started) {
      cam->mr_lb_x = gi->mouse.x;
      cam->mr_lb_y = gi->mouse.y;
      cam->selection_started = false;
      cam->selection->hide();
      if(!cam->skill_here
      && CAM_IsThereUnitsHere(gi, ca, cam->mp_lb_x, cam->mp_lb_y, cam->mr_lb_x, cam->mr_lb_y)) {
        if(!GI_IsKeyDown(gi, OIS::KC_LSHIFT)
        && !GI_IsKeyDown(gi, OIS::KC_RSHIFT))
          CAM_FreeUnitSelection(gi, ca);
        TR_unit_ptr u = CAM_PickUnit(gi, ca, cam->mp_lb_x, cam->mp_lb_y);
        if((GI_IsKeyDown(gi, OIS::KC_LSHIFT)
        || GI_IsKeyDown(gi, OIS::KC_RSHIFT))
        && !GI_IsKeyDown(gi, OIS::KC_LCONTROL)
        && !GI_IsKeyDown(gi, OIS::KC_RCONTROL)
        && ABS(cam->mp_lb_x-cam->mr_lb_x) < PICKING_THRESHOLD
        && ABS(cam->mp_lb_y-cam->mr_lb_y) < PICKING_THRESHOLD
        && CAM_IsUnitSelected(gi, ca, u))
          CAM_SubToSelection(gi, ca, u);
        else {
          CAM_SelectUnits(
            gi, ca, 
            cam->mp_lb_x, cam->mp_lb_y, cam->mr_lb_x, cam->mr_lb_y,
            (GI_IsKeyDown(gi, OIS::KC_LCONTROL) || GI_IsKeyDown(gi, OIS::KC_RCONTROL)) && u != TR_null ? unit(u).unit_type : TR_unit_type_count,
            (GI_IsKeyDown(gi, OIS::KC_LCONTROL) || GI_IsKeyDown(gi, OIS::KC_RCONTROL)));
        }
      }
      GI_StopClicksPropagation(gi);
    }
    if(!GI_IsKeyDown(gi, OIS::KC_LSHIFT)
    && !GI_IsKeyDown(gi, OIS::KC_RSHIFT)
    && cam->used_skill) {
      cam->used_skill = false;
      cam->skill_here = false;
    }
    if(GI_IsKeyPressed(gi, OIS::KC_ESCAPE)
    && cam->skill_here) {
      cam->skill_here = false;
      GI_StopKeyPropagation(gi, OIS::KC_ESCAPE);
    }
    #if !RELEASE
    if(GI_IsKeyPressed(gi, OIS::KC_F3))
      gi->show_all = !gi->show_all;
    if(GI_IsKeyPressed(gi, OIS::KC_F4))
      gi->show_lagometer = !gi->show_lagometer;
    #endif
    if(GI_IsKeyPressed(gi, OIS::KC_F7)) {
      do_debug = !do_debug;
    }
  } else {
    cam->selection_started = false;
    cam->selection->hide();
    cam->skill_here = false;
  }
  /* set camera */
  cam->camera->setPosition(
    Ogre::Vector3(
      #if MINIMAP_MODE
      64,
      200,
      64
      #else
      (float)cam->viewport_x/(1<<POS_SHIFT), 
      CAMERA_HEIGHT*5,
      (float)cam->viewport_y/(1<<POS_SHIFT)
      #endif
      ));
}

bool CAM_UpdateHUD(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam      = &camera(ca);
  TR_player_ptr p       = GI_GetPlayerPtr(gi, cam->player_id);
  TR_gp_player_ptr mp   = player(p).user_data;
  TR_gp_player_t *mpl   = &gp_player(mp);
  cam->skill_info_overlay->hide();
  bool result = gi->hud_info_cb(gi, ca);
  if(result) {
    if(cam->selected_units_head != TR_null) {
      DO_TIMES(MAX_SKILLS_PER_CAMERA)
        cam->skill_overlays[f]->hide();
      if(!cam->skill_here)
        cam->skill_type_count = 0;
      int   skill_ids[MAX_SKILLS_PER_CAMERA];
      TR_skill_type_t skill_types[MAX_SKILLS_PER_CAMERA];
      TR_unit_ptr skill_units[MAX_SKILLS_PER_CAMERA];
      int   skill_count = gi->get_selection_skills_cb(gi, ca, skill_ids, skill_types, skill_units),
            count = 0;
      DO_TIMES(skill_count)
        count++;
      int i = 0;
      DO_TIMES(skill_count) {
        int left = gi->ogre_window->getWidth()/2 - (count+1)*(BTN_WIDTH+8)/2 + i*(BTN_WIDTH+8) + BTN_WIDTH/2,
            top  = gi->ogre_window->getHeight()-12-BTN_WIDTH;
        char shortcut = gi->get_skill_shortcut_cb(gi, skill_types[f]);
        bool is_inside = gi->mouse.x >= left
                      && gi->mouse.x < left + BTN_WIDTH
                      && gi->mouse.y >= top 
                      && gi->mouse.y < top + BTN_WIDTH;
        bool gonna_be_clicked = (is_inside && gi->lbtn_down) || (shortcut != 0 && GI_IsKeyDown(gi, GI_CharToKeyCode(gi, shortcut)));
        bool is_clicked = (is_inside && gi->lbtn_up) || (shortcut != 0 && GI_IsKeyPressed(gi, GI_CharToKeyCode(gi, shortcut)));
        /* patch */
        if(is_clicked && shortcut != 'Q')
          cam->skill_type_count = 0;
        Ogre::OverlayContainer *panel = cam->skill_overlays[f]->getChild("skill " + i2a(ca) + " " + i2a(f) + " panel");
        assert(panel);
        panel->setMetricsMode(Ogre::GMM_PIXELS);
        panel->setLeft(left);
        panel->setTop(top);
        panel->setWidth(BTN_WIDTH);
        panel->setHeight(BTN_WIDTH);
        panel->setMaterialName(gi->get_skill_material_cb(gi, skill_types[f]));
        Ogre::PanelOverlayElement *panel2 = (Ogre::PanelOverlayElement*)cam->skill_overlays[f]->getChild("skill " + i2a(ca) + " " + i2a(f) + " panel 2");
        assert(panel2);
        panel2->setMetricsMode(Ogre::GMM_PIXELS);
        panel2->setLeft(left);
        panel2->setTop(top);
        panel2->setWidth(BTN_WIDTH);
        panel2->setHeight(BTN_WIDTH);
        panel2->setMaterialName("selection_mat_white");
        panel2->setUV(0, 0, 1, 1); 
        if(is_inside)
          panel2->show();
        else
          panel2->hide();
        Ogre::PanelOverlayElement *panel3 = (Ogre::PanelOverlayElement*)cam->skill_overlays[f]->getChild("skill " + i2a(ca) + " " + i2a(f) + " panel 3");
        assert(panel3);
        panel3->setMetricsMode(Ogre::GMM_PIXELS);
        panel3->setLeft(left);
        panel3->setTop(top);
        panel3->setWidth(BTN_WIDTH);
        panel3->setHeight(BTN_WIDTH);
        panel3->setMaterialName("selection_mat_green");
        panel3->setUV(0, 0, 1, 1); 
        if((is_inside 
        && gi->lbtn_down)
        || gonna_be_clicked)
          panel3->show();
        else
          panel3->hide();
        Ogre::TextAreaOverlayElement *text = (Ogre::TextAreaOverlayElement*)panel->getChild("skill " + i2a(ca) + " " + i2a(f) + " shortcut");
        assert(text);
        if(shortcut != 0) {
          text->setMetricsMode(Ogre::GMM_PIXELS);
          text->setLeft(3);
          text->setTop(0);
          text->setCaption(str(1, shortcut));
          text->setFontName("cuckoo");
          text->setCharHeight(20);
          text->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
          text->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
        } else 
          text->hide();
        /* texture */
        float min_cooldown = 0;
        bool can_cast = false,
             not_enough_mana = true,
             not_enough_level = true,
             is_passive = gi->get_skill_is_passive_cb(gi, skill_types[f]);
        TR_unit_ptr u = cam->selected_units_head;
        while(u != TR_null) {
          TR_unit_t *un = &unit(u);
          if(UN_SkillId(gi, u, skill_types[f]) != -1) {
            int skill_cooldown = gi->get_skill_cooldown_cb(gi, p, u, skill_types[f]);
            float cooldown = 
              un->silenced != 0 ? 0.f :
              skill_cooldown == 0 ? 1.f : 
              (float)un->skills_cooldown[skill_ids[f]]/skill_cooldown;
            bool nem = mpl->mana < gi->get_skill_mana_cost_cb(gi, p, u, skill_types[f]),
                 nel = !gi->get_skill_is_available_cb(gi, skill_types[f]) 
                    && un->skill_levels[skill_types[f]] == 0;
            not_enough_mana = not_enough_mana && nem;
            not_enough_level = not_enough_level && nel;
            bool local_can_cast = un->silenced == 0 && !nem && !nel;
            can_cast = can_cast || local_can_cast;            
            if(local_can_cast
            && min_cooldown < cooldown)
              min_cooldown = cooldown;
          }
          u = unit(u).snext[ca];
        }
        Ogre::OverlayContainer *panel_tex = (Ogre::OverlayContainer*)
          cam->skill_overlay_tex_panels[f]->
            getChild("skill " + i2a(ca) + " " + i2a(f) + " panel tex");
        panel_tex->setMetricsMode(Ogre::GMM_PIXELS);
        panel_tex->setLeft(left);
        panel_tex->setTop(top);
        panel_tex->setWidth(BTN_WIDTH);
        panel_tex->setHeight(BTN_WIDTH);
        Ogre::HardwarePixelBufferSharedPtr pixels = cam->skill_overlay_texs[f]->getBuffer();
        pixels->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox &box = pixels->getCurrentLock();
        int *dest = (int*)box.data;
        float angle = 2.f*3.14*min_cooldown-3.14;
        FOR_RECT(0, BTN_WIDTH-1, 0, BTN_WIDTH-1) {
          bool in_angle = angle < 3.14 && min_cooldown != 1.f ? atan2(-(i-BTN_WIDTH/2), j-BTN_WIDTH/2) <= angle : true;
          int r = 0, 
              g = 0, 
              b = 0;
          if(!can_cast) {
            if(not_enough_level) {
              r = 50;
              g = 50;
              b = 50;
            } else
            if(not_enough_mana) {
              r = 80;
              g = 30;
              b = 150;
            } else {
              r = 150;
              g = 51;
              b = 51;
            }
          }
          TCOD_color_t color = TCOD_color_RGB(r, g, b);
          /*if(can_cast
          && (is_inside || gonna_be_clicked))
            color = TCOD_color_RGB(191, 200, 245);*/
          if(!in_angle && !not_enough_level)
            color = TCOD_color_lerp(color, TCOD_black, 0.5f);
          dest[box.rowPitch*j+i] = 
            (!is_passive && (!in_angle || !can_cast /*|| is_inside || gonna_be_clicked*/) ? 0xaa000000 : 0)|COLOR(color);
        }
        pixels->unlock();
        if(is_inside) {
          str info = gi->get_skill_info_cb(gi, skill_types[f]);
          Ogre::OverlayContainer *panel = cam->skill_info_overlay->getChild("skill info " + i2a(ca) + " panel"); 
          assert(panel);
          panel->setMetricsMode(Ogre::GMM_PIXELS);
          panel->setLeft(gi->ogre_window->getWidth()/2-info.size()/2*8);
          panel->setTop(gi->ogre_window->getHeight()-18-48*3+8+22);
          panel->setWidth(gi->ogre_window->getWidth());
          panel->setHeight(24);
          panel->hide();
          cam->skill_info_text_overlay->setCaption(info);
          cam->skill_info_text_overlay->setLeft(0);
          cam->skill_info_text_overlay->setTop(0);
          cam->skill_info_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
          cam->skill_info_text_overlay->setFontName("cuckoo");
          cam->skill_info_text_overlay->setCharHeight(22);
          cam->skill_info_text_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
          cam->skill_info_text_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
          cam->skill_info_overlay->show();
        }
        /* shows */
        cam->skill_overlay_tex_panels[f]->show();
        cam->skill_overlays[f]->show();
        i++;
        /* cast */
        if(is_clicked
        && !cam->selection_started) {
          bool found = false;
          DO_TIMES2(cam->skill_type_count)
            if(cam->skill_types[i] == skill_types[f]) {
              found = true;
              break;
            }
          if(!found) {
            cam->skill_here = true;
            cam->skill_types[cam->skill_type_count] = skill_types[f];
            cam->skill_type_count = MIN(cam->skill_type_count+1, MAX_SKILLS_PER_CAMERA-1);
          }
        }
        if(is_clicked || gonna_be_clicked)
          GI_StopClicksPropagation(gi);
      }
      DO_TIMES(MAX_SKILLS_PER_CAMERA-skill_count) {
        cam->skill_overlay_tex_panels[skill_count+f]->hide();
        cam->skill_overlays[skill_count+f]->hide();
      }
      #if RECORD_DEMO
      DO_TIMES(MAX_SKILLS_PER_CAMERA) {
        cam->skill_overlay_tex_panels[f]->hide();
        cam->skill_overlays[f]->hide();
      }
      #endif
    } else {
      DO_TIMES(MAX_SKILLS_PER_CAMERA) {
        cam->skill_overlay_tex_panels[f]->hide();
        cam->skill_overlays[f]->hide();
      }
    }
  } 
  /* HUD main information */
  return result;
}

void CAM_UpdateSelection(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  if(cam->selection_started) {
    int s_cx = cam->mp_lb_x, 
        s_cy = cam->mp_lb_y,
        e_cx = gi->mouse.x, 
        e_cy = gi->mouse.y;
    IN_ORDER(s_cx, s_cy, e_cx, e_cy)
    Ogre::OverlayContainer *panel = cam->selection->getChild("selection_overlay_panel");
    assert(panel);
    panel->setLeft(s_cx);
    panel->setTop(s_cy);
    panel->setWidth(e_cx-s_cx);
    panel->setHeight(e_cy-s_cy);
  }
}

void CAM_UpdateBanner(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  assert(cam->banner_node);
  int pos_x = 0,
        pos_y = 0,
        k = 0;
  bool skill_here = false,
       can_hide = true;
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(UO_GetFrontType(gi, u) != TR_unit_order_none
    && !UO_GetFront(gi, u)->generated) {
      int x = (UO_GetFront(gi, u)->target != TR_null ? unit(UO_GetFront(gi, u)->target).x + UN_GetSizeShift(gi, SIZE(&unit(UO_GetFront(gi, u)->target))): UO_GetFront(gi, u)->click_cx + UN_GetSizeShift(gi, SIZE(un))),
          y = (UO_GetFront(gi, u)->target != TR_null ? unit(UO_GetFront(gi, u)->target).y + UN_GetSizeShift(gi, SIZE(&unit(UO_GetFront(gi, u)->target))): UO_GetFront(gi, u)->click_cy + UN_GetSizeShift(gi, SIZE(un)));
      pos_x += x;
      pos_y += y;
      k++;
      skill_here = UO_GetFront(gi, u)->skill_here && skill_here;
    }
    if(gi->draw_unit_goal_cb)
      can_hide = gi->draw_unit_goal_cb(gi, ca, u) && can_hide;
    u = unit(u).snext[ca];
  }
  if(k > 0 && can_hide)
    cam->banner_node->setPosition(Ogre::Vector3((float)(pos_x/k)/(1<<POS_SHIFT), 0.01, (float)(pos_y/k)/(1<<POS_SHIFT)));
  cam->banner_ent->setVisible(!can_hide || (!skill_here && cam->selected_units_head != TR_null));
  if(cam->banner_anim_state)
    cam->banner_anim_state->addTime((float)gi->game_time_step*0.001f*ANIMATION_SPEED_FACTOR);
}

void CAM_UpdateMouseHover(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cl_p = GI_GetPlayerPtr(gi, cam->player_id);
  if(cam->last_hover != null)
    cam->last_hover->setVisible(false);
  TR_unit_ptr u = CAM_PickUnit(gi, ca, gi->mouse.x, gi->mouse.y);
  if(u != TR_null) {
    Ogre::Entity *ent = gi->ogre_scene->getEntity("unit " + i2a(u) + " hover");
    ent->setVisible(PL_IsInFov(gi, cl_p, u));
    cam->last_hover = ent;
  }
}

void CAM_UpdateFovMapTex(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cl_p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cl_pl = &player(cl_p);
  Ogre::HardwarePixelBufferSharedPtr pixels = cam->fovmap_tex->getBuffer();
  pixels->lock(Ogre::HardwareBuffer::HBL_DISCARD);
  const Ogre::PixelBox &box = pixels->getCurrentLock();
  int *fovmap = gi->fov_maps[cl_pl->team_id];
  unsigned int *dest = (unsigned int*)box.data;
  for(int j = 0; j < RANGE_MAP_SIZE_Y; j++) {
    for(int i = 0; i < RANGE_MAP_SIZE_X; i++) {
      #if MINIMAP_MODE
      *dest = 0xffffffff;
      #else
      TCOD_color_t c = TCOD_color_multiply_scalar(AT(static_light_map, i, j), (float)MAX(115, *fovmap)/255);
      *dest = COLOR(c);
      #endif
      dest++;
      fovmap++;
    }
    dest += box.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(box.format);
  }
  pixels->unlock();
}

void CAM_UpdateMarchTex(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cl_p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cl_pl = &player(cl_p);
  Ogre::HardwarePixelBufferSharedPtr pixels = cam->march_tex->getBuffer();
  pixels->lock(Ogre::HardwareBuffer::HBL_DISCARD);
  const Ogre::PixelBox &box = pixels->getCurrentLock();
  int *dest = (int*)box.data;
  #if THREADS  
  EnterCriticalSection(&debug_crit);
  #endif  
  memcpy(dest, gi->march_tex, sizeof(int)*MAP_SIZE*8*8);
  #if THREADS    
  LeaveCriticalSection(&debug_crit);
  #endif
  //DO_TIMES(MAP_SIZE*8*8)
  //  *dest++ = 0;
  //dest = (int*)box.data;
  //for(int j = 0; j < MAP_SIZE_Y*8; j++) {
  //  for(int i = 0; i < MAP_SIZE_X*8; i++) {
  //    *dest = MIN(255, AT(clearance_map, i>>3, j>>3)/30);
  //    dest++;
  //  }
  //}
  //dest = (int*)box.data;
  //DO_TIMES(gi->edge_count[0]) {
  //  int x0 = gi->edges[0][f].x0>>(POS_SHIFT-3),
  //      x1 = gi->edges[0][f].x1>>(POS_SHIFT-3),
  //      y0 = gi->edges[0][f].y0>>(POS_SHIFT-3),
  //      y1 = gi->edges[0][f].y1>>(POS_SHIFT-3);
  //  TCOD_line_init(x0, y0, x1, y1);
  //  do {
  //    dest[MAP_SIZE_X*8*y0+x0] = 0xff00ff00;
  //  }while(!TCOD_line_step(&x0, &y0));
  //}
  pixels->unlock();
}

void CAM_UpdateMinimapTex(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cam_p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cam_pl = &player(cam_p);
  Ogre::HardwarePixelBufferSharedPtr pixels = cam->minimap_camera_tex->getBuffer();
  pixels->lock(Ogre::HardwareBuffer::HBL_DISCARD);
  const Ogre::PixelBox &box = pixels->getCurrentLock();
  int *dest = (int*)box.data;
  int *fov_map = gi->fov_maps[cam_pl->team_id];
  DO_TIMES(MINIMAP_SIZE)
    *dest++ = 0;
  /*FOR_RECT(0, MINIMAP_SIZE_X-1, 0, MINIMAP_SIZE_Y-1) {
    int x = i*MAP_SIZE_X/MINIMAP_SIZE_X,
        y = j*MAP_SIZE_Y/MINIMAP_SIZE_Y;
    int r = AT_ENV(x, y) == TR_WALL ? 0x33 : 0x99;
    r = ((MIN(255, 25+AT2(fov_map, x, y)) * r)>>8)&0xff;
    *dest = 0xff000000 | (r<<16) | (r<<8) | r;
    dest++;
  }*/
  dest = (int*)box.data;
  TR_unit_ptr u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(un->player_id != cam->player_id) {
      bool vis = PL_IsInFov(gi, cam_p, u);
      un->ent[un->anim_step]->setVisible(vis);
      if(vis) {
        int color = un->team_id == cam_pl->team_id ? 0xff4444ff : 0xffff0000;
        int pos_x = (un->x>>POS_SHIFT)*MINIMAP_RATIO_X,
            pos_y = (un->y>>POS_SHIFT)*MINIMAP_RATIO_Y;
        FOR_RECT(pos_x-2, pos_x+2,pos_y-2, pos_y+2)
          dest[box.rowPitch*j+i] = color;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y-3)+pos_x-2+f] = 0xcccccccc;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y+3)+pos_x-2+f] = 0xcccccccc;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y-2+f)+pos_x-3] = 0xcccccccc;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y-2+f)+pos_x+3] = 0xcccccccc;
      }
    }
    u = un->next;
  }
  u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(un->player_id == cam->player_id) {
      bool vis = PL_IsInFov(gi, cam_p, u);
      un->ent[un->anim_step]->setVisible(vis);
      if(vis) {
        int color = 0xff00bb00;
        int pos_x = (un->x>>POS_SHIFT)*MINIMAP_RATIO_X,
            pos_y = (un->y>>POS_SHIFT)*MINIMAP_RATIO_Y;
        FOR_RECT(pos_x-2, pos_x+2,pos_y-2, pos_y+2)
          dest[box.rowPitch*j+i] = color;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y-3)+pos_x-2+f] = 0xcccccccc;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y+3)+pos_x-2+f] = 0xcccccccc;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y-2+f)+pos_x-3] = 0xcccccccc;
        DO_TIMES(5)
          dest[box.rowPitch*(pos_y-2+f)+pos_x+3] = 0xcccccccc;
      }
    }
    u = un->next;
  }
  dest = (int*)box.data;
  Ogre::Vector3 p[4];
  p[0] = CAM_ScreenToWorld(gi, ca, 0, 0);
  p[1] = CAM_ScreenToWorld(gi, ca, gi->ogre_window->getWidth(), 0);
  p[2] = CAM_ScreenToWorld(gi, ca, gi->ogre_window->getWidth(), gi->ogre_window->getHeight());
  p[3] = CAM_ScreenToWorld(gi, ca, 0, gi->ogre_window->getHeight());
  int x0, y0, x1, y1;
  DO_TIMES(4) {
    x0 = p[f].x*MINIMAP_RATIO_X; 
    y0 = p[f].z*MINIMAP_RATIO_Y;
    x1 = p[(f+1)&3].x*MINIMAP_RATIO_X; 
    y1 = p[(f+1)&3].z*MINIMAP_RATIO_Y;
    TCOD_line_init(x0, y0, x1, y1);
    do {
      if(INSIDE_MINI_TEX(x0, y0))
        dest[box.rowPitch*y0+x0] = 0xcccccccc;
    } while(!TCOD_line_step(&x0, &y0));
  }
  pixels->unlock();
}

void CAM_UpdateLagometer(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cam_p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cam_pl = &player(cam_p);
  #if !RELEASE
  if(gi->show_lagometer) {
    Ogre::HardwarePixelBufferSharedPtr pixels = cam->lagometer_tex->getBuffer();
    pixels->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox &box = pixels->getCurrentLock();
    int *dest = (int*)box.data;
    FOR_RECT(0, LAGOMETER_SIZE-1, 0, LAGOMETER_SIZE-1)
      ON_LAGOMETER(i, j) = 0xffffffff;
    DO_TIMES(LAGOMETER_SIZE-1) {
      int i = gi->aver_frame_times_counter-MAX_AVER_FRAME_TIMES-1+f;
      if(i<0) i = MAX_AVER_FRAME_TIMES+i;
      int j = (i+1)%MAX_AVER_FRAME_TIMES;
      int x = f, 
          y = LAGOMETER_SIZE-1-(int)(1.f/(gi->aver_frame_times[i]/1000.f));
      TCOD_line_init(
        x, 
        y,
        f+1,
        LAGOMETER_SIZE-1-(int)(1.f/(gi->aver_frame_times[j]/1000.f)));
      do{
        assert(x>=0 && x<LAGOMETER_SIZE);
        if(y<0)y=0;
        if(y>=LAGOMETER_SIZE)y=LAGOMETER_SIZE-1;
        ON_LAGOMETER(x, y) = COLOR(TCOD_red);
      }while(!TCOD_line_step(&x,&y));
    }
    for(int f = 0 ; f < LAGOMETER_SIZE; f++)
      ON_LAGOMETER(f, LAGOMETER_SIZE-1-60) = COLOR(TCOD_blue);
    pixels->unlock();
    /* overlay placement */
    Ogre::OverlayContainer *panel = cam->lagometer->getChild("lagometer_overlay_panel");
    assert(panel);
    panel->setLeft(gi->ogre_window->getWidth()-LAGOMETER_SIZE);
    panel->setTop(gi->ogre_window->getHeight()-4*LAGOMETER_SIZE);
    panel->setWidth(LAGOMETER_SIZE);
    panel->setHeight(LAGOMETER_SIZE);
    cam->lagometer->show();
  } else
     cam->lagometer->hide();
  #else
  cam->lagometer->hide();
  #endif
}

void CAM_UpdateUnitRender(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cam_p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *cam_pl = &player(cam_p);
  TR_unit_ptr u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    bool vis = PL_IsInFov(gi, cam_p, u);
    un->select_ent->setVisible(un->select_ent->getVisible() && vis);
    un->ent[un->anim_step]->setVisible(vis);
    if(vis) {
      /* selection node */
      float x = (float)un->x/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)),
            z = (float)un->y/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un));
      un->select_node->setPosition(
      un->node->convertWorldToLocalPosition(Ogre::Vector3(x, 0.01f, z)));
      /*un->select_node->setOrientation(
        un->node->convertWorldToLocalOrientation(
          Ogre::Quaternion(
            Ogre::Radian(-atan2(
              cam->viewport_y-UNIT_Y(un), 
              cam->viewport_x-UNIT_X(un)) + 3.14/2), 
            Ogre::Vector3(0, 1, 0))));*/
      /* hp */
      Ogre::Vector2 pos = 
        CAM_WorldToScreen(
          gi, ca, 
          (float)un->x/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)), 
          (float)un->y/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un))-BAR_HEIGHT(un), 
          0);
      ST_SetBufPos(gi, ca, u, pos, vis);
      if((pos.x != -1 || pos.y != -1)
      /*&& GI_IsKeyDown(gi, OIS::KC_LMENU)*/) {
        pos.x *= gi->ogre_window->getWidth();
        pos.y *= gi->ogre_window->getHeight();
        Ogre::OverlayContainer *panel = un->hp_overlay->getChild("hp " + i2a(u) + " panel");
        assert(panel);
        panel->setLeft(pos.x-HP_BAR_SIZE(un)/2);
        panel->setTop(pos.y);
        panel->setMetricsMode(Ogre::GMM_PIXELS);
        panel->setWidth(un->hp * HP_BAR_SIZE(un) / MAX_HP(un));
        panel->setHeight(4);
        if(INSIDE_SCREEN(panel->getLeft(), panel->getTop())
        || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop())
        || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop()+panel->getHeight())
        || INSIDE_SCREEN(panel->getLeft(), panel->getTop()+panel->getHeight())) {
          panel->setMaterialName(
            cam->player_id == un->player_id ? "hp_mat_me" : 
            cam_pl->team_id == un->team_id ? "hp_mat_same_team" : 
            "hp_mat_enemy");
          un->hp_overlay->setZOrder(99);
          #if !MINIMAP_MODE
          un->hp_overlay->show();
          #endif
        } else
          un->hp_overlay->hide();
        /* LEVEL */
        if(HERO(un)) {
          Ogre::TextAreaOverlayElement *text_level = (Ogre::TextAreaOverlayElement*)panel->getChild("level text " + i2a(u));
          text_level->setMetricsMode(Ogre::GMM_PIXELS);
          text_level->setLeft(-12);
          text_level->setTop(-8);
          text_level->setCaption(i2a(LEVEL(un)));
          text_level->setFontName("cuckoo");
          text_level->setCharHeight(21);
          text_level->setColourBottom(Ogre::ColourValue(217.f/255, 26.f/255, 0.f/255));
          text_level->setColourTop(Ogre::ColourValue(217.f/255, 26.f/255, 0.f/255));
        }
        /* HIT HP */
        if(un->hit_hp > un->hp) {
          Ogre::OverlayContainer *panel = un->hp_hit_overlay->getChild("hp hit " + i2a(u) + " panel");
          assert(panel);
          panel->setLeft(pos.x-HP_BAR_SIZE(un)/2);
          panel->setTop(pos.y);
          panel->setMetricsMode(Ogre::GMM_PIXELS);
          panel->setWidth(un->hit_hp * HP_BAR_SIZE(un) / MAX_HP(un));
          panel->setHeight(4);
          if(INSIDE_SCREEN(panel->getLeft(), panel->getTop())
          || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop())
          || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop()+panel->getHeight())
          || INSIDE_SCREEN(panel->getLeft(), panel->getTop()+panel->getHeight())) {
            panel->setMaterialName("hp_hit_mat");
            un->hp_hit_overlay->setZOrder(98);
            #if !MINIMAP_MODE
            un->hp_hit_overlay->show();
            #endif
          } else
            un->hp_hit_overlay->hide();
        } else
          un->hp_hit_overlay->hide();
        Ogre::OverlayContainer *back_panel = un->hp_back_overlay->getChild("hp back " + i2a(u) + " panel");
        assert(back_panel);
        back_panel->setLeft(pos.x-HP_BAR_SIZE(un)/2-2);
        back_panel->setTop(pos.y-2);
        back_panel->setMetricsMode(Ogre::GMM_PIXELS);
        back_panel->setWidth(HP_BAR_SIZE(un) + 4);
        back_panel->setHeight(8);
        if(INSIDE_SCREEN(back_panel->getLeft(), back_panel->getTop())
        || INSIDE_SCREEN(back_panel->getLeft()+back_panel->getWidth(), back_panel->getTop())
        || INSIDE_SCREEN(back_panel->getLeft()+back_panel->getWidth(), back_panel->getTop()+back_panel->getHeight())
        || INSIDE_SCREEN(back_panel->getLeft(), back_panel->getTop()+back_panel->getHeight())) {
          back_panel->setMaterialName("bar_back_mat");
          un->hp_back_overlay->setZOrder(97);
          #if !MINIMAP_MODE
          un->hp_back_overlay->show();
          #endif
        } else
          un->hp_back_overlay->hide();
        if(MAX_MANA(un) != 0) {
          Ogre::OverlayContainer *panel = un->mn_overlay->getChild("mn " + i2a(u) + " panel");
          assert(panel);
          panel->setLeft(pos.x-MN_BAR_SIZE(un)/2);
          panel->setTop(6+pos.y);
          panel->setMetricsMode(Ogre::GMM_PIXELS);
          panel->setWidth(un->mana * MN_BAR_SIZE(un) / MAX_MANA(un));
          panel->setHeight(4);
          if(INSIDE_SCREEN(panel->getLeft(), panel->getTop())
          || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop())
          || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop()+panel->getHeight())
          || INSIDE_SCREEN(panel->getLeft(), panel->getTop()+panel->getHeight())) {
            panel->setMaterialName("mn_mat");
            un->mn_overlay->setZOrder(99);
            #if !MINIMAP_MODE
            un->mn_overlay->show();
            #endif
          } else
            un->mn_overlay->hide();
          /* HIT MANA */
          if(un->hit_mana > un->mana) {
            Ogre::OverlayContainer *panel = un->mn_hit_overlay->getChild("mn hit " + i2a(u) + " panel");
            assert(panel);
            panel->setLeft(pos.x-MN_BAR_SIZE(un)/2);
            panel->setTop(6+pos.y);
            panel->setMetricsMode(Ogre::GMM_PIXELS);
            panel->setWidth(un->hit_mana * MN_BAR_SIZE(un) / MAX_MANA(un));
            panel->setHeight(4);
            if(INSIDE_SCREEN(panel->getLeft(), panel->getTop())
            || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop())
            || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop()+panel->getHeight())
            || INSIDE_SCREEN(panel->getLeft(), panel->getTop()+panel->getHeight())) {
              panel->setMaterialName("mn_hit_mat");
              un->mn_hit_overlay->setZOrder(98);
              #if !MINIMAP_MODE
              un->mn_hit_overlay->show();
              #endif
            } else
              un->mn_hit_overlay->hide();
          } else
            un->mn_hit_overlay->hide();
          Ogre::OverlayContainer *back_panel = un->mn_back_overlay->getChild("mn back " + i2a(u) + " panel");
          assert(back_panel);
          back_panel->setLeft(pos.x-MN_BAR_SIZE(un)/2-2);
          back_panel->setTop(6+pos.y-2);
          back_panel->setMetricsMode(Ogre::GMM_PIXELS);
          back_panel->setWidth(MN_BAR_SIZE(un) + 4);
          back_panel->setHeight(8);
          if(INSIDE_SCREEN(back_panel->getLeft(), back_panel->getTop())
          || INSIDE_SCREEN(back_panel->getLeft()+back_panel->getWidth(), back_panel->getTop())
          || INSIDE_SCREEN(back_panel->getLeft()+back_panel->getWidth(), back_panel->getTop()+back_panel->getHeight())
          || INSIDE_SCREEN(back_panel->getLeft(), back_panel->getTop()+back_panel->getHeight())) {
            back_panel->setMaterialName("bar_back_mat");
            un->mn_back_overlay->setZOrder(97);
            #if !MINIMAP_MODE
            un->mn_back_overlay->show();
            #endif
          } else
            un->mn_back_overlay->hide();
        }
        /* TIME TO LIVE */
        if(TIME_TO_LIVE(un) > 0) {
          TR_status_effect_ptr s = UN_HasStatusEffect(gi, u, TR_status_effect_invocation_fate);
          assert(s != TR_null);
          TR_status_effect_t *st = &status_effect(s);
          Ogre::OverlayContainer *panel = un->time_to_live_overlay->getChild("time_to_live " + i2a(u) + " panel");
          assert(panel);
          panel->setLeft(pos.x-HP_BAR_SIZE(un)/2);
          panel->setTop(6+pos.y);
          panel->setMetricsMode(Ogre::GMM_PIXELS);
          panel->setWidth(st->amount * HP_BAR_SIZE(un) / TIME_TO_LIVE(un));
          panel->setHeight(2);
          if(INSIDE_SCREEN(panel->getLeft(), panel->getTop())
          || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop())
          || INSIDE_SCREEN(panel->getLeft()+panel->getWidth(), panel->getTop()+panel->getHeight())
          || INSIDE_SCREEN(panel->getLeft(), panel->getTop()+panel->getHeight())) {
            panel->setMaterialName("mn_mat");
            un->time_to_live_overlay->setZOrder(99);
            #if !MINIMAP_MODE
            un->time_to_live_overlay->show();
            #endif
          } else
            un->time_to_live_overlay->hide();
          Ogre::OverlayContainer *back_panel = un->time_to_live_back_overlay->getChild("time_to_live back " + i2a(u) + " panel");
          assert(back_panel);
          back_panel->setLeft(pos.x-HP_BAR_SIZE(un)/2-2);
          back_panel->setTop(6+pos.y-2);
          back_panel->setMetricsMode(Ogre::GMM_PIXELS);
          back_panel->setWidth(HP_BAR_SIZE(un) + 4);
          back_panel->setHeight(6);
          if(INSIDE_SCREEN(back_panel->getLeft(), back_panel->getTop())
          || INSIDE_SCREEN(back_panel->getLeft()+back_panel->getWidth(), back_panel->getTop())
          || INSIDE_SCREEN(back_panel->getLeft()+back_panel->getWidth(), back_panel->getTop()+back_panel->getHeight())
          || INSIDE_SCREEN(back_panel->getLeft(), back_panel->getTop()+back_panel->getHeight())) {
            back_panel->setMaterialName("bar_back_mat");
            un->time_to_live_back_overlay->setZOrder(97);
            #if !MINIMAP_MODE
            un->time_to_live_back_overlay->show();
            #endif
          } else
            un->time_to_live_back_overlay->hide();
        }
      } else {
        un->hp_overlay->hide();
        un->hp_hit_overlay->hide();
        un->hp_back_overlay->hide();
        if(MAX_MANA(un) != 0) {
          un->mn_overlay->hide();
          un->mn_hit_overlay->hide();
          un->mn_back_overlay->hide();
        }
      }
    } else {
      un->hp_overlay->hide();
      un->hp_hit_overlay->hide();
      un->hp_back_overlay->hide();
      if(MAX_MANA(un) != 0) {
        un->mn_overlay->hide();
        un->mn_hit_overlay->hide();
        un->mn_back_overlay->hide();
      }
    }
    /* selection node */
    un->select_ent->setMaterial(Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName(un->team_id == TEAM_ID(cam->player_id) ? "selection_mat_green" : "selection_mat_red")));
    un->hover_ent->setMaterial(Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName(un->team_id == TEAM_ID(cam->player_id) ? "selection_mat_white" : "selection_mat_white_red")));
    /* callback */
    gi->on_update_unit_render_cb(gi, ca, cam_p, u);
    /* next unit */
    u = un->next;
  }
}

void CAM_UpdateAndRenderWindow(SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  if(cam->window_opened) {
    /*FOR_RECT(cam->window_start_x, cam->window_end_x, cam->window_start_y, cam->window_end_y) {
      CL_RenderChar(
        surface, gi, WHITE_SQUARE,
        TCOD_black, 180, i, j);
      gi->post_render_mask[CON_RES_X*j+i] = gi->frame_mark;
    }*/
    if(!cam->window_locked) {
      //BTN_Render(surface, gi, &gi->close_btn);
      if(BTN_IsClicked(gi, &gi->close_btn)
      || GI_IsKeyPressed(gi, OIS::KC_ESCAPE)) 
        CAM_CloseWindow(gi, ca);
    }
  }
}

void CAM_UpdateAndRenderChat(SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  if(gi->display_message_counter > 0) {
    /*FOR_RECT(0, 30, -10, -2)
      CL_RenderChar(
        surface, gi, WHITE_SQUARE, TCOD_black, 
        50*gi->display_message_counter/DISPLAY_MESSAGE_SPAN, 
        1+i, HUD_START_Y+j);*/
    /*DO_TIMES(MESSAGE_QUEUE_COUNT) {
      if(gi->message_queue[f].length() > 0) {
        if(gi->username_queue[f].length() > 0)
          CL_RenderString(
            surface, gi, gi->username_queue[f] + ": ", gi->sent_to_all_queue[f] ? TCOD_red : TCOD_orange, 
            205*gi->display_message_counter/DISPLAY_MESSAGE_SPAN,
            1, HUD_START_Y-3-f);
        CL_RenderString(
          surface, gi, gi->message_queue[f], TCOD_white, 
          205*gi->display_message_counter/DISPLAY_MESSAGE_SPAN,
          1 + (gi->username_queue[f].length() > 0 ? str(gi->username_queue[f] + ": ").length() : 0), 
          HUD_START_Y-3-f);
      }
    }*/
  }
  if(cam->typing) {
    gi->display_message_counter = DISPLAY_MESSAGE_SPAN;
    cam->chat_btn.pos_x = 1 + game_desc.player_name.length()+2;
    cam->chat_btn.pos_y = HUD_START_Y-1;
    BTN_UpdateEntry(gi, &cam->chat_btn);
    if(cam->chat_btn.s.length() > 20)
      cam->chat_btn.s.resize(20); 
    /*DO_TIMES(2) 
      FOR_RECT(0, 30, -1, -1)
        CL_RenderChar(
          surface, gi, WHITE_SQUARE, TCOD_black, 50, 
          1+i, HUD_START_Y+j);*/
    /*CL_RenderString(
      surface, gi, 
      game_desc.player_name + ": ", cl->send_to_all ? TCOD_red : TCOD_orange, 205,
      1, HUD_START_Y-1);
    BTN_Render(surface, gi, &cl->chat_btn);*/
  }
}

void CAM_UpdateFlash(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  cam->flash_span -= gi->frame_time;
  cam->flash_text_overlay->setLeft(gi->ogre_window->getWidth()/2-cam->flash.size()/2*8);
  cam->flash_text_overlay->setTop(3*gi->ogre_window->getHeight()/4);
  cam->flash_text_overlay->setColourBottom(Ogre::ColourValue((float)cam->flash_color.r/255, (float)cam->flash_color.g/255, (float)cam->flash_color.b/255));
  cam->flash_text_overlay->setColourTop(Ogre::ColourValue((float)cam->flash_color.r/255, (float)cam->flash_color.g/255, (float)cam->flash_color.b/255));
  cam->flash_text_overlay->setCaption(cam->flash);
  cam->flash_text_overlay->show();
  cam->flash_text_overlay->
    getMaterial()->
      getTechnique(0)->
        getPass(0)->
          setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  cam->flash_text_overlay->
    getMaterial()->
      getTechnique(0)->
        getPass(0)->
          setDepthWriteEnabled(false);
  cam->flash_text_overlay->
    getMaterial()->
      getTechnique(0)->
        getPass(0)->
          getTextureUnitState(0)->
            setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_MANUAL, 1.0, (float)cam->flash_span/FLOATING_TEXT_LIFE_SPAN);
}

void CAM_UpdateWinLose(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *pl = &player(p);
  if(pl->win || pl->lose) {
    str s = pl->win ? "Victory" : "Defeat";
    cam->win_lose_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
    cam->win_lose_text_overlay->setLeft(gi->ogre_window->getWidth()/2 - s.size()*16/2);
    cam->win_lose_text_overlay->setTop(gi->ogre_window->getHeight()/3);
    cam->win_lose_text_overlay->setCaption(s);
    cam->win_lose_text_overlay->setFontName("cuckoo");
    cam->win_lose_text_overlay->setCharHeight(60);
    TCOD_color_t color = pl->win ? TCOD_orange : TCOD_red;
    cam->win_lose_text_overlay->setColourBottom(Ogre::ColourValue((float)color.r/255, (float)color.g/255, (float)color.b/255));
    cam->win_lose_text_overlay->setColourTop(Ogre::ColourValue((float)color.r/255, (float)color.g/255, (float)color.b/255));
    cam->win_lose_overlay->show();
  } else
    cam->win_lose_overlay->hide();
}

void CAM_CloseWindow(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  cam->hud_state = 0;
  cam->window_opened = false;
  GI_StopKeyPropagation(gi, OIS::KC_ESCAPE);
}

bool CAM_IsUnitSelected(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, short gs) {
  TR_camera_t *cam = &camera(ca);
  if(gs == -1) {
    TR_unit_ptr u2 = cam->selected_units_head;
    while(u2 != TR_null) {
      if(u == u2) 
        return true;
      u2 = unit(u2).snext[ca];
    }
    return false;
  } 
  TR_unit_ptr u2 = cam->gs_head[gs];
  while(u2 != TR_null) {
    if(u == u2) 
      return true;
    u2 = unit(u2).gsnext[gs];
  }
  return false;
}

short CAM_GetSelectedUnitsCount(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_unit_ptr u = cam->selected_units_head;
  int i = 0;
  while(u != TR_null) {
    i++;
    u = unit(u).snext[ca];
  }
  return i;
}

bool CAM_IsThereUnitsHere(TR_game_instance_t *gi, TR_camera_ptr ca, int s_cx, int s_cy, int e_cx, int e_cy) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *pl = &player(p);
  IN_ORDER(s_cx, s_cy, e_cx, e_cy)
  TR_unit_ptr units[MAX_UNIT];
  int          count = 0;
  if(ABS(s_cx-e_cx) < PICKING_THRESHOLD
  && ABS(s_cy-e_cy) < PICKING_THRESHOLD)
    return CAM_PickUnit(gi, ca, s_cx, s_cy) != TR_null;
  else {
    Ogre::PlaneBoundedVolume bv = cam->camera->getCameraToViewportBoxVolume((float)s_cx/gi->ogre_window->getWidth(), (float)s_cy/gi->ogre_window->getHeight(), (float)e_cx/gi->ogre_window->getWidth(), (float)e_cy/gi->ogre_window->getHeight());
    TR_unit_ptr u = gi->unit_head;
    while(u != TR_null) {
      TR_unit_t *un = &unit(u);
      Ogre::Vector3 pos = 
        Ogre::Vector3(
          (float)un->x/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)), 
          FLYING(un) ? FLYING_HEIGHT : 0, 
          (float)un->y/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)));
      Ogre::AxisAlignedBox bbox = un->ent[0]->getBoundingBox();
      DO_TIMES(MESH_COUNT(un))
        bbox.merge(un->ent[f]->getBoundingBox());
      Ogre::Vector3 min = bbox.getMinimum(),
                    max = bbox.getMaximum();
      min.x *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); min.y *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); min.z *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un);
      max.x *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); max.y *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); max.z *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un);
      Ogre::AxisAlignedBox aabb(min+pos, max+pos);
      if(bv.intersects(aabb))
        return true;
      u = un->next;
    }
  }
  return false;
}

void CAM_SelectUnits(TR_game_instance_t *gi, TR_camera_ptr ca, int s_cx, int s_cy, int e_cx, int e_cy, TR_unit_type_t type, bool extended) {
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cam->player_id);
  TR_player_t *pl = &player(p);
  IN_ORDER(s_cx, s_cy, e_cx, e_cy)
  TR_unit_ptr units[MAX_UNIT];
  int          count = 0;
  bool picking = ABS(s_cx-e_cx) < PICKING_THRESHOLD
              && ABS(s_cy-e_cy) < PICKING_THRESHOLD;
  if(!extended
  && picking)
    units[count++] = CAM_PickUnit(gi, ca, s_cx, s_cy);
  else {
    Ogre::PlaneBoundedVolume bv = cam->camera->getCameraToViewportBoxVolume((float)s_cx/gi->ogre_window->getWidth(), (float)s_cy/gi->ogre_window->getHeight(), (float)e_cx/gi->ogre_window->getWidth(), (float)e_cy/gi->ogre_window->getHeight());
    TR_unit_ptr pick_u;
    if(extended
    && picking) {
      pick_u = CAM_PickUnit(gi, ca, s_cx, s_cy);
      assert(pick_u != TR_null);
    }
    TR_unit_ptr u = gi->unit_head;
    while(u != TR_null) {
      TR_unit_t *un = &unit(u);
      int pos_x = un->x>>POS_SHIFT,
          pos_y = un->y>>POS_SHIFT,
          pick_x = unit(pick_u).x>>POS_SHIFT,
          pick_y = unit(pick_u).y>>POS_SHIFT;
      if(extended
      && picking
      && (pos_x-pick_x) * (pos_x-pick_x) + (pos_y-pick_y) * (pos_y-pick_y)
      < SAME_UNIT_TYPE_SIZE)
        units[count++] = u;
      else 
      if(!picking) {
        Ogre::Vector3 pos = 
          Ogre::Vector3(
            (float)un->x/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)), 
            FLYING(un) ? FLYING_HEIGHT : 0, 
            (float)un->y/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)));
        Ogre::AxisAlignedBox bbox = un->ent[0]->getBoundingBox();
        DO_TIMES(MESH_COUNT(un))
          bbox.merge(un->ent[f]->getBoundingBox());
        Ogre::Vector3 min = bbox.getMinimum(),
                      max = bbox.getMaximum();
        min.x *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); min.y *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); min.z *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un);
        max.x *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); max.y *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); max.z *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un);
        Ogre::AxisAlignedBox aabb(min+pos, max+pos);
        if(bv.intersects(aabb))
          units[count++] = u;
      }
      u = un->next;
    }
  }
  bool no_sub_group = cam->selected_sub_group == -1;
  bool mines = false;
  DO_TIMES(count) {
    TR_unit_ptr u = units[f];
    TR_unit_t *un = &unit(u);
    if(un->player_id == cam->player_id
    && (type == TR_unit_type_count
    || un->unit_type == type)) {
      mines = true;
      break;
    }
  }
  bool moving_units = false;
  if(type == TR_unit_type_count) {
    DO_TIMES(count) {
      TR_unit_ptr u = units[f];
      TR_unit_t *un = &unit(u);
      if(CAN_BE_PUSHED(un)) {
        moving_units = true;
        break;
      }
    }
  }
  TR_unit_type_t unit_types[TR_unit_type_count];
  int unit_type_count = 0;
  DO_TIMES(count) {
    TR_unit_ptr u = units[f];
    TR_unit_t *un = &unit(u);
    if(PL_IsInFov(gi, p, u)
    && (type == TR_unit_type_count
    || un->unit_type == type)
    && (!mines
    || un->player_id == cam->player_id)
    && (!moving_units
    || CAN_BE_PUSHED(un))
    && !CAM_IsUnitSelected(gi, ca, u)) {
      CAM_AddToSelection(gi, ca, u);
      /* audio */
      bool already = false;
      DO_TIMES(unit_type_count)
        if(unit_types[f] == unit(u).unit_type) {
          already = true;
          break;
        }
      if(!already
      && gi->on_select_unit_sound_cb) {
        unit_types[unit_type_count++] = un->unit_type;
        gi->on_select_unit_sound_cb(gi, un->unit_type, -1, -1);
      }
    }
  }
  if(no_sub_group) {
    cam->selected_sub_group = 
      unit(cam->selected_units_head).unit_type;
    cam->hud_state = 0;
    cam->skill_here = false;
    cam->window_opened = false;
  }
}

void CAM_AddToSelection(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u) {
  if(!CAM_IsUnitSelected(gi, ca, u)) {
    TR_camera_t *cam = &camera(ca);
    cam->selected_gs = -1;
    cam->window_opened = false;
    TR_unit_t *un = &unit(u);
    un->select_ent->setVisible(true);
    if(cam->selected_sub_group == -1) {
      cam->selected_sub_group = un->unit_type;
      cam->hud_state = 0;
      cam->skill_here = false;
      cam->window_opened = false;
    }
    if(cam->selected_units_head == TR_null) {
      un->snext[ca] = TR_null;
      un->sprevious[ca] = TR_null;
      cam->selected_units_head = u;
    } else 
    if(unit(cam->selected_units_head).unit_type 
    <= un->unit_type) {
      un->snext[ca] = cam->selected_units_head;
      unit(cam->selected_units_head).sprevious[ca] = u;
      un->sprevious[ca] = TR_null;
      cam->selected_units_head = u;
    } else {
      TR_unit_ptr u2 = cam->selected_units_head;
      while(unit(u2).snext[ca] != TR_null
         && unit(unit(u2).snext[ca]).unit_type > un->unit_type)
        u2 = unit(u2).snext[ca];
      TR_unit_t *un2 = &unit(u2);
      if(un2->snext[ca] == TR_null) {
        un->sprevious[ca] = u2;
        un->snext[ca] = TR_null;
        un2->snext[ca] = u;
      } else {
        unit(un2->snext[ca]).sprevious[ca] = u;
        un->snext[ca] = un2->snext[ca];
        un2->snext[ca] = u;
        un->sprevious[ca] = u2;
      }
    }
  }
}

void CAM_SubToSelection(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, short gs) {
  TR_camera_t     *cam  = &camera(ca);
  TR_unit_t *un        = &unit(u);
  un->select_ent->setVisible(false);
  cam->selected_gs = -1;
  cam->window_opened = false;
  if(gs == -1) {  
    TR_unit_ptr u2 = cam->selected_units_head;
    TR_unit_ptr previous = u2;
    bool done = false;
    while(!done 
       && u2 != TR_null) {
      if(u == u2) {
        if(previous == u2) {
          cam->selected_units_head =
            unit(cam->selected_units_head).snext[ca];
        } else {
          unit(previous).snext[ca] = unit(u2).snext[ca];
          if(unit(u2).snext[ca])
            unit(unit(u2).snext[ca]).sprevious[ca] = previous;
        }
        done = true;
      }
      previous = u2;
      u2 = unit(u2).snext[ca];
    }
    assert(done);
    if(cam->selected_sub_group == unit(u).unit_type) {
      bool change = true;
      TR_unit_ptr u2 = cam->selected_units_head;
      while(u2 != TR_null) {
        if(unit(u2).unit_type 
        == cam->selected_sub_group)
          change = false;
        u2 = unit(u2).snext[ca];
      }
      if(change) {
        if(cam->selected_units_head != TR_null)
          cam->selected_sub_group = 
            unit(cam->selected_units_head).unit_type;
        else
          cam->selected_sub_group = -1;
        cam->hud_state = 0;
        cam->skill_here = false;
        cam->window_opened = false;
      }
    }
  } else {
    TR_unit_ptr u2 = cam->gs_head[gs];
    TR_unit_ptr previous = u2;
    bool done = false;
    while(!done 
       && u2 != TR_null) {
      if(u == u2) {
        if(previous == u2) {
          cam->gs_head[gs] =
            unit(cam->gs_head[gs]).gsnext[gs];
        } else {
          unit(previous).gsnext[gs] = unit(u2).gsnext[gs];
          if(unit(u2).gsnext[gs])
            unit(unit(u2).gsnext[gs]).gsprevious[gs] = previous;
        }
        done = true;
      }
      previous = u2;
      u2 = unit(u2).gsnext[gs];
    }
    assert(done);
    if(cam->gs_sub_group[gs] == unit(u).unit_type) {
      bool change = true;
      TR_unit_ptr u2 = cam->gs_head[gs];
      while(u2 != TR_null) {
        if(unit(u2).unit_type 
        == cam->selected_sub_group)
          change = false;
        u2 = unit(u2).gsnext[gs];
      }
      if(change)
        if(cam->gs_head[gs] != TR_null)
          cam->gs_sub_group[gs] = 
            unit(cam->gs_head[gs]).unit_type;
        else
          cam->gs_sub_group[gs] = -1;
    }
  }
}

void CAM_FreeUnitSelection(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  while(cam->selected_units_head != TR_null)
    CAM_SubToSelection(gi, ca, cam->selected_units_head);
  assert(cam->selected_units_head == TR_null);
  cam->selected_sub_group  = -1;
  cam->hud_state           = 0;
  cam->selected_gs         = -1;
  cam->window_opened       = false;
}

void CAM_AddToGroupSelection(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, int gs) {
  TR_camera_t *cam = &camera(ca);
  if(cam->gs_head[gs] != TR_null)
    unit(cam->gs_head[gs]).gsprevious[gs] = u;
  unit(u).gsprevious[gs] = TR_null;
  unit(u).gsnext[gs] = cam->gs_head[gs];
  cam->gs_head[gs] = u;
}

void CAM_CopySelectionTo(TR_game_instance_t *gi, TR_camera_ptr ca, int gs) {
  TR_camera_t *cam = &camera(ca);
  cam->gs_head[gs] = TR_null;
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null) {
    CAM_AddToGroupSelection(gi, ca, u, gs);
    u = unit(u).snext[ca];
  }
  cam->gs_sub_group[gs] = cam->selected_sub_group;
  cam->selected_gs = gs;
}

void CAM_RestoreGroupSelection(TR_game_instance_t *gi, TR_camera_ptr ca, int gs, bool free_selection) {
  TR_camera_t *cam = &camera(ca);
  if(cam->gs_head[gs] != TR_null) {
    if(cam->selected_gs == gs)
      CAM_FocusOnUnits(gi, ca);
    if(free_selection)
      CAM_FreeUnitSelection(gi, ca);
    TR_unit_ptr u = cam->gs_head[gs];
    while(u != TR_null) {
      CAM_AddToSelection(gi, ca, u);
      u = unit(u).gsnext[gs];
    }
    cam->selected_sub_group = cam->gs_sub_group[gs];
    cam->hud_state = 0;
    cam->selected_gs = gs;
    cam->skill_here = false;
    cam->window_opened = false;
  }
}

void CAM_SelectNextSubGroup(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null
     && unit(u).unit_type >= cam->selected_sub_group)
    u = unit(u).snext[ca];
  if(u != TR_null)
    cam->selected_sub_group = unit(u).unit_type;
  else
  if(cam->selected_units_head != TR_null)
    cam->selected_sub_group = 
      unit(cam->selected_units_head).unit_type;
  else
    assert(cam->selected_sub_group == -1);
  cam->hud_state = 0;
  cam->skill_here = false;
  cam->window_opened = false;
}

int CAM_GetRSquared(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  int area  = 0;
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null) {
    TR_unit_t *un   = &unit(u);
    /* it is possible to select other players' units */
    if(un->player_id == cam->player_id)
      area += SIZE(un)*SIZE(un);
    u = un->snext[ca];
  }
  return area / 3.14;
}

void CAM_FocusOnUnits(TR_game_instance_t *gi, TR_camera_ptr ca) {
  /* todo: cluster recognition */
  TR_camera_t *cam = &camera(ca);
  int pos_x = 0,
      pos_y = 0,
      k = 0;
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null) {
    pos_x += unit(u).x+UN_GetSizeShift(gi, SIZE(&unit(u)));
    pos_y += unit(u).y+UN_GetSizeShift(gi, SIZE(&unit(u)));
    u = unit(u).snext[ca];
    k++;
  }
  if(k > 0)
    CAM_LookAt(gi, ca, pos_x/k, pos_y/k);
}

void CAM_LookAt(TR_game_instance_t *gi, TR_camera_ptr ca, int x, int y) {
  TR_camera_t *cam = &camera(ca);
  float d = CAMERA_HEIGHT / tan(CAMERA_ANGLE * 3.14 / 180);
  cam->viewport_x = x;
  cam->viewport_y = y + (float)(d*(1<<POS_SHIFT));
  BOUND_VIEWPORT
}

void CAM_UpdateWindowCloseButton(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  gi->close_btn.pos_x             = cam->window_end_x;
  gi->close_btn.pos_y             = cam->window_start_y;
  gi->close_btn.color             = TCOD_dark_red;
  gi->close_btn.s                 = "X";
  gi->close_btn.highlight         = false;
  gi->close_btn.entry             = false;
  gi->close_btn.shortcut          = 'x';
  gi->close_btn.shortcut_special  = (TCOD_keycode_t)-1;
}

void CAM_OpenOptionWindow(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  cam->window_opened     = true;
  cam->hud_state         = HUD_STATE_OPTION_WINDOW;
  cam->window_size_x     = 30;
  cam->window_size_y     = 15;
  cam->window_start_x    = (CON_RES_X-cam->window_size_x)/2;
  cam->window_start_y    = ((HUD_START_Y-cam->window_size_y)/2);
  cam->window_end_x      = (cam->window_start_x+cam->window_size_x);
  cam->window_end_y      = (cam->window_start_y+cam->window_size_y); 
  CAM_UpdateWindowCloseButton(gi, ca);
}

void CAM_UpdateOptionWindow(SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  if(cam->window_opened
  && cam->hud_state == HUD_STATE_OPTION_WINDOW) {
    gi->central_hud_btns.btn_count                  = 1;
    gi->central_hud_btns.pos_x                      = (cam->window_start_x+cam->window_end_x)/2;
    gi->central_hud_btns.pos_y                      = cam->window_start_y+8;
    gi->central_hud_btns.btns[0].color              = TCOD_white;
    gi->central_hud_btns.btns[0].s                  = "(Q)uit";
    gi->central_hud_btns.btns[0].highlight          = true;
    gi->central_hud_btns.btns[0].entry              = false;
    gi->central_hud_btns.btns[0].shortcut           = 'Q';
    gi->central_hud_btns.btns[0].shortcut_special   = (TCOD_keycode_t)-1;
    gi->central_hud_btns.btns[0].pos_x              = gi->central_hud_btns.pos_x;
    gi->central_hud_btns.btns[0].pos_y              = gi->central_hud_btns.pos_y;
    BTN_Render(surface, gi, &gi->central_hud_btns, true, 1);
    if(BTN_IsClicked(gi, &gi->close_btn)
    || GI_IsKeyPressed(gi, OIS::KC_ESCAPE)) {
      cam->window_opened = false;
      cam->hud_state = 0;
      GI_StopKeyPropagation(gi, OIS::KC_ESCAPE);
    } else
    if(BTN_IsClicked(gi, &gi->central_hud_btns.btns[0])) {
      if(game_desc.PvE)
        GAME_SaveGame(gi, gp_gi.level);
      GI_DisconnectFromMasterServer(gi);
      GI_Free(gi);
      gi->state = 0;
      TR_Quit(TCOD_sys_elapsed_seconds());
      gi->flash_btn.s = "";
    }
  }
}

void CAM_OpenWaitWindow(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  cam->window_opened     = true;
  cam->window_locked     = true;
  cam->hud_state         = HUD_STATE_WAIT_WINDOW;
  cam->window_size_x     = 30;
  cam->window_size_y     = 15;
  cam->window_start_x    = (CON_RES_X-cam->window_size_x)/2;
  cam->window_start_y    = ((HUD_START_Y-cam->window_size_y)/2);
  cam->window_end_x      = (cam->window_start_x+cam->window_size_x);
  cam->window_end_y      = (cam->window_start_y+cam->window_size_y); 
}

void CAM_UpdateWaitWindow(SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  if(cam->hud_state == HUD_STATE_WAIT_WINDOW) {
    bool one = false;
    DO_TIMES(MAX_PLAYER)
      if(gi->is_timingout[f]) {
        gi->timeout_timeouts[f] -= gi->frame_time;
          if(gi->timeout_timeouts[f] <= 0)
            gi->is_timingout[f] = false;
        one = gi->timeout_timeouts[f];
        break;
      }
    if(one) {
      int y = cam->window_start_y+1;
      /*DO_TIMES(MAX_PLAYER)
        if(gi->is_timingout[f]) {
          CL_RenderString(
            surface, gi, "Waiting for " + gi->timeout_player_names[f] + ": " + i2a(gi->timeouts[f]) + "s left", 
            TCOD_white, 255,
            cam->window_start_x+1,
            y);
          y+=2;
        }*/
    } /*else
      CL_RenderString(
        surface, gi, "Waiting for a player...", 
        TCOD_white, 255,
        cam->window_start_x+5,
        cam->window_start_y+7);*/
    gi->central_hud_btns.btn_count                  = 1;
    gi->central_hud_btns.pos_x                      = (cam->window_start_x+cam->window_end_x)/2;
    gi->central_hud_btns.pos_y                      = cam->window_start_y+cam->window_size_y-1;
    gi->central_hud_btns.btns[0].color              = TCOD_white;
    gi->central_hud_btns.btns[0].s                  = "(Q)uit";
    gi->central_hud_btns.btns[0].highlight          = true;
    gi->central_hud_btns.btns[0].entry              = false;
    gi->central_hud_btns.btns[0].shortcut           = 'Q';
    gi->central_hud_btns.btns[0].shortcut_special   = (TCOD_keycode_t)-1;
    gi->central_hud_btns.btns[0].pos_x              = gi->central_hud_btns.pos_x;
    gi->central_hud_btns.btns[0].pos_y              = gi->central_hud_btns.pos_y;
    /*BTN_Render(surface, gi, &gi->central_hud_btns, true, 1);
    if(BTN_IsClicked(gi, &gi->central_hud_btns.btns[0])) {
      GI_DisconnectFromMasterServer(gi);
      GI_Free(gi);
      gi->state = 0;
      TR_Quit(TCOD_sys_elapsed_seconds());
      gi->flash_btn.s = "";
    }*/
  }
}

void CAM_CloseWaitWindow(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
  if(cam->hud_state == HUD_STATE_WAIT_WINDOW) {
    cam->window_opened     = false;
    cam->window_locked     = false;
    cam->hud_state         = 0;
    DO_TIMES(MAX_PLAYER)
      gi->is_timingout[f] = false;
  }
}

void CAM_SetFlash(TR_game_instance_t *gi, TR_camera_ptr ca, const str &s, TCOD_color_t flash_color) {
  TR_camera_t *cam  = &camera(ca);
  cam->flash         = s;
  cam->flash_span    = CLIENT_FLASH_SPAN;
  cam->flash_color   = flash_color;
}

TR_unit_ptr CAM_PickUnit(TR_game_instance_t *gi, TR_camera_ptr ca, int x, int y) {
  TR_camera_t *cam  = &camera(ca);
  float        min_dist = 10000000;
  TR_unit_ptr result = TR_null;
  TR_unit_ptr u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    Ogre::Vector3 pos = 
      Ogre::Vector3(
        (float)un->x/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)), 
        FLYING(un) ? FLYING_HEIGHT : 0, 
        (float)un->y/(1<<POS_SHIFT)+UN_GetSizeShiftFloat(gi, SIZE(un)));
    Ogre::AxisAlignedBox bbox = un->ent[0]->getBoundingBox();
    DO_TIMES(MESH_COUNT(un))
      bbox.merge(un->ent[f]->getBoundingBox());
    Ogre::Vector3 min = bbox.getMinimum(),
                  max = bbox.getMaximum();
    min.x *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); min.y *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); min.z *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un);
    max.x *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); max.y *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un); max.z *= 0.75 * 0.25 * SIZE(un) * MODEL_SCALE(un);
    Ogre::AxisAlignedBox aabb(min+pos, max+pos);
    Ogre::Ray r = cam->camera->getCameraToViewportRay((float)x/gi->ogre_window->getWidth(), (float)y/gi->ogre_window->getHeight());
    if(r.intersects(aabb).first
    && min_dist > r.intersects(aabb).second) {
      min_dist = r.intersects(aabb).second;
      result = u;
    }
    u = un->next;
  }
  return result;
}

Ogre::Vector3 CAM_RayToVector(TR_game_instance_t *gi, TR_camera_ptr ca, Ogre::Ray r) {
  TR_camera_t *cam  = &camera(ca);
  float min_dist = 9999999;
  cam->ray_query->setRay(r);
  Ogre::RaySceneQueryResult &result = cam->ray_query->execute();
  Ogre::RaySceneQueryResult::iterator iter = result.begin();
  for(; iter != result.end(); iter++)
    if(min_dist > iter->distance)
      min_dist = iter->distance;
  if(result.begin() != result.end())
    return r.getPoint(min_dist);
  return Ogre::Vector3(9999999, 9999999, 9999999);
}

Ogre::Ray CAM_GetMouseRay(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam  = &camera(ca);
  return cam->camera->getCameraToViewportRay(
    (float)gi->mouse.x/gi->ogre_window->getWidth(), 
    (float)gi->mouse.y/gi->ogre_window->getHeight());
}

Ogre::Vector2 CAM_WorldToScreen(TR_game_instance_t *gi, TR_camera_ptr ca, float x, float y, float z) {
  TR_camera_t *cam  = &camera(ca);
  Ogre::Vector2 result;
  Ogre::Vector3 pos = Ogre::Vector3(x, z, y);
  Ogre::Plane p = 
    Ogre::Plane(
      Ogre::Vector3(
        cam->camera->getDerivedOrientation().zAxis()), 
        cam->camera->getDerivedPosition());
  if(p.getSide(pos) 
  != Ogre::Plane::NEGATIVE_SIDE) {
    result.x = -1;
    result.y = -1;
  } else {
    Ogre::Vector3 point = 
      cam->camera->getProjectionMatrix() * 
      cam->camera->getViewMatrix() * 
      pos;
    result.x = point.x*0.5 + 0.5f;
    result.y = 1 - (point.y*0.5 + 0.5f);
  }
  return result;
}

Ogre::Vector3 CAM_ScreenToWorld(TR_game_instance_t *gi, TR_camera_ptr ca, int x, int y) {
  TR_camera_t *cam  = &camera(ca);
  return CAM_RayToVector(
          gi, ca, 
          cam->camera->getCameraToViewportRay(
            (float)x/gi->ogre_window->getWidth(), 
            (float)y/gi->ogre_window->getHeight()));
}

/*
 * client
 */

void CL_Init(TR_game_instance_t *gi, int argc, char** argv) {
  for(int i = 0; i < MAX_CLIENT; i++) {
    if(i != MAX_CLIENT-1)
      client(i).fnext = i+1;
    else
      client(i).fnext = TR_null;
  }
  gi->free_client_head = 0;
  gi->client_head      = TR_null;
  log("Clients initialized.");
}

TR_client_ptr CL_New(TR_game_instance_t *gi, TR_cmd_store_ptr cs, int type, int player_id) {
  if(gi->free_client_head != TR_null) {
    TR_client_ptr r     = gi->free_client_head;
    TR_client_t *cl     = &client(r);
    gi->free_client_head    = client(gi->free_client_head).fnext;
    cl->type                = type;
    cl->cmd_id              = 0;
    cl->cs                  = cs;
    cl->previous            = TR_null;
    cl->peer                = null;
    cl->host                = null;
    cl->user_data           = TR_null;
    cl->offline             = false;
    cl->connected           = false;
    cl->generate_eot        = cl->type != REPLAY_CLIENT;
    cl->player_id           = player_id;
    cl->leech_replay_next_turn = -1;
    cl->next                = gi->client_head;
    if(cl->type == REPLAY_CLIENT)
      CL_LoadReplay(gi, r);
    if(gi->client_head != TR_null)
      client(gi->client_head).previous = r;
    gi->client_head         = r;
    return r;
  } 
  log("No more client left.");
  return TR_null;
}

void CL_Free(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  if(gi->client_head == c)
    gi->client_head = cl->next;
  if(cl->previous != TR_null)
    client(cl->previous).next = cl->next;
  if(cl->next != TR_null)
    client(cl->next).previous = cl->previous;
  cl->fnext = gi->free_client_head;
  if(cl->host 
  && cl->peer) {
    enet_peer_disconnect_later(cl->peer, 0);
    ENetEvent e;
    while(enet_host_service(cl->host, &e, 500) != ENET_EVENT_TYPE_NONE);
    enet_peer_reset(cl->peer);
    enet_host_destroy(cl->host);
    log(gi->name + " cl: disconnection.");
  }
  gi->free_client_head = c;
}

void CL_FreeAll(TR_game_instance_t *gi) {
  while(gi->client_head != TR_null)
    CL_Free(gi, gi->client_head);
  log("Clients freed.");
}

void CL_Connect(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl         = &client(c);
  if(!cl->offline) {
    cl->host                = null;
    ENetPacket *packet =
      enet_packet_create(null, 0, ENET_PACKET_FLAG_RELIABLE);
    TR_seed_game_packet_t data;
    data.type         = TR_cmd_type_seed_game;
    data.player_id    = cl->player_id;
    log("cl player id: " + i2a(cl->player_id));
    enet_packet_resize(packet, sizeof(data));
    memcpy(packet->data, &data, sizeof(data));
    enet_peer_send(gi->peer, 0, packet);
    enet_host_flush(gi->host);
  }
}

void CL_WaitForConnection(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  if(cl->host) {
    if(!cl->connected) {
      ENetEvent e;
      if(enet_host_service(cl->host, &e, 50) != ENET_EVENT_TYPE_NONE) {
        switch(e.type) {
          case ENET_EVENT_TYPE_CONNECT:
            log("CL_WaitForConnection CL: Connection to master server succeed.");
            cl->connected = true;
            break;
          case ENET_EVENT_TYPE_DISCONNECT:
            log("CL_WaitForConnection CL: cannot connect to master server.");
            TR_Quit(888);
            break;
          case ENET_EVENT_TYPE_RECEIVE:
            log("CL: a packet has been received. It is not supposed to receive packet.");
            enet_packet_destroy(e.packet);
            break;
        }
      }
    } else
      enet_host_service(cl->host, null, 0);
  }
}

bool CL_UpdateConnection(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  if(!cl->offline) {
    ENetEvent e;
    while(enet_host_service(cl->host, &e, 0) != ENET_EVENT_TYPE_NONE) {
      switch(e.type) {
        case ENET_EVENT_TYPE_CONNECT:
          log("CL_UpdateConnection CL: Connection to master server succeed.");
          /* in 1v0, this connection can be triggered */
          return false; /* free it? */
        case ENET_EVENT_TYPE_DISCONNECT:
          log("CL_UpdateConnection CL: cannot connect to master server.");
          TR_Quit(888);
          return true; /* free it? */
        case ENET_EVENT_TYPE_RECEIVE:
          log("CL: a packet has been received.");
          enet_packet_destroy(e.packet);
          return false; /* free it? */
      }
    }
  }
  return false; /* free it? */
}

void CL_Seed(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  TR_cmd_store_t *cs = &cmd_store(cl->cs);
  if(cs->cmd_head != TR_null) {
    int len = 0;
    TR_cmd_ptr cm = cs->cmd_head;
    while(cm != TR_null) {
      TR_cmd_t *cmd = &cmd(cm);
      if(!cmd->sent
      && gi->turn + MAX_ADVANCE_TURN > cmd->turn) {
        switch(cmd->type) {
          case TR_cmd_type_unit_order:
            len += sizeof(TR_unit_order_packet_t);
            break;
          case TR_cmd_type_spawn_unit:
            len += sizeof(TR_spawn_unit_packet_t);
            break;
          case TR_cmd_type_stop_unit:
            len += sizeof(TR_stop_unit_packet_t);
            break;
          case TR_cmd_type_build_unit:
            len += sizeof(TR_build_unit_packet_t);
            break;
          case TR_cmd_type_generic:
            len += sizeof(TR_generic_packet_t);
            break;
          case TR_cmd_type_end_of_turn:
            len += sizeof(TR_end_of_turn_packet_t);
            break;
        }
      }
      cm = cmd->cmd_next;
    }
    if(len > 0) {
      len += sizeof(TR_cmd_type_t);
      bool dyn_alloc = len >= ZLIB_CHUNK;
      unsigned char *input      = dyn_alloc ? (unsigned char*)malloc(len) : zlib_in,
                    *pack_data  = input,
                    *output     = dyn_alloc ? (unsigned char*)malloc(len) : zlib_out;
      cm = cs->cmd_head;
      while(cm != TR_null) {
        TR_cmd_t *cmd = &cmd(cm);
        if(!cmd->sent
        && gi->turn + MAX_ADVANCE_TURN > cmd->turn) {
          cmd->sent = true;
          switch(cmd->type) {
            case TR_cmd_type_unit_order: {
                TR_unit_order_packet_t data;
                data.type             = TR_cmd_type_unit_order;
                data.id               = cmd->id;
                data.turn             = cmd->turn;
                data.cmd_mask         = cmd->cmd_mask;
                data.r_squared        = cmd->r_squared;
                data.target_cx        = cmd->target_cx;
                data.target_cy        = cmd->target_cy;
                data.click_cx         = cmd->click_cx;
                data.click_cy         = cmd->click_cy;
                data.target           = cmd->target;
                data.target_player_id = cmd->target_player_id;
                data.target_cmd_id    = cmd->target_cmd_id;
                data.skill_here     = cmd->skill_here;
                data.skill_type       = cmd->skill_type;
                data.push_back        = cmd->push_back;
                memcpy(pack_data, &data, sizeof(data));
                pack_data += sizeof(data);
              }
              break;
            case TR_cmd_type_spawn_unit: {
                TR_spawn_unit_packet_t data;
                data.type           = TR_cmd_type_spawn_unit;
                data.id             = cmd->id;
                data.unit_type      = cmd->unit_type;
                data.turn           = cmd->turn;
                data.cmd_mask       = cmd->cmd_mask;
                data.target_cx      = cmd->target_cx;
                data.target_cy      = cmd->target_cy;
                data.user_data_type = cmd->user_data_type;
                data.find_pos       = cmd->find_pos;
                memcpy(pack_data, &data, sizeof(data));
                pack_data += sizeof(data);
              }
              break;
            case TR_cmd_type_stop_unit: {
                TR_stop_unit_packet_t data;
                data.type           = TR_cmd_type_stop_unit;
                data.id             = cmd->id;
                data.turn           = cmd->turn;
                memcpy(pack_data, &data, sizeof(data));
                pack_data += sizeof(data);
              }
              break;
            case TR_cmd_type_build_unit: {
                TR_build_unit_packet_t data;
                data.type               = TR_cmd_type_build_unit;
                data.id                 = cmd->id;
                data.turn               = cmd->turn;
                data.cmd_mask           = cmd->cmd_mask;
                data.unit_type          = cmd->unit_type;
                data.player_id          = cmd->player_id;
                data.target_player_id   = cmd->target_player_id;
                data.target_cx          = cmd->target_cx;
                data.target_cy          = cmd->target_cy;
                data.start_it           = cmd->start_it;
                data.cancel_it          = cmd->cancel_it;
                data.user_data_type     = cmd->user_data_type;
                memcpy(pack_data, &data, sizeof(data));
                pack_data += sizeof(data);
              }
              break;
            case TR_cmd_type_generic: {
                TR_generic_packet_t data;
                data.type               = TR_cmd_type_generic;
                data.id                 = cmd->id;
                data.turn               = cmd->turn;
                data.cmd_type           = cmd->cmd_type;
                data.cmd_mask           = cmd->cmd_mask;
                data.unit_type          = cmd->unit_type;
                data.player_id          = cmd->player_id;
                data.target_cx          = cmd->target_cx;
                data.target_cy          = cmd->target_cy;
                memcpy(pack_data, &data, sizeof(data));
                pack_data += sizeof(data);
              }
              break;
            case TR_cmd_type_end_of_turn: {
                TR_end_of_turn_packet_t data;
                data.type             = TR_cmd_type_end_of_turn;
                data.id               = cmd->id;
                data.turn             = cmd->turn;
                data.turn_frame_count = cmd->turn_frame_count;
                data.frame_time_step  = cmd->frame_time_step;
                data.hash             = cmd->hash;
                memcpy(pack_data, &data, sizeof(data));
                pack_data += sizeof(data);
              }
              break;
          }
        }
        cm = cmd->cmd_next;
      }
      *(TR_cmd_type_t*)pack_data = TR_cmd_type_none;
      z_stream strm;
      strm.zalloc     = Z_NULL;
      strm.zfree      = Z_NULL;
      strm.opaque     = Z_NULL;
      // we have cpu en masse
      TR_ZErr(deflateInit(&strm, 9));
      strm.avail_in   = len;
      strm.next_in    = input;
      strm.avail_out  = dyn_alloc ? len : ZLIB_CHUNK;
      strm.next_out   = output;
      TR_ZErr(deflate(&strm, Z_FINISH));
      assert(strm.avail_in == 0);
      int out_len = (dyn_alloc ? len : ZLIB_CHUNK) - strm.avail_out;
      deflateEnd(&strm);
      /* replay */
      TR_buffer_t *buf = new TR_buffer_t;
      buf->data_length = out_len;
      buf->data = new char[buf->data_length];
      memcpy(buf->data, output, out_len);
      cs->cmd_buffers.push_front(buf);
      /* enet send */
      if(!cl->offline) {
        ENetPacket *packet = 
          enet_packet_create(
            null, 
            0, 
            ENET_PACKET_FLAG_RELIABLE);
        enet_packet_resize(packet, out_len);
        memcpy(packet->data, output, out_len);
        enet_peer_send(cl->peer, 0, packet);
        enet_host_flush(cl->host);
      }
      /* free */
      if(dyn_alloc) {
        free(input);
        free(output);
      }
      //log("" + i2a(len) + " " + i2a(out_len));
    }
  }
}

bool CL_Update(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  if(cl->type == HUMAN_CLIENT) 
    CL_UpdateInputs(gi, c);
  else
  if(cl->type == REPLAY_CLIENT)
    CL_LeechReplay(gi, c);
  if(gi->update_client_cb)
    gi->update_client_cb(gi, c);
  /* finally seeds for everybody */
  if(cl->type != REPLAY_CLIENT)
    CL_Seed(gi, c);
  return CL_UpdateConnection(gi, c);
}

void CL_EndOfTurn(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl         = &client(c);
  if(cl->generate_eot) {
    TR_cmd_ptr r            = CMD_New(gi, cl->cmd_id++);
    TR_cmd_t *cmd           = &cmd(r);
    cmd->type               = TR_cmd_type_end_of_turn;
    cmd->turn               = gi->turn + CMD_TURN_SHIFT;
    cmd->frame_time_step    = gi->aver_frame_time;
    int target              = ceil((float)gi->max_ping / MAX(1, cmd->frame_time_step));
    cmd->turn_frame_count   = MIN(MAX_TURN_FRAME_COUNT, target);
    if(!cl->offline) {
      int share   = gi->turn&15,
          offset  = (MAP_SIZE>>4)*share;
      static bool map[MAP_SIZE];
      DO_TIMES(MAP_SIZE>>4)
        map[offset + f] = gi->unit_map[offset + f] != TR_null;
      cmd->hash = TR_Hash((unsigned char*)map, MAP_SIZE*sizeof(bool));
    } else
      cmd->hash = 0;
    CS_InsertCmd(gi, cl->cs, r);
    // seed immediately
    CL_Seed(gi, c);
  }
}

void CL_UpdateInputs(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr cl_p = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *cl_pl = &player(cl_p);
  Ogre::Vector3 mouse_p = CAM_GetWorldMousePosition(gi, ca);
  BOUND_MINMAX(1, MAP_SIZE_X-2, mouse_p.x) 
  BOUND_MINMAX(1, MAP_SIZE_Y-2, mouse_p.z)
  if(!cl_pl->lose 
  && !cl_pl->win 
  && !cam->window_opened 
  && !cam->typing) {
    #if !RELEASE
    if(GI_IsKeyPressed(gi, OIS::KC_F5))
      gi->ogre_tray->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    if(GI_IsKeyPressed(gi, OIS::KC_F6))
      gi->ogre_tray->hideFrameStats();
    if(GI_IsKeyPressed(gi, OIS::KC_F9))
      GI_DumpDesyncedTrace(gi);
    if(GI_IsKeyPressed(gi, OIS::KC_O)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(30) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_skeleton_mage, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_P)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_archer_of_yssath, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_I)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_archer_of_thelmeril, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_U)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_mask_of_vaaror, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_Y)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_woodsman_of_vale, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_T)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_warrior_of_mushof, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_R)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_thief_of_tavratt, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_F)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_disciple_of_waja, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_G)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_savage_of_ridgar, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_H)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_halfling_of_the_hills, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_J)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_magma_golem_of_lacdragrath, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_K)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_widow_of_krulah, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_L)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_footmen, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_M)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_dark_bat_of_ida, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_V)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_beholder_of_doom, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_B)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_raven_of_krul, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_N)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(5) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_common_skeleton_warrior, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_C)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(1) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_vorguul, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_X)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(1) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_red_dragon_of_the_titraknus, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_A)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(1) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_witch_of_the_lake, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_Z)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(1) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_cursed_chest, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_D)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(1) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT), TR_unit_type_guard_of_the_misty_keep, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    if(GI_IsKeyPressed(gi, OIS::KC_S)
    && INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
      if(mouse_p.x != 9999999)
        DO_TIMES(1) {
          CL_CmdSpawnUnit(gi, c, TR_null, (float)mouse_p.x*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), (float)mouse_p.z*(1<<POS_SHIFT) + rand()%(5<<POS_SHIFT), TR_unit_type_moon_gnome, USER_DATA_PLAYER_UNIT);
        }
      else
        log("rayon trop loin");
    }
    
    #endif
    if(GI_IsKeyPressed(gi, OIS::KC_S)) {
      TR_unit_ptr u = cam->selected_units_head;
      while(u != TR_null) {
        TR_unit_t *un = &unit(u);
        CL_CmdUnitStop(gi, c, u);
        u = un->snext[ca];
      }
    }
    if(cam->skill_here) {
      DO_TIMES(cam->skill_type_count) {
        TR_skill_type_t skill_type = cam->skill_types[f];
        if((gi->lbtn_up 
        || gi->get_skill_direct_cast_cb(gi, skill_type))
        && (INSIDE_MINI(gi->mouse.x, gi->mouse.y)
        || INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y))) {
          bool direct_cast = gi->get_skill_direct_cast_cb(gi, skill_type);
          TR_id_t cmd_mask = cl->cmd_id++;
          int r_squared = CAM_GetRSquared(gi, ca);
          bool ground_only = INSIDE_MINI(gi->mouse.x, gi->mouse.y);
          TR_unit_type_t unit_types[TR_unit_type_count];
          str            unit_type_can_cast[TR_unit_type_count];
          int unit_type_count = 0;
          TR_unit_ptr target = ground_only ? TR_null : CAM_PickUnit(gi, ca, gi->mouse.x, gi->mouse.y);
          target = target != TR_null ? (PL_IsInFov(gi, cl_p, target) ? target : TR_null) : TR_null;
          int min_dist = 0x7fffffff;
          TR_unit_ptr min_u = TR_null;
          bool cast_one_by_one = gi->skill_cast_one_by_one_cb(gi, skill_type);
          TR_unit_ptr u = cam->selected_units_head;
          while(u != TR_null) {
            TR_unit_t *un = &unit(u);
            int skill_id = UN_SkillId(gi, u, skill_type);
            if(skill_id != -1) {
              int x = -1, y;
              if(direct_cast) {
                x = un->x + UN_GetSizeShift(gi, SIZE(&unit(u)));
                y = un->y + UN_GetSizeShift(gi, SIZE(&unit(u)));
              } else {
                if(INSIDE_MINI(gi->mouse.x, gi->mouse.y)) {
                  x = ((gi->mouse.x-MINIMAP_START_X)*MINIMAP_INV_RATIO_X)<<POS_SHIFT;
                  y = ((gi->mouse.y-MINIMAP_START_Y)*MINIMAP_INV_RATIO_Y)<<POS_SHIFT;
                } else
                if(INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
                  x = (float)mouse_p.x*(1<<POS_SHIFT);
                  y = (float)mouse_p.z*(1<<POS_SHIFT);
                }
              }
              if(cast_one_by_one) {
                if(gi->can_cast_skill_cb(gi, cl_p, u, skill_type, target, x, y, false) == "") {
                  if(!CL_HasOrders(gi, c, u, true, skill_type)
                  && (gi->get_skill_continue_cb(gi, skill_type)
                  || un->skills_cooldown[skill_id] >= gi->get_skill_cooldown_cb(gi, cl_p, u, skill_type))) {
                    int dist = target != TR_null ? UN_GetDist(gi, u, target) : UN_GetDist(gi, u, x, y);
                    if(dist < min_dist) {
                      min_dist = dist;
                      min_u = u;
                    }
                  } else {
                    /* fail */
                    if(un->player_id == cam->player_id) {
                      bool already = false;
                      DO_TIMES(unit_type_count)
                        if(unit_types[f] == un->unit_type) {
                          already = true;
                          unit_type_can_cast[f] = unit_type_can_cast[f] == "" ? "" :
                            "It's not ready yet.";
                          break;
                        }
                      if(!already) {
                        unit_types[unit_type_count] = un->unit_type;
                        unit_type_can_cast[unit_type_count] = 
                          "It's not ready yet.";
                        unit_type_count++;
                      }
                    }
                  }
                } else {
                  /* fail */
                  if(un->player_id == cam->player_id) {
                    bool already = false;
                    DO_TIMES(unit_type_count)
                      if(unit_types[f] == un->unit_type) {
                        already = true;
                        unit_type_can_cast[f] = unit_type_can_cast[f] == "" ? "" :
                          gi->can_cast_skill_cb(
                            gi, cl_p, u, skill_type, 
                            target, x, y, false);
                        break;
                      }
                    if(!already) {
                      unit_types[unit_type_count] = un->unit_type;
                      unit_type_can_cast[unit_type_count] = 
                        gi->can_cast_skill_cb(
                          gi, cl_p, u, skill_type, 
                          target, x, y, false);
                      unit_type_count++;
                    }
                  }
                }
              } else {
                if(gi->can_cast_skill_cb(gi, cl_p, u, skill_type, target, x, y, false) == "") {
                  if(gi->get_skill_continue_cb(gi, skill_type)
                  || un->skills_cooldown[skill_id] >= gi->get_skill_cooldown_cb(gi, cl_p, u, skill_type)) {
                    CL_CmdUnitOrder(
                      gi, c, u, x, y, target,
                      GI_IsKeyDown(gi, OIS::KC_LSHIFT) || GI_IsKeyDown(gi, OIS::KC_RSHIFT), 
                      true, skill_type, ground_only, 
                      cmd_mask, r_squared);
                  } else {
                    /* fail */
                    if(un->player_id == cam->player_id) {
                      bool already = false;
                      DO_TIMES(unit_type_count)
                        if(unit_types[f] == un->unit_type) {
                          already = true;
                          unit_type_can_cast[f] = unit_type_can_cast[f] == "" ? "" :
                            "It's not ready yet.";
                          break;
                        }
                      if(!already) {
                        unit_types[unit_type_count] = un->unit_type;
                        unit_type_can_cast[unit_type_count] = 
                          "It's not ready yet.";
                        unit_type_count++;
                      }
                    }
                  }
                } else {
                  /* fail */
                  if(un->player_id == cam->player_id) {
                    bool already = false;
                    DO_TIMES(unit_type_count)
                      if(unit_types[f] == un->unit_type) {
                        already = true;
                        unit_type_can_cast[f] = unit_type_can_cast[f] == "" ? "" :
                          gi->can_cast_skill_cb(
                            gi, cl_p, u, skill_type, 
                            target, x, y, false);
                        break;
                      }
                    if(!already) {
                      unit_types[unit_type_count] = un->unit_type;
                      unit_type_can_cast[unit_type_count] = 
                        gi->can_cast_skill_cb(
                          gi, cl_p, u, skill_type, 
                          target, x, y, false);
                      unit_type_count++;
                    }
                  }
                }
              }
            }
            u = un->snext[ca];
          }
          if(cast_one_by_one) {
            if(min_u != TR_null) {
              int x = -1, y;
              if(direct_cast) {
                x = unit(min_u).x + UN_GetSizeShift(gi, SIZE(&unit(min_u)));
                y = unit(min_u).y + UN_GetSizeShift(gi, SIZE(&unit(min_u)));
              } else {
                if(INSIDE_MINI(gi->mouse.x, gi->mouse.y)) {
                  x = ((gi->mouse.x-MINIMAP_START_X)*MINIMAP_INV_RATIO_X)<<POS_SHIFT;
                  y = ((gi->mouse.y-MINIMAP_START_Y)*MINIMAP_INV_RATIO_Y)<<POS_SHIFT;
                } else
                if(INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y)) {
                  x = (float)mouse_p.x*(1<<POS_SHIFT);
                  y = (float)mouse_p.z*(1<<POS_SHIFT);
                }
              }
              CL_CmdUnitOrder(
                gi, c, min_u, x, y, target,
                /* we don't want push back for cast-one-by-one, 
                   we want shift to continue casting, not pushing back */ 
                false,
                true, skill_type, ground_only, 
                cmd_mask, r_squared);
              /* fail */
              if(unit(min_u).player_id == cam->player_id) {
                bool already = false;
                DO_TIMES(unit_type_count)
                  if(unit_types[f] == unit(min_u).unit_type) {
                    already = true;
                    unit_type_can_cast[f] = unit_type_can_cast[f] == "" ? "" :
                      gi->can_cast_skill_cb(
                        gi, cl_p, min_u, skill_type, 
                        target, x, y, false);
                    break;
                  }
                if(!already) {
                  unit_types[unit_type_count] = unit(min_u).unit_type;
                  unit_type_can_cast[unit_type_count] = 
                    gi->can_cast_skill_cb(
                      gi, cl_p, min_u, skill_type, 
                      target, x, y, false);
                  unit_type_count++;
                }
              }
            } else {
              PLAY_SOUND(gp_gi.miss_sound, "miss")
            }
          }
          DO_TIMES(unit_type_count) {
            gi->on_skill_sound_cb(
              gi, skill_type, 
              -1, -1, unit_type_can_cast[f] == "");
            if(unit_type_can_cast[f] != "") {
              PLAY_SOUND(gp_gi.miss_sound, "miss")
              CAM_SetFlash(gi, ca, unit_type_can_cast[f], TCOD_red);
            }
          }
          if((!GI_IsKeyDown(gi, OIS::KC_LSHIFT)
          && !GI_IsKeyDown(gi, OIS::KC_RSHIFT))
          || gi->get_skill_direct_cast_cb(gi, skill_type)) {
            cam->skill_here = false;
          } else
            cam->used_skill = true;
        }
      }
    }
    if(gi->rbtn_up) {
      if(cam->skill_here)
        cam->skill_here = false;
      else
        if(INSIDE_MINI(gi->mouse.x, gi->mouse.y))
          CL_CmdUnitOrderOnSelection(
            gi, 
            c, 
            ((gi->mouse.x-MINIMAP_START_X)*MINIMAP_INV_RATIO_X)<<POS_SHIFT, 
            ((gi->mouse.y-MINIMAP_START_Y)*MINIMAP_INV_RATIO_Y)<<POS_SHIFT,
            TR_null,
            GI_IsKeyDown(gi, OIS::KC_LSHIFT) || GI_IsKeyDown(gi, OIS::KC_RSHIFT),
            false);
        else
        if(INSIDE_VIEWPORT(gi->mouse.x, gi->mouse.y))
          CL_CmdUnitOrderOnSelection(
            gi, 
            c, 
            (float)mouse_p.x*(1<<POS_SHIFT),
            (float)mouse_p.z*(1<<POS_SHIFT),
            CAM_PickUnit(gi, ca, gi->mouse.x, gi->mouse.y),
            GI_IsKeyDown(gi, OIS::KC_LSHIFT) || GI_IsKeyDown(gi, OIS::KC_RSHIFT),
            false);
    }
  }
}

void CL_CmdUnitOrderOnSelection(TR_game_instance_t *gi, TR_client_ptr c, int to_x, int to_y, TR_unit_ptr target, bool push_back, bool skill_here, TR_skill_type_t skill_type, bool ground_only, TR_id_t cmd_mask, int r_squared, TR_ptr_t user_data, TR_unit_order_cb_t unit_order_completed_cb, TR_unit_order_cb_t unit_order_failed_cb) {
  if(ground_only)
    UN_FindClosestPoint(gi, 1, &to_x, &to_y);
  BOUND((MAP_SIZE_X-2)<<POS_SHIFT, to_x);
  BOUND((MAP_SIZE_Y-2)<<POS_SHIFT, to_y);
  TR_client_t *cl = &client(c);
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl = &player(p);
  cl->cmd_id++;
  cmd_mask = cmd_mask == -1 ? cl->cmd_id : cmd_mask;
  cl->cmd_id++;
  int min_x = (MAP_SIZE_X-1)<<POS_SHIFT, 
      min_y = (MAP_SIZE_Y-1)<<POS_SHIFT, 
      max_x = 0, 
      max_y = 0,
      area  = 0,
      count = 0;
  TR_unit_ptr u = cam->selected_units_head;
  while(u != TR_null) {
    TR_unit_t *un   = &unit(u);
    /* it is possible to select other players' units */
    if(un->player_id == cl->player_id
    && CAN_BE_PUSHED(un)) {
      if(un->x < min_x) 
        min_x = un->x;
      if(un->y < min_y) 
        min_y = un->y;
      if(un->x > max_x) 
        max_x = un->x;
      if(un->y > max_y) 
        max_y = un->y;
      area += SIZE(un)*SIZE(un);
      count+= SIZE(un);
    }
    u = un->snext[ca];
  }
  int a = area / 3.14;
  int x = (min_x + max_x)/2;
  int y = (min_y + max_y)/2;
  int m = max(max_x-min_x, max_y-min_y);
  target =
       !ground_only
    && target != TR_null 
    && PL_IsInFov(gi, p, target) 
    ? target : TR_null;
  if(target != TR_null) {
    target = skill_here 
          && (!gi->skill_looks_for_unit_cb(gi, skill_type)
          || gi->skill_looks_for_enemy_cb(gi, skill_type)
          && unit(target).team_id == pl->team_id) ? TR_null : target;
    if(skill_here 
    && gi->skill_looks_for_unit_cb(gi, skill_type)
    && target == TR_null)
      skill_here = false;
  }
  /* right click on enemy generate an attack */
  bool generated = false;
  if(!skill_here
  && target != TR_null
  && unit(target).team_id
  != pl->team_id)
    generated = true;
  /* we can't attack a unit from the same team */
  bool  in_formation  = 
             !skill_here
          && (target == TR_null 
          || unit(target).team_id == pl->team_id)
          && m < (count<<POS_SHIFT)
          && m < MAX_FORMATION_MOVE
          && ABS(x - to_x) < MAX_FORMATION_MOVE
          && ABS(y - to_y) < MAX_FORMATION_MOVE
          && (to_x < (min_x + (max_x-min_x)*0) || to_x > (max_x - (max_x-min_x)*0) 
          ||  to_y < (min_y + (max_y-min_y)*0) || to_y > (max_y - (max_y-min_y)*0));
  TR_unit_type_t unit_types[TR_unit_type_count];
  int unit_type_count = 0;
  u = cam->selected_units_head;
  while(u != TR_null) {
    TR_unit_t *un   = &unit(u);
    if(generated) {
      skill_type = gi->default_skill_type_cb(gi, u, target);
      if(skill_type != -1)
        skill_here       = true;
      else
        skill_here       = false;
    } else
      skill_here       = false;
    /* it is possible to select other players' units */
    if(un->player_id == cam->player_id) {
      TR_cmd_ptr r             = CMD_New(gi, un->cmd_id);
      if(r != TR_null) {
        TR_cmd_t *cmd           = &cmd(r);
        cmd->turn               = gi->turn + CMD_TURN_SHIFT;
        cmd->cmd_mask           = cmd_mask;
        cmd->type               = TR_cmd_type_unit_order;
        cmd->r_squared          = in_formation 
                               || skill_here 
                               || (target != TR_null 
                               && unit(target).team_id != pl->team_id) ? 0 : 
                               r_squared == -1 ? a : r_squared;
        cmd->target_cx          = to_x + (in_formation ? un->x - x : 0);
        cmd->target_cy          = to_y + (in_formation ? un->y - y : 0);
        cmd->click_cx           = to_x;
        cmd->click_cy           = to_y;
        cmd->target             = target != TR_null ? unit(target).cmd_id : TR_null;
        cmd->target_player_id   = target != TR_null ? unit(target).player_id : TR_null;
        cmd->target_cmd_id      = target != TR_null ? unit(target).cmd_id : TR_null;
        cmd->skill_here         = skill_here;
        cmd->skill_type           = skill_type;
        cmd->push_back          = push_back;
        cmd->user_data          = user_data;
        cmd->unit_order_completed_cb  = unit_order_completed_cb;
        cmd->unit_order_failed_cb     = unit_order_failed_cb;
        CS_InsertCmd(gi, cl->cs, r);
        /* audio */
        bool already = false;
        DO_TIMES(unit_type_count)
          if(unit_types[f] == unit(u).unit_type) {
            already = true;
            break;
          }
        if(!already) {
          unit_types[unit_type_count++] = unit(u).unit_type;
          if(skill_here)
            gi->on_skill_sound_cb(
              gi, skill_type, -1, -1, 
              gi->can_cast_skill_cb(gi, p, u, skill_type, target, to_x, to_y, false) == "");
          else
            gi->on_move_unit_sound_cb(gi, unit(u).unit_type, -1, -1);
        }
      }
      gi->on_unit_order_cb(
        gi, u, target, un->cmd_id, un->player_id, 
        cmd_mask == -1 ? cl->cmd_id : cmd_mask, 
        un->unit_type, to_x, to_y, skill_here, skill_type);
    } else 
    if(un->team_id == TEAM_ID(cam->player_id)) {
      gi->on_unit_order_cb(
        gi, u, target, un->cmd_id, un->player_id, 
        cmd_mask == -1 ? cl->cmd_id : cmd_mask, 
        un->unit_type, to_x, to_y, skill_here, skill_type);
    }
    u = un->snext[ca];
  }
}

void CL_CmdUnitOrder(TR_game_instance_t *gi, TR_client_ptr c, TR_unit_ptr u, int to_x, int to_y, TR_unit_ptr target, bool push_back, bool skill_here, TR_skill_type_t skill_type, bool ground_only, TR_id_t cmd_mask, int r_squared, TR_ptr_t user_data, TR_unit_order_cb_t unit_order_completed_cb, TR_unit_order_cb_t unit_order_failed_cb, bool play_sound) {
  if(ground_only)
    UN_FindClosestPoint(gi, 1, &to_x, &to_y);
  BOUND((MAP_SIZE_X-2)<<POS_SHIFT, to_x);
  BOUND((MAP_SIZE_Y-2)<<POS_SHIFT, to_y);
  TR_client_t *cl = &client(c);
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  TR_unit_t *un   = &unit(u);
  TR_player_ptr p = GI_GetPlayerPtr(gi, cl->player_id);
  TR_player_t *pl = &player(p);
  cl->cmd_id++;
  cmd_mask = cmd_mask == -1 ? cl->cmd_id : cmd_mask;
  cl->cmd_id++;
  target =
       !ground_only
    && target != TR_null 
    && PL_IsInFov(gi, p, target) 
    ? target : TR_null;
  if(target != TR_null) {
    target = skill_here 
          && (!gi->skill_looks_for_unit_cb(gi, skill_type)
          || gi->skill_looks_for_enemy_cb(gi, skill_type)
          && unit(target).team_id == pl->team_id) ? TR_null : target;
    if(skill_here 
    && gi->skill_looks_for_unit_cb(gi, skill_type)
    && target == TR_null)
      skill_here = false;
  }
  /* we can't attack a unit from the same team */
  if(un->player_id == cl->player_id) {
    TR_cmd_ptr r    = CMD_New(gi, un->cmd_id);
    if(r != TR_null) {
      TR_cmd_t *cmd                = &cmd(r);
      cmd->turn                     = gi->turn + CMD_TURN_SHIFT;
      cmd->cmd_mask                 = cmd_mask;
      cmd->type                     = TR_cmd_type_unit_order;
      cmd->r_squared                = r_squared;
      cmd->target_cx                = to_x;
      cmd->target_cy                = to_y;
      cmd->click_cx                 = to_x;
      cmd->click_cy                 = to_y;
      cmd->target                   = target != TR_null ? unit(target).cmd_id : TR_null;
      cmd->target_player_id         = target != TR_null ? unit(target).player_id : TR_null;
      cmd->target_cmd_id            = target != TR_null ? unit(target).cmd_id : TR_null;
      cmd->skill_here               = skill_here;
      cmd->skill_type               = skill_type;
      cmd->push_back                = push_back;
      cmd->user_data                = user_data;
      cmd->unit_order_completed_cb  = unit_order_completed_cb;
      cmd->unit_order_failed_cb     = unit_order_failed_cb;
      CS_InsertCmd(gi, cl->cs, r);
      /* audio */
      if(play_sound) {
        if(skill_here)
          gi->on_skill_sound_cb(
            gi, skill_type, -1, -1,
            gi->can_cast_skill_cb(gi, p, u, skill_type, target, to_x, to_y, false) == "");
        else
          gi->on_move_unit_sound_cb(gi, unit(u).unit_type, -1, -1);
      }
    }
    gi->on_unit_order_cb(
      gi, u, target, un->cmd_id, un->player_id, 
      cmd_mask == -1 ? cl->cmd_id : cmd_mask, un->unit_type, 
      to_x, to_y, skill_here, skill_type);
  } else 
  if(un->team_id == TEAM_ID(cam->player_id)) {
    gi->on_unit_order_cb(
      gi, u, target, un->cmd_id, un->player_id, 
      cmd_mask == -1 ? cl->cmd_id : cmd_mask, un->unit_type, 
      to_x, to_y, skill_here, skill_type);
  }
}

void CL_CmdUnitStop(TR_game_instance_t *gi, TR_client_ptr c, TR_unit_ptr u) {
  TR_client_t *cl      = &client(c);
  TR_unit_t *un        = &unit(u);
  TR_cmd_ptr r         = CMD_New(gi, un->cmd_id);
  TR_cmd_t *cmd        = &cmd(r);
  cmd->turn             = gi->turn + CMD_TURN_SHIFT;
  cmd->type             = TR_cmd_type_stop_unit;
  CS_InsertCmd(gi, cl->cs, r);
}

void CL_CmdSpawnUnit(TR_game_instance_t *gi, TR_client_ptr c, TR_id_t from_u, int to_x, int to_y, TR_unit_type_t unit_type, TR_user_data_type_t user_data_type, TR_ptr_t user_data, bool find_pos) {
  BOUND((MAP_SIZE_X-1)<<POS_SHIFT, to_x);
  BOUND((MAP_SIZE_Y-1)<<POS_SHIFT, to_y);
  TR_client_t *cl      = &client(c);
  TR_cmd_ptr r         = CMD_New(gi, cl->cmd_id++);
  TR_cmd_t *cmd        = &cmd(r);
  cmd->turn             = gi->turn + CMD_TURN_SHIFT;
  cmd->type             = TR_cmd_type_spawn_unit;
  cmd->unit_type        = unit_type;
  cmd->target_cx        = to_x;
  cmd->target_cy        = to_y;
  cmd->cmd_mask         = from_u;
  cmd->user_data_type   = user_data_type;
  cmd->user_data        = user_data;
  cmd->find_pos         = find_pos;
  CS_InsertCmd(gi, cl->cs, r);
}

void CL_CmdBuildOrder(TR_game_instance_t *gi, TR_client_ptr c, TR_id_t builder_id, TR_id_t builder_player_id, TR_id_t unit_player_id, TR_unit_type_t unit_type, TR_user_data_type_t user_data_type, int target_cx, int target_cy, bool start_it) {
  TR_client_t *cl = &client(c);
  TR_cmd_ptr r           = CMD_New(gi, builder_id);
  TR_cmd_t *cmd          = &cmd(r);
  cmd->turn              = gi->turn + CMD_TURN_SHIFT;
  cmd->type              = TR_cmd_type_build_unit;
  cmd->cmd_mask          = cl->cmd_id++;
  cmd->unit_type         = unit_type;
  cmd->target_cx         = target_cx;
  cmd->target_cy         = target_cy;
  cmd->player_id         = builder_player_id;
  cmd->target_player_id  = unit_player_id;
  cmd->start_it          = start_it;
  cmd->cancel_it         = false;
  cmd->user_data_type    = user_data_type;
  CS_InsertCmd(gi, cl->cs, r);
}

void CL_CmdCancelBuildOrder(TR_game_instance_t *gi, TR_client_ptr c, TR_id_t builder_id, TR_id_t builder_player_id, TR_id_t unit_player_id, TR_id_t unit_id) {
  TR_client_t *cl       = &client(c);
  TR_cmd_ptr r          = CMD_New(gi, builder_id);
  TR_cmd_t *cmd         = &cmd(r);
  cmd->turn              = gi->turn + CMD_TURN_SHIFT;
  cmd->type              = TR_cmd_type_build_unit;
  cmd->cmd_mask          = unit_id;
  cmd->player_id         = builder_player_id;
  cmd->target_player_id  = unit_player_id;
  cmd->cancel_it         = true;
  CS_InsertCmd(gi, cl->cs, r);
}

void CL_CmdGeneric(TR_game_instance_t *gi, TR_client_ptr c, TR_id_t cmd_type, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy) {
  TR_client_t *cl        = &client(c);
  TR_cmd_ptr r           = CMD_New(gi, id);
  TR_cmd_t *cmd          = &cmd(r);
  cmd->turn              = gi->turn + CMD_TURN_SHIFT;
  cmd->type              = TR_cmd_type_generic;
  cmd->cmd_type          = cmd_type;
  cmd->cmd_mask          = cmd_mask;
  cmd->unit_type         = unit_type;
  cmd->target_cx         = target_cx;
  cmd->target_cy         = target_cy;
  cmd->player_id         = player_id;
  CS_InsertCmd(gi, cl->cs, r);
}

bool CL_HasOrders(TR_game_instance_t *gi, TR_client_ptr c, TR_unit_ptr u, bool skill_here, TR_skill_type_t skill_type) {
  TR_client_t *cl = &client(c);
  TR_unit_t *un   = &unit(u);
  TR_cmd_ptr r = cmd_store(cl->cs).cmd_head;
  while(r != TR_null) {
    if(cmd(r).id == un->cmd_id
    && (!skill_here
    || (cmd(r).skill_here
    && cmd(r).skill_type == skill_type)))
      return true;
    r = cmd(r).cmd_next;
  }
  return false;
}

void CL_LoadReplay(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  assert(game_desc.replay != "");
  FILE *file = fopen(str("./replay/" + game_desc.replay + "/p" + i2a(cl->player_id)).c_str(), "r+b");
  assert(file);
  fseek(file, 0L, SEEK_END);
  cl->replay_size = ftell(file);
  if(cl->replay_size > 0) {
    log("Player" + i2a(cl->player_id) + "'s replay size is: " + i2a(cl->replay_size));
    cl->replay_data = new unsigned char[cl->replay_size];
    cl->replay_data_offset = 0;
    fseek(file, 0L, SEEK_SET);
    fread(cl->replay_data, cl->replay_size, 1, file);
  } else
    cl->replay_data_offset = -1;
  fclose(file);
}

void CL_LeechReplay(TR_game_instance_t *gi, TR_client_ptr c) {
  TR_client_t *cl = &client(c);
  if(cl->replay_data_offset != -1
  && (cl->leech_replay_next_turn == -1
  || gi->turn >= cl->leech_replay_next_turn)) {
    int max_turn = gi->turn;
    while(cl->replay_data_offset != -1
       && max_turn <= gi->turn+10+3) {
      int data_length = *(int*)&cl->replay_data[cl->replay_data_offset];
      cl->replay_data_offset += sizeof(int);
      max_turn = CS_LeechCompressedBuffer(gi, cl->cs, data_length, &cl->replay_data[cl->replay_data_offset]);
      cl->replay_data_offset += data_length;
      if(cl->replay_data_offset >= cl->replay_size) {
        assert(cl->replay_data_offset == cl->replay_size);
        cl->replay_data_offset = -1;
      }
    }
    cl->leech_replay_next_turn = gi->turn+10;
  }
}

/*
 * player
 */

void PL_Init(TR_game_instance_t *gi) {
  for(int i = 0; i < MAX_PLAYER; i++) {
    player(i).tcod_map = TCOD_map_new(MAP_SIZE_X, MAP_SIZE_Y);
    if(i != MAX_PLAYER-1)
      player(i).fnext = i+1;
    else
      player(i).fnext = TR_null;
  }
  gi->free_player_head = 0;
  gi->player_head      = TR_null;
  log("Players initialized.");
}

TR_player_ptr PL_New(TR_game_instance_t *gi, TR_ptr_t user_data) {
  if(gi->free_player_head != TR_null) {
    TR_player_ptr r = gi->free_player_head;
    gi->free_player_head = player(gi->free_player_head).fnext;
    TR_player_ptr p        = r;
    TR_player_t *pl        = &player(p);
    pl->win                 = false;
    pl->lose                = false;
    pl->inert               = false;
    pl->human               = false;
    pl->worker_count        = 0;
    pl->unit_head           = TR_null;
    pl->user_data           = user_data;
    pl->previous            = TR_null;
    pl->next                = gi->player_head;
    pl->cs                  = TR_null;
    pl->update_frame_mark   = 0xffffffff;
    TCOD_map_clear(pl->tcod_map, true, true);
    FOR_RECT(0, MAP_SIZE_X-1, 0, MAP_SIZE_Y-1)
      if(AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT) 
      == TR_WALL)
        TCOD_map_set_properties(pl->tcod_map, i, j, false, false);
    DO_TIMES(MAX_WORKER) {
      TR_worker_ptr w = f;
      TR_worker_t *wo = &worker(w);
      wo->close_mark = 0;
      DO_TIMES(MAP_SIZE) {
        wo->close_map[f] = -1;
        wo->f_map[f] = -1;
      }
    }
    DO_TIMES(MAX_UNIT_TYPE)
      memcpy(&pl->unit_dic[f], &gi->unit_dic[f], sizeof(TR_unit_class_t));
    if(gi->player_head != TR_null)
      player(gi->player_head).previous = r;
    gi->player_head = r;
    return r;
  } 
  log("No more player left.");
  return TR_null;
}

void PL_Free(TR_game_instance_t *gi, TR_player_ptr p) {
  PL_KillAll(gi, p);
  if(gi->player_head == p)
    gi->player_head = player(p).next;
  if(player(p).previous != TR_null)
    player(player(p).previous).next = player(p).next;
  if(player(p).next != TR_null)
    player(player(p).next).previous = player(p).previous;
  player(p).fnext = gi->free_player_head;
  gi->free_player_head = p;
}

void PL_FreeAll(TR_game_instance_t *gi) {
  while(gi->player_head != TR_null)
    PL_Free(gi, gi->player_head);
  log("Players freed.");
}

bool PL_NextTurn(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  TR_cmd_ptr r = cmd_store(pl->cs).cmd_head;
  while(r != TR_null) {
    TR_cmd_t *cmd = &cmd(r);
    if(cmd->turn == gi->turn+1 
    && cmd->type == TR_cmd_type_end_of_turn)
      return true;
    r = cmd->cmd_next;
  }
  return pl->inert || game_desc.replay != "";
}

TR_cadaver_ptr PL_KillUnit(TR_game_instance_t *gi, TR_player_ptr killer_p, TR_player_ptr unit_p, TR_unit_ptr u, bool with_blood) {
  TR_player_t *pl = &player(unit_p);
  TR_unit_t *un = &unit(u);
  TR_cadaver_ptr result = TR_null;
  un->hp_overlay->hide();
  un->hp_back_overlay->hide();
  TR_particle_system_ptr ps = un->ps_head;
  while(ps != TR_null) {
    TR_particle_system_t *psys = &particle_system(ps);
    PS_Stop(gi, ps);
    un->node->removeChild(psys->ogre_node);
    gi->ogre_scene->getRootSceneNode()->addChild(psys->ogre_node);
    psys->x = UNIT_X(un);
    psys->y = UNIT_Y(un);
    psys->z *= 0.25*SIZE(un)*MODEL_SCALE(un);
    psys->ogre_node->setPosition((float)psys->x/(1<<POS_SHIFT), psys->z, (float)psys->y/(1<<POS_SHIFT));
    psys->free_it = true;
    ps = psys->unext;
  }
  if(un->forced_action_ps != TR_null) {
    PS_Stop(gi, un->forced_action_ps);
    un->forced_action_ps = TR_null;
  }
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("hp " + i2a(u) + " panel");
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("level text " + i2a(u));
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("hp hit " + i2a(u) + " panel");
  Ogre::OverlayManager::getSingleton().destroyOverlayElement("hp back " + i2a(u) + " panel");
  Ogre::OverlayManager::getSingleton().destroy(un->hp_overlay);
  Ogre::OverlayManager::getSingleton().destroy(un->hp_hit_overlay);
  Ogre::OverlayManager::getSingleton().destroy(un->hp_back_overlay);
  if(MAX_MANA(un) > 0) {
    un->mn_overlay->hide();
    un->mn_back_overlay->hide();
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("mn " + i2a(u) + " panel");
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("mn hit " + i2a(u) + " panel");
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("mn back " + i2a(u) + " panel");
    Ogre::OverlayManager::getSingleton().destroy(un->mn_overlay);
    Ogre::OverlayManager::getSingleton().destroy(un->mn_hit_overlay);
    Ogre::OverlayManager::getSingleton().destroy(un->mn_back_overlay);
  }
  if(TIME_TO_LIVE(un) > 0) {
    un->time_to_live_overlay->hide();
    un->time_to_live_back_overlay->hide();
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("time_to_live " + i2a(u) + " panel");
    Ogre::OverlayManager::getSingleton().destroyOverlayElement("time_to_live back " + i2a(u) + " panel");
    Ogre::OverlayManager::getSingleton().destroy(un->time_to_live_overlay);
    Ogre::OverlayManager::getSingleton().destroy(un->time_to_live_back_overlay);
  }
  bool with_cadaver = true;
  if(gi->on_death_cb
  && killer_p != TR_null)
    with_cadaver = gi->on_death_cb(gi, killer_p, unit_p, u);
  if(with_cadaver
  && with_blood) {
    TR_cadaver_ptr cc = CA_New(gi);
    if(cc != TR_null) {
      TR_cadaver_t *ca = &cadaver(cc);
      ca->mesh_count = MESH_COUNT(un);
      DO_TIMES(ca->mesh_count)
        ca->ent_name[f] = MESH(un)[f];
      ca->anim_name = "Die";
      ca->x = un->x+UN_GetSizeShift(gi, SIZE(un));
      ca->y = un->y+UN_GetSizeShift(gi, SIZE(un));
      if(CAN_BE_PUSHED(un)) {
        ca->lookat_x = un->lookat_x;
        ca->lookat_y = un->lookat_y;
      }
      ca->ori = un->node->_getDerivedOrientation();
      ca->flying = FLYING(un);
      ca->size = SIZE(un);
      ca->model_scale = MODEL_SCALE(un);
      ca->y_angle = Y_ANGLE(un);
      ca->y_offset = Y_OFFSET(un);
      ca->fade = true;
      result = cc;
    }
  }
  pl->cmd_to_mem_unit_map.erase(un->cmd_id);
  if(pl->unit_head == u)
    pl->unit_head = un->pnext;
  if(un->pprevious != TR_null)
    unit(un->pprevious).pnext = un->pnext;
  if(un->pnext != TR_null)
    unit(un->pnext).pprevious = un->pprevious;
  TR_camera_ptr ca = GI_GetCamera(gi);
  TR_camera_t *cam = &camera(ca);
  if(cam->last_hover == un->hover_ent)
    cam->last_hover = null;
  DO_TIMES(MAX_GROUP_SELECTION)
    if(CAM_IsUnitSelected(gi, ca, u, f))
      CAM_SubToSelection(gi, ca, u, f);
  if(CAM_IsUnitSelected(gi, ca, u))
    CAM_SubToSelection(gi, ca, u);
  /* must be before the ent is destroyed */
  UN_Free(gi, u);
  un->select_node->detachObject(un->select_ent);
  un->select_node->detachObject(un->hover_ent);
  gi->ogre_scene->getRootSceneNode()->removeChild(un->select_node);
  gi->ogre_scene->destroyEntity(un->select_ent);
  gi->ogre_scene->destroyEntity(un->hover_ent);
  gi->ogre_scene->destroySceneNode(un->select_node);
  DO_TIMES(MESH_COUNT(un))
    un->node->detachObject(un->ent[f]);
  gi->ogre_scene->getRootSceneNode()->removeChild(un->node);
  DO_TIMES(MESH_COUNT(un))
    gi->ogre_scene->destroyEntity(un->ent[f]);
  gi->ogre_scene->destroySceneNode(un->node);
  return result;
}

void PL_KillAll(TR_game_instance_t *gi, TR_player_ptr p) {
  while(player(p).unit_head != TR_null)
    PL_KillUnit(gi, TR_null, p, player(p).unit_head, false);
}

bool PL_IsUnitAlive(TR_game_instance_t *gi, TR_player_ptr p, TR_id_t cmd_id) {
  assert(p != TR_null);
  TR_player_t *pl = &player(p);
  TR_unit_ptr u2  = pl->unit_head;
  while(u2 != TR_null) {
    TR_unit_t *un2 = &unit(u2);
    if(cmd_id == un2->cmd_id)
      return true;
    u2 = un2->pnext;
  }
  return false;
}

int PL_IsInFov(TR_game_instance_t *gi, TR_player_ptr p, int pos_x, int pos_y) {
  TR_player_t *pl = &player(p);
  return INSIDE_MAP(pos_x, pos_y) && ON_FOV_MAP(pos_x, pos_y);
}

bool PL_IsInFov(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  TR_player_t *pl = &player(p);
  TR_unit_t   *un = &unit(u);
  /* this fun is bottleneck */
  if(TEAM_ID(pl->id) == un->team_id)
    return true;
  short size = SIZE(un);
  if(size==1)
    return PL_IsInFov(gi, p, (UNIT_X(un)>>(POS_SHIFT+RANGE_SHIFT)), (UNIT_Y(un)>>(POS_SHIFT+RANGE_SHIFT)));
  SOCLE(size)
    if(PL_IsInFov(
        gi, p, 
        ((UNIT_X(un)>>POS_SHIFT)+i)>>RANGE_SHIFT, 
        ((UNIT_Y(un)>>POS_SHIFT)+j)>>RANGE_SHIFT))
      return true;
  return false;
}

int PL_IsInMagicVision(TR_game_instance_t *gi, TR_player_ptr p, int pos_x, int pos_y) {
  TR_player_t *pl = &player(p);
  return INSIDE_MAP(pos_x, pos_y) && ON_MAGIC_VISION_MAP(pos_x, pos_y);
}

bool PL_IsInMagicVision(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  TR_player_t *pl = &player(p);
  TR_unit_t   *un = &unit(u);
  /* this fun is bottleneck */
  if(TEAM_ID(pl->id) == un->team_id)
    return true;
  short size = SIZE(un);
  if(size==1)
    return PL_IsInMagicVision(gi, p, (UNIT_X(un)>>(POS_SHIFT+RANGE_SHIFT)), (UNIT_Y(un)>>(POS_SHIFT+RANGE_SHIFT)));
  SOCLE(size)
    if(PL_IsInMagicVision(
        gi, p, 
        ((UNIT_X(un)>>POS_SHIFT)+i)>>RANGE_SHIFT, 
        ((UNIT_Y(un)>>POS_SHIFT)+j)>>RANGE_SHIFT))
      return true;
  return false;
}

int PL_GetLight(TR_game_instance_t *gi, TR_player_ptr p, short pos_x, short pos_y) {
  return MIN(255, AMBIENT_LIGHT + PL_IsInFov(gi, p, pos_x, pos_y));
}

void PL_ProcessCmdStore(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t       *pl = &player(p);
  TR_cmd_store_t    *cs = &cmd_store(pl->cs);
  TR_cmd_ptr r;
  /* spawns */
  r = cs->cmd_head;
  while(r != TR_null) {
    TR_cmd_t *cmd = &cmd(r);
    if(cmd->turn == gi->turn) {
      switch(cmd->type) {
        case TR_cmd_type_spawn_unit: {
            TR_unit_ptr u = 
              PL_SpawnUnit(
                gi, 
                p, 
                cmd->id,
                cmd->target_cx,
                cmd->target_cy, 
                cmd->unit_type, 
                cmd->user_data_type,
                cmd->user_data,
                cmd->find_pos);
            if(gi->on_spawn_unit_cb) {
              assert(cmd->cmd_mask == TR_null
                  || pl->cmd_to_mem_unit_map.find(cmd->cmd_mask)
                  != pl->cmd_to_mem_unit_map.end());
              gi->on_spawn_unit_cb(gi, cmd->cmd_mask == TR_null ? TR_null : pl->cmd_to_mem_unit_map[cmd->cmd_mask], u);
            }
          } break;
        case TR_cmd_type_stop_unit: {
            if(pl->cmd_to_mem_unit_map.find(cmd->id) 
            != pl->cmd_to_mem_unit_map.end()) {
              TR_unit_ptr u = pl->cmd_to_mem_unit_map[cmd->id];
              if(UO_GetFrontType(gi, u) != TR_unit_order_none 
              && !UO_GetFront(gi, u)->forced_action)
                UO_FreeAll(gi, u);
            }
          } break;
      }
    }
    r = cmd->cmd_next;
  }
  r = cs->cmd_head;
  while(r != TR_null) {
    TR_cmd_t *cmd = &cmd(r);
    if(cmd->turn == gi->turn) {
      switch(cmd->type) {
        case TR_cmd_type_unit_order: {
          /* the unit may be dead */
          if(pl->cmd_to_mem_unit_map.find(cmd->id) 
          != pl->cmd_to_mem_unit_map.end()) {
            BOUND((MAP_SIZE_X-1)<<POS_SHIFT, cmd->target_cx);
            BOUND((MAP_SIZE_Y-1)<<POS_SHIFT, cmd->target_cy);
            TR_unit_ptr u             = pl->cmd_to_mem_unit_map[cmd->id];
            TR_unit_t *un            = &unit(u);
            if((UO_GetFrontType(gi, u) != TR_unit_order_skill
            || cmd->push_back
            || gi->get_skill_can_stop_cb(gi, UO_GetFront(gi, u)->skill_type))
            && (UO_GetFrontType(gi, u) == TR_unit_order_none || !UO_GetFront(gi, u)->forced_action)
            && (cmd->target == TR_null
            || cmd->push_back
            || UO_GetFrontType(gi, u) == TR_unit_order_none
            || (UO_GetFront(gi, u)->skill_here 
            && UO_GetFront(gi, u)->skill_type != cmd->skill_type)
            || !UN_IsFollowingThisTarget(gi, u, player(GI_GetPlayerPtr(gi, cmd->target_player_id)).cmd_to_mem_unit_map[cmd->target]))) {
              TR_unit_order_ptr o;
              if(cmd->push_back)
                o = UO_PushBack(gi, u);
              else {
                if(UO_GetFrontType(gi, u) == TR_unit_order_skill
                && UO_GetFront(gi, u)->skill_type != cmd->skill_type)
                  un->skill = 0;
                if(!cmd->skill_here
                || !gi->get_skill_direct_cast_cb(gi, cmd->skill_type))
                  UO_FreeAll(gi, u);
                else 
                if(UO_GetFrontType(gi, u) != TR_unit_order_none
                && UO_GetFrontType(gi, u) != TR_unit_order_follow_goal) {
                  UO_FreeFront(gi, u);
                  assert(UO_GetFrontType(gi, u) 
                    == TR_unit_order_follow_goal);
                  UO_GetFront(gi, u)->last_target_cx = -1;
                }
                un->wait          = 0;
                un->blocked_time  = 0;
                o = UO_PushFront(gi, u);
              }
              gi->has_processed_cmds      = true;
              int shift                   = UN_GetSizeShift(gi, SIZE(un));
              TR_unit_order_t *uo        = &order(o);
              uo->generated               = false;
              uo->user_data               = cmd->user_data;
              uo->unit_order_completed_cb = cmd->unit_order_completed_cb;
              uo->unit_order_failed_cb    = cmd->unit_order_failed_cb;
              if(cmd->target == TR_null) {
                uo->order             = TR_unit_order_follow_goal;
                uo->target            = TR_null;
                uo->target_cx         = cmd->target_cx-shift;
                uo->target_cy         = cmd->target_cy-shift;
                uo->click_cx          = cmd->click_cx-shift;
                uo->click_cy          = cmd->click_cy-shift;
                uo->cmd_mask          = cmd->cmd_mask;
                uo->r_squared         = cmd->r_squared;
                uo->d_squared         = 0;
                uo->skill_here        = cmd->skill_here;
                uo->skill_type          = cmd->skill_type;
                uo->precise_target    = true;
              } else {
                /* there's a precise target */
                TR_player_ptr target_p = GI_GetPlayerPtr(gi, cmd->target_player_id);
                assert(target_p != TR_null);
                TR_player_t *target_pl = &player(target_p);
                if(PL_IsUnitAlive(gi, target_p, cmd->target_cmd_id)) {
                  uo->order             = TR_unit_order_follow_goal;
                  assert(target_pl->cmd_to_mem_unit_map.find(cmd->target)
                      != target_pl->cmd_to_mem_unit_map.end());
                  uo->target            = 
                    target_pl->cmd_to_mem_unit_map[cmd->target];
                  uo->target_player_id  = cmd->target_player_id;
                  uo->target_cmd_id     = cmd->target_cmd_id;
                  uo->target_cx         = cmd->target_cx-shift;
                  uo->target_cy         = cmd->target_cy-shift;
                  uo->click_cx          = cmd->click_cx-shift;
                  uo->click_cy          = cmd->click_cy-shift;
                  uo->cmd_mask          = uo->target;
                  uo->r_squared         = cmd->r_squared;
                  uo->d_squared         = 0;
                  uo->last_target_cx    = -1;
                  uo->skill_here        = cmd->skill_here;
                  uo->skill_type          = cmd->skill_type;
                  uo->precise_target    = true;
                }
              }
            }
          }
        } break;
      case TR_cmd_type_build_unit: {
          TR_player_ptr p2 = GI_GetPlayerPtr(gi, cmd->player_id);
          if(player(p2).cmd_to_mem_unit_map.find(cmd->id) 
          != player(p2).cmd_to_mem_unit_map.end()) {
            /* cancel */
            TR_unit_ptr u             = player(p2).cmd_to_mem_unit_map[cmd->id];
            TR_unit_t *un           = &unit(u);
            if(cmd->cancel_it) {
              if(un->build_order_head[cmd->target_player_id] != TR_null) {
                TR_build_order_ptr b = un->build_order_head[cmd->target_player_id];
                while(b != TR_null) {
                  TR_build_order_t *bo = &build_order(b);
                  if(bo->unit_id == cmd->cmd_mask) {
                    if(gi->on_cancel_build_cb)
                      gi->on_cancel_build_cb(gi, b);
                    BO_Free(gi, u, b);
                    break;
                  }
                  b = bo->next;
                }
              }
            } else {
              /* build */
              TR_build_order_ptr b    = BO_New(gi);
              TR_build_order_t *bo    = &build_order(b);
              bo->unit_type           = cmd->unit_type;
              bo->unit_id             = cmd->cmd_mask;
              bo->unit_player_id      = cmd->target_player_id;
              bo->target_cx           = cmd->target_cx;
              bo->target_cy           = cmd->target_cy;
              bo->started             = cmd->start_it;
              bo->user_data_type      = cmd->user_data_type;
              BO_PushBackBuildOrder(gi, u, b);
            }
          }
        } break;
      case TR_cmd_type_generic: {
          if(gi->on_generic_cmd_cb)
            gi->on_generic_cmd_cb(
              gi, 
              cmd->cmd_type,
              cmd->id, 
              cmd->player_id, 
              cmd->cmd_mask, 
              cmd->unit_type, 
              cmd->target_cx, 
              cmd->target_cy);
        } break;
      }
    }
    r = cmd->cmd_next;
  }
}

TR_unit_ptr PL_SpawnUnit(TR_game_instance_t *gi, TR_player_ptr p, TR_id_t cmd_id, int x, int y, TR_unit_type_t unit_type, TR_user_data_type_t user_data_type, TR_ptr_t user_data, bool find_pos) {
  TR_player_t *pl = &player(p);
  TR_unit_ptr u   = UN_New(gi);
  if(u != TR_null) {
    TR_unit_t *un                      = &unit(u);
    assert(pl->cmd_to_mem_unit_map.find(cmd_id) 
        == pl->cmd_to_mem_unit_map.end());
    pl->cmd_to_mem_unit_map[cmd_id]       = u;
    un->user_data_type                    = user_data_type;
    un->user_data                         = user_data;
    un->unit_type                         = unit_type;
    un->unit_class                        = pl->unit_dic[un->unit_type];
    un->player_id                         = pl->id;
    un->magic_vision                      = MAGIC_VISION(un);
    un->team_id                           = TEAM_ID(un->player_id);
    un->cmd_id                            = cmd_id;
    if(find_pos)
      UN_FindClosestPoint(gi, SIZE(un), &x, &y, false);
    un->x                                 = x;
    un->y                                 = y;
    //DTRACE("Spawn");
    //DTRACEI(x);
    //DTRACEI(y);
    un->old_env_x                         = -1;
    un->old_env_y                         = -1;
    un->lookat_x                          = un->x+0;
    un->lookat_y                          = un->y+1;
    un->old_lookat_x                      = un->x+0;
    un->old_lookat_y                      = un->y+1;
    un->hp = un->virtual_hp = un->hit_hp  = MAX_HP(un);
    un->mana = un->hit_mana               = MAX_MANA(un);
    un->skill_count                       = un->unit_class.skill_count;
    DO_TIMES(un->skill_count)
      un->skills[f] = un->unit_class.skills[f];
    un->node = gi->ogre_scene->getRootSceneNode()->createChildSceneNode("unit " + i2a(u));
    un->node->setPosition(Ogre::Vector3(0, 0, 0));
    un->node->setScale(Ogre::Vector3(0.25*SIZE(un)*MODEL_SCALE(un), 0.25*SIZE(un)*MODEL_SCALE(un), 0.25*SIZE(un)*MODEL_SCALE(un)));
    un->node->setOrientation(Ogre::Quaternion(Ogre::Degree(Y_ANGLE(un)), Ogre::Vector3(0, 1, 0)));
    assert(un->node);
    DO_TIMES(MESH_COUNT(un)) {
      int i = f;
      un->ent[i] = gi->ogre_scene->createEntity("unit ent " + i2a(u) + " " + i2a(i), MESH(un)[i] + ".mesh");
      assert(un->ent[i]);
      un->ent[i]->setVisible(i == 0);
      un->ent[i]->setCastShadows(false);
      un->ent[i]->setUserAny(Ogre::Any(u));
      un->ent[i]->setQueryFlags(0);
      un->node->attachObject(un->ent[i]);
      DO_TIMES(un->ent[i]->getNumSubEntities()) {
        if(!un->ent[i]->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      getTextureUnitState("FovMapTex")) {
          Ogre::TextureUnitState *tex = 
            un->ent[i]->
              getSubEntity(f)->
                getMaterial()->
                  getTechnique(0)->
                    getPass(0)->
                      createTextureUnitState("FovMapTex");
          tex->setName("FovMapTex");
          tex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        }
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setVertexProgram("Ogre/BasicVertexPrograms/AmbientOneTextureUnified", false);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setFragmentProgram("Ogre/BasicFragmentPrograms/AlphaOneTexture", false);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setShadowCasterVertexProgram("Ogre/BasicVertexPrograms/AmbientShadowCasterUnified");
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setShadowCasterFragmentProgram("Ogre/BasicFragmentPrograms/ShadowCasterWithAlpha");
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              setCullingMode(Ogre::CULL_NONE); 
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setSceneBlending(Ogre::SBT_REPLACE);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              setDepthWriteEnabled(true);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              setDepthCheckEnabled(true);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              setTransparencyCastsShadows(false);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setDepthWriteEnabled(true);
        un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
        if(un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 0)
          un->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(0)->
                      setTextureFiltering(Ogre::TFO_NONE);
        if(un->ent[i]->
          getSubEntity(f)->
            getMaterial()->
              getTechnique(0)->
                getPass(0)->
                  getNumTextureUnitStates() > 1)
          un->ent[i]->
            getSubEntity(f)->
              getMaterial()->
                getTechnique(0)->
                  getPass(0)->
                    getTextureUnitState(1)->
                      setTextureFiltering(Ogre::TFO_NONE);
      }
    }
    /* anim */
    PLAY_UNIT_ANIM(u, "Idle", true)
    un->anim_span = IDLE_ANIM_SPAN;
    /* select */
    un->select_ent = gi->ogre_scene->createEntity("unit " + i2a(u) + " select", un->team_id == game_desc.local_team_id ? "selection_green.mesh" : "selection_red.mesh");
    un->select_ent->setCastShadows(false);
    un->select_ent->setVisible(false);
    un->hover_ent = gi->ogre_scene->createEntity("unit " + i2a(u) + " hover", un->team_id == game_desc.local_team_id ? "selection_white.mesh" : "selection_white_red.mesh");
    un->hover_ent->setCastShadows(false);
    un->hover_ent->setVisible(false);
    un->select_node = un->node->createChildSceneNode("unit " + i2a(u) + " select");
    un->select_node->attachObject(un->select_ent);
    un->select_node->attachObject(un->hover_ent);
    un->select_node->setScale(Ogre::Vector3((float)4*SELECT_SCALE(un), (float)4*SELECT_SCALE(un), (float)4*SELECT_SCALE(un)));
    /* hp and mn overlay */
    un->hp_overlay = Ogre::OverlayManager::getSingleton().create("hp " + i2a(u));
    un->hp_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "hp " + i2a(u) + " panel"));
    un->hp_overlay->getChild("hp " + i2a(u) + " panel")->addChild((Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "level text " + i2a(u)));
    un->hp_overlay->hide();
    un->hp_hit_overlay = Ogre::OverlayManager::getSingleton().create("hp hit " + i2a(u));
    un->hp_hit_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "hp hit " + i2a(u) + " panel"));
    un->hp_hit_overlay->hide();
    un->hp_back_overlay = Ogre::OverlayManager::getSingleton().create("hp back " + i2a(u));
    un->hp_back_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "hp back " + i2a(u) + " panel"));
    un->hp_back_overlay->hide();
    if(MAX_MANA(un) > 0) {
      un->mn_overlay = Ogre::OverlayManager::getSingleton().create("mn " + i2a(u));
      un->mn_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "mn " + i2a(u) + " panel"));
      un->mn_overlay->hide();
      un->mn_hit_overlay = Ogre::OverlayManager::getSingleton().create("mn hit " + i2a(u));
      un->mn_hit_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "mn hit " + i2a(u) + " panel"));
      un->mn_hit_overlay->hide();
      un->mn_back_overlay = Ogre::OverlayManager::getSingleton().create("mn back " + i2a(u));
      un->mn_back_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "mn back " + i2a(u) + " panel"));
      un->mn_back_overlay->hide();
    }
    if(TIME_TO_LIVE(un) > 0) {
      un->time_to_live_overlay = Ogre::OverlayManager::getSingleton().create("time_to_live " + i2a(u));
      un->time_to_live_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "time_to_live " + i2a(u) + " panel"));
      un->time_to_live_overlay->hide();
      un->time_to_live_back_overlay = Ogre::OverlayManager::getSingleton().create("time_to_live back " + i2a(u));
      un->time_to_live_back_overlay->add2D((Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "time_to_live back " + i2a(u) + " panel"));
      un->time_to_live_back_overlay->hide();
    }
    /* bbox */
    /*#if !RELEASE
    Ogre::OBBoxRenderable *bbox_renderable = new Ogre::OBBoxRenderable();
    bbox_renderable->setupVertices(un->ent->getBoundingBox());
    un->node->attachObject(bbox_renderable);
    #endif*/
    UN_PutInMap(gi, u);
    if(pl->unit_head != TR_null)
      unit(pl->unit_head).pprevious = u;
    un->pnext = pl->unit_head;
    pl->unit_head = u;
    return u;
  }
  return TR_null;
}

void PL_UpdateUnitsPart1(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  TR_unit_ptr u;
  /* unit order */
  //DTRACE("part1")
  u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    //DTRACE("unit")
    //DTRACEI(u)
    //DTRACEI(un->unit_type)
    //DTRACEI(un->wait)
    //DTRACEI(un->x)
    //DTRACEI(un->y)
    //DTRACEI(UO_GetFrontType(gi, u));
    /* ORDERS */
    
    un->dx = un->dy = 0;
    un->wait -= gi->game_time_step;
    if(un->wait < 0) un->wait = 0;
    if(un->wait <= 0) {
      TR_unit_ptr u2;
      bool done = true;
      /* FORCED ACTION HANDLING */
      if(UO_GetFrontType(gi, u) != TR_unit_order_none
      && UO_GetFront(gi, u)->forced_action
      && (!PL_IsUnitAlive(gi, GI_GetPlayerPtr(gi, UO_GetFront(gi, u)->forced_action_unit_player_id), UO_GetFront(gi, u)->forced_action_unit_cmd_id)
      || unit(UO_GetFront(gi, u)->forced_action_unit).forced_action_marker != UO_GetFront(gi, u)->forced_action_mark)) {
        if(un->forced_action_ps != TR_null) {
          PS_Stop(gi, un->forced_action_ps);
          un->forced_action_ps = TR_null;
        }
        UO_FreeFront(gi, u);
        done = false;
      } else {
        if(UO_GetFrontType(gi, u) != TR_unit_order_none
        && UO_GetFront(gi, u)->forced_action)
          if(un->forced_action_ps == TR_null) {
            START_PARTICLE("forced_action", UNIT_X(un), UNIT_Y(un), BAR_HEIGHT(un)-0.25, 0.02)
            un->forced_action_ps = ps;
          } else {
            TR_particle_system_t *psys = &particle_system(un->forced_action_ps);
            psys->x = UNIT_X(un);
            psys->y = UNIT_Y(un);
          }
        /* NORMAL ORDER HANDLING */
        switch(UO_GetFrontType(gi, u)) {
          case TR_unit_order_none: {
              un->move = 0;
              un->skill = 0;
              un->think += gi->game_time_step;
              if(un->think >= NONETHINK_SPAN) {
                un->think = 0;
                /* regular aggro check */
                if(un->no_aggro == 0
                && gi->default_skill_type_cb(gi, u, TR_null) != -1
                && PL_GenFollowUnit(gi, p, u, 0, gi->default_skill_type_cb(gi, u, TR_null), false)) {
                  UN_PropagateAttack(gi, u, UO_GetFront(gi, u)->target, true);
                  done = false;
                } else {
                  PLAY_UNIT_ANIM(u, "Idle", true)
                  un->anim_span = IDLE_ANIM_SPAN;
                }
              }
            } break;
          case TR_unit_order_follow_goal: {
              TR_unit_order_t *uo = UO_GetFront(gi, u);
              done = PL_UpdateFollowGoal(gi, p, u);
              if(done) {
                un->skill = 0;
                un->move = 0;
                bool following_friend = uo->target != TR_null 
                                     && player(uo->target_player_id).team_id == un->team_id;
                if(gi->default_skill_type_cb(gi, u, TR_null) != -1
                && PL_GenFollowUnit(gi, p, u, 0, gi->default_skill_type_cb(gi, u, TR_null) /* attack */, following_friend)) {
                  done = false;
                } else {
                  PLAY_UNIT_ANIM(u, "Idle", true)
                  un->anim_span = IDLE_ANIM_SPAN;
                }
              }
            } break;
          case TR_unit_order_move:
            #if THREADS        
            {
              un->think += gi->game_time_step;
              TR_unit_order_t *uo = UO_GetFront(gi, u);
              TR_unit_t *target_un = null;
              if(uo->target != TR_null)
                target_un = &unit(uo->target);
              int range,
                  skill_id;
              if(uo->skill_here) {
                skill_id = UN_SkillId(gi, u, uo->skill_type);
                assert(skill_id != -1);
                range = gi->get_skill_range_cb(gi, p, u, uo->skill_type);
              }
              /* 'un' is following an enemy by aggro  
                 - it's not a precise target - 
                 so 'un' might found another enemy 
                 in it's attack range in which case
                 it does attack it */
              if(uo->skill_here
              && !uo->precise_target
              /* this is for optimization => this is bottleneck */
              && un->think >= 1000 && !(un->think=0)
              && gi->skill_looks_for_unit_cb(gi, uo->skill_type)
              && (gi->get_skill_continue_cb(gi, uo->skill_type)
              || un->skills_cooldown[UN_SkillId(gi, u, uo->skill_type)] >= gi->get_skill_cooldown_cb(gi, p, u, uo->skill_type))
              && (u2=PL_GetFirstUnitInRange(
                    gi, p, u, MAX(range, AGGRO_RANGE(un)), range, 
                    gi->skill_looks_for_enemy_cb(gi, uo->skill_type),
                    gi->skill_look_for_ground_cb(gi, uo->skill_type),
                    gi->skill_look_for_air_cb(gi, uo->skill_type)))
              != TR_null
              && u2 != uo->target) {
                UO_FreeFront(gi, u);
                assert(UO_GetFrontType(gi, u) 
                == TR_unit_order_follow_goal);
                UO_GetFront(gi, u)->last_target_cx = -1;
                PL_GenFollowUnit(gi, p, u, uo->cmd_mask, uo->skill_type, uo->following_friend);
                done = false;
              } else
              /* towers and co don't move 
                 it lags without that and I've forgot why 
                 does nothing and the uo can be used
                 as rally point */
              if(MOVE_SPEED(un) <= 0) {
                UO_FreeFront(gi, u);
              } else
              /* if 'un' is following a friendly unit 
                 which has stopped - 'un' might be now 
                 following an enemy - it sometimes 
                 checks for the original friendly unit 
                 to move */
              if(uo->following_friend
              && UN_CheckTargetMove(gi, u)) {
                UO_FreeFront(gi, u);
                UO_FreeFront(gi, u);
                assert(UO_GetFrontType(gi, u) 
                    == TR_unit_order_follow_goal);
                done = false;
              } else 
              if(uo->skill_here
              && uo->target != TR_null
              && PL_IsInFov(gi, p, uo->target)
              && UN_GetDist(gi, u, uo->target) 
              <= MAX(1, range 
              - (CAN_BE_PUSHED(un) 
              && UO_GetFrontType(gi, uo->target) == TR_unit_order_move
              && (target_un->x-un->x)*target_un->ddx + (target_un->y-un->y)*target_un->ddy > 0 ?
               ((MOVE_SPEED(target_un)/10 * (ATTACK_SPEED(un)>>2))>>POS_SHIFT)+1 : 0))<<POS_SHIFT) {
                UO_FreeFront(gi, u);
                assert(UO_GetFrontType(gi, u) 
                == TR_unit_order_follow_goal);
                UO_GetFront(gi, u)->last_target_cx = -1;
                PL_UpdateFollowGoal(gi, p, u);
                done = false;
              } else
              /* regular 'skill_here' check */
              if(uo->skill_here
              && uo->target == TR_null
              && ((gi->skill_looks_for_unit_cb(gi, uo->skill_type)
              && (gi->get_skill_continue_cb(gi, uo->skill_type)
              || un->skills_cooldown[skill_id] >= gi->get_skill_cooldown_cb(gi, p, u, uo->skill_type))
              && PL_GenFollowUnit(gi, p, u, uo->cmd_mask, uo->skill_type, uo->following_friend, true))
              || (!gi->skill_looks_for_unit_cb(gi, uo->skill_type)
              && UN_GetDist(gi, u, uo->target_cx, uo->target_cy) 
              <= range<<POS_SHIFT))) {
                /* if not looking for units, it don't care of the move anymore, it want the skill */
                if(!gi->skill_looks_for_unit_cb(gi, uo->skill_type))
                  UO_FreeFront(gi, u);
                assert(UO_GetFrontType(gi, u) 
                == TR_unit_order_follow_goal);
                done = false;
              } else {
                /* NO MOVE HERE */
                if(uo->with_counter) {
                  if(uo->counter <= 0
                  || (uo->target != TR_null
                  && (!PL_IsUnitAlive(gi, GI_GetPlayerPtr(gi, uo->target_player_id), uo->target_cmd_id)
                  || !PL_IsInFov(gi, p, uo->target)
                  || (uo->skill_here
                  && !uo->precise_target
                  && !PL_IsReachable(gi, u, uo->target, range))))) {
                    UO_FreeFront(gi, u);
                    assert(UO_GetFrontType(gi, u) 
                    == TR_unit_order_follow_goal);
                    UO_GetFront(gi, u)->last_target_cx = -1;
                    done = false;
                  }
                  /* after so it doesn't infinite-loop */
                  uo->counter -= gi->game_time_step;
                }
              }
            }
            #endif        
            break;
          case TR_unit_order_skill: {
              TR_unit_order_t *uo = UO_GetFront(gi, u);
              if(uo->following_friend
              && UN_CheckTargetMove(gi, u)) {
                UO_FreeFront(gi, u);
                UO_FreeFront(gi, u);
                assert(UO_GetFrontType(gi, u) 
                    == TR_unit_order_follow_goal);
                done = false;
              } else {
                /* the animation test is (mainly) for the attack animation */
                if(AN_AlreadyPlaying(gi, u, gi->get_skill_anim_name_cb(gi, uo->skill_type).c_str(), false) == TR_null
                || un->skill == 0) {
                  if(un->skill == 0) {
                    un->skill_state = 0;
                    un->skill_state2 = 0;
                    un->skill_state3 = 0;
                  }
                  PLAY_UNIT_ANIM(u, "Idle", true)
                  un->anim_span = IDLE_ANIM_SPAN;
                }
                if(uo->target != TR_null
                && uo->skill_type == gi->default_skill_type_cb(gi, u, uo->target)) {
                  TR_unit_t *target_un    = &unit(uo->target);
                  TR_player_ptr target_p  = GI_GetPlayerPtr(gi, uo->target_player_id);  
                  assert(target_p != TR_null);
                  int skill_id            = UN_SkillId(gi, u, uo->skill_type);
                  if(un->skill_marks[skill_id] != un->virtual_hp_cache
                  && PL_IsUnitAlive(gi, target_p, uo->target_cmd_id)
                  && gi->can_cast_skill_cb(gi, p, u, uo->skill_type, uo->target, uo->target_cx, uo->target_cy, true) == "") {
                    un->virtual_hp_cache   = un->skill_marks[skill_id];
                    int damage             = UN_GetDamage(gi, uo->target, ATTACK_DAMAGE(un), ATTACK_TYPE(un));
                    un->virtual_hp_cache2  = damage;
                    target_un->virtual_hp -= damage;
                  }
                }
                un->move = 0;
                uo->started = true;
              }
            } break;
          default:
            assert(false);
        }
      }
      if(done)
        u = un->pnext;
    } else {
      /* waiting */
      PLAY_UNIT_ANIM(u, "Idle", true)
      un->anim_span = IDLE_ANIM_SPAN;
      un->move = 0;
      un->skill = 0;
      u = un->pnext;
    }
  }
}

void PL_UpdateUnitsPart2(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  /* workers */
  PL_InitWorkers(gi, p);
  TR_unit_ptr u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    /* unit order */
    if(un->wait <= 0) {
      if(UO_GetFrontType(gi, u) 
      == TR_unit_order_move) {
        TR_unit_order_t *uo = UO_GetFront(gi, u);
        /* start move */
        if(!uo->started) {
          if(!uo->starting) {
            if(PL_StartMoveOrder(gi, p, u))
              uo->starting = true;
          }
        }
      }
    }
    /* update unit cb */
    gi->on_update_unit_cb(gi, p, u);
    /* hit hp and mana */
    if((gi->game_frame_mark&1)==0) {
      if(un->hp < un->hit_hp)
        un->hit_hp   -= MAX(1, (un->hit_hp-un->hp)>>3);
      if(un->mana < un->hit_mana)
        un->hit_mana -= MAX(1, (un->hit_mana-un->mana)>>3);
    }
    if(un->hp > un->hit_hp)
      un->hit_hp = un->hp;
    if(un->mana > un->hit_mana)
      un->hit_mana = un->mana;
    /* SKILLS, ST and AN */
    DO_TIMES(MAX_SKILLS_PER_UNIT)
      un->skills_cooldown[f] 
        += gi->game_time_step;
    TR_particle_system_ptr ps = un->ps_head,
                            ps_prev = TR_null;
    while(ps != TR_null) {
      TR_particle_system_t *psys = &particle_system(ps);
      assert(!psys->free_it);
      if(PS_IsStopped(gi, ps)) {
        psys->free_it = true;
        un->node->removeChild(psys->ogre_node);
        gi->ogre_scene->getRootSceneNode()->addChild(psys->ogre_node);
        if(ps_prev == TR_null) {
          ps = psys->unext;
          un->ps_head = ps;
        } else {
          ps = psys->unext;
          particle_system(ps_prev).unext = ps;
        }
      } else {
        ps_prev = ps;
        ps = psys->unext;
      }
    }
    /* regen */
    int ideal_hp_regen = HP_REGEN(un)*gi->game_time_step + un->hp_regen_remaining,
        hp_regen = ideal_hp_regen/(1000*100);
    un->hp_regen_remaining = ideal_hp_regen - hp_regen*(1000*100);
    UN_AddHP(gi, u, hp_regen);
    int ideal_mana_regen = MANA_REGEN(un)*gi->game_time_step + un->mana_regen_remaining,
        mana_regen = ideal_mana_regen/(1000*100);
    un->mana_regen_remaining = ideal_mana_regen - mana_regen*(1000*100);
    UN_SetMana(gi, u, un->mana+mana_regen);
    /* stun forces */
    un->dx += un->stun_force_x;
    un->dy += un->stun_force_y;
    un->stun_force_x = 98 * un->stun_force_x / 100;
    un->stun_force_y = 98 * un->stun_force_y / 100;
    /* ANIM TIME */
    un->anim_time += gi->game_time_step;
    if(un->anim_time >= un->anim_span) {
      un->anim_time -= un->anim_span;
      un->ent[un->anim_step]->setVisible(false);
      un->anim_step = un->anim_step+1 >= MESH_COUNT(un) ? 0 : un->anim_step+1;
      un->ent[un->anim_step]->setVisible(true);
    }
    /* various */
    ST_Update(gi, p, u);
    AN_Update(gi, u);
    /* build */
    DO_TIMES(MAX_PLAYER)
      if(un->build_order_head[f] != TR_null) {
        build_order(un->build_order_head[f]).started = true;
        TR_build_order_ptr b = un->build_order_head[f];
        while(b != TR_null) {
          TR_build_order_t *bo = &build_order(b);
          if(bo->started) {
            bo->progress += gi->game_time_step;
            if(bo->progress >= pl->unit_dic[bo->unit_type].build_time) {
              TR_unit_ptr u2 = 
                PL_SpawnUnit(
                  gi, 
                  GI_GetPlayerPtr(gi, bo->unit_player_id), 
                  bo->unit_id,
                  bo->target_cx, 
                  bo->target_cy, 
                  bo->unit_type,
                  bo->user_data_type);
              if(gi->on_spawn_unit_cb)
                gi->on_spawn_unit_cb(gi, u, u2);
              BO_Free(gi, u, b);
            }
          }
          b = bo->next;
        }
      }
    u = un->pnext;
  }
  /* workers */
  PL_StartWorkers(gi, p);
}

void PL_UpdateUnitsPart3(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  TR_unit_ptr u;
  /* workers */
  PL_CloseWorkers(gi, p);
  /* move order */
  u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(un->wait <= 0) {
      switch(UO_GetFrontType(gi, u)) {
        case TR_unit_order_move:
          #if THREADS        
          {
            TR_unit_order_t *uo = UO_GetFront(gi, u);
            if(!uo->started) {
              if(uo->starting) {
                if(uo->started_proxy) {
                  if(uo->found_path_proxy) {
                    uo->started       = true;
                    uo->starting      = false;
                    uo->started_proxy = false;
                    un->chase_cx      = uo->chase_cx_proxy;
                    un->chase_cy      = uo->chase_cy_proxy;
                    un->path_len      = uo->path_len_proxy;
                    assert(un->path_len >= 0);
                    assert(un->path_index == 0);
                    if(un->path_len == 0) {
                      UO_FreeFront(gi, u);
                      assert(UO_GetFrontType(gi, u) 
                      == TR_unit_order_follow_goal);
                      UO_GetFront(gi, u)->last_target_cx = -1;
                      PLAY_UNIT_ANIM(u, "Idle", true)
                      un->anim_span = IDLE_ANIM_SPAN;
                    } else {
                      un->move += gi->game_time_step * MAX(0, MOVE_SPEED(un));
                      un->skill = 0;
                      if(un->ent[0]->hasAnimationState("Walk"))
                        PLAY_UNIT_ANIM(u, "Walk", true)
                      else
                        PLAY_UNIT_ANIM(u, "Idle", true)
                      un->anim_span = WALK_ANIM_SPAN;
                    }
                  } else {
                    UO_FreeFront(gi, u);
                    assert(UO_GetFrontType(gi, u) 
                    == TR_unit_order_follow_goal);
                    UO_GetFront(gi, u)->last_target_cx = -1;
                    un->wait = UNIT_WAIT_TIME;
                    PLAY_UNIT_ANIM(u, "Idle", true)
                    un->anim_span = IDLE_ANIM_SPAN;
                  }
                }
              }
            } else {
              un->move += gi->game_time_step * MAX(0, MOVE_SPEED(un));
              un->skill = 0;
            }
          }
          #endif        
          break;
        case TR_unit_order_skill: {
          un->skill += gi->game_time_step;
        } break;
      }
    }
    u = un->pnext;
  }
}

bool PL_UpdateFollowGoal(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  TR_player_t *pl             = &player(p);
  TR_unit_t *un               = &unit(u);
  TR_unit_order_t *follow_uo  = UO_GetFront(gi, u);
  //DTRACE("PL_UpdateFollowGoal")
  bool result                  = true;
  /* 'un' might be following an enemy 
     while its initial friendly target 
     has stopped but it may be moving 
     again at any moment */
  if(follow_uo->following_friend
  && UN_CheckTargetMove(gi, u)) {
    UO_FreeFront(gi, u);
    result = false;
  } else {
    /* dist from enemy */
    int dist = 0x7fffffff;
    if(follow_uo->target != TR_null
    && pl->team_id != TEAM_ID(follow_uo->target_player_id)
    && follow_uo->generated)
      dist = UN_GetDist(gi, u, follow_uo->target);
    int range,
        skill_id;
    if(follow_uo->skill_here) {
      skill_id = UN_SkillId(gi, u, follow_uo->skill_type);
      assert(skill_id != -1);
      range = gi->get_skill_range_cb(gi, p, u, follow_uo->skill_type);
    }
    /* 'un' is not its own target and   
        target is alive (else it stops) */
    if(((follow_uo->target == TR_null
    && !follow_uo->done)
    || (follow_uo->target != TR_null
    && (u != follow_uo->target || (follow_uo->skill_here && !gi->skill_looks_for_enemy_cb(gi, follow_uo->skill_type)))
    && PL_IsUnitAlive(gi, GI_GetPlayerPtr(gi, follow_uo->target_player_id), follow_uo->target_cmd_id)
    && PL_IsInFov(gi, p, follow_uo->target)
    && (!follow_uo->generated 
    || !follow_uo->skill_here
    || PL_IsReachable(gi, u, follow_uo->target, range))))
    && (!follow_uo->skill_here
    || gi->get_skill_continue_cb(gi, follow_uo->skill_type)
    || un->skills_cooldown[skill_id] >= gi->get_skill_cooldown_cb(gi, p, u, follow_uo->skill_type))
    && (!follow_uo->skill_here
    || gi->can_cast_skill_cb(
        gi, p, u, follow_uo->skill_type, follow_uo->target, 
        follow_uo->target_cx, follow_uo->target_cy, false) == "")) {
      //DTRACE("bahla")
      TR_unit_t *target_un = null;
      if(follow_uo->target != TR_null) { 
        target_un = &unit(follow_uo->target);
        follow_uo->target_cx  = 
        follow_uo->click_cx   = target_un->x+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un)),
        follow_uo->target_cy  = 
        follow_uo->click_cy   = target_un->y+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un));
      }
      bool is_enemy = follow_uo->target != TR_null
                   && (pl->team_id != TEAM_ID(follow_uo->target_player_id));
      bool skill_it = follow_uo->skill_here
                   && ((follow_uo->target != TR_null
                   && UN_GetDist(gi, u, follow_uo->target) 
                   <= (range<<POS_SHIFT)
                   && PL_IsInFov(gi, p, follow_uo->target))
                   || (follow_uo->target == TR_null
                   && !gi->skill_looks_for_unit_cb(gi, follow_uo->skill_type)
                   && UN_GetDist(gi, u, follow_uo->target_cx, follow_uo->target_cy) 
                   <= (range<<POS_SHIFT)));
      if(follow_uo->target == TR_null
      || skill_it
      || is_enemy
      || follow_uo->last_target_cx != follow_uo->target_cx
      || follow_uo->last_target_cy != follow_uo->target_cy) {
        //DTRACE("goal: here")
        if((un->silenced == 0 || !skill_it)
        && (un->rooted == 0 || skill_it)
        && (un->uncontrolable == 0 || follow_uo->generated)
        && un->stun == 0) {
          TR_unit_order_ptr o         = UO_PushFront(gi, u);
          TR_unit_order_t  *uo        = &order(o);
          if(skill_it
          && CAN_BE_PUSHED(un)
          && follow_uo->target != TR_null) {
            skill_it = UN_GetDist(gi, u, follow_uo->target) <=
              MAX(1, range 
              - (UO_GetFrontType(gi, follow_uo->target) == TR_unit_order_move 
              && (target_un->x-un->x)*target_un->ddx + (target_un->y-un->y)*target_un->ddy > 0 ?
               ((MOVE_SPEED(target_un)/10 * (ATTACK_SPEED(un)>>2))>>POS_SHIFT)+1 : 0))<<POS_SHIFT;
          }
          //DTRACEI(skill_it)
          uo->order                   = skill_it ? TR_unit_order_skill : TR_unit_order_move;
          uo->target                  = follow_uo->target;
          uo->target_player_id        = follow_uo->target_player_id;
          uo->target_cmd_id           = follow_uo->target_cmd_id;
          uo->target_cx               = follow_uo->target_cx;
          uo->target_cy               = follow_uo->target_cy;
          uo->click_cx                = follow_uo->click_cx;
          uo->click_cy                = follow_uo->click_cy;
          uo->generated               = follow_uo->generated;
          uo->cmd_mask                = follow_uo->cmd_mask;
          if(follow_uo->target != TR_null) {
            if(is_enemy) 
              uo->r_squared = 0;
            else
              uo->r_squared = MAX(FOLLOW_R_SQUARED, SIZE(target_un) * SIZE(target_un));
          } else {
            if(follow_uo->skill_here)
              uo->r_squared = MAX(SIZE(un)*SIZE(un), MIN(follow_uo->r_squared, FOLLOW_R_SQUARED));
            else
              uo->r_squared = SIZE(un)*SIZE(un);//FOLLOW_R_SQUARED;//MAX(SIZE(un)*SIZE(un), follow_uo->r_squared);
          }
          uo->d_squared               = skill_it ? range * range : 0;
          unsigned int seed           = u;
          int dx = (un->x - uo->target_cx)>>POS_SHIFT,
              dy = (un->y - uo->target_cy)>>POS_SHIFT,
              dist = TR_ISqrt(dx * dx + dy * dy);
          uo->counter                 = 200 + (TR_Rand(&seed)&127);//dist < (MAX_FORMATION_MOVE>>POS_SHIFT) ? 50 : 200 + (TR_Rand(&seed)&127);
          uo->with_counter            = true;
          uo->precise_target          = follow_uo->precise_target;
          uo->skill_here              = follow_uo->skill_here;
          uo->skill_type              = follow_uo->skill_type;
          uo->following_friend        = follow_uo->following_friend;
          uo->forced_action           = follow_uo->forced_action;
          uo->forced_action_mark      = follow_uo->forced_action_mark;
          uo->forced_action_unit      = follow_uo->forced_action_unit;
          uo->forced_action_unit_cmd_id    = follow_uo->forced_action_unit_cmd_id;
          uo->forced_action_unit_player_id = follow_uo->forced_action_unit_player_id;
          follow_uo->done             = false;
          follow_uo->last_target_cx   = follow_uo->target_cx;
          follow_uo->last_target_cy   = follow_uo->target_cy;
          result = false; /* done? */
        } else {
          //UO_FreeFront(gi, u);
          un->wait = UNIT_WAIT_TIME;
          /* ultimately that should be false here 
             but I've spend too much on the loop of hell */
          result = true; /* done? */
        }
      } else {
        /* if 'un' has reached it's friendly target, it
           might now found an enemy target to shoot at */
        if(follow_uo->done
        && !is_enemy
        && gi->default_skill_type_cb(gi, u, TR_null) != -1)
          result = 
            !PL_GenFollowUnit(
              gi, 
              p, 
              u, 
              follow_uo->cmd_mask, 
              gi->default_skill_type_cb(gi, u, TR_null) /* attack */,
              true);
      }
    } else {
      /* 'un' has reached its goal
         or un' is following itself
         or its target is dead */
      UO_FreeFront(
        gi, u,
        follow_uo->done); /* completed? */
      result = false; /* done? */
    }
  }
  return result; /* done? */
}

bool PL_StartMoveOrder(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  DTRACE("PL_StartMoveOrder");
  TR_player_t *pl = &player(p);
  if(pl->free_worker_count > 0) {
    TR_worker_ptr w         = pl->free_workers[pl->current_worker];
    assert(pl->free_worker_count > 0);
    pl->current_worker       = (pl->current_worker+1)%pl->free_worker_count;
    TR_worker_t *wo         = &worker(w);
    if(wo->task_count < MAX_WORKER_TASK) {
      TR_unit_order_t *uo   = UO_GetFront(gi, u);
      TR_unit_t *un         = &unit(u),
                 *target_un  = uo->target != TR_null ? &unit(uo->target) : null;
      /* no reset of the blocked time for some reasons :) */
      un->path_len          = 0;
      un->path_index        = 0;
      wo->task_cost         += TR_ISqrt(((un->x>>POS_SHIFT) - (uo->target_cx>>POS_SHIFT)) * ((un->x>>POS_SHIFT) - (uo->target_cx>>POS_SHIFT))
                                       + ((un->y>>POS_SHIFT) - (uo->target_cy>>POS_SHIFT)) * ((un->y>>POS_SHIFT) - (uo->target_cy>>POS_SHIFT)))
                              * SIZE(un)*SIZE(un)/(4*4);
      TR_worker_task_t *ta = &gi->worker_tasks[p][w][wo->task_count++];
      ta->u                 = u;
      ta->pos_x             = un->x;
      ta->pos_y             = un->y;
      ta->target_cx         = uo->target_cx;
      ta->target_cy         = uo->target_cy;
      ta->click_cx          = uo->click_cx;
      ta->click_cy          = uo->click_cy;
      ta->skill_here        = uo->skill_here;
      ta->range             = ta->skill_here ? gi->get_skill_range_cb(gi, p, u, uo->skill_type) : 0;
      ta->type              = un->unit_type;
      ta->size              = SIZE(un);
      ta->target_size       = uo->target != TR_null ? SIZE(target_un) : 0;
      ta->flying            = FLYING(un);
      ta->look_for_ground   = ta->skill_here ? gi->skill_look_for_ground_cb(gi, uo->skill_type) : false;
      ta->look_for_air      = ta->skill_here ? gi->skill_look_for_air_cb(gi, uo->skill_type) : false;
      ta->chase_cx_proxy    = &uo->chase_cx_proxy;
      ta->chase_cy_proxy    = &uo->chase_cy_proxy;
      ta->path_len_proxy    = &uo->path_len_proxy;
      ta->task_id_proxy     = &uo->task_id;
      ta->started_proxy     = &uo->started_proxy;
      ta->found_path_proxy  = &uo->found_path_proxy;
      ta->has_target        = uo->target != TR_null;
      ta->look_for_unit     = uo->skill_here 
                           && uo->target == TR_null 
                           && gi->skill_looks_for_unit_cb(gi, uo->skill_type);
      ta->unit_here_mode    = !ta->look_for_unit ? TR_get_unit_here_mode_all :
                              gi->skill_looks_for_enemy_cb(gi, uo->skill_type) ? 
                              TR_get_unit_here_mode_enemies : TR_get_unit_here_mode_friends;
      ta->selected          = SELECTED(u);
      ta->team_id           = un->team_id;
      ta->id = uo->task_id  = gi->task_mark++;
      assert(!uo->started_proxy);
      DTRACEI(ta->u);
      DTRACEI(uo->target);
      DTRACEI(ta->skill_here);
      DTRACEI(ta->range);
      DTRACEI(ta->range);
      DTRACEI(ta->pos_x);
      DTRACEI(ta->pos_y);
      DTRACEI(ta->target_cx);
      DTRACEI(ta->target_cy);
      DTRACEI(ta->click_cx);
      DTRACEI(ta->click_cy);
      return true;
    }
  }
  return false;
}

bool PL_MoveAndSkills(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  bool done = true;
  TR_unit_ptr u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(UO_GetFront(gi, u) 
    && UO_GetFront(gi, u)->started) {
      switch(UO_GetFrontType(gi, u)) {
        case TR_unit_order_move:
          done = PL_MoveUnit(gi, p, u) && done;
          break;
        case TR_unit_order_skill:
          done = PL_UnitSkill(gi, p, u) && done;
          break;
      }
    }
    u = un->pnext;
  }
  return true;
}

bool PL_MoveUnit(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  //DTRACE("PL_MoveUnit")
  TR_player_t *pl     = &player(p);
  TR_unit_t *un       = &unit(u);
  TR_unit_order_t *uo = UO_GetFront(gi, u);
  int px, py, dx, dy, dist;
  if(un->path_len > 0 
  && un->path_index < un->path_len
  && un->move > 0) {
    bool done,
         reduced = false;
    do {
      done = true;
      int *pt = path_point_ptr(u, un->path_index);
      px = pt[0];
      py = pt[1];
      //DTRACE("ici")
      //DTRACEI(un->path_index)
      //DTRACEI(px)
      //DTRACEI(py)
      dx = px - un->x;
      dy = py - un->y;
      while(ABS(dx) > 150 || ABS(dy) > 150) {
        dx >>= 1;
        dy >>= 1;
        reduced = true;
      }
      dist = dx * dx + dy * dy;
      if((px - un->x) == 0 
      && (py - un->y) == 0) {
        un->path_index++;
        done = un->path_index >= un->path_len;
        if(done)
          UO_FreeFront(gi, u, true);
      }
    } while(!done);
    if(dist != 0
    && uo == UO_GetFront(gi, u)) {
      int d   = TR_ISqrt(dist)<<2;
      int ddx = dx * un->move / (d*10),
          ddy = dy * un->move / (d*10);
      bool bob = !reduced && ddx * ddx + ddy * ddy > dist;
      if(bob) {
        un->dx += dx;
        un->dy += dy;
      } else {
        un->dx += ddx;
        un->dy += ddy;
      }
      //DTRACE("la")
      //DTRACEI(u)
      //DTRACEI(un->unit_type)
      //DTRACEI(un->dx)
      //DTRACEI(un->dy)
      if(bob) {
        un->path_index++;
        if(un->path_index >= un->path_len)
          UO_FreeFront(gi, u, true);
      } else
      if(d > (1<<(POS_SHIFT-3)))
        UN_LookAt(gi, u, px, py);
    }
    un->move = 0;
  }
  return true;
}

bool PL_GenFollowUnit(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_id_t cmd_mask, TR_skill_type_t skill_type, bool following_friend, bool free_front) {
  TR_player_t *pl    = &player(p);
  TR_unit_t *un      = &unit(u);
  if(un->stun == 0
  && gi->skill_looks_for_unit_cb(gi, skill_type)) {
    int range = gi->get_skill_range_cb(gi, p, u, skill_type);
    TR_unit_ptr target = 
      PL_GetFirstUnitInRange(
        gi, p, u, MAX(range, AGGRO_RANGE(un)), range,
        gi->skill_looks_for_enemy_cb(gi, skill_type),
        gi->skill_look_for_ground_cb(gi, skill_type),
        gi->skill_look_for_air_cb(gi, skill_type));
    if(target != TR_null)
      return PL_GenFollowTarget(gi, p, u, target, cmd_mask, skill_type, following_friend, free_front);
  }
  return false;
}

bool PL_GenFollowTarget(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_ptr target, TR_id_t cmd_mask, TR_skill_type_t skill_type, bool following_friend, bool free_front) {
  TR_player_t *pl      = &player(p);
  TR_unit_t *un        = &unit(u);
  TR_unit_t *target_un = &unit(target);
  assert(target != TR_null);
  if(gi->can_cast_skill_cb(
      gi, p, u, skill_type, target, 
      unit(target).x, unit(target).y, false) == ""
  && PL_IsUnitAlive(
      gi, 
      GI_GetPlayerPtr(gi, target_un->player_id), 
      target_un->cmd_id)) {
    /* regular attack here need the move freed */
    if(free_front)
      UO_FreeFront(gi, u);
    TR_unit_order_ptr o           = UO_PushFront(gi, u);
    TR_unit_order_t *follow_uo    = &order(o);
    follow_uo->order              = TR_unit_order_follow_goal;
    follow_uo->target             = target;
    follow_uo->target_player_id   = target_un->player_id;
    follow_uo->target_cmd_id      = target_un->cmd_id;
    follow_uo->target_cx          = target_un->x+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un));
    follow_uo->target_cy          = target_un->y+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un));
    follow_uo->cmd_mask           = target;
    follow_uo->r_squared          = 0;
    follow_uo->d_squared          = 0;
    follow_uo->last_target_cx     = -1;
    follow_uo->skill_here         = true;
    follow_uo->skill_type         = skill_type;
    follow_uo->precise_target     = false;
    follow_uo->following_friend   = following_friend;
    return true;
  }
  return false;
}

bool PL_GenForcedAction(TR_game_instance_t *gi, TR_unit_ptr u, bool skill_here, TR_skill_type_t skill_type, TR_id_t cmd_mask, int x, int y, TR_unit_ptr target, TR_unit_ptr forced_action_unit) {
  TR_unit_t *un = &unit(u);
  if(UO_GetFrontType(gi, u) == TR_unit_order_none 
  || !UO_GetFront(gi, u)->forced_action) {
    UO_FreeAll(gi, u);
    un->skill         = 0;
    un->wait          = 0;
    un->blocked_time  = 0;
    TR_unit_order_ptr o = UO_PushFront(gi, u);
    TR_unit_order_t *uo              = &order(o);
    uo->generated                     = false;
    TR_unit_t *fa_un                 = &unit(forced_action_unit);
    uo->forced_action                 = true;
    uo->forced_action_mark            = fa_un->forced_action_marker;
    uo->forced_action_unit            = forced_action_unit;
    uo->forced_action_unit_cmd_id     = fa_un->cmd_id;
    uo->forced_action_unit_player_id  = fa_un->player_id;
    if(target == TR_null) {
      uo->order             = TR_unit_order_follow_goal;
      uo->target            = TR_null;
      uo->target_cx         = x;
      uo->target_cy         = y;
      uo->click_cx          = x;
      uo->click_cy          = y;
      uo->cmd_mask          = cmd_mask;
      uo->r_squared         = 0;
      uo->d_squared         = 0;
      uo->skill_here        = skill_here;
      uo->skill_type        = skill_type;
      uo->precise_target    = true;
    } else {
      uo->order             = TR_unit_order_follow_goal;
      uo->target            = target;
      uo->target_player_id  = unit(target).player_id;
      uo->target_cmd_id     = unit(target).cmd_id;
      uo->target_cx         = x;
      uo->target_cy         = y;
      uo->click_cx          = x;
      uo->click_cy          = y;
      uo->cmd_mask          = target;
      uo->r_squared         = 0;
      uo->d_squared         = 0;
      uo->last_target_cx    = -1;
      uo->skill_here        = skill_here;
      uo->skill_type        = skill_type;
      uo->precise_target    = true;
    }
  }
}

TR_unit_ptr PL_GetFirstUnitInRange(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, int range, int skill_range, bool look_for_enemy, bool look_for_ground, bool look_for_air) {
  /* 'un' is 'skilling here' here */
  static TR_unit_ptr units[MAX_UNIT];
  int                 unit_count = 0;
  TR_player_t *pl         = &player(p);
  TR_unit_t   *un         = &unit(u);
  if(un->blind == 0) {
    TR_get_unit_here_t data;
    UN_GetUnitsHere(
      /* +1 so it's odd so got a good real socle */
      &data, gi, u, un->team_id, (range<<1)+1, UNIT_X(un), UNIT_Y(un), 
      look_for_ground, look_for_air, 
      look_for_enemy ? TR_get_unit_here_mode_enemies : 
      TR_get_unit_here_mode_friends);
    DO_TIMES(data.count) {
      TR_unit_ptr u2 = data.result[f];
      if(PL_IsInFov(gi, p, u2)) {
        int dist = UN_GetDist(gi, u, u2);
        if(dist <= (range<<POS_SHIFT)
        && PL_IsReachable(gi, u, u2, skill_range))
          units[unit_count++] = u2;
      }
    }
    return gi->get_unit_in_range_cb(gi, p, u, units, unit_count, skill_range);
  }
  return TR_null;
}

bool PL_UnitSkill(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u) {
  /* I would have prefer that it drop each time it has casted because 
     we perform the same test as "goal" and "part1" but
     there is an animation glitch (attack here) which seem to be removable
     only if the unit skills continuously */
  //DTRACE("PL_UnitSkill")
  TR_player_t *pl         = &player(p);
  TR_unit_t   *un         = &unit(u);
  TR_unit_order_t *uo     = UO_GetFront(gi, u);
  TR_unit_t *target_un    = null;
  TR_player_ptr target_p  = TR_null;
  int skill_id = UN_SkillId(gi, u, uo->skill_type);
  assert(skill_id != -1);
  if(uo->target != TR_null) {
    target_un  = &unit(uo->target);
    target_p   = GI_GetPlayerPtr(gi, uo->target_player_id);  
  }
  if(CAN_BE_PUSHED(un)
  && !gi->get_skill_direct_cast_cb(gi, uo->skill_type))
    if(uo->target != TR_null)
      UN_LookAt(gi, u, UNIT_X(target_un), UNIT_Y(target_un));
    else
      UN_LookAt(gi, u, uo->target_cx, uo->target_cy);
  //DTRACEI(un->unit_type)
  //DTRACEI(uo->skill_type)
  //DTRACEI(un->skill_is_cooling_down[skill_id])
  //DTRACEI(uo->target_cmd_id)
  //DTRACEI(un->skill)
  int skill_cast_speed = gi->get_skill_cast_speed_cb(gi, p, u, uo->skill_type);
  int range = gi->get_skill_range_cb(gi, p, u, uo->skill_type);
  if((uo->target == TR_null
  || PL_IsUnitAlive(gi, target_p, uo->target_cmd_id))
  && gi->can_cast_skill_cb(gi, p, u, uo->skill_type, uo->target, uo->target_cx, uo->target_cy, true) == "") {
    if(!un->skill_is_cooling_down[skill_id]
    || un->skills_cooldown[skill_id] >= gi->get_skill_cooldown_cb(gi, p, u, uo->skill_type)) {
      un->skill_is_cooling_down[skill_id] = false;
      bool do_continue = gi->cast_skill_cb(gi, p, u, un->order_head, uo->target_cx, uo->target_cy);
      //DTRACEI(do_continue)
      if(un->skill >= skill_cast_speed) {
        un->skill_marks[skill_id]++;
        un->skill_state = 0;
        un->skill_state2 = 0;
        /* 1 here is because 0 is reserved for idle animation */
        un->skill = MAX(1, un->skill - skill_cast_speed);
        /* if 'target' is doing nothing 
           and is targeted by an enemy
           it attacks */
        if(uo->target != TR_null
        && un->team_id != target_un->team_id)
          UN_PropagateAttack(gi, uo->target, u);
        if(do_continue)
          un->skill_is_cooling_down[skill_id] = true;
      }
      if(!do_continue) {
        UO_FreeFront(gi, u, true);
        assert(UO_GetFrontType(gi, u)
        == TR_unit_order_follow_goal);
        UO_FreeFront(gi, u, true);
        /* must be after free front */
        un->skill_marks[skill_id]++;
      } else
      if(uo->target != TR_null
      && (!PL_IsInFov(gi, p, uo->target)
      || UN_GetDist(gi, u, uo->target) 
       > (range<<POS_SHIFT))) {
        UO_FreeFront(gi, u);
        PLAY_UNIT_ANIM(u, "Idle", true)
        /* must be after free front */
        un->skill_marks[skill_id]++;
        un->anim_span = IDLE_ANIM_SPAN;
      }
    } else {
      un->skill = 0;
      if(uo->target == TR_null
      || (uo->target != TR_null
      && (!PL_IsInFov(gi, p, uo->target)
      || UN_GetDist(gi, u, uo->target) 
       > (range<<POS_SHIFT)))) {
        UO_FreeFront(gi, u);
        PLAY_UNIT_ANIM(u, "Idle", true)
        /* must be after free front */
        un->skill_marks[skill_id]++;
        un->anim_span = IDLE_ANIM_SPAN;
      }
    }
  } else {
    UO_FreeFront(gi, u); 
    PLAY_UNIT_ANIM(u, "Idle", true)
    /* must be after free front */
    un->skill_marks[skill_id]++;
    un->anim_span = IDLE_ANIM_SPAN;
  }
  return true; /* done? */
}

void PL_InitWorkers(TR_game_instance_t *gi, TR_player_ptr p) {
  TR_player_t *pl = &player(p);
  #if THREADS  
  pl->free_worker_count = 0;
  pl->current_worker = 0;
  DO_TIMES(MAX_WORKER) {
    TR_worker_t *wo  = &worker(f);
    wo->gi            = gi;
    wo->p             = p;
    wo->w             = f;
    wo->task_count    = 0;
    wo->task_cost     = 0;
    wo->handle        = null;
    pl->free_workers[pl->free_worker_count++] = f;
    /* I don't understand why it's not working,
       it's not like if it's witchcraft,
       oh but it is... */
    //break;
  }
  #endif    
}

void PL_StartWorkers(TR_game_instance_t *gi, TR_player_ptr p) {
  #if THREADS  
  TR_player_t *pl   = &player(p);
  DO_TIMES(pl->free_worker_count) {
    TR_worker_ptr w   = pl->free_workers[f];
    TR_worker_t *wo   = &worker(w);
    if(wo->task_count > 0) {
      wo->handle        = CreateThread(null, 0, PL_WorkerFun, (void*)wo, 0, &wo->id); 
      assert(wo->handle);
    }
  }
  #endif    
}

void PL_CloseWorkers(TR_game_instance_t *gi, TR_player_ptr p) {
  #if THREADS  
  TR_player_t *pl   = &player(p);
  DO_TIMES(pl->free_worker_count) {
    TR_worker_ptr w = pl->free_workers[f];
    TR_worker_t *wo = &worker(w);
    if(wo->task_count > 0)
      PL_CloseWorker(gi, p, w);
  }
  #endif    
}

void PL_CloseWorker(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w) {
  #if THREADS
  TR_player_t *pl = &player(p);
  TR_worker_t *wo = &worker(w);
  DWORD r = WaitForSingleObject(wo->handle, 10000);
  assert(r!=WAIT_FAILED);
  CloseHandle(wo->handle);
  DO_TIMES(wo->task_count) {
    TR_worker_task_t *ta = &gi->worker_tasks[p][w][f];
    /* meanwhile, the task may have been cancelled */
    if(ta->id == *ta->task_id_proxy) {
      assert(UO_GetFrontType(gi, ta->u) 
      == TR_unit_order_move
      && UO_GetFront(gi, ta->u)->starting);
      *ta->chase_cx_proxy   = ta->chase_cx;
      *ta->chase_cy_proxy   = ta->chase_cy;
      *ta->path_len_proxy   = ta->path_len;
      *ta->found_path_proxy = ta->found_path;
      *ta->started_proxy    = true;
      for(int i = 0; i < ta->path_len; i++) {
        int *path_pt_worker_ptr = path_point_worker_ptr(i);
        int *path_pt_ptr = path_point_ptr(ta->u, i);
        path_pt_ptr[0] = path_pt_worker_ptr[0];
        path_pt_ptr[1] = path_pt_worker_ptr[1];
      }
    }
  }
  #endif  
}

DWORD WINAPI PL_WorkerFun(void *params) {
  #if THREADS  
  TR_worker_t *wo               = (TR_worker_t*)params;
  TR_worker_ptr w               = wo->w;
  TR_game_instance_t *gi        = wo->gi;
  TR_player_ptr p               = wo->p;
  wo->callback_data.start_mark   = gi->close_map_mark3[p][w];
  int i = 0,
      close_mark = 0;
  do {
    TR_worker_task_t *ta = &gi->worker_tasks[p][w][i++];
    /* follow things */
    wo->callback_data.follow_mode = false;
    wo->callback_data.can_follow  = !ta->flying;
    int *close_map   = ta->flying ? gi->flying_close_map[ta->size-1] : gi->close_map[ta->size-1];
    /* continue */
    int orig_x = ta->pos_x + UN_GetSizeShift(gi, ta->size),
        orig_y = ta->pos_y + UN_GetSizeShift(gi, ta->size),
        oi     = (orig_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
        oj     = (orig_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
        dest_x = ta->target_cx + UN_GetSizeShift(gi, ta->size),
        dest_y = ta->target_cy + UN_GetSizeShift(gi, ta->size),
        di     = (dest_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
        dj     = (dest_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
    PL_SnapPoint(gi, ta->size, orig_x, orig_y, oi, oj, di, dj, &ta->pos_x, &ta->pos_y, false, ta->flying, ta->skill_here);
    orig_x     = ta->pos_x + UN_GetSizeShift(gi, ta->size);
    orig_y     = ta->pos_y + UN_GetSizeShift(gi, ta->size);
    oi         = (orig_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
    oj         = (orig_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
    /* if we look for a path from 0xffffffff to 0xffffffff (close_map speaking), 
       then it may scan all the map and we don't want that */
    assert(ON_CLOSE_MAP(oi, oj) != 0xffffffff);
    int fx         = ta->target_cx + UN_GetSizeShift(gi, ta->size),
        fy         = ta->target_cy + UN_GetSizeShift(gi, ta->size),
        dx         = ta->target_cx, 
        dy         = ta->target_cy;
    PL_SnapPoint(gi, ta->size, orig_x, orig_y, oi, oj, di, dj, &dx, &dy, ta->skill_here, ta->flying, ta->skill_here);
    dest_x     = dx + UN_GetSizeShift(gi, ta->size),
    dest_y     = dy + UN_GetSizeShift(gi, ta->size),
    di         = (dest_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
    dj         = (dest_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
    ta->chase_cx = dx;
    ta->chase_cy = dy;
    bool pathfind = PL_ASTAR_DoesCollide(gi, p, w, ta->size, orig_x, orig_y, dest_x, dest_y, ta->flying);
    if(pathfind) {
      /* we don't want 'un' to keep its formation 
         for the follow_mode optimization
         since their is a pathfinding */
      if(!ta->skill_here) {
        dx = ta->click_cx;
        dy = ta->click_cy;
        PL_SnapPoint(gi, ta->size, orig_x, orig_y, oi, oj, di, dj, &dx, &dy, false /* ta->skill_here */, ta->flying, false /* ta->skill_here */);
        ta->chase_cx = dx;
        ta->chase_cy = dy;
      } 
      int ddx   = ta->target_cx - dx,
          ddy   = ta->target_cy - dy,
          dist  = ddx * ddx + ddy * ddy,
          size_diff;
      if(dist < 0 || ddx > MAX_DIST || ddy > MAX_DIST)
        dist = 0x7fffffff;
      dist = TR_ISqrt(dist);
      if(ta->has_target) {
        size_diff = (ta->size+ta->target_size)<<(POS_SHIFT-1);
        dist = MAX(0, dist-size_diff);
      }
      ta->found_path = false;
      if(!ta->skill_here
      || !ta->has_target
      || dist <= ta->range<<POS_SHIFT) {
        orig_x       = ta->pos_x + UN_GetSizeShift(gi, ta->size);
        orig_y       = ta->pos_y + UN_GetSizeShift(gi, ta->size);
        oi           = (orig_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
        oj           = (orig_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT; 
        dest_x       = dx + UN_GetSizeShift(gi, ta->size);
        dest_y       = dy + UN_GetSizeShift(gi, ta->size);
        di           = (dest_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
        dj           = (dest_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
        if(ON_CLOSE_MAP(di, dj) == ON_CLOSE_MAP(oi, oj)) {
          /* get units near goal */
          TR_get_unit_here_t data;
          if(ta->look_for_unit)
            UN_GetUnitsHere(&data, gi, ta->u, ta->team_id, ta->range<<1, dest_x, dest_y, ta->look_for_ground, ta->look_for_air, ta->unit_here_mode);
          /* follow things */
          wo->close_mark++;
          close_map = gi->close_map3[p][w][ta->size-1];
          if(ON_CLOSE_MAP(di, dj) < wo->callback_data.start_mark)
            ON_CLOSE_MAP(di, dj) = gi->close_map_mark3[p][w]++;
          wo->callback_data.mark = ON_CLOSE_MAP(di, dj);
          /* continue */
          int gx = (orig_x-dest_x)>>(POS_SHIFT-3),
              gy = (orig_y-dest_y)>>(POS_SHIFT-3);
          bool fast = TR_ISqrt(gx*gx + gy*gy) > MAX_FORMATION_MOVE>>(POS_SHIFT-3);
          TR_astar_node_ptr free_head = 0;
          TR_astar_node_ptr open_list = TR_null;
          TR_astar_node_ptr n = free_head++;
          PL_ASTAR_InitNode(gi, p, w, n, TR_null, dest_x, dest_y, orig_x, orig_y, oi, oj, false, fast);
          PL_ASTAR_InsertSortedNode(gi, p, w, n, &open_list);
          int c = 0;
          wo->path_len = 0;
          while(!ta->found_path
             && open_list != TR_null) {
            TR_astar_node_ptr n = PL_ASTAR_PopNode(gi, p, w, &open_list);
            TR_astar_node_t *no = &astar_node(n);
            if(ta->skill_here
            && ta->has_target) {
              int ddx = no->x-fx,
                  ddy = no->y-fy;
              dist = ddx*ddx + ddy*ddy;
              if(dist < 0 || ddx > MAX_DIST || ddy > MAX_DIST)
                dist = 0x7fffffff;
              dist = TR_ISqrt(dist);
              dist = MAX(0, dist-size_diff);
            }
            bool at_target_range = ta->skill_here
                                && ta->has_target
                                && dist <= (ta->range<<POS_SHIFT),
                 found_unit_at_range = false;
            if(ta->look_for_unit) {
              DO_TIMES(data.count) {
                TR_unit_ptr u = data.result[f];
                TR_unit_t *un = &unit(u);
                int ddx = UNIT_X(un) - no->x,
                    ddy = UNIT_Y(un) - no->y;
                dist = ddx*ddx + ddy*ddy;
                if(dist < 0 || ddx > MAX_DIST || ddy > MAX_DIST)
                  dist = 0x7fffffff;
                dist = TR_ISqrt(dist);
                size_diff = (ta->size+SIZE(un))<<(POS_SHIFT-1);
                dist = MAX(0, dist-size_diff);
                if(dist <= ta->range<<POS_SHIFT) {
                  found_unit_at_range = true;
                  _finished = 1;  
                }
              }
            }
            if((no->x == dest_x
            && no->y == dest_y)
            || at_target_range
            || found_unit_at_range) {
              PL_ASTAR_MakePath(gi, p, w, n, ta);
              ta->found_path = true;
              if(at_target_range
              || found_unit_at_range)
                wo->callback_data.can_follow = false;
              //if(ta->selected) {
              //  PL_UpdateMarchTex(gi, p, w, ta->pos_x, ta->pos_y, dx, dy, wo->pool, n, true);
              //}
            } else {
              c++;
              PL_ASTAR_ExpandNode(
                gi, p, w, &free_head, n, 
                &open_list, ta->size, 
                dest_x, dest_y,
                di, dj, fast, ta->flying);
            }
          }
          assert(ta->found_path);
          /* follow things */
          if(wo->callback_data.can_follow) {
            close_map = gi->close_map3[p][w][ta->size-1];
            ON_CLOSE_MAP(oi, oj) = wo->callback_data.mark;
            ON_CLOSE_MAP(di, dj) = wo->callback_data.mark;
            DO_TIMES(wo->path_len)
              ON_CLOSE_MAP(wo->path_i[f], wo->path_j[f]) = wo->callback_data.mark;
          }
          /* continue */
          ta->path_len = 0;
          int k = wo->path_len-1;
          while(ta->path_len <= 2
             && k > 0) {
            int pos_x = wo->path_x[k],
                pos_y = wo->path_y[k];
            bool done = false;
            for(int f = 0 ; !done && f < k; f++) {
              if(f == k-1
              || !PL_ASTAR_DoesCollide(
                    gi, p, w, ta->size, 
                    wo->path_x[f],
                    wo->path_y[f],
                    pos_x, 
                    pos_y,
                    ta->flying)) {
                int *path_pt_ptr = path_point_worker_ptr(ta->path_len);
                path_pt_ptr[0] = wo->path_x[f] - UN_GetSizeShift(gi, ta->size);
                path_pt_ptr[1] = wo->path_y[f] - UN_GetSizeShift(gi, ta->size);
                ta->path_len++;
                k = f;
                done = true;
              }
            }
          }
        }
      }
    } else {
      int ddx   = ta->target_cx - dx,
          ddy   = ta->target_cy - dy,
          dist  = ddx * ddx + ddy * ddy;
      if(dist < 0 || ddx > MAX_DIST || ddy > MAX_DIST)
        dist = 0x7fffffff;
      dist = TR_ISqrt(dist);
      if(ta->has_target) {
        int size_diff = (ta->size+ta->target_size)<<(POS_SHIFT-1);
        dist = MAX(0, dist-size_diff);
      }
      ta->found_path = false;
      if(!ta->skill_here
      || !ta->has_target
      || dist <= ta->range<<POS_SHIFT) {
        ta->found_path = true;
        ta->path_len = 0;
        int *path_pt_ptr = path_point_worker_ptr(ta->path_len);
        path_pt_ptr[0] = dest_x - UN_GetSizeShift(gi, ta->size);
        path_pt_ptr[1] = dest_y - UN_GetSizeShift(gi, ta->size);
        ta->path_len++;
      }
      //if(ta->selected)
      //  PL_UpdateMarchTex(gi, p, w, ta->pos_x, ta->pos_y, dx, dy, wo->pool, TR_null, true);
    }
  } while(i < wo->task_count);
  #endif  
}

bool PL_ASTAR_DoesCollide(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, int size, int x0, int y0, int x1, int y1, bool flying) {
  TR_worker_t *wo = &worker(w);
  /* I don't understand why it's not working. It's not like it's witchcraft. 
     Oh.. but it is. */
  /*int rx        = x1-x0,
      ry        = y1-y0,
      ray_dir_x = rx,
      ray_dir_y = ry,
      c         = 1,
      half_size = size<<(POS_SHIFT-1);
  while(ABS(ABS(ray_dir_x)-ABS(ray_dir_y)) > 50
     || ABS(ray_dir_x) > 150 
     || ABS(ray_dir_y) > 150) {
    ray_dir_x >>= 1;
    ray_dir_y >>= 1;
    c <<= 1;
  }
  int start_x       = x0>>POS_SHIFT,
      start_y       = y0>>POS_SHIFT,  
      end_x         = x1>>POS_SHIFT,
      end_y         = y1>>POS_SHIFT,  
      x             = start_x,
      y             = start_y,
      side_size     = 1<<POS_SHIFT,
      slope_x       = ray_dir_x == 0 ? 0 : side_size * ray_dir_y / ray_dir_x,
      slope_y       = ray_dir_y == 0 ? 0 : side_size * ray_dir_x / ray_dir_y;
  int delta_dist_x  = TR_ISqrt(side_size * side_size + slope_x * slope_x),
      delta_dist_y  = TR_ISqrt(side_size * side_size + slope_y * slope_y),
      d             = TR_ISqrt(ray_dir_x*ray_dir_x + ray_dir_y*ray_dir_y),
      dc            = d*c,
      side_dist_x, side_dist_y, step_x, step_y;
  if(ray_dir_x < 0) {
    step_x = -1;
    side_dist_x = ((x0 - (x<<POS_SHIFT)) * delta_dist_x)>>POS_SHIFT;
  } else {
    step_x = 1;
    side_dist_x = ((side_size + (x<<POS_SHIFT) - x0) * delta_dist_x)>>POS_SHIFT;
  }
  if(ray_dir_y < 0) {
    step_y = -1;
    side_dist_y = ((y0 - (y<<POS_SHIFT)) * delta_dist_y)>>POS_SHIFT;
  } else {
    step_y = 1;
    side_dist_y = ((side_size + (y<<POS_SHIFT) - y0) * delta_dist_y)>>POS_SHIFT;
  }
  while((end_x - x) * step_x >= 0
     && (end_y - y) * step_y >= 0) {
    if(ray_dir_x == 0) {
      side_dist_y += delta_dist_y;
      y += step_y;
    } else
    if(ray_dir_y == 0) {
      side_dist_x += delta_dist_x;
      x += step_x;
    } else
    if(side_dist_x == side_dist_y) {
      side_dist_x += delta_dist_x;
      x += step_x;
      side_dist_y += delta_dist_y;
      y += step_y;
    } else
    if(side_dist_x < side_dist_y) {
      side_dist_x += delta_dist_x;
      x += step_x;
    } else {
      side_dist_y += delta_dist_y;
      y += step_y; 
    }
    int dist = ray_dir_x == 0 ? side_dist_y : 
               ray_dir_y == 0 ? side_dist_x : 
               side_dist_x < side_dist_y ? side_dist_x : side_dist_y;
    if(dist <= dc) {
      int xv = x0 + ray_dir_x * dist / dc * c,
          yv = y0 + ray_dir_y * dist / dc * c,
          xa = (xv-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
          ya = (yv-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
      int v0 = AT2(wo->clearance_map, xa,   ya),
          v1 = AT2(wo->clearance_map, xa+1, ya),
          v2 = AT2(wo->clearance_map, xa,   ya+1),
          v3 = AT2(wo->clearance_map, xa+1, ya+1),
          s  = xv - ((xa<<POS_SHIFT)+(1<<(POS_SHIFT-1))),
          t  = yv - ((ya<<POS_SHIFT)+(1<<(POS_SHIFT-1))),
          s0 = ((side_size-s)*v0 + s*v1)>>POS_SHIFT,
          s1 = ((side_size-s)*v2 + s*v3)>>POS_SHIFT,
          v  = ((side_size-t)*s0 + t*s1)>>POS_SHIFT;
      if(v < half_size - (1<<(POS_SHIFT-5)))
        return true;
    }
  }
  return false;*/
  int x000      = x0>>(POS_SHIFT-3),
      y000      = y0>>(POS_SHIFT-3),
      x00       = x000,
      y00       = y000,
      x11       = x1>>(POS_SHIFT-3),
      y11       = y1>>(POS_SHIFT-3),
      half_size = size<<(POS_SHIFT-1),
      side_size = 1<<POS_SHIFT,
      ddx       = ABS(x11-x00), sx = x00<x11 ? 1 : -1,
      ddy       = ABS(y11-y00), sy = y00<y11 ? 1 : -1,
      err       = (ddx>ddy ? ddx : -ddy)/2, e2;
  int *clearance_map = flying ? gi->flying_clearance_map : gi->clearance_map;
  while(x00 != x11 || y00 != y11) {
    int e2 = err;
    if(e2 >-ddx) { err -= ddy; x00 += sx; }
    if(e2 < ddy) { err += ddx; y00 += sy; }
    if((ABS(x00-x000) > 2
    ||  ABS(y00-y000) > 2)
    && (ABS(x00-x11) > 2
    ||  ABS(y00-y11) > 2)) {
      int xv = (x00<<(POS_SHIFT-3)),
          yv = (y00<<(POS_SHIFT-3)),
          xa = (xv-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
          ya = (yv-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
          v0 = AT2(clearance_map, xa,   ya),
          v1 = AT2(clearance_map, xa+1, ya),
          v2 = AT2(clearance_map, xa,   ya+1),
          v3 = AT2(clearance_map, xa+1, ya+1),
          s  = xv - ((xa<<POS_SHIFT)+(1<<(POS_SHIFT-1))),
          t  = yv - ((ya<<POS_SHIFT)+(1<<(POS_SHIFT-1))),
          s0 = (((side_size)-s)*v0 + s*v1)>>POS_SHIFT,
          s1 = (((side_size)-s)*v2 + s*v3)>>POS_SHIFT,
          v  = (((side_size)-t)*s0 + t*s1)>>POS_SHIFT;
      if(v < half_size)
        return true;
    }
  }
  return false;
}

void PL_ASTAR_MakePath(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr node, TR_worker_task_t *ta) {
  TR_worker_t *wo = &worker(w);
  while(node != TR_null) {
    TR_astar_node_t *no = &astar_node(node);
    wo->path_x[wo->path_len] = no->x;
    wo->path_y[wo->path_len] = no->y;
    wo->path_i[wo->path_len] = no->i;
    wo->path_j[wo->path_len] = no->j;
    wo->path_len++;
    node = no->parent;  
  }
}

void PL_ASTAR_InitNode(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr node, TR_astar_node_ptr parent, int dest_x, int dest_y, int x, int y, int i, int j, bool r, bool fast) {
  TR_worker_t *wo = &worker(w);
  TR_astar_node_t *no = &astar_node(node);
  no->x       = x;
  no->y       = y;
  no->i       = i;
  no->j       = j;
  no->parent  = parent;
  int dx, dy;
  TR_astar_node_t *pa = null;
  if(parent != TR_null) {
    pa = &astar_node(parent);
    dx = (no->x - pa->x)>>(POS_SHIFT-6);
    dy = (no->y - pa->y)>>(POS_SHIFT-6);
  }
  no->k       = parent == TR_null ? 0 : pa->k + 1;
  fast        = no->k > 6;
  int d       = dx * dx + dy * dy;
  no->g       = parent == TR_null ? 0 : pa->g + (fast ? d : TR_ISqrt(d));
  dx          = (no->x - dest_x)>>(POS_SHIFT-6);
  dy          = (no->y - dest_y)>>(POS_SHIFT-6);
  d           = dx * dx + dy * dy;
  no->h       = (fast ? d : TR_ISqrt(d));
  no->f       = no->g + no->h;
  no->r       = r;
  no->next    = TR_null;
}

void PL_ASTAR_InsertSortedNode(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr node, TR_astar_node_ptr *h) {
  TR_worker_t *wo = &worker(w);
  int i = astar_node(node).i,
      j = astar_node(node).j;
  AT2(wo->close_map, i, j) = wo->close_mark;
  AT2(wo->f_map, i, j) = astar_node(node).f;
  astar_node(node).next = TR_null;
  if(astar_node(node).f != 0x7fffffff) {
    if(*h == TR_null)
      *h = node;
    else {
      TR_astar_node_ptr n    = *h,
                         prev = TR_null;
      while(n != TR_null
         && astar_node(n).f < astar_node(node).f) {
        TR_astar_node_t *no = &astar_node(n);
        prev = n;
        n = no->next;
      }
      if(n == TR_null)
        astar_node(prev).next = node;
      else 
      if(prev == TR_null) {
        astar_node(node).next = *h;
        *h = node;
      } else {
        astar_node(prev).next = node;
        astar_node(node).next = n;
      }
    }
  }
}

inline
TR_astar_node_ptr PL_ASTAR_PopNode(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr *h) {
  TR_worker_t *wo = &worker(w);
  TR_astar_node_ptr result = *h;
  *h = astar_node(*h).next;
  return result;
}

bool PL_ASTAR_ExpandNode(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr *free_head, TR_astar_node_ptr node, TR_astar_node_ptr *open_list, int size, int dest_x, int dest_y, int di, int dj, bool fast, bool flying) {
  TR_worker_t *wo = &worker(w);
  TR_astar_node_t *no = &astar_node(node);
  static int node_demi = 1<<(POS_SHIFT-1),
             node_size = 1<<POS_SHIFT;
  if(no->i == di
  && no->j == dj) {
    TR_astar_node_ptr n = (*free_head)++;
    assert(*free_head < MAX_ASTAR_NODE);
    PL_ASTAR_InitNode(gi, p, w, n, node, dest_x, dest_y, dest_x, dest_y, di, dj, true, fast);
    PL_ASTAR_InsertSortedNode(gi, p, w, n, open_list);
  } else {
    /* follow things */
    int *close_map = gi->close_map3[p][w][size-1];
    wo->callback_data.follow_mode = wo->callback_data.follow_mode 
                             || (wo->callback_data.can_follow
                             && ON_CLOSE_MAP(no->i, no->j) 
                             == wo->callback_data.mark);
    short *march_map = flying ? gi->flying_march_map[size-1] : gi->march_map[size-1];
    int mask = AT2(march_map, no->i, no->j);
    DO_TIMES(march_allow_point_count[mask]) {
      int i = no->i + march_allow_x[mask][f],
          j = no->j + march_allow_y[mask][f];
      TR_astar_node_ptr n = (*free_head)++;
      if(i == di
      && j == dj)
        PL_ASTAR_InitNode(gi, p, w, n, node, dest_x, dest_y, dest_x, dest_y, i, j, true, fast);
      else {
        bool r;
        int x, y;
        #include "expand_node.cpp"
        PL_ASTAR_InitNode(gi, p, w, n, node, dest_x, dest_y, x, y, i, j, r, fast);
      }
      /* follow things */
      if(wo->callback_data.follow_mode)
        if(ON_CLOSE_MAP(astar_node(n).i, astar_node(n).j) < wo->callback_data.start_mark)
          astar_node(n).f = 0x7fffffff;
      /* continue */
      if(AT2(wo->close_map, i, j) != wo->close_mark
      || AT2(wo->f_map, i, j) > astar_node(n).f)
        PL_ASTAR_InsertSortedNode(gi, p, w, n, open_list);
      else
        (*free_head)--;
    }
  }
}

bool PL_IsReachable(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, int range) {
  TR_unit_t *un        = &unit(u),
             *target_un = &unit(target);
  int pos_x     = un->x,
      pos_y     = un->y,
      dx        = target_un->x+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un)),
      dy        = target_un->y+UN_GetSizeShift(gi, SIZE(target_un))-UN_GetSizeShift(gi, SIZE(un)),
      target_cx = dx,
      target_cy = dy,
      orig_x    = pos_x + UN_GetSizeShift(gi, SIZE(un)),
      orig_y    = pos_y + UN_GetSizeShift(gi, SIZE(un)),
      oi        = (orig_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
      oj        = (orig_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
      dest_x    = dx + UN_GetSizeShift(gi, SIZE(un)),
      dest_y    = dy + UN_GetSizeShift(gi, SIZE(un)),
      di        = (dest_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT,
      dj        = (dest_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
  if(CAN_BE_PUSHED(un))
    PL_SnapPoint(gi, SIZE(un), orig_x, orig_y, oi, oj, di, dj, &pos_x, &pos_y, false, FLYING(un), true);
  orig_x        = pos_x + UN_GetSizeShift(gi, SIZE(un));
  orig_y        = pos_y + UN_GetSizeShift(gi, SIZE(un));
  oi            = (orig_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
  oj            = (orig_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
  /* if we look for a path from 0xffffffff to 0xffffffff (close_map speaking), 
     then it may scan all the map and we don't want that */
  int *close_map   = FLYING(un) ? gi->flying_close_map[SIZE(un)-1] : gi->close_map[SIZE(un)-1];
  if(CAN_BE_PUSHED(un))
    assert(ON_CLOSE_MAP(oi, oj) != 0xffffffff);
  PL_SnapPoint(gi, SIZE(un), orig_x, orig_y, oi, oj, di, dj, &dx, &dy, true, FLYING(un), true);
  dest_x    = dx + UN_GetSizeShift(gi, SIZE(un));
  dest_y    = dy + UN_GetSizeShift(gi, SIZE(un));
  di        = (dest_x-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
  dj        = (dest_y-(1<<(POS_SHIFT-1)))>>POS_SHIFT;
  int ddx       = target_cx - dx,
      ddy       = target_cy - dy,
      dist      = ddx * ddx + ddy * ddy,
      size_diff = (SIZE(un)+SIZE(target_un))<<(POS_SHIFT-1);
  if(dist < 0 || ddx > MAX_DIST || ddy > MAX_DIST)
    dist = 0x7fffffff;
  return MAX(0, TR_ISqrt(dist)-size_diff) <= (range<<POS_SHIFT);
}

bool PL_SnapPoint(TR_game_instance_t *gi, int size, int ox, int oy, int oi, int oj, int di, int dj, int *x, int *y, bool find_nearest_origin, bool flying, bool skill_here) {
  int *clearance_map = flying ? gi->flying_clearance_map : gi->clearance_map;
  int pos_x     = *x + UN_GetSizeShift(gi, size),
      pos_y     = *y + UN_GetSizeShift(gi, size),
      demi      = 1<<(POS_SHIFT-1),
      x0        = (((pos_x-demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
      x1        = (((pos_x+demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
      y0        = (((pos_y-demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
      y1        = (((pos_y+demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
      s         = pos_x - x0,
      t         = pos_y - y0,
      sv0       = AT2(clearance_map, x0>>POS_SHIFT, y0>>POS_SHIFT),
      sv1       = AT2(clearance_map, x1>>POS_SHIFT, y0>>POS_SHIFT),
      sv2       = AT2(clearance_map, x0>>POS_SHIFT, y1>>POS_SHIFT),
      sv3       = AT2(clearance_map, x1>>POS_SHIFT, y1>>POS_SHIFT),
      tv0       = sv0 + (sv1 - sv0) * s / (x1 - x0),
      tv1       = sv2 + (sv3 - sv2) * s / (x1 - x0),
      v         = tv0 + (tv1 - tv0) * t / (y1 - y0),
      half_size = size<<(POS_SHIFT-1);
  int         *edge_count = flying ? gi->flying_edge_count : gi->edge_count;
  TR_edge_t  *edges      = flying ? gi->flying_edges[size-1] : gi->edges[size-1];
  if(v < half_size
  && edge_count[size-1] != 0) {
    int *close_map = flying ? gi->flying_close_map[size-1] : gi->close_map[size-1];
    int min_dist = 0x7fffffff;
    TR_edge_t *min_e = null;
    int shift = UN_GetSizeShift(gi, MAX_UNIT_SIZE);
    if(!skill_here) {
      REAL_SOCLE(MAX_UNIT_SIZE, pos_x-shift-demi, pos_y-shift-demi) {
        if(INSIDE_MAP(i, j)) {
          DO_TIMES(2) {
            int e = AT2((flying ? gi->flying_edge_map : gi->edge_map)[size-1], i, j)[f];
            if(e != -1) {
              TR_edge_t *edg = &edges[e];
              if(ON_CLOSE_MAP(edg->i, edg->j) == ON_CLOSE_MAP(di, dj)) {
                int dist  = PL_DistFromEdge(gi, size, pos_x, pos_y, edg);
                if(dist < min_dist
                || (dist == min_dist
                && min_dist != 0x7fffffff
                && PL_DistFromEdge(gi, size, ox, oy, edg) 
                <  PL_DistFromEdge(gi, size, ox, oy, min_e))) {
                  min_dist = dist;
                  min_e = edg;
                }
              }
            } 
          }
        }
      }
    }
    if(min_e == null) {
      REAL_SOCLE(MAX_UNIT_SIZE, pos_x-shift-demi, pos_y-shift-demi) {
        if(INSIDE_MAP(i, j)) {
          DO_TIMES(2) {
            int e = AT2((flying ? gi->flying_edge_map : gi->edge_map)[size-1], i, j)[f];
            if(e != -1) {
              TR_edge_t *edg = &edges[e];
              if(ON_CLOSE_MAP(oi, oj) == 0xffffffff
              || ON_CLOSE_MAP(edg->i, edg->j) == ON_CLOSE_MAP(oi, oj)) {
                int dist  = PL_DistFromEdge(gi, size, pos_x, pos_y, edg);
                if(dist < min_dist
                || (dist == min_dist
                && min_dist != 0x7fffffff
                && PL_DistFromEdge(gi, size, ox, oy, edg) 
                <  PL_DistFromEdge(gi, size, ox, oy, min_e))) {
                  min_dist = dist;
                  min_e = edg;
                }
              }
            } 
          }
        }
      }
    }
    if(min_e == null) {
      DO_TIMES(edge_count[size-1]) {
        TR_edge_t *edg = &edges[f];
        if(ON_CLOSE_MAP(oi, oj) == 0xffffffff
        || ON_CLOSE_MAP(edg->i, edg->j) == ON_CLOSE_MAP(oi, oj)) {
          int dist = PL_DistFromEdge(gi, size, pos_x, pos_y, edg);
          if(dist < min_dist
          || (dist == min_dist
          && min_dist == 0x7fffffff)
          || (dist == min_dist
          && min_dist != 0x7fffffff
          && PL_DistFromEdge(gi, size, ox, oy, edg) 
          <  PL_DistFromEdge(gi, size, ox, oy, min_e))) {
            min_dist = dist;
            min_e = edg;
          }
        }
      }
    }
    assert(min_e != null);
    if(min_e != null) {
      if(find_nearest_origin) {
        *x = ox;
        *y = oy;
      }
      PL_SnapToEdge(gi, size, x, y, min_e);
    }
    return true;
  }
  return false;
}

int PL_DistFromEdge(TR_game_instance_t *gi, int size, int x, int y, TR_edge_t *edge) {
  assert(edge);
  int dex = (edge->x1 - edge->x0)>>(POS_SHIFT-6),
      dey = (edge->y1 - edge->y0)>>(POS_SHIFT-6),
      de  = TR_ISqrt(dex * dex + dey * dey),
      dx  = (x - edge->x0)>>(POS_SHIFT-6),
      dy  = (y - edge->y0)>>(POS_SHIFT-6),
      result = -1;
  if(de == 0) {
    result = dx * dx + dy * dy;
    if(result < 0 || dx > MAX_DIST || dy > MAX_DIST) 
      result = 0x7fffffff;
    result = TR_ISqrt(result);
  } else {
    int dot = (dx * dex + dy * dey) / de;
    if(dot <= 0) {
      result = dx * dx + dy * dy;
      if(result < 0 || dx > MAX_DIST || dy > MAX_DIST) 
        result = 0x7fffffff;
      result = TR_ISqrt(result);
    } else
    if(dot >= de) {
      dx = x - edge->x1;
      dy = y - edge->y1;
      result = dx * dx + dy * dy;
      if(result < 0 || dx > MAX_DIST || dy > MAX_DIST) 
        result = 0x7fffffff;
      result = TR_ISqrt(result);
    } else
      result = ABS(-dey * dx + dex * dy) / de;
  }
  assert(result >= 0);
  return result;
}

void PL_SnapToEdge(TR_game_instance_t *gi, int size, int *x, int *y, TR_edge_t *edge) {
  int pos_x   = *x + UN_GetSizeShift(gi, size),
      pos_y   = *y + UN_GetSizeShift(gi, size),
      dex     = edge->x1 - edge->x0,
      dey     = edge->y1 - edge->y0,
      de      = TR_ISqrt(dex * dex + dey * dey),
      dx      = pos_x - edge->x0,
      dy      = pos_y - edge->y0;
  if(de == 0) {
    *x = edge->x0 - UN_GetSizeShift(gi, size);
    *y = edge->y0 - UN_GetSizeShift(gi, size);
  } else {
    int dot = (dx * dex + dy * dey) / de;
    if(dot <= 0) {
      *x = edge->x0 - UN_GetSizeShift(gi, size);
      *y = edge->y0 - UN_GetSizeShift(gi, size);
    } else
    if(dot >= de) {
      *x = edge->x1 - UN_GetSizeShift(gi, size);
      *y = edge->y1 - UN_GetSizeShift(gi, size);
    } else {
      *x = edge->x0 + (edge->x1 - edge->x0) * dot / de - UN_GetSizeShift(gi, size);
      *y = edge->y0 + (edge->y1 - edge->y0) * dot / de - UN_GetSizeShift(gi, size);
    }
  }
}

void PL_UpdateMarchTex(TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, int x0, int y0, int x1, int y1, TR_astar_node_t pool[], TR_astar_node_ptr node, bool render_ground) {
  TR_worker_t *wo = &worker(w);
  int size = 2;
  #if THREADS  
  EnterCriticalSection(&debug_crit);
  #endif
  static int colors[] = {0xffff0000, 0xff00ff00, 0xff0000ff, 0xffff00ff, 0xffffff00, 0xff00ffff};
  int *close_map = gi->close_map[size-1];
  FOR_RECT(8, (MAP_SIZE_X<<3)-8, 8, (MAP_SIZE_Y<<3)-8)
    gi->march_tex[MAP_SIZE_X*8*j+i] = 0;
  if(render_ground) {
    FOR_RECT(8, (MAP_SIZE_X<<3)-8, 8, (MAP_SIZE_Y<<3)-8)
      gi->march_tex[MAP_SIZE_X*8*j+i] = (((i-4)>>3)&1) ? ((((j-4)>>3)&1) ? 0xffff0000 : 0) : ((((j-4)>>3)&1) ? 0 : 0xffff0000);
      //gi->march_tex[MAP_SIZE_X*8*j+i] = AT2(wo->march_map[size-1], ((i-4)>>3), ((j-4)>>3)) == 12 ? 0xffff0000 : 0;
      //if(ON_CLOSE_MAP((i-4)>>3, (j-4)>>3) != 0xffffffff)
      //  gi->march_tex[MAP_SIZE_X*8*j+i] = colors[ON_CLOSE_MAP((i-4)>>3, (j-4)>>3)];
      //else
      //  gi->march_tex[MAP_SIZE_X*8*j+i] = 0;
    /**/
    DO_TIMES(gi->edge_count[size-1]) {
      int x0 = gi->edges[size-1][f].x0>>(POS_SHIFT-3),
          x1 = gi->edges[size-1][f].x1>>(POS_SHIFT-3),
          y0 = gi->edges[size-1][f].y0>>(POS_SHIFT-3),
          y1 = gi->edges[size-1][f].y1>>(POS_SHIFT-3);
      TCOD_line_init(x0, y0, x1, y1);
      do {
        gi->march_tex[MAP_SIZE_X*8*y0+x0] = 
             gi->edges[size-1][f].r0 == 1 
          || gi->edges[size-1][f].r1 == 1 ? 
          0xff00ff00 : 0xffff0000;
      }while(!TCOD_line_step(&x0, &y0));
    }
  }
  gi->march_tex[MAP_SIZE_X*8*((y0+UN_GetSizeShift(gi, size))>>(POS_SHIFT-3))+((x0+UN_GetSizeShift(gi, size))>>(POS_SHIFT-3))] = 0xffffffff;
  gi->march_tex[MAP_SIZE_X*8*((y1+UN_GetSizeShift(gi, size))>>(POS_SHIFT-3))+((x1+UN_GetSizeShift(gi, size))>>(POS_SHIFT-3))] = 0xffffffff;
  if(node != TR_null) {
    TR_astar_node_ptr parent = node;
    node = astar_node(node).parent;
    while(node != TR_null) {
      TR_astar_node_t *no = &astar_node(node);
      int x0 = astar_node(parent).x>>(POS_SHIFT-3),
          y0 = astar_node(parent).y>>(POS_SHIFT-3),
          x1 = no->x>>(POS_SHIFT-3),
          y1 = no->y>>(POS_SHIFT-3);
      TCOD_line_init(x0, y0, x1, y1);
      do {
        gi->march_tex[MAP_SIZE_X*8*y0+x0] = astar_node(parent).r ? 0xff0000ff : 0xffffffff;
      } while(false && !TCOD_line_step(&x0, &y0));
      parent = node;
      node = no->parent;
    }
  }
  #if THREADS    
  LeaveCriticalSection(&debug_crit);
  #endif
}

void PL_SetSkillCooldown(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_type_t unit_type, TR_skill_type_t skill_type) {
  TR_unit_t *un = &unit(u);
  int skill_id = UN_SkillId(gi, u, skill_type);
  assert(skill_id != -1);
  int min_cooldown = PL_GetMinCooldown(gi, p, unit_type, skill_id);
  assert(min_cooldown != -1);
  un->skills_cooldown[skill_id] = min_cooldown;
}

int PL_GetMinCooldown(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_type_t unit_type, TR_id_t skill_id) {
  TR_player_t *pl = &player(p);
  bool found = false;
  int min_cooldown = 0x7fffffff;
  TR_unit_ptr u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(un->unit_type == unit_type
    && un->skills_cooldown[skill_id] < min_cooldown) {
      min_cooldown = un->skills_cooldown[skill_id];
      found = true;
    }
    u = un->pnext;
  }
  return found ? min_cooldown : -1;
}

void PL_CooldownAll(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_type_t unit_type, TR_id_t skill_id) {
  TR_player_t *pl = &player(p);
  TR_unit_ptr u = pl->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(un->unit_type == unit_type)
      un->skills_cooldown[skill_id] = 0;
    u = un->pnext;
  }
}

/*
 * game instance
 */

bool GI_Init(TR_game_instance_t *gi, int _argc, char **_argv) {
  /* init enet */
  enet_initialize();
  enet_address_set_host(&gi->master_server_addr, game_desc.master_server.c_str());
  gi->master_server_addr.port = MASTER_SERVER_PORT;
  /* init ping thread */
  GI_InitPingThread(gi);
  /* init ogre */
  log("Initializing ogre...");
  try {
    gi->ogre_root = new Ogre::Root("./data/plugin.cfg", "");
    //gi->ogre_root->showConfigDialog();
    assert(gi->ogre_root->getAvailableRenderers().size() != 0);
    gi->ogre_root->setRenderSystem(gi->ogre_root->getAvailableRenderers().at(0));
    run = true;
    if(run) {
      gi->ogre_root->installPlugin(PU_NEW ParticleUniverse::ParticleUniversePlugin());
      gi->ogre_root->initialise(false, "TownRush");
      Ogre::NameValuePairList params;
      params["FSAA"] = "0";
      params["vsync"] = "yes";
      #if RECORD_DEMO
      gi->ogre_window = gi->ogre_root->createRenderWindow("TownRush", 1280, 720, FULLSCREEN, &params);
      #else
      gi->ogre_window = gi->ogre_root->createRenderWindow("TownRush", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), FULLSCREEN, &params);
      #endif
      /* CONTINUE INIT */
      gi->ogre_window->setDeactivateOnFocusChange(false);
      GI_InitOIS(gi);
      gi->ogre_scene = gi->ogre_root->createSceneManager(Ogre::ST_GENERIC, "scene");
      //gi->ogre_scene->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));
      //gi->ogre_scene->setShadowTextureSelfShadow(true);
      //gi->ogre_scene->setShadowCasterRenderBackFaces(false);
      GI_InitOgreResources(gi);
      /* LOADING */
      {gi->loading_overlay   = Ogre::OverlayManager::getSingleton().create("loading overlay ");
       Ogre::OverlayContainer *panel = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "loading panel ");
       panel->setMetricsMode(Ogre::GMM_PIXELS);
       panel->setPosition(0, 0);
       panel->setDimensions(100, 100);
       gi->loading_overlay->setZOrder(220);
       gi->loading_overlay->show();
       gi->loading_overlay->add2D(panel);
       gi->loading_text_overlay = 
         (Ogre::TextAreaOverlayElement*)Ogre::OverlayManager::getSingleton().createOverlayElement(
            "TextArea", "loading text overlay ");
       gi->loading_text_overlay->setMetricsMode(Ogre::GMM_PIXELS);
       gi->loading_text_overlay->setLeft(gi->ogre_window->getWidth()/2 - str("Waiting for the other players...").size()*14/2);
       gi->loading_text_overlay->setTop(gi->ogre_window->getHeight()/2);
       gi->loading_text_overlay->setColourBottom(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
       gi->loading_text_overlay->setColourTop(Ogre::ColourValue((float)TCOD_orange.r/255, (float)TCOD_orange.g/255, (float)TCOD_orange.b/255));
       gi->loading_text_overlay->setCaption("Waiting for the other players...");
       gi->loading_text_overlay->setFontName("cuckoo");
       gi->loading_text_overlay->setCharHeight(35);
       panel->addChild(gi->loading_text_overlay);}
      resx = gi->ogre_window->getWidth();
      resy = gi->ogre_window->getHeight();
      OIS::MouseState &mouse_state = const_cast<OIS::MouseState&>(gi->ogre_mouse->getMouseState());
      mouse_state.X.abs = gi->ogre_window->getWidth() / 2;
      mouse_state.Y.abs = gi->ogre_window->getHeight() / 2;
      //gi->ogre_scene->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
      /* tray */
      gi->ogre_tray = new OgreBites::SdkTrayManager("Bahla", gi->ogre_window, gi->ogre_mouse, &gi->ogre_class);
      gi->ogre_tray->hideCursor();
      #if RELEASE
      gi->ogre_tray->hideFrameStats();
      #endif
    }
  } catch(Ogre::Exception &e) {
    log(e.getFullDescription().c_str());
  }
  log("Ogre initialized.");
  /* audio */
  gc_initialize(0);
  gi->gau_manager         = gau_manager_create_custom(GA_DEVICE_TYPE_OPENAL, GAU_THREAD_POLICY_SINGLE, 3, 512);
  gi->gau_mixer           = gau_manager_mixer(gi->gau_manager);
  gi->gau_stream_manager  = gau_manager_streamManager(gi->gau_manager);
  /* rest */
  gp_gi.coin_sound = null;
  global_gi = gi;
  #if !RELEASE
  gi->show_all = false;
  gi->show_lagometer = false;
  #endif
  const char *seed = game_desc.game_name.c_str();
  color_shift = TR_Rand((unsigned int*)seed);
  gi->host = null;
  gi->peer = null;
  gi->state = 0;
  gi->key_pressed_count = 0;
  gi->add_state_count = 0;
  gi->remove_state_count = 0;
  gi->minimap_cleared = true;
  gi->frame_time = 0;
  gi->game_frame_mark = 0;
  gi->cursor_tex = "cursor2";
  gi->last_cursor_tex = "";
  gi->cmd_id     = 1000000000; /* use to spawn units and to be sure there is no colision with clients */
  gi->colision_mark = 0;
  gi->penetration_mark = 0;
  gi->aver_frame_times_counter = 0;
  gi->aver_wait_times_counter = 0;
  DO_TIMES(MAX_AVER_FRAME_TIMES)
    gi->aver_frame_times[f] = NORMAL_TIME_STEP;
  DO_TIMES(MAX_AVER_WAIT_TIMES)
    gi->aver_wait_times[f] = 0;
  DO_TIMES(MAX_MAX_PING_TIMES)
    gi->pings[f] = 100;
  DO_TIMES(MAX_PLAYER)
    gi->is_timingout[f] = false;
  gi->square_x[0] = +1; gi->square_y[0] = +1;
  gi->square_x[1] = +1; gi->square_y[1] = -1;
  gi->square_x[2] = +1; gi->square_y[2] = +0;
  gi->square_x[3] = -1; gi->square_y[3] = +1;
  gi->square_x[4] = -1; gi->square_y[4] = -1;
  gi->square_x[5] = -1; gi->square_y[5] = +0;
  gi->square_x[6] = +0; gi->square_y[6] = +1;
  gi->square_x[7] = +0; gi->square_y[7] = -1;
  gi->task_mark         = 0;
  gi->close_map_mark    = 0;
  DO_TIMES(MAX_UNIT_SIZE+1) {
    int size = f+1;
    halfes[f] = floor((float)size*0.5f - 0.5f);
  }
  DO_TIMES(MAX_UNIT_SIZE) {
    int size = f+1;
    DO_TIMES(MAP_SIZE) {
      int i = f;
      DO_TIMES(2)
        gi->edge_map[size-1][i][f] = -1;
    }
  }
  CMD_Init(gi, argc, argv);
  CS_Init(gi, argc, argv);
  RS_Init(gi, argc, argv);
  UO_Init(gi, argc, argv);
  BO_Init(gi, argc, argv);
  TR_Init(gi, argc, argv);
  PS_Init(gi, argc, argv);
  BA_Init(gi, argc, argv);
  BT_Init(gi, argc, argv);
  FT_Init(gi, argc, argv);
  LI_Init(gi, argc, argv);
  SO_Init(gi, argc, argv);
  ST_Init(gi, argc, argv);
  FL_Init(gi, argc, argv);
  EX_Init(gi, argc, argv);
  CA_Init(gi, argc, argv);
  BE_Init(gi, argc, argv);
  LG_Init(gi, argc, argv);
  NO_Init(gi, argc, argv);
  AN_Init(gi, argc, argv);
  UN_Init(gi, argc, argv);
  PL_Init(gi);
  CAM_Init(gi, argc, argv);
  CL_Init(gi, argc, argv);
  DO_TIMES(MAX_PLAYER) {
    int p = f;
    DO_TIMES(MAX_WORKER) {
      int w = f;
      gi->close_map_mark3[p][w] = 0;
      DO_TIMES(MAX_UNIT_SIZE) {
        int g = f;
        int *close_map3 = gi->close_map3[p][w][g];
        DO_TIMES(MAP_SIZE)
          close_map3[f] = 0xffffffff;
      }
    }
  }
  #include "ui.cpp"
  log("game instance initialized");
  return run;
}

void GI_InitOgreResources(TR_game_instance_t *gi) {
  Ogre::ConfigFile cf;
  cf.load("./data/resource.cfg");
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
  Ogre::String secName, typeName, archName;
  while(seci.hasMoreElements()) {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for(i = settings->begin(); i != settings->end(); ++i) {
      typeName = i->first;
      archName = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
    }
  }
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void GI_InitOIS(TR_game_instance_t *gi) {
  size_t hWnd = 0;
  OIS::ParamList params;
  gi->ogre_window->getCustomAttribute("WINDOW", &hWnd);
  params.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));
  params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
  params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
  params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
  params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
  /* windows */
  gi->ogre_input = OIS::InputManager::createInputSystem(params);
  gi->ogre_keyboard = static_cast<OIS::Keyboard*>(gi->ogre_input->createInputObject(OIS::OISKeyboard, true));
  gi->ogre_keyboard->setEventCallback(&gi->ogre_class);
  gi->ogre_mouse = static_cast<OIS::Mouse*>(gi->ogre_input->createInputObject(OIS::OISMouse, true));
  gi->ogre_mouse->getMouseState().height = gi->ogre_window->getHeight();
  gi->ogre_mouse->getMouseState().width  = gi->ogre_window->getWidth();
  gi->ogre_mouse->setEventCallback(&gi->ogre_class);

}

void GI_InitGame(TR_game_instance_t *gi, game_desc_t *gd, TR_init_game_cb_t init_game_cb) {
  /* plugued-in init */
  (*init_game_cb)(gi, gd);
  /* connection */
  gi->players_connected = false;
  TR_client_ptr c = gi->client_head;
  while(c != TR_null) {
    TR_client_t *cl = &client(c);
    CL_Connect(gi, c);
    c = cl->next;
  }
  TR_remote_cmd_store_ptr r = gi->remote_cmd_store_head;
  while(r != TR_null) {
    TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
    RS_Connect(gi, r); 
    r = rs->next;
  }
  /* misc */
  DO_TIMES(MAX_TEAM) {
    int i = f;
    DO_TIMES(RANGE_MAP_SIZE) {
      gi->fov_maps[i][f] = 0;
      gi->magic_vision_maps[i][f] = 0;
      gi->working_fov_maps[i][f] = 0;
      gi->working_magic_vision_maps[i][f] = 0;
    }
  }
  //TCOD_sys_sleep_milli(40000);
  log("The game is initialized.");
}

void GI_StartGame(TR_game_instance_t *gi, TR_start_game_cb_t start_game_cb) {
  gi->desync            = false;
  gi->minimap_cleared   = false;
  gi->flash_btn.s       = "";
  gi->game_time_step    = 0;
  gi->frame_time_step   = NORMAL_TIME_STEP;
  gi->game_time_acc     = 0;
  gi->turn              = -2;
  gi->turn_frame_count  = STARTING_TURN_FRAME_COUNT;
  gi->turn_acc          = gi->turn_frame_count;
  gi->game_time         = gi->last_t = TR_GetTime();
  gi->wait_time         = 0;
  GI_EndOfTurn(gi);
  gi->turn_acc          = gi->turn_frame_count;
  GI_StartCamera(gi, GI_GetCamera(gi));
  (*start_game_cb)(gi);
  gi->loading_overlay->hide();
  log("The game has started.");
}

void GI_StartCamera(TR_game_instance_t *gi, TR_camera_ptr ca) {
  TR_camera_t *cam = &camera(ca);
}

void GI_Free(TR_game_instance_t *gi) {
  GI_DisconnectFromMasterServer(gi);
  GI_SaveReplay(gi);
  /* it may not be affected */
  if(gi->free_all_cb)
    gi->free_all_cb(gi);
  CL_FreeAll(gi);
  PL_FreeAll(gi);
  UN_FreeAll(gi);
  NO_FreeAll(gi);
  SO_FreeAll(gi);
  FL_FreeAll(gi);
  EX_FreeAll(gi);
  CA_FreeAll(gi);
  BE_FreeAll(gi);
  LG_FreeAll(gi);
  LI_FreeAll(gi);
  PS_FreeAll(gi);
  BA_FreeAll(gi);
  TR_FreeAll(gi);
  BT_FreeAll(gi);
  FT_FreeAll(gi);
  RS_FreeAll(gi);
  CS_FreeAll(gi);
  CMD_FreeAll(gi);
  CAM_FreeAll(gi);
  log("Game instance freed.");
}

void GI_Close(TR_game_instance_t *gi) {
  GI_ClosePingThread(gi);
  GI_Free(gi);
  gau_manager_destroy(gi->gau_manager);
  gc_shutdown();
  GI_KillHost(gi);
  /* kill ogre */
  OIS::InputManager::destroyInputSystem(gi->ogre_input);
  delete gi->ogre_tray;
  delete gi->ogre_root;
}

void GI_InitEnvMap(TR_game_instance_t *gi) {
  /*generate_map();
  FOR_RECT(0, RANGE_MAP_SIZE_X-1, 0, RANGE_MAP_SIZE_Y-1) {
    if(char_map[i>>2][j>>2] == '#')
      AT_ENV(i, j) = TR_WALL;
    else
      AT_ENV(i, j) = TR_EMPTY;
  }*/
  TCOD_image_t walls = TCOD_image_load("./data/map walls.png");
  FOR_RECT(0, RANGE_MAP_SIZE_X-1, 0, RANGE_MAP_SIZE_Y-1) {
    TCOD_color_t c = TCOD_image_get_pixel(walls, i, j);
    if(IS_COLOR(255, 255, 255)
    || IS_COLOR(0, 0, 255)) 
      AT_ENV(i, j) = TR_WALL;
    else
      AT_ENV(i, j) = TR_EMPTY;
  }
  /* visibility map */
  gi->wall_map = TCOD_map_new(RANGE_MAP_SIZE_X, RANGE_MAP_SIZE_Y);
  TCOD_map_clear(gi->wall_map, true, true);
  FOR_RECT(0, RANGE_MAP_SIZE_X-1, 0, RANGE_MAP_SIZE_Y-1)
    if(AT_ENV(i, j) == TR_WALL)
      TCOD_map_set_properties(gi->wall_map, i, j, false, false);
  log("env_map initialized.");
}

void GI_InitLightMap(TR_game_instance_t *gi) {
  DO_TIMES(MAP_SIZE)
    gi->static_light_map[f] = TCOD_color_multiply_scalar(AMBIENT_LIGHTMAP,1);
  TR_light_ptr l = gi->light_head;
  while(l != TR_null) {
    TR_light_t *li = &light(l);
    int x = li->x,
        y = li->y,
        r = li->r>>POS_SHIFT;
    TCOD_map_compute_fov(
      gi->wall_map,
      gi->tcod_light_map, 
      x>>RANGE_SHIFT, 
      y>>RANGE_SHIFT, 
      r>>RANGE_SHIFT, 
      true, 
      FOV_RESTRICTIVE);
    FOR_RECT(-r, r-1, -r, r-1) {
      if(INSIDE_MAP(x+i, y+j)
      && TCOD_map_is_in_fov(gi->tcod_light_map, (x+i)>>RANGE_SHIFT, (y+j)>>RANGE_SHIFT)
      && (i*i + j*j) < r*r) {
        AT(static_light_map, x+i, y+j) = 
          TCOD_color_add(
            AT(static_light_map, x+i, y+j), 
            TCOD_color_multiply_scalar(
              TCOD_color_multiply_scalar(li->c, li->i), 
              1));
      }
    }
    l = li->next;
  }
}

void GI_UpdateTime(TR_game_instance_t *gi) {
  GI_UpdateAverFrameTime(gi, gi->frame_time);
  gi->game_time_acc += gi->frame_time;
  if(!gi->desync) {
    if(gi->turn_acc < gi->turn_frame_count) {
      if(gi->game_time_acc >= gi->frame_time_step) {
        gi->game_time_acc -= gi->frame_time_step;
        gi->game_time_step = gi->frame_time_step;
        gi->turn_acc++;
      } else
        gi->game_time_step = 0;  
    } else {
      gi->game_time_step = 0;
      gi->wait_time += gi->frame_time_step;
      gi->game_time_acc = 0;
      if(gi->wait_time > 1000) 
        CAM_OpenWaitWindow(gi, GI_GetCamera(gi));
    }
  } else
    gi->game_time_step = 0;
  gi->game_time                 += gi->game_time_step;
  gi->turn_frame_count_dec_acc  += gi->game_time_step;
  if(gi->game_time_step > 0) {
    gi->game_frame_mark++;
    //DTRACE("GI_UpdateTime")
    //DTRACEI(gi->game_frame_mark)
    //DTRACEI(gi->game_time_step)
    //DTRACEI(gi->wait_time)
    //DTRACEI(gi->turn_frame_count)
  }
}

void GI_UpdateTurn(TR_game_instance_t *gi) {
  if(gi->turn_acc >= gi->turn_frame_count) {
    bool change_turn    = true;
    TR_player_ptr p     = gi->player_head;
    while(p != TR_null) {
      change_turn = PL_NextTurn(gi, p) && change_turn;
      p = player(p).next;
    }
    if(change_turn)
      GI_EndOfTurn(gi);
  }
}

void GI_Update(TR_game_instance_t *gi) {
  /* closed? */
  if(GI_IsKeyPressed(gi, OIS::KC_ESCAPE)
  || client_gi.ogre_window->isClosed()) {
    run = false;
    if(game_desc.PvE) {
      GAME_SaveGame(gi, gp_gi.level);
      TR_Quit(TCOD_sys_elapsed_seconds());
    }
  }
  // basic time
  TR_time_t tt           = TR_GetTime(),
            frame_time  = tt - gi->last_t;
  if(frame_time < 0 || gi->game_time_acc < 0) 
      log(str("bug ") + f2a(frame_time) + " " + f2a(gi->game_time_acc) + " " + f2a(gi->game_time_step));
  if(frame_time < 0) frame_time = 0; // the bug (tm)
  gi->frame_time = MIN(frame_time, MAX_FRAME_TIME);
  gi->last_t     = tt;
  /* Pumping iron */
  Ogre::WindowEventUtilities::messagePump();
  gi->key_pressed_count = 0;
  gi->ogre_keyboard->capture();
  gi->ogre_mouse->capture();
  /* listen master server */
  LISTEN_MASTER_SERVER
  /* check timeout */
  GI_CheckForTimeout(gi, &e, TR_state_game);
  /* check timeout */
  GI_CheckForGameClosed(gi, &e, TR_state_game);
  /* ping */
  GI_Ping(gi, &e);
  /* message */
  GI_Message(gi, &e);
  // general inputs
  GI_UpdateInputs(gi);
  // states
  #include "state.cpp"
  GI_UpdateState(gi);
  /* check connection */
  GI_CheckConnectionToMasterServer(gi, &e, TR_state_game);
  /* audio */
  gau_manager_update(gi->gau_manager);
  /* ogre render */
  gi->ogre_frame_event.timeSinceLastFrame = (float)gi->frame_time*0.001f;
  gi->ogre_frame_event.timeSinceLastEvent = (float)gi->frame_time*0.001f;
  gi->ogre_tray->frameRenderingQueued(gi->ogre_frame_event);
  gi->ogre_tray->refreshCursor();
  gi->ogre_root->renderOneFrame();
  EnterCriticalSection(&ping_thread_disconnect_crit);
  if(run) run = !game_desc.online || ping_thread_run;
  LeaveCriticalSection(&ping_thread_disconnect_crit);
}

void GI_UpdateGame(TR_game_instance_t *gi) {
  if(gi->game_time_step != 0) {
    /* debug */
    #if !RELEASE
    //GI_CheckListIntegrity(gi);
    #endif
    TR_player_ptr p;
    /* STOP WORKERS */
    p = gi->player_head;
    while(p != TR_null) {
      TR_player_t *pl = &player(p);
      PL_UpdateUnitsPart3(gi, p);
      p = pl->next;
    }
    /* MOVE AND SKILL */
    int player_count = 0;
    TR_player_ptr ps[MAX_PLAYER];
    p = gi->player_head;
    while(p != TR_null) {
      ps[player_count++] = p;
      p = player(p).next;
    }
    int i = 0;
    while(i < player_count) {
      int k = TR_Rand(&gi->working_seed)%player_count;
      TR_player_ptr p = ps[k];
      if(player(p).update_frame_mark != gi->game_frame_mark) {
        player(p).update_frame_mark = gi->game_frame_mark;
        PL_MoveAndSkills(gi, p);
        i++;
      }
    }
    /* MISC */
    GI_UnitRepulsion(gi);
    /* PROCESS CMDS */
    gi->has_processed_cmds = false;
    p = gi->player_head;
    while(p != TR_null) {
      TR_player_t *pl = &player(p);
      PL_ProcessCmdStore(gi, p);
      CS_ClearCmdStore(gi, pl->cs);
      p = pl->next;
    }
    /* MARCHING SQUARES */
    GI_UpdateWorkerUnits(gi);
    GI_UpdateMarchingSquares(gi);  
    if(gi->has_processed_cmds) {
      /* UNIT ORDERS CHANGES */
      p = gi->player_head;
      while(p != TR_null) {
        TR_player_t *pl = &player(p);
        PL_UpdateUnitsPart1(gi, p);
        p = pl->next;
      }
      /* MARCHING SQUARES */
      GI_UpdateWorkerUnits(gi);
      GI_UpdateMarchingSquares(gi);
    }
    /* UNIT ORDERS CHANGES */
    p = gi->player_head;
    while(p != TR_null) {
      TR_player_t *pl = &player(p);
      gi->update_player_cb(gi, p);
      PL_UpdateUnitsPart1(gi, p);
      p = pl->next;
    }
    /* MISC !!!not in worker!!! after cmds */
    GI_UpdateMiscs(gi);
    /* START WORKERS */
    p = gi->player_head;
    while(p != TR_null) {
      TR_player_t *pl = &player(p);
      PL_UpdateUnitsPart2(gi, p);
      p = pl->next;
    }
    /* FOV MAP */
    GI_SnapUnits(gi);
    GI_UpdateFOVMaps(gi);
  }
}

void GI_UpdateMiscs(TR_game_instance_t *gi) {
  if(gi->game_time_step != 0) {
    TR_player_ptr cam_p = GI_GetPlayerPtr(gi, camera(GI_GetCamera(gi)).player_id);
    TR_ball_ptr b = gi->ball_head;
    while(b != TR_null) {
      TR_ball_ptr b2 = b;
      b = ball(b).next;
      if(BA_Update(gi, b2))
        BA_Free(gi, b2);
    }
    TR_burn_trace_ptr bt = gi->burn_trace_head;
    while(bt != TR_null) {
      TR_burn_trace_ptr bt2 = bt;
      bt = burn_trace(bt).next;
      if(BT_Update(gi, bt2))
        BT_Free(gi, bt2);
    }
    TR_floating_text_ptr ft = 
      gi->floating_text_head;
    while(ft != TR_null) {
      TR_floating_text_ptr ft2 = ft;
      ft = floating_text(ft).next;
      if(FT_Update(gi, ft2))
        FT_Free(gi, ft2);
    }
    TR_fluid_ptr f = gi->fluid_head;
    while(f != TR_null) {
      TR_fluid_ptr f2 = f;
      f = fluid(f).next;
      if(FL_Update(gi, f2))
        FL_Free(gi, f2);
    }
    TR_explosion_ptr e = gi->explosion_head;
    while(e != TR_null) {
      TR_explosion_ptr e2 = e;
      e = explosion(e).next;
      if(EX_Update(gi, e2))
        EX_Free(gi, e2);
    }
    TR_build_explosion_ptr bb = gi->build_explosion_head;
    while(bb != TR_null) {
      TR_build_explosion_ptr bb2 = bb;
      bb = build_explosion(bb).next;
      if(BE_Update(gi, bb2))
        BE_Free(gi, bb2);
    }
    TR_cadaver_ptr cc = gi->cadaver_head;
    while(cc != TR_null) {
      TR_cadaver_ptr cc2 = cc;
      cc = cadaver(cc).next;
      if(CA_Update(gi, cc2))
        CA_Free(gi, cc2);
    }
    TR_lightning_ptr lg = gi->lightning_head;
    while(lg != TR_null) {
      TR_lightning_ptr lg2 = lg;
      lg = lightning(lg).next;
      if(LG_Update(gi, lg2))
        LG_Free(gi, lg2);
    }
    TR_particle_system_ptr ps = gi->particle_system_head;
    while(ps != TR_null) {
      TR_particle_system_ptr ps2 = ps;
      ps = particle_system(ps).next;
      if(PS_Update(gi, cam_p, ps2))
        PS_Free(gi, ps2);
    }
  }
}

void GI_UpdateWorkerUnits(TR_game_instance_t *gi) {
  gi->worker_unit_count = 0; 
  TR_unit_ptr u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(!CAN_BE_PUSHED(un)
    || (UO_GetFrontType(gi, u) 
    == TR_unit_order_skill
    && gi->get_skill_is_blocking_cb(gi, UO_GetFront(gi, u)->skill_type))) {
      gi->worker_units_x[gi->worker_unit_count] = un->x;
      gi->worker_units_y[gi->worker_unit_count] = un->y;
      gi->worker_units_flying[gi->worker_unit_count] = FLYING(un);
      gi->worker_units_size[gi->worker_unit_count] = SIZE(un)<<POS_SHIFT;
      gi->worker_unit_count++;
    } 
    u = un->next;
  }
}

void GI_UpdateMarchingSquares(TR_game_instance_t *gi) {
  memcpy(gi->clearance_map, gi->clearance_map_base, sizeof(int)*MAP_SIZE);
  memcpy(gi->flying_clearance_map, gi->clearance_map_base2, sizeof(int)*MAP_SIZE);
  DO_TIMES(gi->worker_unit_count) {
    if(gi->worker_units_flying[f]) 
      GI_DrawUnitClearance(gi, gi->worker_units_size[f], gi->worker_units_x[f], gi->worker_units_y[f], gi->flying_clearance_map);
    else
      GI_DrawUnitClearance(gi, gi->worker_units_size[f], gi->worker_units_x[f], gi->worker_units_y[f], gi->clearance_map);
  }
  DO_TIMES(2) {
    int size = f+1;
    GI_MarchingSquares(
      gi, 
      gi->clearance_map, 
      gi->edges[size-1], 
      &gi->edge_count[size-1], 
      gi->edge_map[size-1],
      gi->march_map[size-1],
      0, 0, MAP_SIZE_X-1, MAP_SIZE_Y-1, 
      size<<(POS_SHIFT-1));
    int *close_map = gi->close_map[size-1];
    DO_TIMES(MAP_SIZE)
      close_map[f] = 0xffffffff;
    int close_mark = 0;
    FOR_RECT(0, MAP_SIZE_X-1, 0, MAP_SIZE_Y-1)
      GI_PathfinderScanMap(gi, size, i, j, &close_mark, close_map, gi->march_map[size-1]);
  }
  int size = 1;
  GI_MarchingSquares(
    gi, 
    gi->flying_clearance_map, 
    gi->flying_edges[size-1], 
    &gi->flying_edge_count[size-1], 
    gi->flying_edge_map[size-1],
    gi->flying_march_map[size-1],
    0, 0, MAP_SIZE_X-1, MAP_SIZE_Y-1, 
    size<<(POS_SHIFT-1));
  int *close_map = gi->flying_close_map[size-1];
  DO_TIMES(MAP_SIZE)
    close_map[f] = 0xffffffff;
  int close_mark = 0;
  FOR_RECT(0, MAP_SIZE_X-1, 0, MAP_SIZE_Y-1)
    GI_PathfinderScanMap(gi, size, i, j, &close_mark, close_map, gi->flying_march_map[size-1]);
}

inline
void GI_PathfinderScanMap(TR_game_instance_t *gi, int size, int pos_x, int pos_y, int *close_mark, int close_map[MAP_SIZE], short march_map[MAP_SIZE]) {
  if(ON_CLOSE_MAP(pos_x, pos_y) == 0xffffffff
  && AT2(march_map, pos_x, pos_y) != 15) {
    static short scan_map_x[MAP_SIZE];
    static short scan_map_y[MAP_SIZE];
    static int scan_map_counter;     
    *close_mark = *close_mark+1;
    int mark = *close_mark;
    scan_map_counter = 0;
    ON_CLOSE_MAP(pos_x, pos_y) = mark;
    scan_map_x[scan_map_counter] = pos_x;
    scan_map_y[scan_map_counter] = pos_y;
    scan_map_counter++;
    while(scan_map_counter > 0) {
      pos_x = scan_map_x[scan_map_counter-1];
      pos_y = scan_map_y[scan_map_counter-1];
      scan_map_counter--;
      short mask = AT2(march_map, pos_x, pos_y);
      DO_TIMES(march_allow_point_count[mask]) {
        int i = march_allow_x[mask][f],
            j = march_allow_y[mask][f];
        int x = pos_x+i,
            y = pos_y+j,
            *onclosemap = &close_map[y*MAP_SIZE_X+x];
        if(x >= 0 && x < MAP_SIZE_X 
        && y >= 0 && y < MAP_SIZE_Y
        && *onclosemap != mark) {
          *onclosemap = mark;
          scan_map_x[scan_map_counter] = x;
          scan_map_y[scan_map_counter] = y;
          scan_map_counter++;
        }
      }
    }
  }
}

void GI_UpdateFOVMaps(TR_game_instance_t *gi) {
  //DTRACE("GI_UpdateFOVMaps")
  if((gi->game_frame_mark&7) == 0) {
    DO_TIMES(game_desc.team_count) {
      int team_id = f;
      memcpy(gi->fov_maps[team_id], gi->working_fov_maps[team_id], sizeof(int)*RANGE_MAP_SIZE);
      DO_TIMES(RANGE_MAP_SIZE) {
        gi->working_fov_maps[team_id][f] = 0;
        gi->working_magic_vision_maps[team_id][f] = 0;
      }
    }
  }
  /* FIXME: these loops are illed */
  DO_TIMES(game_desc.team_count) {
    TR_id_t team_id      = f;
    int *fov_map          = gi->working_fov_maps[team_id],
        *magic_vision_map = gi->working_magic_vision_maps[team_id];
    TR_player_ptr p = gi->player_head;
    while(p != TR_null) {
      //DTRACEI(p)
      TR_player_t *pl = &player(p);
      if(pl->team_id == team_id) {
        TR_unit_ptr u = pl->unit_head;
        DTRACEI(u)
        while(u != TR_null) {
          TR_unit_t *un = &unit(u);
          if((u&7) == (gi->game_frame_mark&7)) {
            bool has_magic_vision = un->magic_vision > 0;
            int x   = (un->x>>POS_SHIFT)>>RANGE_SHIFT,
                y   = (un->y>>POS_SHIFT)>>RANGE_SHIFT,
                vis = VISIBILITY_RANGE(un)>>RANGE_SHIFT,
                opt_fact = (1<<16) / (vis * vis),
                r = x - vis,
                s = x + vis;
            int offset              = (y-vis)*RANGE_MAP_SIZE_X,
                *fov_mapj           = fov_map + offset,
                *magic_vision_mapj  = magic_vision_map + offset;
            for(int j = y-vis, endj = y+vis; j <= endj; j++) {
              if(j >= 0 && j < RANGE_MAP_SIZE_Y) {
                int *fov_mapi           = fov_mapj + r,
                    *magic_vision_mapi  = magic_vision_mapj + r;
                for(int i = r; i <= s; i++) {
                  if((*fov_mapi != 255 || has_magic_vision)
                  && i >= 0 
                  && i < RANGE_MAP_SIZE_X) {
                    int light = UN_GetLight(gi, u, i, j, x, y, opt_fact);
                    *fov_mapi = MAX(*fov_mapi, light);
                    if(has_magic_vision)
                      *magic_vision_mapi = MAX(*magic_vision_mapi, light);
                  }
                  fov_mapi++;
                  magic_vision_mapi++;
                }
              }
              fov_mapj += RANGE_MAP_SIZE_X;
              magic_vision_mapj += RANGE_MAP_SIZE_X;
            }
          }
          u = un->pnext;
        }
      }
      p = pl->next;
    }
  }
}

void GI_SnapUnits(TR_game_instance_t *gi) {
  /* clearance */
  memcpy(gi->clearance_patch, gi->clearance_map_base2, sizeof(int)*MAP_SIZE);
  /* units */
  TR_unit_ptr u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(!CAN_BE_PUSHED(un))
      GI_DrawUnitClearanceContribution(gi, u, gi->clearance_patch);
    u = un->next;
  }
  u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(CAN_BE_PUSHED(un)) {
      int x         = un->x + UN_GetSizeShift(gi, SIZE(un)),
          y         = un->y + UN_GetSizeShift(gi, SIZE(un)),
          demi      = 1<<(POS_SHIFT-1),
          x0        = (((x-demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
          x1        = (((x+demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
          y0        = (((y-demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
          y1        = (((y+demi)>>POS_SHIFT)<<POS_SHIFT)+demi,
          s         = x - x0,
          t         = y - y0,
          sv0       = AT(clearance_patch, x0>>POS_SHIFT, y0>>POS_SHIFT),
          sv1       = AT(clearance_patch, x1>>POS_SHIFT, y0>>POS_SHIFT),
          sv2       = AT(clearance_patch, x0>>POS_SHIFT, y1>>POS_SHIFT),
          sv3       = AT(clearance_patch, x1>>POS_SHIFT, y1>>POS_SHIFT),
          tv0       = sv0 + (sv1 - sv0) * s / (x1 - x0),
          tv1       = sv2 + (sv3 - sv2) * s / (x1 - x0),
          v         = tv0 + (tv1 - tv0) * t / (y1 - y0),
          half_size = SIZE(un)<<(POS_SHIFT-1);
      /* we don't want threshold here 
         because the units shake if there is one */
      if(v < half_size) {
        GI_BuildPatchEdges(gi, u);
        GI_SnapUnit(gi, u);
      }
    }
    u = un->next;
  }
}

void GI_DrawUnitClearanceContribution(TR_game_instance_t *gi, TR_unit_ptr u, int clearance_map[MAP_SIZE]) {
  TR_unit_t *un = &unit(u);
  REAL_SOCLE(MAX_UNIT_SIZE*2+SIZE(un), un->x, un->y) {
    if(INSIDE_MAP(i, j)) {
      int pos_x   = (i<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
          pos_y   = (j<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
          pos2_x  = un->x + UN_GetSizeShift(gi, SIZE(un)),
          pos2_y  = un->y + UN_GetSizeShift(gi, SIZE(un)),
          dist    = TR_ISqrt((pos2_x-pos_x) * (pos2_x-pos_x) + (pos2_y-pos_y) * (pos2_y-pos_y));
      dist = MAX(0, dist-(SIZE(un)<<(POS_SHIFT-1)));
      if(dist < AT2(clearance_map, i, j))
        AT2(clearance_map, i, j) = dist;
    }
  }
}

void GI_DrawTreesClearanceContribution(TR_game_instance_t *gi, int x0, int y0, int x1, int y1) {
  int xx0 = x0>>POS_SHIFT,
      xx1 = x1>>POS_SHIFT,
      yy0 = y0>>POS_SHIFT,
      yy1 = y1>>POS_SHIFT;
  memcpy(gi->clearance_map_base2, gi->clearance_map_base, sizeof(int)*MAP_SIZE);
  FOR_RECT(xx0, xx1, yy0, yy1) {
    bool vis = AT_ENV(i, j) != TR_WALL;
    TCOD_map_set_properties(gi->wall_map, i, j, vis, vis);
  }
  TR_tree_ptr tt = gi->tree_head;
  while(tt != TR_null) {
    TR_tree_t *tree = &tree(tt);
    int size      = MAX_UNIT_SIZE*2+tree->size,
        big_size  = size<<POS_SHIFT;
    if(tree->x + big_size >= x0 && tree->x - big_size <= x1
    && tree->y + big_size >= y0 && tree->y - big_size <= y1) {
      REAL_SOCLE(size, tree->x, tree->y) {
        if(INSIDE_MAP(i, j)) {
          int pos_x   = (i<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
              pos_y   = (j<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
              pos2_x  = tree->x,
              pos2_y  = tree->y,
              dist    = TR_ISqrt((pos2_x-pos_x) * (pos2_x-pos_x) + (pos2_y-pos_y) * (pos2_y-pos_y));
          dist = MAX(0, dist-(tree->size<<(POS_SHIFT-1)));
          if(dist == 0)
            TCOD_map_set_properties(gi->wall_map, i, j, false, false);
          if(dist < AT(clearance_map_base2, i, j))
            AT(clearance_map_base2, i, j) = dist;
        }
      }
    }
    tt = tree->next;
  }
}

void GI_BuildPatchEdges(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  int size     = MAX_UNIT_SIZE*2+SIZE(un),
      half     = (int)(floor((float)size*0.5f - 0.5f))<<POS_SHIFT,
      start_i  = (un->x - half)>>POS_SHIFT,
      end_i    = (un->x - half + (size<<POS_SHIFT)-1)>>POS_SHIFT,
      start_j  = (un->y - half)>>POS_SHIFT,
      end_j    = (un->y - half + (size<<POS_SHIFT)-1)>>POS_SHIFT;
  GI_MarchingSquares(
    gi, 
    gi->clearance_patch, 
    gi->patch_edges, 
    &gi->patch_edge_count, 
    gi->edge_patch,
    gi->march_patch,
    start_i, start_j, end_i, end_j, 
    SIZE(un)<<(POS_SHIFT-1));
}

int GI_DistFromEdge(TR_game_instance_t *gi, TR_edge_t *edge, int x, int y) {
  int dex = edge->x1 - edge->x0,
      dey = edge->y1 - edge->y0,
      de  = TR_ISqrt(dex * dex + dey * dey),
      dx  = x - edge->x0,
      dy  = y - edge->y0,
      result;
  if(de == 0)
    result = TR_ISqrt(dx * dx + dy * dy);
  else {
    int dot = (dx * dex + dy * dey) / de;
    if(dot <= 0)
      result = TR_ISqrt(dx * dx + dy * dy);
    else
    if(dot >= de)
      result = TR_ISqrt((x - edge->x1) * (x - edge->x1) + (y - edge->y1) * (y - edge->y1));
    else
      result = ABS(-dey * dx + dex * dy) / de;
  }
  assert(result >= 0);
  return result;
}

void GI_SnapToEdge(TR_game_instance_t *gi, TR_unit_ptr u, TR_edge_t *edge) {
  TR_unit_t *un = &unit(u);
  UN_RemoveFromMap(gi, u);
  int x   = un->x + UN_GetSizeShift(gi, SIZE(un)),
      y   = un->y + UN_GetSizeShift(gi, SIZE(un)),
      dex = edge->x1 - edge->x0,
      dey = edge->y1 - edge->y0,
      de  = TR_ISqrt(dex * dex + dey * dey),
      dx  = x - edge->x0,
      dy  = y - edge->y0;
  if(de == 0) {
    un->x = edge->x0 - UN_GetSizeShift(gi, SIZE(un));
    un->y = edge->y0 - UN_GetSizeShift(gi, SIZE(un));
  } else {
    int dot = (dx * dex + dy * dey) / de;
    if(dot <= 0) {
      un->x = edge->x0 - UN_GetSizeShift(gi, SIZE(un));
      un->y = edge->y0 - UN_GetSizeShift(gi, SIZE(un));
    } else
    if(dot >= de) {
      un->x = edge->x1 - UN_GetSizeShift(gi, SIZE(un));
      un->y = edge->y1 - UN_GetSizeShift(gi, SIZE(un));
    } else {
      un->x = edge->x0 + (edge->x1 - edge->x0) * dot / de - UN_GetSizeShift(gi, SIZE(un));
      un->y = edge->y0 + (edge->y1 - edge->y0) * dot / de - UN_GetSizeShift(gi, SIZE(un));
    }
  }
  UN_PutInMap(gi, u);
}

void GI_SnapUnit(TR_game_instance_t *gi, TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  int x = un->x + UN_GetSizeShift(gi, SIZE(un)),
      y = un->y + UN_GetSizeShift(gi, SIZE(un));
  int min_dist = 0x7fffffff;
  TR_edge_t *min_e = null; 
  DO_TIMES(gi->patch_edge_count) {
    TR_edge_t *edg = &gi->patch_edges[f];
    int dist = GI_DistFromEdge(gi, edg, x, y);
    if(dist < min_dist) {
      min_dist = dist;
      min_e = edg;
    }
  }
  //assert(min_e != null);
  if(min_e != null) {
    int dx = min_e->x1 - min_e->x0,
        dy = min_e->y1 - min_e->y0,
        nx = -dy,
        ny = dx;
    if(un->ddx*nx + un->ddy*ny <= 0)
      GI_SnapToEdge(gi, u, min_e);
  }
}

void GI_InitClearanceMapBase(TR_game_instance_t *gi) {
  for(int l = 0; l < MAP_SIZE_Y; l++)
    for(int k = 0; k < MAP_SIZE_X; k++) {
      AT(clearance_map_base, k, l) = MAX_UNIT_SIZE<<(POS_SHIFT-1);
      AT(clearance_map_base2, k, l) = MAX_UNIT_SIZE<<(POS_SHIFT-1);
    }
  for(int l = 0; l < MAP_SIZE_Y; l++)
    for(int k = 0; k < MAP_SIZE_X; k++) {
      int pos_x = k,
          pos_y = l,
          off = 0, 
          endi, endj;
      bool found = false;
      while(!found 
         && off < MAP_SIZE_X) {
        int i, j;
        endi = pos_x+off;
        for(i = pos_x-off; i <= endi; i++) {
          j = pos_y-off;
          if(INSIDE_MAP(i, j)) {
            if(AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT)
            == TR_WALL) {
              int x0 = pos_x<<POS_SHIFT,
                  y0 = pos_y<<POS_SHIFT,
                  x1 = i<<POS_SHIFT,
                  y1 = j<<POS_SHIFT;
              int dist = TR_ISqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1)) - (1<<(POS_SHIFT-1));
              AT(clearance_map_base, pos_x, pos_y) = 
                MIN(AT(clearance_map_base, pos_x, pos_y), dist);
              found = true;
            }
          }
        }
        endi = pos_x+off;
        for(i = pos_x-off; i <= endi; i++) {
          j = pos_y+off;
          if(INSIDE_MAP(i, j)) {
            if(AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT)
            == TR_WALL) {
              int x0 = pos_x<<POS_SHIFT,
                  y0 = pos_y<<POS_SHIFT,
                  x1 = i<<POS_SHIFT,
                  y1 = j<<POS_SHIFT;
              int dist = TR_ISqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1)) - (1<<(POS_SHIFT-1));
              AT(clearance_map_base, pos_x, pos_y) = 
                MIN(AT(clearance_map_base, pos_x, pos_y), dist);
              found = true;
            }
          }
        }
        endj = pos_y+off;
        for(j = pos_y-off; j <= endj; j++) {
          i = pos_x-off;
          if(INSIDE_MAP(i, j)) {
            if(AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT)
            == TR_WALL) {
              int x0 = pos_x<<POS_SHIFT,
                  y0 = pos_y<<POS_SHIFT,
                  x1 = i<<POS_SHIFT,
                  y1 = j<<POS_SHIFT;
              int dist = TR_ISqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1)) - (1<<(POS_SHIFT-1));
              AT(clearance_map_base, pos_x, pos_y) = 
                MIN(AT(clearance_map_base, pos_x, pos_y), dist);
              found = true;
            }
          }
        }
        endj = pos_y+off;
        for(j = pos_y-off; j <= endj; j++) {
          i = pos_x+off;
          if(INSIDE_MAP(i, j)) {
            if(AT_ENV(i>>RANGE_SHIFT, j>>RANGE_SHIFT)
            == TR_WALL) {
              int x0 = pos_x<<POS_SHIFT,
                  y0 = pos_y<<POS_SHIFT,
                  x1 = i<<POS_SHIFT,
                  y1 = j<<POS_SHIFT;
              int dist = TR_ISqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1)) - (1<<(POS_SHIFT-1));
              AT(clearance_map_base, pos_x, pos_y) = 
                MIN(AT(clearance_map_base, pos_x, pos_y), dist);
              found = true;
            }
          }
        }
        off++;
      }
    }
  //memcpy(gi->clearance_map_base2, gi->clearance_map_base, sizeof(int)*MAP_SIZE);
}

void GI_DrawUnitClearance(TR_game_instance_t *gi, int size, int x, int y, int *clearance_map) {
  int size2 = size>>POS_SHIFT;
  REAL_SOCLE(MAX_UNIT_SIZE*2+size2, x, y) {
    if(INSIDE_MAP(i, j)) {
      int pos_x = (i<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
          pos_y = (j<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
          pos2_x = x + (size2 > 0 ? UN_GetSizeShift(gi, size2) : 0),
          pos2_y = y + (size2 > 0 ? UN_GetSizeShift(gi, size2) : 0);
      int dist = TR_ISqrt((pos2_x-pos_x) * (pos2_x-pos_x) + (pos2_y-pos_y) * (pos2_y-pos_y));
      dist = MAX(0, dist-(size>>1));
      AT2(clearance_map, i, j) = MIN(AT2(clearance_map, i, j), dist);
    }
  }
}

void GI_MarchingSquares(TR_game_instance_t *gi, int clearance_map[MAP_SIZE], TR_edge_t *edges, int *counter, int edge_map[MAP_SIZE][2], short march_map[MAP_SIZE], int x0, int y0, int x1, int y1, int half_size) {
  static
  int one_edge_idx[16][4] = {
    {-1, -1, -1, -1}, /* 0 */
    {3, 2, 3, 0}, /* 1 */
    {2, 1, 2, 3}, /* 2 */
    {2, 1, 3, 0}, /* 3 */
    {1, 0, 1, 2}, /* 4 */
    {-1, -1, -1, -1}, /* 5 */
    {1, 0, 2, 3}, /* 6 */
    {1, 0, 3, 0}, /* 7 */
    {0, 3, 0, 1}, /* 8 */
    {3, 2, 0, 1}, /* 9 */
    {-1, -1, -1, -1}, /* 10 */
    {2, 1, 0, 1}, /* 11 */
    {0, 3, 1, 2}, /* 12 */
    {3, 2, 1, 2}, /* 13 */
    {0, 3, 2, 3}, /* 14 */
    {-1, -1, -1, -1}, /* 15 */
  },
  two_edge_idx[16][2] = {
    {-1, -1}, /* 0 */
    {-1, -1}, /* 1 */
    {-1, -1}, /* 2 */
    {-1, -1}, /* 3 */
    {-1, -1}, /* 4 */
    {1, 4}, /* 5 */
    {-1, -1}, /* 6 */
    {-1, -1}, /* 7 */
    {-1, -1}, /* 8 */
    {-1, -1}, /* 9 */
    {8, 2}, /* 10 */
    {-1, -1}, /* 11 */
    {-1, -1}, /* 12 */
    {-1, -1}, /* 13 */
    {-1, -1}, /* 14 */
    {-1, -1}, /* 15 */
  };
  *counter = 0;
  FOR_RECT(x0, x1-1, y0, y1-1) {
    int v[4] = {AT2(clearance_map, i, j),
                AT2(clearance_map, i+1, j),
                AT2(clearance_map, i+1, j+1),
                AT2(clearance_map, i, j+1)},
        f[4] = {v[0] < half_size ? 1 : 0,
                v[1] < half_size ? 1 : 0,
                v[2] < half_size ? 1 : 0,
                v[3] < half_size ? 1 : 0},
        mask = (f[0]<<3)|(f[1]<<2)|(f[2]<<1)|f[3],
        pos_x = (i<<POS_SHIFT) + (1<<(POS_SHIFT-1)),
        pos_y = (j<<POS_SHIFT) + (1<<(POS_SHIFT-1));
    AT2(march_map, i, j) = mask;
    switch(mask) {
      case 0:
      case 15:
        /* no edge */
        AT2(edge_map, i, j)[0] = -1;
        AT2(edge_map, i, j)[1] = -1;
        break;
      case 5:
      case 10:
        AT2(edge_map, i, j)[0] = GI_BuildEdge(gi, edges, counter, v, f, one_edge_idx[two_edge_idx[mask][0]], half_size, pos_x, pos_y, i, j);
        AT2(edge_map, i, j)[1] = GI_BuildEdge(gi, edges, counter, v, f, one_edge_idx[two_edge_idx[mask][1]], half_size, pos_x, pos_y, i, j);
        break;
      default:
        AT2(edge_map, i, j)[0] = GI_BuildEdge(gi, edges, counter, v, f, one_edge_idx[mask], half_size, pos_x, pos_y, i, j);
        AT2(edge_map, i, j)[1] = -1;
        break;
    }
  }
}

void GI_BuildReflex(TR_game_instance_t *gi, TR_edge_t *edges, int edge_map[MAP_SIZE][2], int edge_count) {
  /* adjacency */
  FOR_RECT(0, MAP_SIZE_X-1, 0, MAP_SIZE_Y-1) {
    int x = i,
        y = j,
        e0 = AT2(edge_map, x, y)[0],
        e1 = AT2(edge_map, x, y)[1];
    if(e0 != -1) {
      TR_edge_t *edge0 = &edges[e0];
      FOR_RECT(x-1, x+1, y-1, y+1)
        if(INSIDE_MAP(i, j)
        && (i != x || j != y)) {
          int e2 = AT2(edge_map, i, j)[0],
              e3 = AT2(edge_map, i, j)[1];
          if(e2 != -1) {
            TR_edge_t *edge2 = &edges[e2];
            if(edge0->x0 == edge2->x1
            && edge0->y0 == edge2->y1)
              edge0->e0 = e2;
            else
            if(edge0->x1 == edge2->x0
            && edge0->y1 == edge2->y0)
              edge0->e1 = e2;
            if(e3 != -1) {
              TR_edge_t *edge3 = &edges[e3];
              if(edge0->x0 == edge3->x1
              && edge0->y0 == edge3->y1)
                edge0->e0 = e3;
              else
              if(edge0->x1 == edge3->x0
              && edge0->y1 == edge3->y0)
                edge0->e1 = e3;
            }
          }
          if(e1 != -1) {
            TR_edge_t *edge1 = &edges[e1];
            if(e2 != -1) {
              TR_edge_t *edge2 = &edges[e2];
              if(edge1->x0 == edge2->x1
              && edge1->y0 == edge2->y1)
                edge1->e0 = e2;
              else
              if(edge1->x1 == edge2->x0
              && edge1->y1 == edge2->y0)
                edge1->e1 = e2;
              if(e3 != -1) {
                TR_edge_t *edge3 = &edges[e3];
                if(edge1->x0 == edge3->x1
                && edge1->y0 == edge3->y1)
                  edge1->e0 = e3;
                else
                if(edge1->x1 == edge3->x0
                && edge1->y1 == edge3->y0)
                  edge1->e1 = e3;
              }
            }
          }
        }
    }
  }
  /* reflex */
  DO_TIMES(edge_count) {
    TR_edge_t *edg = &edges[f];
    if(edg->e0 != -1) {
      TR_edge_t *edg0 = &edges[edg->e0];
      if(edg->r0 == -1) {
        assert(edg->x0 == edg0->x1 && edg->y0 == edg0->y1);
        int p0x = edg->x1, 
            p0y = edg->y1,
            p1x = edg->x0, 
            p1y = edg->y0,
            p2x = edg0->x0, 
            p2y = edg0->y0;
        if((p0x - p1x) * (p2y - p1y) - (p2x - p1x) * (p0y - p1y) < 0)
          edg->r0 = edg0->r1 = 1;
        else
          edg->r0 = edg0->r1 = 0;
      }
      if(edg->e1 != -1) {
        TR_edge_t *edg1 = &edges[edg->e1];
        if(edg->r1 == -1) {
          assert(edg->x1 == edg1->x0 && edg->y1 == edg1->y0);
          int p0x = edg->x0, 
              p0y = edg->y0,
              p1x = edg->x1, 
              p1y = edg->y1,
              p2x = edg1->x1, 
              p2y = edg1->y1;
          if((p0x - p1x) * (p2y - p1y) - (p2x - p1x) * (p0y - p1y) > 0)
            edg->r1 = edg1->r0 = 1;
          else
            edg->r1 = edg1->r0 = 0;
        }
      }
    }
  }
}

inline
int GI_BuildEdge(TR_game_instance_t *gi, TR_edge_t *edges, int *counter, int *v, int *f, int *idx, int iso, int pos_x, int pos_y, int i, int j) {
  static 
  int square_x[4] = {0, 1<<POS_SHIFT, 1<<POS_SHIFT, 0},
      square_y[4] = {0, 0, 1<<POS_SHIFT, 1<<POS_SHIFT};
  int i0 = idx[0],
      i1 = idx[1],
      i2 = idx[2],
      i3 = idx[3],
      a  = iso - v[i0],
      b  = iso - v[i2];
  int result = (*counter)++;
  TR_edge_t *edge = &edges[result];
  edge->x0 = pos_x + square_x[i0] + a * (square_x[i1] - square_x[i0]) / (v[i1] - v[i0]);
  edge->y0 = pos_y + square_y[i0] + a * (square_y[i1] - square_y[i0]) / (v[i1] - v[i0]);
  edge->x1 = pos_x + square_x[i2] + b * (square_x[i3] - square_x[i2]) / (v[i3] - v[i2]);
  edge->y1 = pos_y + square_y[i2] + b * (square_y[i3] - square_y[i2]) / (v[i3] - v[i2]);
  edge->i  = i;
  edge->j  = j;
  edge->e0 = edge->e1 = -1;
  edge->r0 = edge->r1 = -1;
  return result;
}

void GI_UnitRepulsion(TR_game_instance_t *gi) {
  /* fov */
  TR_unit_ptr u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    int env_x = (UNIT_X(un))>>(POS_SHIFT+RANGE_SHIFT),
        env_y = (UNIT_Y(un))>>(POS_SHIFT+RANGE_SHIFT);
    if(un->old_env_x != env_x
    || un->old_env_y != env_y) {
      TCOD_map_compute_fov(
        FLYING(un) ? un->visibility_map : gi->wall_map,
        un->visibility_map, 
        env_x, 
        env_y, 
        VISIBILITY_RANGE(un)>>RANGE_SHIFT, 
        true, 
        FOV_RESTRICTIVE);
      un->old_env_x = env_x;
      un->old_env_y = env_y;
    }
    u = un->next;
  }
  /* colisions */
  static short         colision_matrix[MAX_UNIT][MAX_UNIT];
  static TR_contact_t pool[MAX_CONTACT];
  TR_contact_ptr      free_contacts = 0,
                       contacts      = TR_null;
  /* find contacts */
  //DTRACE("DD")
  gi->colision_mark++;
  gi->penetration_mark++;
  u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    /* penetration and contacts */
    GI_FindPenetrations(gi, colision_matrix, u);
    //GI_FindUnitContacts(gi, colision_matrix, pool, u, &free_contacts, &contacts);
    un->ddx = un->dx;
    un->ddy = un->dy;
    //DTRACEI(un->ddx)
    //DTRACEI(un->ddy)
    if(UO_GetFrontType(gi, u) == TR_unit_order_none
    && TR_ISqrt(un->ddx*un->ddx + un->ddy*un->ddy) > 60)
      UN_LookAt(gi, u, un->x+un->ddx*100, un->y+un->ddy*100);
    u = un->next;
  }
  /* handle contacts */
  while(contacts != TR_null) {
    TR_contact_ptr c = contacts;
    contacts = contact(contacts).next;
    TR_contact_t *cont = &contact(c);
    assert(cont->ua!=cont->ub);
    TR_unit_t *una = &unit(cont->ua);
    TR_unit_t *unb = &unit(cont->ub);
    assert(cont->t >= 0 && cont->t <= 100);
    UN_RemoveFromMap(gi, cont->ua);
    una->x += una->dx * cont->t / 100;
    una->y += una->dy * cont->t / 100;
    UN_PutInMap(gi, cont->ua);
    UN_RemoveFromMap(gi, cont->ub);
    unb->x += unb->dx * cont->t / 100;
    unb->y += unb->dy * cont->t / 100;
    UN_PutInMap(gi, cont->ub);
    int drax  = una->dx * (100-cont->t) / 100,
        dray  = una->dy * (100-cont->t) / 100,
        drbx  = unb->dx * (100-cont->t) / 100,
        drby  = unb->dy * (100-cont->t) / 100,
        nx    = unb->x-una->x,
        ny    = unb->y-una->y,
        tx    = -ny,
        ty    = nx,
        dts   = tx*tx + ty*ty;
    assert(dts > 0);
    int dt    = TR_ISqrt(dts),
        da    = (tx * drax + ty * dray) / dt,
        db    = (tx * drbx + ty * drby) / dt;
    if(nx * una->dx + ny * una->dy > 0) {
      una->dx = (tx * da/* - nx * da*/) / dt;
      una->dy = (ty * da/* - ny * da*/) / dt;
    } else {
      una->dx = drax;
      una->dy = dray;
    }
    if(nx * unb->dx + ny * unb->dy < 0) {
      unb->dx = (tx * db/* + nx * db*/) / dt;
      unb->dy = (ty * db/* + ny * db*/) / dt;
    } else {
      unb->dx = drbx;
      unb->dy = drby;
    }
    gi->colision_mark++;
    GI_RemoveUnitContacts(gi, pool, &contacts, cont->ua, cont->ub);
    GI_FindUnitContacts(gi, colision_matrix, pool, cont->ua, &free_contacts, &contacts);
    GI_FindUnitContacts(gi, colision_matrix, pool, cont->ub, &free_contacts, &contacts);
  }
  /* move and look at */
  u = gi->unit_head;
  while(u != TR_null) {
    TR_unit_t *un = &unit(u);
    if(CAN_BE_PUSHED(un)) {
      /* move */
      UN_RemoveFromMap(gi, u);
      un->x += un->dx;
      un->y += un->dy;
      UN_PutInMap(gi, u);
      un->dx = 0;
      un->dy = 0;
      /* look at */
      int lookat_x = (un->lookat_x + 3*un->old_lookat_x)>>2,
          lookat_y = (un->lookat_y + 3*un->old_lookat_y)>>2;
      un->old_lookat_x = lookat_x;
      un->old_lookat_y = lookat_y;
      /*un->node->lookAt(
        Ogre::Vector3(
          (float)(un->x+lookat_x)/(1<<POS_SHIFT), 
          Y_OFFSET(un) + (FLYING(un) ? FLYING_HEIGHT : 0), 
          (float)(un->y+lookat_y)/(1<<POS_SHIFT)),
        Ogre::Node::TS_WORLD,
        Ogre::Vector3::UNIT_Z);*/
    }
    if(un->old_lookat_x != 0) 
      un->node->setScale(
        Ogre::Vector3(
          (un->old_lookat_x > 0 ? -1.f : 1.f)*0.25*SIZE(un)*MODEL_SCALE(un), 
          0.25*SIZE(un)*MODEL_SCALE(un), 
          0.25*SIZE(un)*MODEL_SCALE(un)));
    u = un->next;
  }
}

void GI_FindUnitContacts(TR_game_instance_t *gi, short colision_matrix[MAX_UNIT][MAX_UNIT], TR_contact_t pool[], TR_unit_ptr u, TR_contact_ptr *free_contacts, TR_contact_ptr *contacts) {
  TR_unit_t *un = &unit(u);
  if(CAN_BE_PUSHED(un)) {
    TR_get_unit_here_t data;
    UN_GetUnitsHere(&data, gi, u, un->team_id, SIZE(un)+1, un->x, un->y, !FLYING(un), FLYING(un));
    DO_TIMES(data.count) {
      TR_unit_ptr u2 = data.result[f];
      TR_unit_t *un2 = &unit(u2);
      if(CAN_BE_PUSHED(un2)
      && un2->colision_mark != gi->colision_mark
      && colision_matrix[u][u2] == 0) {
        colision_matrix[u][u2] =
        colision_matrix[u2][u] = 1;
        TR_contact_ptr c   = (*free_contacts)++;
        assert(*free_contacts < MAX_CONTACT);
        TR_contact_t *cont = &contact(c);
        cont->xa            = un->x + UN_GetSizeShift(gi, SIZE(un));
        cont->ya            = un->y + UN_GetSizeShift(gi, SIZE(un));
        cont->xb            = un2->x + UN_GetSizeShift(gi, SIZE(un2));
        cont->yb            = un2->y + UN_GetSizeShift(gi, SIZE(un2));
        cont->ra            = 90*(SIZE(un)<<(POS_SHIFT-1))/100;
        cont->rb            = 90*(SIZE(un2)<<(POS_SHIFT-1))/100;
        cont->ua            = u;
        cont->ub            = u2;
        cont->next          = TR_null;
        if(GI_HandlePotentialCollision(gi, pool, c, un->dx - un2->dx, un->dy - un2->dy))
          GI_InsertContact(gi, pool, c, contacts);
        else
          (*free_contacts)--;
      }
    }
    un->colision_mark = gi->colision_mark;
  }
}

void GI_FindPenetrations(TR_game_instance_t *gi, short colision_matrix[MAX_UNIT][MAX_UNIT], TR_unit_ptr u) {
  TR_unit_t *un = &unit(u);
  if(CAN_BE_PUSHED(un)) {
    TR_get_unit_here_t data;
    UN_GetUnitsHere(&data, gi, u, un->team_id, SIZE(un)+1, un->x, un->y, !FLYING(un), FLYING(un));
    DO_TIMES(data.count) {
      TR_unit_ptr u2 = data.result[f];
      TR_unit_t *un2 = &unit(u2);
      if(CAN_BE_PUSHED(un2)
      && un2->penetration_mark != gi->penetration_mark) {
        int dx = UNIT_X(un)-UNIT_X(un2),
            dy = UNIT_Y(un)-UNIT_Y(un2),
            r  = 4*90*((SIZE(un)+SIZE(un2))<<(POS_SHIFT-1))/(3*100),
            ds = dx*dx + dy*dy;
        assert(ds >= 0);
        int d = TR_ISqrt(ds);
        if(d < r) {
          if(d != 0) {
            int ddx = (dx * (r-d) / d)>>1,
                ddy = (dy * (r-d) / d)>>1;
            if(ABS(ddx) < 1<<(POS_SHIFT-6) 
            && ABS(ddy) < 1<<(POS_SHIFT-6)) 
              ddx = ddy = 0;
            if(un->player_id == un2->player_id
            && UN_CheckFollowTarget(gi, u, u2)) {
              un->dx += ddx>>1;
              un->dy += ddy>>1;
            } else
            if(un->player_id == un2->player_id
            && UN_CheckFollowTarget(gi, u2, u)) {
              un2->dx -= ddx>>1;
              un2->dy -= ddy>>1;
            } else {
              if(un->team_id != un2->team_id) {
                /*if(UO_GetFrontType(gi, u) == TR_unit_order_none
                || UO_GetFrontType(gi, u2) == TR_unit_order_none)*/ {
                  if(UO_GetFrontType(gi, u) != TR_unit_order_skill) {
                    un->dx  += ddx>>1;
                    un->dy  += ddy>>1;
                  }
                  if(UO_GetFrontType(gi, u2) != TR_unit_order_skill) {
                    un2->dx -= ddx>>1;
                    un2->dy -= ddy>>1;
                  }
                }
              } else
              if(UO_GetFrontType(gi, u)
              == UO_GetFrontType(gi, u2)) {
                /* if not, some are pushed and aren't at range anymore 
                   and it happens in sort a way the player can say what 
                   happened */
                if(UO_GetFrontType(gi, u) != TR_unit_order_skill) {
                  un->dx  += ddx>>1;
                  un->dy  += ddy>>1;
                  un2->dx -= ddx>>1;
                  un2->dy -= ddy>>1;
                }
              } else
              if(UO_GetFrontType(gi, u) 
               < UO_GetFrontType(gi, u2)) {
                un->dx += ddx>>1;
                un->dy += ddy>>1;
              } else {
                un2->dx -= ddx>>1;
                un2->dy -= ddy>>1;
              }
            }
          }
          colision_matrix[u][u2] =
          colision_matrix[u2][u] = -1;
        } else {
          colision_matrix[u][u2] =
          colision_matrix[u2][u] = 0;
        }
      }
    }
    /* update unit orders */
    if(UO_GetFrontType(gi, u)
    == TR_unit_order_move
    && UO_GetFront(gi, u)->started) {
      TR_unit_order_t *uo = UO_GetFront(gi, u);
      int dist;
      if(uo->target != TR_null)
        dist = UN_GetDist(gi, u, uo->target);
      else
        dist = UN_GetDist(gi, u, un->chase_cx, un->chase_cy);
      if(dist <= TR_ISqrt(uo->r_squared)<<POS_SHIFT) {
        un->blocked_time += gi->game_time_step;
        if((un->blocked_time > MAX_BLOCKED_TIME
        && data.count > 0)
        || data.count > 8
        || dist <= (1<<(POS_SHIFT-2))) {
          /* goal reached */
          un->blocked_time = 0;
          UO_FreeFront(gi, u, true);
          if(UO_GetFrontType(gi, u) 
          == TR_unit_order_follow_goal)
            UO_GetFront(gi, u)->done = true;
        }
      }
    }
    un->penetration_mark = gi->penetration_mark;
  }
}

bool GI_HandlePotentialCollision(TR_game_instance_t *gi, TR_contact_t pool[], TR_contact_ptr c, int dx, int dy) {
  TR_contact_t *cont = &contact(c);
  int dvels = dx*dx + dy*dy;
  assert(dvels >= 0);
  if(dvels < 0)
    dvels = 0x7fffffff;
  assert(dvels >= 0);
  int dvel = TR_ISqrt(dvels);
  if(dvel != 0) {
    int dcx = cont->xb - cont->xa,
        dcy = cont->yb - cont->ya,
        d   = (dx * dcx + dy * dcy) / dvel,
        rr  = cont->ra+cont->rb,
        dc  = dcx*dcx + dcy*dcy;
    if(d > 0) {
      int f = dc - d*d,
          rr_squared = rr * rr;
      /* if f is crazily negative, it smashes the sqrt */
      if(f >= 0 
      && f <= rr_squared) {
        int ts = rr_squared - f;
        assert(ts >= 0);
        int t = TR_ISqrt(ts);
        if(d >= t) {
          int k = d - t;
          if(k <= dvel) {
            cont->t = 100 * (d - t) / dvel;
            return true;
          }
        }
      }
    }
  }
  return false;
}

void GI_InsertContact(TR_game_instance_t *gi, TR_contact_t pool[], TR_contact_ptr c, TR_contact_ptr *h) {
  if(*h == TR_null)
    *h = c;
  else {
    TR_contact_ptr n    = *h,
                    prev = TR_null;
    while(n != TR_null
       && contact(n).t < contact(c).t) {
      TR_contact_t *cont = &contact(n);
      prev = n;
      n = cont->next;
    }
    if(n == TR_null)
      contact(prev).next = c;
    else 
    if(prev == TR_null) {
      contact(c).next = *h;
      *h = c;
    } else {
      contact(prev).next = c;
      contact(c).next = n;
    }
  }
}

void GI_RemoveUnitContacts(TR_game_instance_t *gi, TR_contact_t pool[], TR_contact_ptr *h, TR_unit_ptr ua, TR_unit_ptr ub) {
  TR_contact_ptr n    = *h,
                  prev = TR_null;
  while(n != TR_null) {
    TR_contact_t *cont = &contact(n);
    if(cont->ua == ua
    || cont->ua == ub
    || cont->ub == ua
    || cont->ub == ub) {
      if(prev == TR_null) {
        n = cont->next;
        *h = n;
      } else
        contact(prev).next = 
        n = cont->next;
    } else {
      prev = n;
      n = cont->next;
    }
  }
}

void GI_UpdateClients(TR_game_instance_t *gi) {
  if(gi->client_head != TR_null) {
    TR_client_ptr c = gi->client_head;
    while(c != TR_null) {
      TR_client_t *cl = &client(c);
      if(CL_Update(gi, c)) {
        TR_client_ptr c2 = c;
        c = cl->next;
        CL_Free(gi, c2);
      } else
        c = cl->next;
    }
  }
}

void GI_UpdateCameras(TR_game_instance_t *gi) {
  if(gi->camera_head != TR_null) {
    TR_camera_ptr c = gi->camera_head;
    while(c != TR_null) {
      TR_camera_t *cam = &camera(c);
      CAM_Update(gi, c);
      c = cam->next;
    }
  }
}

void GI_UpdateRemoteStores(TR_game_instance_t *gi) {
  TR_remote_cmd_store_ptr r = gi->remote_cmd_store_head;
  while(r != TR_null) {
    RS_Update(gi, r);
    r = remote_cmd_store(r).next;
  }
}

void GI_EndOfTurn(TR_game_instance_t *gi) {
  CAM_CloseWaitWindow(gi, GI_GetCamera(gi));
  GI_UpdateAverWaitTime(gi, gi->wait_time);
  TR_client_ptr c = gi->client_head;
  while(c != TR_null) {
    CL_EndOfTurn(gi, c);
    c = client(c).next;
  }
  gi->frame_time_step = NORMAL_TIME_STEP;
  gi->turn_frame_count = STARTING_TURN_FRAME_COUNT;
  unsigned int hash = 0;
  TR_player_ptr p = gi->player_head;
  while(p != TR_null) {
    TR_cmd_ptr r = cmd_store(player(p).cs).cmd_head;
    while(r != TR_null) {
      TR_cmd_t *cmd = &cmd(r);
      if(cmd->type == TR_cmd_type_end_of_turn
      && cmd->turn == gi->turn+1) {
        gi->turn_frame_count  = player(p).human ? MAX(gi->turn_frame_count, cmd->turn_frame_count) : gi->turn_frame_count;
        gi->frame_time_step   = player(p).human ? MAX(gi->frame_time_step, cmd->frame_time_step) : gi->frame_time_step;
        if(cmd->hash != 0) {
          if(hash != 0
          && hash != cmd->hash) {
            gi->desync = true;
            gi->game_time_step = 0;
            log("desynced");
            log((int)hash);
            log((int)cmd->hash);
            GI_DumpDesyncedTrace(gi);
            assert(false && "desynced");
          }
          hash = cmd->hash;
        }
      }
      r = cmd->cmd_next;
    }
    p = player(p).next;
  }
  gi->turn++;
  gi->turn_acc        = 0;
  gi->wait_time       = 0;
}

void GI_UpdateAverFrameTime(TR_game_instance_t *gi, TR_time_t frame_time) {
  if(gi->game_time_step) {
    gi->aver_frame_times[gi->aver_frame_times_counter++] = frame_time;
    if(gi->aver_frame_times_counter >= MAX_AVER_FRAME_TIMES)
      gi->aver_frame_times_counter = 0;
    gi->aver_frame_time = 0;
    DO_TIMES(MAX_AVER_FRAME_TIMES)
      gi->aver_frame_time += gi->aver_frame_times[f];
    gi->aver_frame_time /= MAX_AVER_FRAME_TIMES;
    /*DO_TIMES(MAX_AVER_FRAME_TIMES)
      gi->aver_frame_time = 
        MAX(gi->aver_frame_time, gi->aver_frame_times[f]);
    gi->aver_frame_time = MIN(gi->aver_frame_time, MAX_TIME_STEP);*/
  }
}

void GI_UpdateAverWaitTime(TR_game_instance_t *gi, TR_time_t wait_time) {
  gi->aver_wait_times[gi->aver_wait_times_counter++] = wait_time;
  if(gi->aver_wait_times_counter >= MAX_AVER_WAIT_TIMES)
    gi->aver_wait_times_counter = 0;
  gi->aver_wait_time = 0;
  DO_TIMES(MAX_AVER_WAIT_TIMES)
    gi->aver_wait_time += gi->aver_wait_times[f];
  gi->aver_wait_time /= MAX_AVER_WAIT_TIMES;
}

void GI_UpdateMaxPing(TR_game_instance_t *gi) {
  /* todo: use the 99th percentile */
  gi->pings[gi->max_ping_times_counter++] = gi->ping;
  if(gi->max_ping_times_counter >= MAX_MAX_PING_TIMES)
    gi->max_ping_times_counter = 0;
  gi->max_ping = 0;
  DO_TIMES(MAX_MAX_PING_TIMES)
    gi->max_ping = MAX(gi->max_ping, gi->pings[f]);
}

void GI_UpdateInputs(TR_game_instance_t *gi) {
  gi->mouse.x = gi->ogre_mouse->getMouseState().X.abs;
  gi->mouse.y = gi->ogre_mouse->getMouseState().Y.abs;
  bool was_ldown = gi->lbtn_down_cache,
       was_rdown = gi->rbtn_down_cache;
  gi->lbtn_down = gi->lbtn_down_cache = gi->ogre_mouse->getMouseState().buttons&(1<<0);
  gi->rbtn_down = gi->rbtn_down_cache = gi->ogre_mouse->getMouseState().buttons&(1<<1);
  gi->lbtn_up = was_ldown && !gi->lbtn_down_cache;
  gi->rbtn_up = was_rdown && !gi->rbtn_down_cache;
  /* hardware mouse cursor */
  #ifdef WIN32
  if(GetCursor() 
  || gi->last_cursor_tex != gi->cursor_tex) {
    gi->last_cursor_tex = gi->cursor_tex;
    Ogre::TexturePtr cursor_tex = 
      Ogre::TextureManager::getSingleton().getByName(gi->cursor_tex + ".png");
    cursor_tex->load();
    Ogre::D3D9TexturePtr d3dtexture = (Ogre::D3D9TexturePtr)cursor_tex;
    IDirect3DTexture9*   normTexture = d3dtexture->getNormTexture();
    IDirect3DSurface9* surface;
    normTexture->GetSurfaceLevel(0, &surface); 
    SetCursor(NULL);
    ((Ogre::D3D9RenderSystem*)gi->ogre_root->getRenderSystem())->
      getDeviceManager()->getActiveDevice()->getD3D9Device()->
      SetCursorProperties(0, 0, surface);
    ((Ogre::D3D9RenderSystem*)gi->ogre_root->getRenderSystem())->
      getDeviceManager()->getActiveDevice()->getD3D9Device()->
      ShowCursor(true);
  }
  #endif
}

bool GI_IsKeyDown(TR_game_instance_t *gi, OIS::KeyCode k) {
  return gi->ogre_keyboard->isKeyDown(k);
}

bool GI_IsKeyPressed(TR_game_instance_t *gi, OIS::KeyCode k) {
  DO_TIMES(gi->key_pressed_count)
    if(gi->key_pressed[f] == k)
      return true;
  return false;
}

void GI_StopKeyPropagation(TR_game_instance_t *gi, OIS::KeyCode k) {
  DO_TIMES(gi->key_pressed_count)
    if(gi->key_pressed[f] == k) {
      gi->key_pressed[f] = OIS::KC_UNASSIGNED;
      break;
    }
}

OIS::KeyCode GI_CharToKeyCode(TR_game_instance_t *gi, char c) {
  switch(c) {
    #include "ois_char_to_key.cpp"
  }
  assert(false);
  return OIS::KC_UNASSIGNED;
}

void GI_StopClicksPropagation(TR_game_instance_t *gi) {
  gi->lbtn_up   = false;
  gi->rbtn_up   = false;
  gi->lbtn_down = false;
  gi->rbtn_down = false;
}

bool GI_InState(TR_game_instance_t *gi, TR_state_t st) {
  return (gi->state & st) != 0;
}

void GI_AddState(TR_game_instance_t *gi, TR_state_t st) {
  gi->add_state[gi->add_state_count++] = st;
}

void GI_RemoveState(TR_game_instance_t *gi, TR_state_t st) {
  gi->remove_state[gi->remove_state_count++] = st;
}

void GI_UpdateState(TR_game_instance_t *gi) {
  for(int f = 0; f < gi->remove_state_count; f++)
    gi->state &= ~gi->remove_state[f];
  for(int f = 0; f < gi->add_state_count; f++)
    gi->state |= gi->add_state[f];
  gi->remove_state_count = 0;
  gi->add_state_count = 0;
}

void GI_ConnectToMasterServer(TR_game_instance_t *gi) {
  if(game_desc.online) {
    gi->address.host = ENET_HOST_ANY;
    gi->address.port = ENET_PORT_ANY;
    gi->host = enet_host_create(&gi->address, MAX_PEER, 1, 0, 0);
    assert(gi->host != null);
    gi->peer = enet_host_connect(gi->host, &gi->master_server_addr, 1, 0);
    assert(gi->peer != null);
    gi->connection_acc = 0;
    gi->frame_time = 0;
    gi->connected = false;
    enet_host_flush(gi->host);
  }
}

void GI_DisconnectFromMasterServer(TR_game_instance_t *gi) {
  if(gi->peer) {
    enet_peer_disconnect(gi->peer, 0);
    ENetEvent e;
    enet_host_service(gi->host, &e, 500);
    enet_peer_reset(gi->peer);
    gi->peer = null;
  }
}

void GI_KillHost(TR_game_instance_t *gi) {
  GI_DisconnectFromMasterServer(gi);
  if(gi->host) {
    enet_host_destroy(gi->host);
    gi->host = null;
  }
}

bool GI_ListenMasterServer(TR_game_instance_t *gi, ENetEvent *e) {
  if(game_desc.online) {
    enet_host_flush(gi->host);
    enet_host_service(gi->host, e, 0);
  }
}

bool GI_CheckForTimeout(TR_game_instance_t *gi, ENetEvent *e, TR_state_t current_state) {
  if(game_desc.online) {
    switch(e->type) {
      case ENET_EVENT_TYPE_RECEIVE:
        TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
        switch(type) {
          case TR_cmd_type_disconnect: {
              TR_disconnect_packet_t *data = 
                    (TR_disconnect_packet_t*)e->packet->data;
              log("Player " + i2a(data->player_id) + " has disconnected.");
              if(data->player_id == game_desc.local_player_id) {
                TR_Quit(666);
              } else {
                TR_player_ptr p = GI_GetPlayerPtr(gi, data->player_id);
                assert(p != TR_null);
                player(p).inert = true;
              }
              return true;  
            } break;
          case TR_cmd_type_timeout_peer_player: {
              TR_timeout_peer_player_packet_t *data = 
                    (TR_timeout_peer_player_packet_t*)e->packet->data;
              gi->is_timingout[data->player_id] = true;
              gi->timeouts[data->player_id] = data->time;
              gi->timeout_player_names[data->player_id] = str(data->player_name);
              gi->timeout_timeouts[data->player_id] = TIMEOUT_TIMEOUT_SPAN;
              CAM_OpenWaitWindow(gi, GI_GetCamera(gi));
              log("Timeout peer player " + str(data->player_name) + " for " + i2a(data->time) + ".");
              return true;  
            } break;
        }
    }
  }
  return false;
}

bool GI_CheckForGameClosed(TR_game_instance_t *gi, ENetEvent *e, TR_state_t current_state) {
  if(game_desc.online) {
    switch(e->type) {
      case ENET_EVENT_TYPE_RECEIVE:
        TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
        if(type == TR_cmd_type_game_closed) {
          TR_game_closed_packet_t *data = 
                (TR_game_closed_packet_t*)e->packet->data;
          log("Game is closed.");
          TR_Quit(777);
          return true;  
        }
    }
  }
  return false;
}

bool GI_CheckConnectionToMasterServer(TR_game_instance_t *gi, ENetEvent *e, TR_state_t current_state) {
  if(game_desc.online) {
    switch(e->type) {
      case ENET_EVENT_TYPE_RECEIVE:
        enet_packet_destroy(e->packet);
        return true;
      case ENET_EVENT_TYPE_CONNECT:
        gi->connected = true;
        log("GI_CheckConnectionToMasterServer: Connection to master server succeed.");
        GI_SuccessFlash(gi, "Connection to master server succeed.");
        return true;
      case ENET_EVENT_TYPE_DISCONNECT:
        gi->connected = false;
        gi->connection_acc = 0;
        log("GI_CheckConnectionToMasterServer: disconnected from master server.");
        GI_Free(gi);
        GI_FailureFlash(gi, "Disconnected from master server.");
        TR_Quit(888);
        return false;
      default:
        if(!gi->connected) {
          gi->connection_acc += gi->frame_time;
          if(gi->connection_acc > SHORT_CONNECTION_TIMEOUT) {
            gi->connection_acc = 0;
            log("GI_CheckConnectionToMasterServer: cannot connect to master server.");
            GI_FailureFlash(gi, "Cannot connect to master server.");
            TR_Quit(888);
            return false;
          }
          if(gi->flash_btn.s == "")
            GI_SuccessFlash(gi, "Connecting to master server...");
        } 
    }
    return false;
  }
  return true;
}

bool GI_JoinGameOnMasterServer(TR_game_instance_t *gi) {
  if(game_desc.online) {
    if(gi->connected) {
      log("Joining game: " + game_desc.game_name);
      GI_SuccessFlash(gi, "Game created successfully.");
      ENetPacket *packet =
        enet_packet_create(null, 0, ENET_PACKET_FLAG_RELIABLE);
      TR_join_game_packet_t data;
      data.type                   = TR_cmd_type_join_game;
      data.team_id                = game_desc.local_team_id;
      strcpy(data.player_name, game_desc.player_name.c_str());
      enet_packet_resize(packet, sizeof(data));
      memcpy(packet->data, &data, sizeof(data));
      enet_peer_send(gi->peer, 0, packet);
      enet_host_flush(gi->host);
    }
    return gi->connected;
  }
  return true;
}

bool GI_WaitServer(TR_game_instance_t *gi, ENetEvent *e) {
  if(game_desc.online) {
    switch(e->type)
      case ENET_EVENT_TYPE_RECEIVE: {
          TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
          if(type == TR_cmd_type_game_ready) {
            TR_game_ready_packet_t *data = 
                  (TR_game_ready_packet_t*)e->packet->data;
            log("Game ready.");
            game_desc.local_player_id = data->player_id;
            game_desc.seed            = data->seed;
            log("Local player id is " + i2a(game_desc.local_player_id) + " and seed is " + u2a(game_desc.seed) +  ".");
            return true;
          }
        }
    return false;
  }  
  return true;
}

bool GI_WaitPlayers(TR_game_instance_t *gi, ENetEvent *e) {
  if(game_desc.online) {
    TR_client_ptr c;
    switch(e->type) {
      case ENET_EVENT_TYPE_RECEIVE: {
        TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
        if(type == TR_cmd_type_port) {
          TR_port_packet_t *data = 
                (TR_port_packet_t*)e->packet->data;
          log("CL: port received.");
          int port      = data->port,
              player_id = data->player_id;
          TR_client_t *cl;
          c = gi->client_head;
          while(c != TR_null) {
            cl = &client(c);
            if(cl->player_id == player_id)
              break;
            c = cl->next;
          }
          cl->address.host          = ENET_HOST_ANY;
          cl->address.port          = 0;
          cl->host                  = enet_host_create(&cl->address, MAX_PEER, 1, 0, 0);
          enet_address_set_host(&cl->seeding_address, game_desc.master_server.c_str());
          cl->seeding_address.port  = port;
          log("Port is " + i2a(cl->seeding_address.port));
          cl->peer                  = enet_host_connect(cl->host, &cl->seeding_address, 1, 0);
          assert(cl->peer);
          enet_host_flush(cl->host);
        }
      }
      break;
    }
    bool done = true;
    c = gi->client_head;
    while(c != TR_null) {
      TR_client_t *cl = &client(c);
      CL_WaitForConnection(gi, c);
      done = done && (cl->offline || cl->connected);
      c = cl->next;
    }
    TR_remote_cmd_store_ptr r = gi->remote_cmd_store_head;
    while(r != TR_null) {
      TR_remote_cmd_store_t *rs = &remote_cmd_store(r);
      RS_WaitForConnection(gi, r); 
      done = done && rs->connected;
      r = rs->next;
    }
    switch(e->type)
      case ENET_EVENT_TYPE_RECEIVE: {
        TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
        if(type == TR_cmd_type_players_connected) {
          TR_players_connected_packet_t *data = 
                (TR_players_connected_packet_t*)e->packet->data;
          log("Players connected: " + i2a(data->missing_player_count) + " missing players.");
          TR_player_ptr p = gi->player_head;
          while(p != TR_null) {
            bool found = false;
            DO_TIMES(data->missing_player_count) {
              if(data->missing_player_ids[f] == player(p).id) {
                found = true;
                break;
              }
            }
            if(found)
              player(p).inert = true;
            p = player(p).next;
          }
          int count = 0;
          c = gi->client_head;
          while(c != TR_null) {
            count++;
            c = client(c).next;
          }
          log("Client count: " + i2a(count));
          gi->players_connected = true;
        }
      }
    return done && gi->players_connected;
  } 
  return true;
}

bool GI_Ping(TR_game_instance_t *gi, ENetEvent *e) {
  if(game_desc.online) {
    gi->ping_acc += gi->frame_time;
    if(gi->ping_acc >= MASTER_SERVER_PING_INTERVAL) {
      gi->ping_acc = 0;
      ENetPacket *packet = 
        enet_packet_create(
          null, 
          0, 
          ENET_PACKET_FLAG_UNSEQUENCED);
      TR_ping_packet_t data;
      data.type         = TR_cmd_type_ping;
      data.time         = TR_GetTime();
      enet_packet_resize(packet, sizeof(data));
      memcpy(packet->data, &data, sizeof(data));
      enet_peer_send(gi->peer, 0, packet);
      enet_host_flush(gi->host);
    }
    switch(e->type) 
      case ENET_EVENT_TYPE_RECEIVE: {
          TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
          switch(type) {
            case TR_cmd_type_ping:
              TR_ping_packet_t *data = 
                (TR_ping_packet_t*)e->packet->data;
              gi->ping = TR_GetTime() - data->time;
              GI_UpdateMaxPing(gi);
              return true;
          }
        }
    return false;
  }
  return false;
}

bool GI_Message(TR_game_instance_t *gi, ENetEvent *e) {
  gi->display_message_counter -= gi->frame_time;
  if(gi->send_message.length() > 0) {
    ENetPacket *packet = 
      enet_packet_create(
        null, 
        0, 
        ENET_PACKET_FLAG_RELIABLE);
    TR_message_packet_t data;
    data.type         = TR_cmd_type_message;
    data.team_id      = gi->send_to_all ? -1 : game_desc.local_team_id;
    memcpy(data.message, gi->send_message.c_str(), gi->send_message.length()+1);
    memcpy(data.username, game_desc.player_name.c_str(), game_desc.player_name.length()+1);
    enet_packet_resize(packet, sizeof(data));
    memcpy(packet->data, &data, sizeof(data));
    enet_peer_send(gi->peer, 0, packet);
    enet_host_flush(gi->host);
    gi->send_message = "";
  }
  switch(e->type) 
    case ENET_EVENT_TYPE_RECEIVE: {
        TR_cmd_type_t type = ((TR_cmd_type_t*)e->packet->data)[0];
        switch(type) {
          case TR_cmd_type_message:
            TR_message_packet_t *data = 
              (TR_message_packet_t*)e->packet->data;
            if(data->team_id == game_desc.local_team_id
            || data->team_id == -1) {
              for(int k = MESSAGE_QUEUE_COUNT-1; k >= 1; k--) {
                gi->message_queue[k] = gi->message_queue[k-1];
                gi->username_queue[k] = gi->username_queue[k-1];
                gi->sent_to_all_queue[k] = gi->sent_to_all_queue[k-1];
              }
              data->message[MAX_MESSAGE_CHAR-1] = 0;
              data->username[MAX_MESSAGE_CHAR-1] = 0;
              gi->message_queue[0] = str(data->message);
              gi->username_queue[0] = str(data->username);
              gi->sent_to_all_queue[0] = data->team_id == -1;
              gi->display_message_counter = DISPLAY_MESSAGE_SPAN;
            }
            return true;
        }
      }
  return false;
}

void GI_InitPingThread(TR_game_instance_t *gi) {
  if(game_desc.online) {
    ping_thread_run = true;
    DWORD id;
    enet_address_set_host(&gi->ping_thread_master_server_address, game_desc.master_server.c_str());
    gi->ping_thread_master_server_address.port = MASTER_SERVER_PORT+1;
    gi->ping_thread_address.host = ENET_HOST_ANY;
    gi->ping_thread_address.port = 0;
    gi->ping_thread_host         = enet_host_create(&gi->ping_thread_address, MAX_PEER, 1, 0, 0);
    gi->ping_thread_peer         = enet_host_connect(gi->ping_thread_host, &gi->ping_thread_master_server_address, 1, 0);
    enet_peer_timeout(gi->ping_thread_peer, 0, 20000, 20000);
    gi->ping_thread_handle = CreateThread(null, 0, GI_PingThread, (void*)gi, 0, &id); 
  }
}

void GI_ClosePingThread(TR_game_instance_t *gi) {
  if(game_desc.online) {
    EnterCriticalSection(&ping_thread_disconnect_crit);
    EnterCriticalSection(&ping_thread_crit);
    ping_thread_run = false;
    LeaveCriticalSection(&ping_thread_crit);
    LeaveCriticalSection(&ping_thread_disconnect_crit);
    DWORD r = WaitForSingleObject(gi->ping_thread_handle, 10000);
    assert(r!=WAIT_FAILED);
    CloseHandle(gi->ping_thread_handle);
    enet_peer_disconnect(gi->ping_thread_peer, 0);
    ENetEvent e;
    enet_host_service(gi->ping_thread_host, &e, 500);
    enet_peer_reset(gi->ping_thread_peer);
    return_error = ping_thread_return_error != 0 ? ping_thread_return_error : return_error;
  }
} 

DWORD WINAPI GI_PingThread(void *params) {
  TR_game_instance_t *gi = (TR_game_instance_t*)params;
  bool run_it;
  do {
    ENetEvent e;
    while(enet_host_service(gi->ping_thread_host, &e, 0) != ENET_EVENT_TYPE_NONE) {
      switch(e.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            log("GI_PingThread: Connection to master server succeed.");
            ENetPacket *packet =
              enet_packet_create(null, 0, ENET_PACKET_FLAG_RELIABLE);
            TR_join_game_packet_t data;
            data.type                   = TR_cmd_type_join_game;
            data.team_id                = game_desc.local_team_id;
            strcpy(data.player_name, game_desc.player_name.c_str());
            enet_packet_resize(packet, sizeof(data));
            memcpy(packet->data, &data, sizeof(data));
            enet_peer_send(gi->ping_thread_peer, 0, packet);
            enet_host_flush(gi->ping_thread_host);
          } break;
        case ENET_EVENT_TYPE_DISCONNECT: {
            log("GI_PingThread: cannot connect to master server or been disconnected.");
            EnterCriticalSection(&ping_thread_disconnect_crit);
            EnterCriticalSection(&ping_thread_crit);
            ping_thread_run = false;
            ping_thread_return_error = 666;
            LeaveCriticalSection(&ping_thread_crit);
            LeaveCriticalSection(&ping_thread_disconnect_crit);
          } break;
        case ENET_EVENT_TYPE_RECEIVE:
          log("GI_PingThread: a packet has been received.");
          enet_packet_destroy(e.packet);
          break;
      }
    }
    EnterCriticalSection(&ping_thread_crit);
    run_it = ping_thread_run;
    LeaveCriticalSection(&ping_thread_crit);
  } while(run_it);
}

void GI_SuccessFlash(TR_game_instance_t *gi, const str &msg) {
  gi->flash_btn.s = msg;
  gi->flash_btn.color = TCOD_green;
}

void GI_FailureFlash(TR_game_instance_t *gi, const str &msg) {
  gi->flash_btn.s = msg;
  gi->flash_btn.color = TCOD_red;
}

int GI_GetPlayerCount(TR_game_instance_t *gi) {
  int result = 0;
  TR_player_ptr p = gi->player_head;
  while(p != TR_null) {
    TR_player_t *pl = &player(p);
    result++;
    p = pl->next;
  }
  return result;
}

TR_player_ptr GI_GetPlayerPtr(TR_game_instance_t *gi, TR_id_t player_id) {
  TR_player_ptr p = gi->player_head;
  while(p != TR_null) {
    TR_player_t *pl = &player(p);
    if(pl->id == player_id)
      return p;
    p = pl->next;
  }
  return TR_null;
}

TR_client_ptr GI_GetClientPtr(TR_game_instance_t *gi, TR_id_t player_id) {
  TR_client_ptr c = gi->client_head;
  while(c != TR_null) {
    TR_client_t *cl = &client(c);
    if(cl->player_id == player_id)
      return c;
    c = cl->next;
  }
  return TR_null;
}

TR_camera_ptr GI_GetCamera(TR_game_instance_t *gi) {
  return gi->camera_head;
}

void GI_CheckListIntegrity(TR_game_instance_t *gi) {
  int i, j, k;
  TR_client_ptr c = gi->client_head;
  k = 0;
  while(c != TR_null) {
    assert((k++) <= MAX_CLIENT);
    c = client(c).next;
  }
  TR_player_ptr p = gi->player_head;
  k = 0;
  while(p != TR_null) {
    TR_cmd_ptr r = cmd_store(player(p).cs).cmd_head;
    i = 0;
    while(r != TR_null) {
      TR_cmd_t *cmd = &cmd(r);
      assert((i++) <= MAX_CMD);
      r = cmd->cmd_next;
    }
    TR_unit_ptr u = player(p).unit_head;
    i = 0;
    while(u != TR_null) {
      TR_unit_t *un = &unit(u);
      TR_unit_order_ptr o = un->order_head;
      j = 0;
      while(o != TR_null) {
        assert((j++) <= MAX_UNIT_ORDER);
        o = order(o).next;
      }
      assert((i++) <= MAX_UNIT);
      u = un->pnext;
    }
    assert((k++) <= MAX_PLAYER);
    p = player(p).next;
  }
  TR_ball_ptr b = gi->ball_head;
  k = 0;
  while(b != TR_null) {
    assert((k++) <= MAX_BALL);
    b = ball(b).next;
  }
  {
    TR_unit_ptr u = gi->free_unit_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_UNIT);
      u = unit(u).fnext;
    }
  }
  {
    TR_player_ptr u = gi->free_player_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_PLAYER);
      u = player(u).fnext;
    }
  }
  {
    TR_client_ptr u = gi->free_client_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_CLIENT);
      u = client(u).fnext;
    }
  }
  {
    TR_unit_order_ptr u = gi->free_unit_order_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_UNIT_ORDER);
      u = order(u).fnext;
    }
  }
  {
    TR_cmd_ptr u = gi->free_cmd_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_CMD);
      u = cmd(u).fnext;
    }
  }
  {
    TR_ball_ptr u = gi->free_ball_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_BALL);
      u = ball(u).fnext;
    }
  }
  {
    TR_cmd_store_ptr u = gi->free_cmd_store_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_PLAYER);
      u = cmd_store(u).fnext;
    }
  }
  {
    TR_remote_cmd_store_ptr u = gi->free_remote_cmd_store_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_PLAYER);
      u = remote_cmd_store(u).fnext;
    }
  }
  {
    TR_particle_system_ptr u = gi->free_particle_system_head;
    k = 0;
    while(u != TR_null) {
      assert((k++) <= MAX_PARTICLE_SYSTEM);
      u = particle_system(u).fnext;
    }
  }
}

short GI_FindClearance(TR_game_instance_t *gi, int cx, int cy) {
  short       off         = 0;
  int         x, y, i, j, endi, endj,
              dx = cx, 
              dy = cy;
  while(off < MAP_SIZE_X) {
    endi = 0+off;
    for(i = 0-off; i <= endi; i++) {
      j = 0-off;
      if(AT_ENV((dx+i)>>RANGE_SHIFT, (dy+j)>>RANGE_SHIFT) == TR_WALL)
        return off;
    }
    endi = 0+off;
    for(i = 0-off; i <= endi; i++) {
      j = 0+off;
      if(AT_ENV((dx+i)>>RANGE_SHIFT, (dy+j)>>RANGE_SHIFT) == TR_WALL)
        return off;
    }
    endj = 0+off;
    for(j = 0-off; j <= endj; j++) {
      i = 0-off;
      if(AT_ENV((dx+i)>>RANGE_SHIFT, (dy+j)>>RANGE_SHIFT) == TR_WALL)
        return off;
    }
    endj = 0+off;
    for(j = 0-off; j <= endj; j++) {
      i = 0+off;
      if(AT_ENV((dx+i)>>RANGE_SHIFT, (dy+j)>>RANGE_SHIFT) == TR_WALL)
        return off;
    }
    off++;
  }
  return 0x7fff;
}

void GI_SaveReplay(TR_game_instance_t *gi) {
  if(game_desc.replay == "") {
    log("Saving replay with seed: " + u2a(game_desc.seed));
    str replay = u2a(game_desc.seed);
    mkdir(str("./replay/r" + replay + "/").c_str());
    FILE *desc_file = fopen(str("./replay/r" + replay + "/desc").c_str(), "a+b");
    fwrite(&game_desc.team_count, sizeof(game_desc.team_count), 1, desc_file);
    fwrite(&game_desc.player_count_per_team, sizeof(game_desc.player_count_per_team), 1, desc_file);
    fwrite(&game_desc.offline_players, sizeof(game_desc.offline_players), 1, desc_file);
    fwrite(&game_desc.seed, sizeof(game_desc.seed), 1, desc_file);
    fwrite(&game_desc.PvE, sizeof(game_desc.PvE), 1, desc_file);
    fclose(desc_file);
    TR_player_ptr p = gi->player_head;
    while(p != TR_null) {
      TR_player_t *pl = &player(p);
      FILE *file = fopen(str("./replay/r" + replay + "/p" + i2a(pl->id)).c_str(), "a+b");
      TR_cmd_store_t *cs = &cmd_store(pl->cs);
      cs->cmd_buffers.reverse();
      for(list<TR_buffer_t*>::iterator iter = cs->cmd_buffers.begin(); iter != cs->cmd_buffers.end(); ++iter) {
        TR_buffer_t *buf = *iter;
        fwrite(&buf->data_length, sizeof(buf->data_length), 1, file);
        fwrite(buf->data, buf->data_length, 1, file);
      }
      fclose(file);
      p = pl->next;
    }
    log("Saving replay done.");
  }
}

void GI_LoadReplayDesc(TR_game_instance_t *gi) {
  FILE *desc_file = fopen(str("./replay/" + game_desc.replay + "/desc").c_str(), "r+b");
  assert(desc_file);
  fread(&game_desc.team_count, sizeof(game_desc.team_count), 1, desc_file);
  fread(&game_desc.player_count_per_team, sizeof(game_desc.player_count_per_team), 1, desc_file);
  fread(&game_desc.offline_players, sizeof(game_desc.offline_players), 1, desc_file);
  fread(&game_desc.seed, sizeof(game_desc.seed), 1, desc_file);
  fread(&game_desc.PvE, sizeof(game_desc.PvE), 1, desc_file);
  log("Seed is: " + u2a(game_desc.seed));
  fclose(desc_file);
}

void GI_DumpDesyncedTrace(TR_game_instance_t *gi) {
  FILE *desc_file = fopen(str("desynced_trace.txt").c_str(), "w+b");
  fwrite(gi->desynced_str.c_str(), gi->desynced_str.size(), 1, desc_file);
  fclose(desc_file);
}

bool TR_ogre_class::keyPressed(const OIS::KeyEvent &e) {
  global_gi->key_pressed[global_gi->key_pressed_count++] = e.key;
}

/*
 * 
 */

int TR_exp(int l, int x, int p) {
  if(l > 0) {
    x = x * p / 100;
    return TR_exp(l-1, x, p);
  }
  return x;
} 

int TR_exp_100(int l, int x, int p) {
  if(l > 0) {
    x = p * x / (100*100);
    return TR_exp_100(l-1, x, p);
  }
  return x;
} 

void TR_Quit(int error) {
  run = false;
  return_error = error;
}

void TR_SetBinDir() {
  bin_dir = str(argv[0]);
  int len = bin_dir.length();
  while(len
      && bin_dir.c_str()[len-1] != '/' 
      && bin_dir.c_str()[len-1] != '\\')
      len--;
  bin_dir.resize(len);
}

TR_time_t TR_GetTime() {
  #ifdef WIN32
    LARGE_INTEGER c;
    QueryPerformanceFrequency(&pfrequency);
    QueryPerformanceCounter(&c);
    return (unsigned int)(c.QuadPart*1000/pfrequency.QuadPart);
  #else
    return 0;
  #endif
}

int TR_Rand(unsigned int *seedp) {
  *seedp = *seedp * 1103515245 + 12345;
  return((unsigned)(*seedp/(1<<22)) % (1<<21));
}

void TR_ZErr(int ret) {
  switch(ret) {
    case Z_BUF_ERROR:
      log("buffer error");
      break;
    case Z_NEED_DICT:
      log("need dict");
      break;
    case Z_ERRNO:
      log("error reading stdin");
      break;
    case Z_STREAM_ERROR:
      log("invalid compression level");
      break;
    case Z_DATA_ERROR:
      log("invalid or incomplete deflate data");
      break;
    case Z_MEM_ERROR:
      log("out of memory");
      break;
    case Z_VERSION_ERROR:
      log("zlib version mismatch!");
      break;
  }
}

unsigned int TR_Hash(unsigned char *data, int len) {
  unsigned int hash = len, tmp;
  int rem;
  if(len > 0 && data != NULL) {
    rem = len & 3;
    len >>= 2;
    for(;len > 0; len--) {
      hash  += get16bits (data);
      tmp    = (get16bits (data+2) << 11) ^ hash;
      hash   = (hash << 16) ^ tmp;
      data  += 2*sizeof (unsigned short);
      hash  += hash >> 11;
    }
    switch(rem) {
      case 3: 
        hash += get16bits (data);
        hash ^= hash << 16;
        hash ^= ((signed char)data[sizeof (unsigned short)]) << 18;
        hash += hash >> 11;
        break;
      case 2: 
        hash += get16bits (data);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
      case 1: 
        hash += (signed char)*data;
          hash ^= hash << 10;
          hash += hash >> 1;
    }
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
  }
  return 0;
}

#include "isqrt.cpp"

void TR_PrintString(const str &s, int x, int y) {
  TCOD_console_print(con, x, y, s.c_str());
}

void TR_PrintString(int i, int x, int y) {
  TR_PrintString(i2a(i), x, y);
}

void TR_PrintString(float f, int x, int y) {
  TR_PrintString(f2a(f), x, y);
}

void TR_PrintDebug(const str &s) { log(s); TR_PrintString(s, 0, CON_RES_Y-1);}
void TR_PrintDebug(int i)        { log(i); TR_PrintString(i, 0, CON_RES_Y-1);}
void TR_PrintDebug(float f)      { log(f); TR_PrintString(f, 0, CON_RES_Y-1);}

str f2a(double f) {
  std::ostringstream ss;
  ss << f;
  return ss.str();
}

str i2a(int i) {
  std::ostringstream ss;
  ss << i;
  return ss.str();
}

str u2a(unsigned int u) {
  std::ostringstream ss;
  ss << u;
  return ss.str();
}

str get_date() {
  time_t      rawtime;
  struct tm  *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  str date(asctime(timeinfo));
  date.resize(date.length()-1);
  return date.c_str();
}

void log_in_file(const char *msg) {
  if(do_debug) {
  #if THREADS  
  EnterCriticalSection(&log_crit);
  #endif  
    FILE *f = fopen((bin_dir + "log.txt").c_str(), "a+");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fclose(f);
    
    if(size < 10000000)
        f = fopen((bin_dir + "log.txt").c_str(), "a+");
    else
        f = fopen("log.txt", "w+");
    fprintf(f, msg);
    fclose(f);
  #if THREADS    
  LeaveCriticalSection(&log_crit);
  #endif
  }
}

bool log(const str &s, bool dummy) {
  log_in_file(str(get_date() + " " + s + "\n").c_str());
  return dummy;
}

bool log(int i, bool dummy) {
  log(i2a(i));
  return dummy;
}

bool log(float f, bool dummy) {
  log(f2a(f));
  return dummy;
}

void exit() {
  log_in_file("End of execution. \n\n");  
#if THREADS  
  DeleteCriticalSection(&log_crit);
#endif  
}

/*
 *
 */

int                     argc;
char                    **argv;
bool                    run,
                        ping_thread_run;
int                     resx, resy;
str                     bin_dir;
    
TR_game_instance_t      client_gi;

LARGE_INTEGER           pfrequency;
float                   ref_time;


TCOD_console_t          con;
unsigned char           zlib_in[ZLIB_CHUNK],
                        zlib_out[ZLIB_CHUNK];

int                     CON_RES_X,
                        CON_RES_Y,
                        LAGOMETER_START_X,
                        offx = 0,
                        offy = 0;

#ifdef WIN32
  CRITICAL_SECTION      log_crit,
                        debug_crit,
                        ping_thread_crit,
                        ping_thread_disconnect_crit;
#endif

short                   halfes[MAX_UNIT_SIZE+1];

TR_game_instance_t      *global_gi;

TCOD_color_t             player_colors[MAX_PLAYER] =
  {TCOD_color_RGB(34, 70, 240), /* blue */
   TCOD_color_RGB(0, 194, 183),
   TCOD_color_RGB(230, 66, 139), /* red */
   TCOD_color_RGB(26, 126, 68),
   TCOD_color_RGB(240, 29, 29), /* red */
   TCOD_color_RGB(82, 0, 134), /* purple */
   TCOD_color_RGB(0, 150, 255),
   TCOD_color_RGB(155, 68, 4) };

short                    color_shift;

int                      MASTER_SERVER_PORT = 40000;

int                      return_error = 0,
                         ping_thread_return_error = 0;

bool                     do_debug = true;
 
int march_allow_x[16][8] = {
      {-1,  0,  1, -1, 1, -1, 0, 1}/*0*/,
      {-1,  0,  1, -1, 1, 0, 1}/*1*/,
      {-1,  0,  1, -1, 1, -1, 0}/*2*/,
      {-1,  0,  1, -1, 1}/*3*/,
      {-1,  0, -1, 1, -1, 0, 1}/*4*/,
      {-1,  0, -1, 1, 0, 1}/*5*/,
      {-1,  0, -1, -1, 0}/*6*/,
      {-1,  0, -1}/*7*/,
      { 0,  1, -1, 1, -1, 0, 1}/*8*/,
      { 0,  1, 1, 0, 1}/*9*/,
      { 0,  1, -1, 1, -1, 0}/*10*/,
      { 0,  1, 1}/*11*/,
      {-1, 1, -1, 0, 1}/*12*/,
      {1, 0, 1}/*13*/,
      {-1, -1, 0}/*14*/,
      {}/*15*/}, 
    march_allow_y[16][8] = {
      {-1, -1, -1,  0, 0,  1, 1, 1}/*0*/,
      {-1, -1, -1,  0, 0, 1, 1}/*1*/,
      {-1, -1, -1,  0, 0,  1, 1}/*2*/,
      {-1, -1, -1,  0, 0}/*3*/,
      {-1, -1,  0, 0,  1, 1, 1}/*4*/,
      {-1, -1,  0, 0, 1, 1}/*5*/,
      {-1, -1,  0,  1, 1}/*6*/,
      {-1, -1,  0}/*7*/,
      {-1, -1,  0, 0,  1, 1, 1}/*8*/,
      {-1, -1, 0, 1, 1}/*9*/,
      {-1, -1,  0, 0,  1, 1}/*10*/,
      {-1, -1, 0}/*11*/,
      { 0, 0,  1, 1, 1}/*12*/,
      {0, 1, 1}/*13*/,
      { 0,  1, 1}/*14*/,
      {}/*15*/}, 
    march_allow_point_count[16] = {
      8/*0*/, 
      7/*1*/, 
      7/*2*/, 
      5/*3*/, 
      7/*4*/, 
      6/*5*/, 
      5/*6*/, 
      3/*7*/, 
      7/*8*/, 
      5/*9*/, 
      6/*10*/, 
      3/*11*/, 
      5/*12*/, 
      3/*13*/, 
      3/*14*/, 
      0/*15*/};

int attack_penetrations[TR_attack_type_count][TR_armor_type_count] = {
  {125,  75,  75, 25}, /* normal */
  {75,  125,  75, 25}, /* piercing */
  {100, 100, 125, 25}, /* magic */
  {50,  50,  100, 100}, /* crashing */
  {100, 100, 100, 100}, /* chaos */
};

/*
 * End
 */ 