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

#ifndef TR_H
#define TR_H

/*
 * std
 */

#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <cstring>
#include <cfloat>
#include <cassert>
#include <list>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <float.h>

#ifdef WIN32
#include <Windows.h>
#elif defined(__linux)
#include "linux.h"
#endif

using namespace std;

/*
 * misc
 */

typedef string str;
#define null NULL

/*
 * includes
 */

#include <pthread.h>
#include <libtcod.h>
#include <jansson.h>
#include <enet/enet.h>
#include <zlib.h>
#include <SDL/SDL.h>

#include <gorilla/ga.h>
#include <gorilla/gau.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreColourValue.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRTShaderSystem.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreLogManager.h>
#include <OgreBorderPanelOverlayElement.h>
#include <ParticleUniversePlugin.h>
#include <ParticleUniverseSystemManager.h>
#ifdef WIN32
#include <OgreD3D9RenderSystem.h>
#include <OgreD3D9DeviceManager.h>
#include <OgreD3D9Device.h>
#include <OgreD3D9Texture.h>
#endif

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>

#include <DotSceneLoader.h>
#include <bbox.h>
   
/*
 TownRush
 */

#define RECORD_DEMO                 false

#define RELEASE                     false
#define FULLSCREEN                  true

#define MINIMAP_MODE                false
#define THREADS                     true

#define DESYNCED_DEBUG              false

#if DESYNCED_DEBUG
#define DTRACE(X)                   gi->desynced_str += str(X) + "\n";
#define DTRACEI(X)                  gi->desynced_str += str(i2a(X)) + "\n";
#else
#define DTRACE(X)      
#define DTRACEI(X)                
#endif

#define NORMAL_TIME_STEP            10
#define MAX_GROUP_SELECTION         10
#define MAX_TIME_STEP               50
#define CMD_TURN_SHIFT              2
#define STARTING_TURN_FRAME_COUNT   1
#define MAX_TURN_FRAME_COUNT        5
#define MAX_AVER_FRAME_TIMES        64
#define MAX_AVER_WAIT_TIMES         1
#define MAX_MAX_PING_TIMES          100 
#define MAX_FRAME_TIME              (MAX_TURN_FRAME_COUNT*NORMAL_TIME_STEP)             
#define ZLIB_CHUNK                  (1<<18) /* 256Kb */
#define MAX_CHAR                    64
#define SMALL_SIZE                  32
#define TR_null                    -1
#define MESSAGE_QUEUE_COUNT         7
#define MAX_MESSAGE_CHAR            128
#define DISPLAY_MESSAGE_SPAN        10000
#define CHAR_SIZE                   16
#define WHITE_SQUARE                ((gi->terminal_h-1)*gi->terminal_w+gi->terminal_w-1)
#define MAP_SIZE_X                  128
#define MAP_SIZE_Y                  128
#define COLOR(C)                    ((((C).r)<<16) + (((C).g)<<8) + (((C).b)<<0))
#define AMBIENT_LIGHT               50
#define AMBIENT_LIGHTMAP            TCOD_color_multiply_scalar(TCOD_color_RGB(97, 97, 180), 1.8)
#define UNIT_AO_STRENGTH            0.90f
#define DIR_LIGHTMAP_X              2
#define DIR_LIGHTMAP_Y              4
#define DIR_LIGHTMAP_COLOR          TCOD_color_multiply_scalar(TCOD_color_RGB(255, 180, 0), 1)
#define MAP_SIZE                    (MAP_SIZE_X * MAP_SIZE_Y)
#define POS_SHIFT                   9
#define SCREEN_SHIFT                (POS_SHIFT-4) /* POS_SHIFT - 'CHAR_SIZE' */
#define MAX_UNIT_SIZE               4
#define RANGE_SHIFT                 0
#define RANGE_MAP_SIZE_X            (MAP_SIZE_X>>RANGE_SHIFT)
#define RANGE_MAP_SIZE_Y            (MAP_SIZE_Y>>RANGE_SHIFT)
#define RANGE_MAP_SIZE              (RANGE_MAP_SIZE_X * RANGE_MAP_SIZE_Y)
#define MINIMAP_SIZE_X              192
#define MINIMAP_SIZE_Y              192
#define MINIMAP_SIZE                (MINIMAP_SIZE_X*MINIMAP_SIZE_Y)
#define MINIMAP_START_X             (gi->ogre_window->getWidth()-MINIMAP_SIZE_X)
#define MINIMAP_START_Y             (gi->ogre_window->getHeight()-MINIMAP_SIZE_Y)
#define MINIMAP_RATIO_X             MINIMAP_SIZE_X/MAP_SIZE_X
#define MINIMAP_RATIO_Y             MINIMAP_SIZE_Y/MAP_SIZE_Y
#define MINIMAP_INV_RATIO_X         MAP_SIZE_X/MINIMAP_SIZE_X
#define MINIMAP_INV_RATIO_Y         MAP_SIZE_Y/MINIMAP_SIZE_Y
#define HUD_HEIGHT                  10
#define BTN_WIDTH                   48
#define MAX_SKILLS_PER_UNIT         32
#define HUD_START_Y                 (CON_RES_Y-HUD_HEIGHT)
#define HUD_RES_Y                   (CON_RES_Y)-(MAP_SIZE_Y)
#define LAGOMETER_SIZE              MAX_AVER_FRAME_TIMES
#define REPULSION_SPEED             3
#define SAME_UNIT_TYPE_SIZE         (10*10)
#define PICKING_THRESHOLD           20
#define FLUID_DIFFUSION             0.00001
#define FLUID_VISCOSITY             0.0000001
#define FLUID_BOX_SIZE              80
#define FLUID_BOX_SIZE_SQUARE       (FLUID_BOX_SIZE*FLUID_BOX_SIZE)
#define TEAM_ID(PLAYER_ID)          (player(GI_GetPlayerPtr(gi, (PLAYER_ID))).team_id)
#define SWAP(T, A, B)               {T tmp; tmp = A; A = B; B = tmp;}
#define AT(M, X, Y)                 (gi->M[(Y)*MAP_SIZE_X+(X)])
#define AT_RANGE(M, X, Y)           (gi->M[(Y)*RANGE_MAP_SIZE_X+(X)])
#define AT_CON(M, X, Y)             (gi->M[(Y)*CON_RES_X+(X)])
#define AT2(M, X, Y)                (M[(Y)*MAP_SIZE_X+(X)])
#define AT_ENV(X, Y)                (gi->env_map[(Y)*RANGE_MAP_SIZE_X+(X)])
#define AT_FL(M, X, Y)              (fl->M[(Y)*FLUID_BOX_SIZE+(X)])
#define AT_FL2(M, X, Y)             (M[(Y)*FLUID_BOX_SIZE+(X)])
#define ON_CON_X(X)                 (((X)-offx)/CHAR_SIZE)
#define ON_CON_Y(Y)                 (((Y)-offy)/CHAR_SIZE)
#define INSIDE_MAP(X, Y) \
  ((X) >= 0 && (X) < MAP_SIZE_X && (Y) >= 0 && (Y) < MAP_SIZE_Y)
#define INSIDE_ENV(X, Y) \
  ((X) >= 0 && (X) < RANGE_MAP_SIZE_X && (Y) >= 0 && (Y) < RANGE_MAP_SIZE_Y)
#define INSIDE_SCREEN(X, Y) \
     ((X) >= 0 && (X) < gi->ogre_window->getWidth() \
   && (Y) >= 0 && (Y) < gi->ogre_window->getHeight())
#define INSIDE_CON(X, Y) \
     ((X) >= 0 && (X) < CON_RES_X && (Y) >= 0 && (Y) < HUD_START_Y \
  && (((X) < MINIMAP_CON_START_X || (X) >= MINIMAP_CON_START_X+MINIMAP_CON_SIZE_X) \
  || ((Y) < MINIMAP_CON_START_Y || (Y) >= MINIMAP_CON_START_Y+MINIMAP_CON_SIZE_Y)))
#define INSIDE_VIEWPORT(X, Y) \
     ((X) >= 0 && (X) < resx && (Y) >= 0 && (Y) < resy)
#define INSIDE_CON_SCREEN_SPACE(X, Y) \
     ((X) >= 0 && (X) < ((CON_RES_X)<<POS_SHIFT) && (Y) >= 0 && (Y) < ((HUD_START_Y)<<POS_SHIFT) \
  && (((X) < ((MINIMAP_CON_START_X)<<POS_SHIFT) || (X) >= ((MINIMAP_CON_START_X+MINIMAP_CON_SIZE_X)<<POS_SHIFT)) \
  || ((Y) < ((MINIMAP_CON_START_Y)<<POS_SHIFT) || (Y) >= ((MINIMAP_CON_START_Y+MINIMAP_CON_SIZE_Y)<<POS_SHIFT))))
#define INSIDE_HUD(X, Y) ((X) >= 0 && (X) < CON_RES_X && (Y) >= HUD_START_Y && (Y) < CON_RES_Y)
#define INSIDE_FULL_CON(X, Y) ((X) >= 0 && (X) < CON_RES_X && (Y) >= 0 && (Y) < CON_RES_Y)
#define INSIDE_MINI(X, Y) \
     ((X) >= MINIMAP_START_X && (X) < MINIMAP_START_X+MINIMAP_SIZE_X \
  &&  (Y) >= MINIMAP_START_Y && (Y) < MINIMAP_START_Y+MINIMAP_SIZE_Y)
#define INSIDE_MINI_TEX(X, Y) \
     ((X) >= 0 && (X) < 0+MINIMAP_SIZE_X \
  &&  (Y) >= 0 && (Y) < 0+MINIMAP_SIZE_Y)
#define INSIDE_FLUID(X, Y) \
     ((X) >= fl->pos_x - FLUID_BOX_SIZE/2 && (X) < fl->pos_x + FLUID_BOX_SIZE/2 \
   && (Y) >= fl->pos_y - FLUID_BOX_SIZE/2 && (Y) < fl->pos_y + FLUID_BOX_SIZE/2)
#define INSIDE_FLUID2(X, Y) \
     ((X) >= 0 && (X) < FLUID_BOX_SIZE \
   && (Y) >= 0 && (Y) < FLUID_BOX_SIZE)
#define BOX_INSIDE_MAP(X1, Y1, X2, Y2, X3, Y3, X4, Y4) \
    (INSIDE_MAP(X1, Y1) \
  || INSIDE_MAP(X2, Y2) \
  || INSIDE_MAP(X3, Y3) \
  || INSIDE_MAP(X4, Y4))
#define BOUND_MINMAX(MINX, MAXX, X) \
  if((X) < (MINX)) (X) = (MINX); \
  if((X) > (MAXX)) (X) = (MAXX);
#define BOUND(BX, X) \
  if((X) < 0) (X) = 0; \
  if((X) > (BX)) (X) = (BX);
#define IN_ORDER(X1, Y1, X2, Y2) \
  if((X1) > (X2)) SWAP(int, (X1), (X2)); \
  if((Y1) > (Y2)) SWAP(int, (Y1), (Y2));
#define BOUND_IN_ORDER(X1, Y1, X2, Y2) \
  BOUND(MAP_SIZE_X-1, X1) BOUND(MAP_SIZE_Y-1, Y1) \
  BOUND(MAP_SIZE_X-1, X2) BOUND(MAP_SIZE_Y-1, Y2) \
  IN_ORDER(X1, Y1, X2, Y2)
#define BOUND_VIEWPORT \
  if(cam->viewport_x < (13<<POS_SHIFT)) \
    cam->viewport_x = (13<<POS_SHIFT); \
  if(cam->viewport_x > ((MAP_SIZE_X-13)<<POS_SHIFT)) \
    cam->viewport_x = ((MAP_SIZE_X-13)<<POS_SHIFT); \
  if(cam->viewport_y < (7<<POS_SHIFT)) \
    cam->viewport_y = (7<<POS_SHIFT); \
  if(cam->viewport_y > ((MAP_SIZE_Y-7)<<POS_SHIFT)) \
    cam->viewport_y = (MAP_SIZE_Y-7)<<POS_SHIFT;
#define FOR_RECT(X1, X2, Y1, Y2) \
  for(int j = Y1, _finished = 0; !_finished && j <= Y2; j++) \
    for(int i = X1; !_finished && i <= X2; i++)
#define FOR_RECT2(X1, X2, Y1, Y2) \
  for(int l = Y1, _finished2 = 0; !_finished2 && l <= Y2; l++) \
    for(int k = X1; !_finished2 && k <= X2; k++)
#define SOCLE(SIZE) \
  int _size = (SIZE)-1; \
  int _half = halfes[_size]; \
  int _a = 0-_half; \
  int _b = _size-_half; \
  FOR_RECT(_a, _b, _a, _b)
#define REAL_SOCLE(SIZE, X, Y) \
  int _half     = (int)(floor((float)(SIZE)*0.5f - 0.5f))<<POS_SHIFT, \
      _start_i  = ((X) - _half)>>POS_SHIFT, \
      _end_i    = ((X) - _half + ((SIZE)<<POS_SHIFT)-1)>>POS_SHIFT, \
      _start_j  = ((Y) - _half)>>POS_SHIFT, \
      _end_j    = ((Y) - _half + ((SIZE)<<POS_SHIFT)-1)>>POS_SHIFT; \
  FOR_RECT(_start_i, _end_i, _start_j, _end_j)
#define DO_TIMES(I) \
  for(int f = 0, _endi = (I), _finished = 0; _finished == 0 && f < _endi; f++)
#define DO_TIMES2(I) \
  for(int i = 0, _endi = (I), _finished = 0; _finished == 0 && i < _endi; i++)
#define STR_EQ(S1, S2) strcmp((S1), (S2)) == 0
#define ON_SCREEN(X, Y) \
  (*(int*)((unsigned char*)surface->pixels + \
    surface->pitch*(Y) + \
    (X)*surface->format->BytesPerPixel))
#define ON_LAGOMETER(X, Y) dest[box.rowPitch*(Y)+(X)]
#define ON_CLOSE_MAP(X, Y) close_map[(int)(Y)*MAP_SIZE_X+(X)]
#define ON_FOV_MAP(X, Y) gi->fov_maps[pl->team_id][RANGE_MAP_SIZE_X*(Y)+(X)]
#define ON_MAGIC_VISION_MAP(X, Y) gi->magic_vision_maps[pl->team_id][RANGE_MAP_SIZE_X*(Y)+(X)]
#define START_BENCH \
  TR_time_t t = TR_GetTime();
#define END_BENCH(ms) \
  if((TR_GetTime() - t) >= ms) \
    dbug((TR_GetTime() - t));
#define SCREEN_UNIT_X(UN)     (offx+(((UN)->x-cl->viewport_x)>>SCREEN_SHIFT)-(halfes[SIZE((UN))-1]*CHAR_SIZE))
#define SCREEN_UNIT_Y(UN)     (offy+(((UN)->y-cl->viewport_y)>>SCREEN_SHIFT)-(halfes[SIZE((UN))-1]*CHAR_SIZE))
#define SCREEN_UNIT_SIZE(UN)  (SIZE((UN))*CHAR_SIZE)
#define IS_COLOR(R, G, B)     (c.r == (R) && c.g == (G) && c.b == (B))
#define SATURATE(X)           MAX(0, MIN(1, (X)))
#define PLAYER_COLOR(ID) player_colors[((ID)+color_shift)&7]

#if RELEASE
  #define MAIN_MUSIC "./data/Asgaror_39_s_Depths.ogg"
#else
  #define MAIN_MUSIC "none"
#endif
#define OPEN_SOUND(S, K) gau_load_sound_file(S, K);

#define PLAY_SOUND(S, TAG) \
  TR_sound_ptr s = SO_New(gi); \
  if(s != TR_null) { \
    TR_sound_t *so = &sound(s); \
    so->tag = TAG; \
    so->handle = gau_create_handle_sound( \
      gi->gau_mixer, S, &gau_on_finish_destroy, 0, 0); \
    ga_handle_play(so->handle); \
    ga_handle_setParamf(so->handle, GA_HANDLE_PARAM_GAIN, 0); \
  }
#define SOUND_VARIATION_COUNT 3
#define PLAY_SOUND_I(S, I, TAG) PLAY_SOUND((S)[(I)], TAG)
#define PLAY_SOUND_LOOP(S) \
  TR_sound_ptr s = SO_New(gi); \
  if(s != TR_null) { \
    TR_sound_t *so = &sound(s); \
    so->handle = gau_create_handle_sound( \
      gi->gau_mixer, (S), 0, 0, &so->loop_src); \
    gau_sample_source_loop_set(so->loop_src, -1, 0); \
    ga_handle_play(so->handle); \
  }
#define WISE_PLAY_SOUND(S, TAG, X, Y, N) \
  if(SO_WiseToPlay(gi, TAG, (X), (Y), (N))) { \
      PLAY_SOUND(S, TAG) \
      TR_sound_t *so = &sound(s); \
      so->x = (X); \
      so->y = (Y); \
    }
#define SELECTED(U) \
  CAM_IsUnitSelected(gi, GI_GetCamera(gi), U)

#define INFO(UN)                          ((UN)->unit_class.info)
#define HERO(UN)                          ((UN)->unit_class.is_hero)
#define LEVEL(UN)                         ((UN)->unit_class.level)
#define GOLD(UN)                          ((UN)->unit_class.gold_price)
#define XP(UN)                            ((UN)->unit_class.xp)
#define BUILD_TIME(UN)                    ((UN)->unit_class.build_time)
#define MOVE_SPEED(UN)                    ((UN)->unit_class.move_speed)
#define ATTACK_SPEED_PER_LEVEL(UN)        ((UN)->unit_class.attack_speed_per_level)
#define ATTACK_SPEED(UN)                  (TR_exp_100(LEVEL(UN)-1, (UN)->unit_class.attack_speed, (100*100)-ATTACK_SPEED_PER_LEVEL(UN)))
#define ATTACK_DAMAGE_PER_LEVEL(UN)       ((UN)->unit_class.attack_damage_per_level)
#define ATTACK_DAMAGE(UN)                 ((UN)->unit_class.attack_damage + LEVEL(UN)*ATTACK_DAMAGE_PER_LEVEL(UN))
#define ATTACK_TYPE(UN)                   ((UN)->unit_class.attack_type)
#define HP_PER_LEVEL(UN)                  ((UN)->unit_class.hp_per_level)
#define TIME_TO_LIVE(UN)                  ((UN)->unit_class.time_to_live)
#define MAX_HP(UN)                        ((UN)->unit_class.max_hp + LEVEL(UN)*HP_PER_LEVEL(UN))
#define HP_REGEN_PER_LEVEL(UN)            ((UN)->unit_class.hp_regen_per_level)
#define HP_REGEN(UN)                      ((UN)->unit_class.hp_regen + LEVEL(UN)*HP_REGEN_PER_LEVEL(UN))
#define MANA_PER_LEVEL(UN)                ((UN)->unit_class.mana_per_level)
#define MAX_MANA(UN)                      ((UN)->unit_class.max_mana + LEVEL(UN)*MANA_PER_LEVEL(UN))
#define MANA_REGEN_PER_LEVEL(UN)          ((UN)->unit_class.mana_regen_per_level)
#define MANA_REGEN(UN)                    ((UN)->unit_class.mana_regen + LEVEL(UN)*MANA_REGEN_PER_LEVEL(UN))
#define ARMOR_PER_LEVEL(UN)               ((UN)->unit_class.armor_per_level)
#define ARMOR(UN)                         ((UN)->unit_class.armor + LEVEL(un)*ARMOR_PER_LEVEL(UN)/10)
#define ARMOR_TYPE(UN)                    ((UN)->unit_class.armor_type)
#define SIZE(UN)                          ((UN)->unit_class.size)
#define VISIBILITY_RANGE(UN)              ((UN)->unit_class.visibility_range)
#define AGGRO_RANGE(UN)                   ((UN)->unit_class.aggro_range)
#define ATTACK_RANGE(UN)                  ((UN)->unit_class.attack_range)
#define CAN_BE_PUSHED(UN)                 ((UN)->unit_class.can_be_pushed)
#define BALL_PARTICLE(UN)                 ((UN)->unit_class.ball_particle)
#define BALL_MESH(UN)                     ((UN)->unit_class.ball_mesh)
#define BALL_MESH_SIZE(UN)                ((UN)->unit_class.ball_mesh_size)
#define BALL_MESH_CINETIC_TYPE(UN)        ((UN)->unit_class.ball_mesh_cinetic_type)
#define BALL_SPEED(UN)                    ((UN)->unit_class.ball_speed)
#define BALL_OFFSET(UN)                   ((UN)->unit_class.ball_offset)
#define BALL_HEIGHT(UN)                   ((UN)->unit_class.ball_height)
#define BALL_SIZE(UN)                     ((UN)->unit_class.ball_size)
#define BALL_EN_CLOCHE(UN)                ((UN)->unit_class.ball_en_cloche)
#define BALL_ADDITIVE(UN)                 ((UN)->unit_class.ball_additive)
#define EXPLOSION_TYPE(UN)                ((UN)->unit_class.explosion_type)
#define MESH(UN)                          ((UN)->unit_class.mesh)
#define MESH_COUNT(UN)                    ((UN)->unit_class.mesh_count)
#define SELECT_SCALE(UN)                  ((UN)->unit_class.select_scale)
#define MODEL_SCALE(UN)                   ((UN)->unit_class.model_scale)
#define ANIM_SPEED(UN)                    ((UN)->unit_class.anim_speed)
#define BAR_HEIGHT(UN)                    ((UN)->unit_class.bar_height)
#define HP_BAR_SIZE(UN)                   ((UN)->unit_class.hp_bar_size)
#define MN_BAR_SIZE(UN)                   ((UN)->unit_class.mn_bar_size)
#define BLEND_WITH_POSE(UN)               ((UN)->unit_class.blend_with_pose)
#define START_BALL_WITH_ATTACK_ANIM(UN)   ((UN)->unit_class.start_ball_with_attack_anim)
#define DIE_WITH_BLOOD(UN)                ((UN)->unit_class.die_with_blood)
#define Y_ANGLE(UN)                       ((UN)->unit_class.y_angle)
#define Y_OFFSET(UN)                      ((UN)->unit_class.y_offset)
#define FLYING(UN)                        ((UN)->unit_class.flying)
#define MAGIC_VISION(UN)                  ((UN)->unit_class.has_magic_vision)
  
#define MAX_PEER                      32
#define MAX_PLAYER                    12
#define MAX_TEAM                      8
#define MAX_PATH_LEN                  (MAP_SIZE_X * 5)
#define MAX_BLOCKED_TIME              1000
#define MAX_SPECIAL_BLOCKED_TIME      3000
#define FOLLOW_COUNTER                150
#define UNIT_WAIT_TIME                350
#define FOLLOW_R_SQUARED              ((SIZE(un)*3) * (SIZE(un)*3))
#define MAX_FORMATION_MOVE            (20<<POS_SHIFT)
#define MAX_DIST                      (20<<POS_SHIFT)
#define MAX_DEADLOCK_TELEPORT         (SIZE(un)*3)
#define MAX_ADVANCE_TURN              10

#define BACKGROUND_COLOR              TCOD_color_RGB(10, 10, 10)
#define KEY_COLOR                     TCOD_color_RGB(255, 0, 255)
#define SPEC_COLOR                    TCOD_color_RGB(20, 20, 20)
#define EMPTY_COLOR                   TCOD_color_RGB(94, 61, 40)//TCOD_color_RGB(116, 88, 62)//TCOD_color_RGB(94, 61, 40)
#define WALL_COLOR                    TCOD_color_RGB(59, 79, 117)//TCOD_color_RGB(76, 91, 118)//TCOD_color_RGB(158, 145, 126)//TCOD_color_RGB(76, 91, 118)//TCOD_dark_grey
#define WATER_COLOR                   TCOD_blue
#define WATER2_COLOR                  TCOD_darkest_blue
#define GRASS_COLOR                   TCOD_color_RGB(52, 150, 52)
#define BUSH1_COLOR                   TCOD_color_RGB(0, 142, 0)
#define BUSH2_COLOR                   TCOD_color_RGB(0, 130, 0)
#define TREE1_COLOR                   TCOD_color_RGB(0, 100, 0)
#define TREE2_COLOR                   TCOD_color_RGB(0, 150, 0)
#define TREE3_COLOR                   TCOD_color_RGB(0, 135, 0)
#define MUSH_COLOR                    TCOD_color_RGB(230, 10, 5)
#define TILE_COLOR                    TCOD_dark_grey
#define TILE2_COLOR                   TCOD_light_grey
#define TORCH_COLOR                   TCOD_amber
#define PLANK_COLOR                   TCOD_color_RGB(97, 58, 16)
#define PLANK2_COLOR                  TCOD_color_RGB(73, 37, 10)
#define BLUE                          TCOD_color_RGB(34, 70, 240)//TCOD_color_RGB(34, 90, 192)
#define RED                           TCOD_color_RGB(240, 29, 29)
#define BLOOD_COLOR                   TCOD_color_RGB(195, 25, 12)
#define BURN_TRACE_COLOR              TCOD_color_RGB(45, 45, 45)
#define FLUID_COLOR                   TCOD_color_RGB(210, 210, 210)
#define LIGHTNING_COLOR               TCOD_color_RGB(150, 200, 255)

#define TR_EMPTY                     '.'   
#define TR_WALL                      '#'
#define TR_WATER                     '~' 
#define TR_WATER2                    '~' 
#define TR_GRASS                     ','
#define TR_BUSH1                     ';'
#define TR_BUSH2                     ':'
#define TR_TREE1                     0x5
#define TR_TREE2                     0x6
#define TR_TREE3                     0x14
#define TR_MUSH                      0x18
#define TR_TILE                      0x16
#define TR_TILE2                     0x4
#define TR_TORCH                     '!' 
#define TR_PLANK                     '=' 
#define TR_PLANK2                    0xbd  

struct                  TR_game_instance_t;

struct TR_buffer_t {
  char *data;
  int  data_length;
};

typedef short           TR_ptr_t;
typedef int             TR_id_t;
typedef int             TR_user_data_type_t;
typedef int             TR_time_t;
typedef unsigned int    TR_ascii_t;
typedef str             TR_net_ip_t;
typedef unsigned short  TR_net_port_t;
typedef short           TR_path_ptr;
typedef short           TR_unit_ptr;
typedef short           TR_unit_type_t;
typedef short           TR_player_ptr;
typedef short           TR_camera_ptr;
#define path(p, w)                  (gi->path_pool[p][w])
#define path2(p, w)                 (gi->path_pool2[p][w])
#define path_point_ptr(id, index)   (gi->paths + (((id) * MAX_PATH_LEN + (index))<<1))
#define MAX_UNIT_ENTITY             4

enum TR_skill_type_t {
  #include "gen/skill_type.cpp"
  TR_skill_type_count
};

/*
 * edge
 */ 
#define MAX_EDGE        MAP_SIZE
#define MAX_REFLEX      2048
#define MAX_PATCH_EDGE  MAX_EDGE
#define TIP_PER_REFLEX  64

struct TR_edge_t {
  int   x0, 
        x1, 
        y0, 
        y1,
        i,
        j, 
        e0, 
        e1,
        r0,
        r1;
};

/* 
 * astar nodes 
 */
typedef short TR_astar_node_ptr;

#define MAX_ASTAR_NODE  (MAP_SIZE)
#define astar_node(N)   (wo->pool[N])
struct TR_astar_node_t {
  int x,
      y,
      i,
      j,
      g,
      h,
      f,
      k;
  bool r;
  TR_astar_node_ptr next,
                     parent;
};

/*
 * contact nodes
 */
typedef short TR_contact_ptr;

#define MAX_CONTACT  MAX_UNIT
#define contact(N)   (pool[N])
struct TR_contact_t {
  int               t,
                    xa, 
                    ya,
                    xb,
                    yb,
                    ra,
                    rb;
  TR_unit_ptr      ua,
                    ub;
  TR_contact_ptr   next;
};

/*
 * buttons
 */

#define BTN_GLARE_SIZE  4
#define MAX_BTN         64

typedef short TR_client_ptr;

struct TR_btn_t {
  TCOD_color_t    color;
  str             s;
  short           pos_x, 
                  pos_y;
  char            shortcut;
  TCOD_keycode_t  shortcut_special;
  bool            highlight,
                  entry;
  void            *user_data;
};

struct TR_btn_list_t {
  TR_btn_t   btns[MAX_BTN];
  short     btn_count;
  short     pos_x,
            pos_y;
};

void BTN_UpdateEntry    (TR_game_instance_t *gi, TR_btn_t *b);
void BTN_Render         (SDL_Surface *surface, TR_game_instance_t *gi, TR_btn_t *b, bool left=false);
void BTN_Render         (SDL_Surface *surface, TR_game_instance_t *gi, TR_btn_list_t *l, bool center_x=true, short step=2, bool left=false);
bool BTN_IsMouseOver    (TR_game_instance_t *gi, TR_btn_t *b, bool left=false);
bool BTN_IsClicked      (TR_game_instance_t *gi, TR_btn_t *b, bool left=false);
bool BTN_GonnaBeClicked (TR_game_instance_t *gi, TR_btn_t *b, bool left=false);
void BTN_CenterX        (TR_game_instance_t *gi, TR_btn_t *b, short axe=-1);
void BTN_CenterY        (TR_game_instance_t *gi, TR_btn_t *b);
void BTN_CenterY        (TR_game_instance_t *gi, TR_btn_list_t *l);

/*
 * unit order
 */

#define MAX_UNIT_ORDER        30000
#define UNIT_X(UN)            ((UN)->x + UN_GetSizeShift(gi, SIZE((UN))))
#define UNIT_Y(UN)            ((UN)->y + UN_GetSizeShift(gi, SIZE((UN))))
#define order(ptr)            (gi->orders[(ptr)])
typedef short TR_unit_order_ptr;
typedef void (*TR_unit_order_cb_t)(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_order_ptr o);

enum TR_unit_order_type_t {
  TR_unit_order_none,
  TR_unit_order_follow_goal,
  TR_unit_order_move,
  TR_unit_order_skill,
  TR_unit_order_count
};

struct TR_unit_order_t {
  TR_unit_order_type_t    order;
  TR_unit_ptr             target,
                          forced_action_unit;
  TR_id_t                 target_cmd_id,
                          target_player_id,
                          forced_action_unit_cmd_id,
                          forced_action_unit_player_id,
                          cmd_mask,
                          task_id;
  TR_skill_type_t         skill_type;
  int                     target_cx,
                          target_cy,
                          click_cx,
                          click_cy,
                          chase_cx_proxy,
                          chase_cy_proxy,
                          path_len_proxy,
                          last_target_cx,
                          last_target_cy,
                          r_squared,
                          d_squared,
                          forced_action_mark;
  TR_unit_order_ptr       fnext,
                          next;
  TR_time_t               counter;                       
  bool                    started,
                          starting,
                          started_proxy,
                          done,
                          with_counter,
                          skill_here,
                          precise_target,
                          following_friend,
                          /* the target/rally point of generated orders aren't rendered */
                          generated,
                          found_path_proxy,
                          forced_action;
  TR_ptr_t                user_data;
  TR_unit_order_cb_t      unit_order_completed_cb,
                          unit_order_failed_cb;
};

void                  UO_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_unit_order_ptr     UO_New          (TR_game_instance_t *gi);
TR_unit_order_ptr     UO_PushFront    (TR_game_instance_t *gi, TR_unit_ptr u);
TR_unit_order_ptr     UO_PushBack     (TR_game_instance_t *gi, TR_unit_ptr u);
void                  UO_FreeFront    (TR_game_instance_t *gi, TR_unit_ptr u, bool completed=false);
void                  UO_FreeAll      (TR_game_instance_t *gi, TR_unit_ptr u);
TR_unit_order_type_t  UO_GetFrontType (TR_game_instance_t *gi, TR_unit_ptr u);
TR_unit_order_t*      UO_GetFront     (TR_game_instance_t *gi, TR_unit_ptr u);
bool                  UO_IsTarget     (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, TR_unit_order_ptr o=TR_null);

/*
 * particle_systems
 */

#define MAX_PARTICLE_SYSTEM        4096
#define particle_system(ptr)       (gi->particle_systems[(ptr)])
typedef short TR_particle_system_ptr;

#define START_PARTICLE(NAME, X, Y, Z, SCALE) \
  TR_particle_system_ptr ps = PS_New(gi); \
  if(ps == TR_null) { \
    ps = gi->particle_system_head; \
    while(ps != TR_null) { \
      log(particle_system(ps).name); \
      ps = particle_system(ps).next; \
    } \
  } \
  assert(ps != TR_null); \
  TR_particle_system_t *psys = &particle_system(ps); \
  psys->name = NAME; \
  psys->x = X; \
  psys->y = Y; \
  psys->z = Z; \
  psys->scale = psys->scale * (SCALE);\
  PS_Start(gi, ps);

struct TR_particle_system_t {
  Ogre::SceneNode                   *ogre_node,
                                    *parent_node;
  ParticleUniverse::ParticleSystem  *ogre_particle;
  str                               name;
  int                               x,
                                    y;
  float                             z,
                                    scale;
  bool                              stopped,
                                    free_it;
  TR_particle_system_ptr           unext,
                                    next,
                                    previous,
                                    fnext;
};

void                      PS_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_particle_system_ptr    PS_New          (TR_game_instance_t *gi);
void                      PS_Free         (TR_game_instance_t *gi, TR_particle_system_ptr b);
void                      PS_FreeAll      (TR_game_instance_t *gi, TR_particle_system_ptr b);
bool                      PS_Update       (TR_game_instance_t *gi, TR_player_ptr cam_p, TR_particle_system_ptr b);
void                      PS_Start        (TR_game_instance_t *gi, TR_particle_system_ptr ps);
void                      PS_Stop         (TR_game_instance_t *gi, TR_particle_system_ptr ps, bool force=false);
bool                      PS_IsStopped    (TR_game_instance_t *gi, TR_particle_system_ptr ps);

/*
 * balls
 */

#define MAX_BALL        512
#define MAX_BALL_SPEED  500
#define ball(ptr)       (gi->balls[(ptr)])
typedef short TR_ball_ptr;
typedef short TR_player_ptr;

enum TR_ball_type_t {
  TR_ball_type_normal,
  TR_ball_type_projectile_one_hit,
  TR_ball_type_projectile_distance,
};

enum TR_ball_mesh_cinetic_type_t {
  TR_ball_mesh_cinetic_type_none,
  TR_ball_mesh_cinetic_type_rotating,
};

enum TR_armor_type_t {
  TR_armor_type_light,
  TR_armor_type_medium,
  TR_armor_type_heavy,
  TR_armor_type_fortified,
  TR_armor_type_count
};

enum TR_attack_type_t {
  TR_attack_type_normal,
  TR_attack_type_piercing,
  TR_attack_type_magic,
  TR_attack_type_crashing,
  TR_attack_type_chaos,
  TR_attack_type_count
};

struct TR_explosion_class_t {
  str             mesh_name;
  TR_unit_type_t  unit_type;
  ga_Sound        *explosion_sound;
  float           size;
  str             particle;
  short           total_frame;
  bool            additive;
};

struct TR_ball_t {
  TR_ball_type_t         ball_type;
  TR_player_ptr          p;
  TR_explosion_class_t   explosion_type;
  TR_particle_system_ptr ps;
  Ogre::SceneNode         *node;
  Ogre::Entity            *ent;
  TR_time_t              move;
  int                     x,
                          y,
                          z,
                          dx,
                          dy,
                          radius,
                          target_cx,
                          target_cy,
                          target_cz,
                          speed,
                          damage,
                          /* used for en cloche */
                          attack_range,
                          distance,
                          /* rtti for the onhit event */
                          type;
  TR_attack_type_t        attack_type;
  /* mesh rotating axe */
  float                   ax,
                          ay,
                          az,
                          mesh_angle;
  TR_ball_mesh_cinetic_type_t ball_mesh_cinetic_type;
  bool                    additive,
                          en_cloche,
                          look_for_ground,
                          look_for_air;
  TR_unit_ptr            u,
                          target;
  TR_id_t                target_player_id,
                          target_cmd_id;
  TR_ball_ptr            next,
                          previous,
                          fnext;
};

void                  BA_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_ball_ptr          BA_New          (TR_game_instance_t *gi);
void                  BA_Free         (TR_game_instance_t *gi, TR_ball_ptr b);
void                  BA_FreeAll      (TR_game_instance_t *gi, TR_ball_ptr b);
bool                  BA_Update       (TR_game_instance_t *gi, TR_ball_ptr b);

/*
 * burn_trace
 */

#define MAX_BURN_TRACE        8392
#define BURN_TRACE_LIFE_SPAN  1000
#define burn_trace(ptr)       (gi->burn_traces[(ptr)])
typedef short TR_burn_trace_ptr;

struct TR_burn_trace_t {
  Ogre::SceneNode *node;
  Ogre::Entity    *ent;
  TCOD_color_t  c;
  TR_time_t    span;
  int           x,
                y;
  TR_burn_trace_ptr  next,
                previous,
                fnext;
};

void                  BT_Init       (TR_game_instance_t *gi, int argc, char** argv);
TR_burn_trace_ptr    BT_New        (TR_game_instance_t *gi);
void                  BT_Free       (TR_game_instance_t *gi, TR_burn_trace_ptr b);
void                  BT_FreeAll    (TR_game_instance_t *gi, TR_burn_trace_ptr b);
bool                  BT_Update     (TR_game_instance_t *gi, TR_burn_trace_ptr b);

/*
 * floating_text
 */

#define MAX_FLOATING_TEXT           4096
#define FLOATING_TEXT_LIFE_SPAN     2000
#define floating_text(ptr)          (gi->floating_texts[(ptr)])
typedef short TR_floating_text_ptr;

struct TR_floating_text_t {
  Ogre::Overlay                 *overlay;
  Ogre::TextAreaOverlayElement  *text_overlay;
  Ogre::OverlayContainer        *panel;
  TCOD_color_t          c;
  TR_time_t            span;
  str                   s;
  int                   x,
                        y,
                        z,
                        char_size,
                        offset_y;
  TR_floating_text_ptr next,
                        previous,
                        fnext;
};

void                  FT_Init       (TR_game_instance_t *gi, int argc, char** argv);
TR_floating_text_ptr FT_New        (TR_game_instance_t *gi);
void                  FT_Free       (TR_game_instance_t *gi, TR_floating_text_ptr ft);
void                  FT_FreeAll    (TR_game_instance_t *gi, TR_floating_text_ptr ft);
bool                  FT_Update     (TR_game_instance_t *gi, TR_floating_text_ptr ft);

/*
 * lights
 */

#define MAX_LIGHT         256
#define light(ptr)        (gi->lights[(ptr)])
typedef short TR_light_ptr;

struct TR_light_t {
  TCOD_color_t  c;
  int           r,
                x,
                y;
  float         i;
  TR_light_ptr next,
                previous,
                fnext;
};

void                  LI_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_light_ptr         LI_New          (TR_game_instance_t *gi);
void                  LI_Free         (TR_game_instance_t *gi, TR_light_ptr l);
void                  LI_FreeAll      (TR_game_instance_t *gi, TR_light_ptr l);
void                  LI_Update       (TR_game_instance_t *gi);

/*
 * sounds
 */

#define MAX_SOUND           256
#define sound(ptr)          (gi->sounds[(ptr)])
#define SOUND_WISE_RADIUS    (10<<POS_SHIFT)
#define SOUND_MAX_WISE_COUNT 5
typedef short TR_sound_ptr;
typedef short TR_client_ptr;

struct TR_sound_t {
  str                   tag;
  ga_Handle             *handle;
  gau_SampleSourceLoop  *loop_src;
  float                 volume;
  TR_unit_ptr          u;
  int                   x,
                        y;
  TR_time_t            time;
  TR_sound_ptr         next,
                        previous,
                        fnext;
};

void                  SO_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_sound_ptr         SO_New          (TR_game_instance_t *gi);
void                  SO_Free         (TR_game_instance_t *gi, TR_sound_ptr l);
void                  SO_FreeAll      (TR_game_instance_t *gi, TR_sound_ptr l);
void                  SO_Update       (TR_game_instance_t *gi, TR_client_ptr c);
bool                  SO_WiseToPlay   (TR_game_instance_t *gi, const str &tag, int pos_x, int pos_y, int frame_diff);

/*
 * fluids
 */

#define MAX_FLUID         32
#define FLUID_MAX_COUNTER 10
#define FLUID_SPAN        5000
#define fluid(ptr)        (gi->fluids[(ptr)])
typedef short TR_fluid_ptr;
typedef short TR_client_ptr;

struct TR_worker_fluid_param_t {
  TR_game_instance_t *gi;
  TR_fluid_ptr f;
  TR_time_t game_time_step;
};

struct TR_fluid_t {
  float                   fluid_s[FLUID_BOX_SIZE_SQUARE],
                          fluid_density[FLUID_BOX_SIZE_SQUARE],
                          fluid_final_density[FLUID_BOX_SIZE_SQUARE],
                          fluid_max_density[FLUID_BOX_SIZE_SQUARE],
                          fluid_Vx[FLUID_BOX_SIZE_SQUARE],
                          fluid_Vy[FLUID_BOX_SIZE_SQUARE],
                          fluid_Vx0[FLUID_BOX_SIZE_SQUARE],
                          fluid_Vy0[FLUID_BOX_SIZE_SQUARE],
                          diffusion;
  short                   counter,
                          pos_x,
                          pos_y;
  TR_time_t              span;
  TCOD_color_t            color;
  TR_fluid_ptr           next,
                          previous,
                          fnext;
  TR_worker_fluid_param_t worker_param;
  HANDLE                  handle;
  DWORD                   id;
};

void          FL_Init       (TR_game_instance_t *gi, int argc, char** argv);
TR_fluid_ptr FL_New        (TR_game_instance_t *gi);
void          FL_Free       (TR_game_instance_t *gi, TR_fluid_ptr l);
void          FL_FreeAll    (TR_game_instance_t *gi, TR_fluid_ptr l);
void          FL_Step       (TR_game_instance_t *gi, TR_fluid_ptr l);
bool          FL_Update     (TR_game_instance_t *gi, TR_fluid_ptr l);
DWORD WINAPI  FL_WorkerFun  (void *param);
void          FL_SetBnd     (TR_game_instance_t *gi, int b, float *x);
void          FL_LinSolve   (TR_game_instance_t *gi, int b, float *x, float *x0, float a, float c, int iter);
void          FL_Diffuse    (TR_game_instance_t *gi, int b, float *x, float *x0, float diff, int iter);
void          FL_Project    (TR_game_instance_t *gi, float *velocX, float *velocY, float *p, float *div, int iter);
void          FL_Advect     (TR_game_instance_t *gi, int b, float *d, float *d0,  float *velocX, float *velocY);

/*
 * explosions
 */

#define MAX_EXPLOSION         MAX_UNIT
#define EXPLOSION_FRAME_SPAN  100
#define explosion(ptr)        (gi->explosions[(ptr)])
typedef short TR_explosion_ptr;

struct TR_explosion_t {
  str                     mesh_name;
  Ogre::SceneNode         *node;
  Ogre::Entity            *ent[MAX_UNIT_ENTITY];
  TR_particle_system_ptr  ps;
  TR_time_t               span;
  int                     x,
                          y,
                          current_frame,
                          total_frame;
  float                   size;
  bool                    additive;
  TR_explosion_ptr        next,
                          previous,
                          fnext;
};

void                    EX_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_explosion_ptr        EX_New          (TR_game_instance_t *gi);
void                    EX_Free         (TR_game_instance_t *gi, TR_explosion_ptr l);
void                    EX_FreeAll      (TR_game_instance_t *gi, TR_explosion_ptr l);
bool                    EX_Update       (TR_game_instance_t *gi, TR_explosion_ptr c);

/*
 * build_explosions
 */

#define MAX_BUILD_EXPLOSION       MAX_FLUID
#define BUILD_EXPLOSION_SPAN      2500
#define build_explosion(ptr)      (gi->build_explosions[(ptr)])
typedef short TR_build_explosion_ptr;
typedef short TR_cadaver_ptr;

struct TR_build_explosion_t {
  str                     ent_name[MAX_UNIT_ENTITY];
  int                     mesh_count;
  TR_cadaver_ptr          c;
  TR_time_t               span;
  int                     state;
  int                     x,
                          y,
                          size;
  float                   model_scale;
  TR_build_explosion_ptr  next,
                          previous,
                          fnext;
};

void                    BE_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_build_explosion_ptr  BE_New          (TR_game_instance_t *gi);
void                    BE_Free         (TR_game_instance_t *gi, TR_build_explosion_ptr l);
void                    BE_FreeAll      (TR_game_instance_t *gi, TR_build_explosion_ptr l);
bool                    BE_Update       (TR_game_instance_t *gi, TR_build_explosion_ptr c);

/*
 * cadavers
 */

#define MAX_CADAVER         MAX_UNIT
#define CADAVER_FADE_SPAN   20000
#define cadaver(ptr)        (gi->cadavers[(ptr)])
typedef short TR_cadaver_ptr;

struct TR_cadaver_t {
  Ogre::AnimationState  *anim_state[MAX_UNIT_ENTITY];
  str                   ent_name[MAX_UNIT_ENTITY],
                        anim_name;
  Ogre::SceneNode       *node;
  Ogre::Entity          *ent[MAX_UNIT_ENTITY];
  Ogre::Quaternion      ori;
  int                   mesh_count;
  TR_time_t             span;
  int                   x,
                        y,
                        force_x,
                        force_y,
                        size;
  float                 lookat_x, 
                        lookat_y,
                        model_scale,
                        y_angle,
                        y_offset;
  bool                  fade,
                        flying;
  TR_cadaver_ptr       next,
                        previous,
                        fnext;
};

void                    CA_Init     (TR_game_instance_t *gi, int argc, char** argv);
TR_cadaver_ptr         CA_New      (TR_game_instance_t *gi);
void                    CA_Free     (TR_game_instance_t *gi, TR_cadaver_ptr l);
void                    CA_FreeAll  (TR_game_instance_t *gi, TR_cadaver_ptr l);
bool                    CA_Update   (TR_game_instance_t *gi, TR_cadaver_ptr c);

/*
 * lightnings
 */

#define MAX_LIGHTNING             256
#define LIGHTNING_STEP_COUNT      6             
#define MAX_LIGHTNING_POINT       (32)
#define LIGHTNING_SPAN            300
#define lightning(ptr)      (gi->lightnings[(ptr)])
typedef short TR_lightning_ptr;

struct TR_lightning_t {
  unsigned int            seed;
  TR_time_t              span;
  float                   opacity;
  short                   start_x,
                          start_y,
                          end_x,
                          end_y,
                          point_count,
                          pos_x[MAX_LIGHTNING_POINT],
                          pos_y[MAX_LIGHTNING_POINT];
  TCOD_color_t            color;
  TR_lightning_ptr next,
                          previous,
                          fnext;
};

void                    LG_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_lightning_ptr       LG_New          (TR_game_instance_t *gi);
void                    LG_Free         (TR_game_instance_t *gi, TR_lightning_ptr l);
void                    LG_FreeAll      (TR_game_instance_t *gi, TR_lightning_ptr l);
bool                    LG_Update       (TR_game_instance_t *gi, TR_lightning_ptr c);
void                    LG_Compute      (TR_game_instance_t *gi, TR_lightning_ptr l);
void                    LG_BuildPoints  (TR_game_instance_t *gi, TR_lightning_ptr l, int x0, int y0, int x1, int y1, int i, int j, int level);

/*
 * status_effects
 */

#define MAX_STATUS_EFFECT       4096
#define status_effect(ptr)      (gi->status_effects[(ptr)])
typedef short TR_status_effect_ptr;

struct TR_status_effect_t {
  Ogre::Overlay             *buf_overlay;
  str                       buf_material;
  TR_player_ptr             dealer_p;
  int                       type,
                            amount;
  TR_time_t                 span;
  TCOD_color_t              color;
  int                       alpha,
                            cache;
  bool                      evil,
                            buf;
  TR_ptr_t                  user_data;
  TR_particle_system_ptr    ps,
                            ps2;
  TR_status_effect_ptr      next,
                            previous,
                            fnext;
};

void                  ST_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_status_effect_ptr  ST_New          (TR_game_instance_t *gi, TR_unit_ptr u, int type);
void                  ST_Free         (TR_game_instance_t *gi, TR_unit_ptr u, TR_status_effect_ptr l);
void                  ST_FreeAll      (TR_game_instance_t *gi, TR_unit_ptr u, TR_status_effect_ptr l);
void                  ST_Update       (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
void                  ST_Remove       (TR_game_instance_t *gi, TR_unit_ptr u, int type);
int                   ST_GetBufCount  (TR_game_instance_t *gi, TR_unit_ptr u);
void                  ST_SetBufPos    (TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, bool vis);

/*
 * build order
 */

#define MAX_BUILD_ORDER     MAX_UNIT
#define build_order(ptr)    (gi->build_orders[(ptr)])
typedef short TR_build_order_ptr;

struct TR_build_order_t {
  TR_unit_type_t       unit_type;
  int                   target_cx,
                        target_cy;
  TR_id_t              unit_id,
                        unit_player_id;
  TR_time_t            progress;
  bool                  started;
  TR_user_data_type_t  user_data_type;
  TR_build_order_ptr   previous,
                        next,
                        fnext;
};

void                  BO_Init                   (TR_game_instance_t *gi, int argc, char** argv);
TR_build_order_ptr   BO_New                    (TR_game_instance_t *gi);
void                  BO_Free                   (TR_game_instance_t *gi, TR_build_order_ptr l);
void                  BO_FreeAll                (TR_game_instance_t *gi, TR_unit_ptr u);
void                  BO_PushFrontBuildOrder    (TR_game_instance_t *gi, TR_unit_ptr u, TR_build_order_ptr b);
void                  BO_PushBackBuildOrder     (TR_game_instance_t *gi, TR_unit_ptr u, TR_build_order_ptr b);
short                 BO_GetBuildOrderCount     (TR_game_instance_t *gi, TR_id_t player_id, TR_client_ptr c, TR_unit_ptr u);

/*
 * trees
 */

#define MAX_TREE       8384//16384
#define tree(ptr)      (gi->trees[(ptr)])
typedef short TR_tree_ptr;

struct TR_tree_t {
  Ogre::SceneNode   *node;
  Ogre::Entity      *ent[2];
  int               size,
                    x,
                    y;
  TR_tree_ptr      next,
                    previous,
                    fnext;
};

void                    TR_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_tree_ptr            TR_New          (TR_game_instance_t *gi);
void                    TR_Free         (TR_game_instance_t *gi, TR_tree_ptr l);
void                    TR_FreeAll      (TR_game_instance_t *gi, TR_tree_ptr l);

/*
 * unit_nodes
 */

#define MAX_UNIT_NODE       (MAX_UNIT*MAX_UNIT_SIZE*MAX_UNIT_SIZE)
#define unit_node(ptr)      (gi->unit_nodes[(ptr)])
typedef short TR_unit_node_ptr;

struct TR_unit_node_t {
  TR_unit_ptr      u;
  TR_unit_node_ptr mnext,
                    next,
                    previous,
                    fnext;
};

void                    NO_Init         (TR_game_instance_t *gi, int argc, char** argv);
TR_unit_node_ptr       NO_New          (TR_game_instance_t *gi);
void                    NO_Free         (TR_game_instance_t *gi, TR_unit_node_ptr l);
void                    NO_FreeAll      (TR_game_instance_t *gi, TR_unit_node_ptr l);

/*
 * animations
 */

#define MAX_ANIMATION           2048
#define ANIMATION_FADE_OUT_SPAN 150
#define ANIMATION_FADE_IN_SPEED 0.20f
#define ANIMATION_SPEED_FACTOR  1.f
#define animation(ptr)          (gi->animations[(ptr)])
typedef short TR_animation_ptr;

#define PLAY_UNIT_ANIM(U, NAME, LOOP) \
  { \
    AN_FadeOut(gi, U); \
    TR_animation_ptr a = AN_AlreadyPlaying(gi, U, NAME); \
    if(a == TR_null \
    && unit(U).ent[0]->hasAnimationState(NAME)) { \
      a = AN_New(gi, U); \
      TR_animation_t *an = &animation(a); \
      DO_TIMES(MESH_COUNT(un)) { \
        an->anim_state[f] = unit(U).ent[f]->getAnimationState(NAME); \
        if(!(LOOP)) \
          an->anim_state[f]->setTimePosition(0); \
        an->anim_state[f]->setLoop(LOOP); \
        an->anim_state[f]->setEnabled(true); \
        an->anim_state[f]->setWeight(an->weight); \
      } \
    } \
    if(a != TR_null) \
      animation(a).speed = ANIM_SPEED(&unit(U)); \
  }

#define PLAY_ANIM(E, AS, NAME, LOOP) \
  if((E)->hasSkeleton() \
  && (E)->hasAnimationState(NAME)) { \
    (AS) = (E)->getAnimationState(NAME); \
    (AS)->setLoop(LOOP); \
    (AS)->setEnabled(true); \
  }

struct TR_animation_t {
  Ogre::AnimationState  *anim_state[MAX_UNIT_ENTITY];
  TR_time_t            fade_span;
  float                 weight,
                        speed;
  TR_animation_ptr     next,
                        previous,
                        unext,
                        uprevious,
                        fnext;
};

void                    AN_Init           (TR_game_instance_t *gi, int argc, char** argv);
TR_animation_ptr       AN_New            (TR_game_instance_t *gi, TR_unit_ptr u);
void                    AN_Free           (TR_game_instance_t *gi, TR_unit_ptr u, TR_animation_ptr l);
void                    AN_FreeAll        (TR_game_instance_t *gi, TR_unit_ptr u);
void                    AN_Update         (TR_game_instance_t *gi, TR_unit_ptr u);
void                    AN_FadeOut        (TR_game_instance_t *gi, TR_unit_ptr u);
TR_animation_ptr       AN_AlreadyPlaying (TR_game_instance_t *gi, TR_unit_ptr u, const char *name, bool fade_in=true);
void                    AN_Reset          (TR_game_instance_t *gi, TR_unit_ptr u, const char *name);

/*
 * unit
 */

#define MAX_CLIENT                  MAX_PLAYER
#define MAX_UNIT                    1024
#define MAX_UNIT_TYPE               128
#define NONETHINK_SPAN              200
#define MAX_FORCE                   8
#define ATTACK_PROPAGATION_RADIUS   4
#define FLYING_HEIGHT               2
#define IDLE_ANIM_SPAN              500
#define WALK_ANIM_SPAN              150
#define unit(ptr)                   (gi->units[(ptr)])
typedef short TR_unit_type_t;

struct TR_unit_class_t {
  str                   info;
  int                   gold_price,
                        xp, /* xp it gives */
                        level;
  TR_time_t            build_time;
  int                   move_speed,
                        attack_speed,
                        attack_speed_per_level,
                        attack_damage,
                        attack_damage_per_level,
                        max_hp,
                        hp_regen,
                        hp_regen_per_level,
                        hp_per_level,
                        time_to_live,
                        max_mana,
                        mana_regen,
                        mana_regen_per_level,
                        mana_per_level,
                        armor,
                        armor_per_level,
                        size,   
                        visibility_range,
                        aggro_range,
                        attack_range,
                        hp_bar_size,
                        mn_bar_size,
                        skill_count;
  TR_skill_type_t       skills[MAX_SKILLS_PER_UNIT];
  TR_attack_type_t      attack_type;
  TR_armor_type_t       armor_type;
  bool                  can_be_pushed,
                        is_hero;
  str                   ball_particle,
                        ball_mesh;
  int                   ball_speed;
  Ogre::Vector3         ball_offset;
  int                   ball_height;
  float                 ball_size,
                        ball_mesh_size;
  TR_ball_mesh_cinetic_type_t ball_mesh_cinetic_type;
  bool                  ball_additive,
                        ball_en_cloche;
  TR_explosion_class_t explosion_type;
  str                   mesh[MAX_UNIT_ENTITY];
  int                   mesh_count;
  float                 select_scale,
                        model_scale,
                        anim_speed,
                        bar_height,
                        y_offset,
                        y_angle;
  bool                  blend_with_pose,
                        start_ball_with_attack_anim,
                        die_with_blood,
                        flying,
                        has_magic_vision;
};

/* please come at my cache-miss party */
struct TR_unit_t {
  Ogre::SceneNode         *node,
                          *select_node;
  Ogre::Entity            *ent[MAX_UNIT_ENTITY],
                          *select_ent,
                          *hover_ent;
  Ogre::Overlay           *hp_back_overlay,
                          *hp_overlay,
                          *hp_hit_overlay,
                          *mn_back_overlay,
                          *mn_overlay,
                          *mn_hit_overlay,
                          *time_to_live_back_overlay,
                          *time_to_live_overlay;
  TR_animation_ptr        animation_head;
  TR_unit_class_t         unit_class;
  TR_user_data_type_t     user_data_type;
  TR_ptr_t                user_data;
  TCOD_map_t              visibility_map;
  TR_unit_type_t          unit_type;
  TR_time_t               think,
                          move,
                          skill,
                          wait,
                          blocked_time,
                          skills_cooldown[MAX_SKILLS_PER_UNIT];
  bool                    skill_is_cooling_down[MAX_SKILLS_PER_UNIT];
  TR_unit_order_ptr       order_head;
  TR_id_t                 cmd_id,
                          player_id,
                          team_id;
  TR_build_order_ptr      build_order_head[MAX_PLAYER];
  int                     anim_span,
                          anim_time,
                          anim_step,
                          x,
                          y,
                          old_env_x,
                          old_env_y,
                          ddx,
                          ddy,
                          dx,
                          dy,
                          lookat_x,
                          lookat_y,
                          old_lookat_x,
                          old_lookat_y,
                          stun_force_x,
                          stun_force_y,
                          hp,
                          virtual_hp,
                          hp_regen_remaining,
                          mana,
                          mana_regen_remaining,
                          hit_hp,
                          hit_mana,
                          path_index,
                          path_len,
                          chase_cx,
                          chase_cy,
                          colision_mark,
                          penetration_mark,
                          skill_state,
                          skill_state2,
                          skill_state3,
                          virtual_hp_cache,
                          virtual_hp_cache2,
                          magic_vision,
                          silenced,
                          rooted,
                          stun,
                          uncontrolable,
                          blind,
                          invincibility,
                          no_aggro,
                          forced_action_marker,
                          skill_count,
                          skill_speed_factors[MAX_SKILLS_PER_UNIT],
                          skill_marks[MAX_SKILLS_PER_UNIT],
                          skill_levels[TR_skill_type_count];
  TR_skill_type_t         skills[MAX_SKILLS_PER_UNIT];
  TR_status_effect_ptr    status_effect_head;
  TR_particle_system_ptr  ps_head,
                          forced_action_ps;
  TR_unit_ptr             fnext,      /* free */
                          previous,   /* in use */
                          next,
                          sprevious[MAX_CLIENT],  /* selected */
                          snext[MAX_CLIENT],
                          pprevious,  /* player */
                          pnext,
                          gsprevious[MAX_GROUP_SELECTION], /* group selection */
                          gsnext[MAX_GROUP_SELECTION];
};

struct TR_get_unit_here_t {
  TR_unit_ptr  result[MAX_UNIT];
  short         count;
};

enum TR_get_unit_here_mode_t {
  TR_get_unit_here_mode_all,
  TR_get_unit_here_mode_friends,
  TR_get_unit_here_mode_enemies,
};

void                  UN_Init                   (TR_game_instance_t *gi, int argc, char** argv);
TR_unit_ptr           UN_New                    (TR_game_instance_t *gi);
void                  UN_Free                   (TR_game_instance_t *gi, TR_unit_ptr u);
void                  UN_FreeAll                (TR_game_instance_t *gi);
int                   UN_DistFromEdge           (TR_game_instance_t *gi, TR_edge_t *edge, int x, int y);
void                  UN_PutInMap               (TR_game_instance_t *gi, TR_unit_ptr u);
void                  UN_RemoveFromMap          (TR_game_instance_t *gi, TR_unit_ptr u);
bool                  UN_CanBeHere              (TR_game_instance_t *gi, int size, int x, int y, bool cannot_be_pushed_only);
bool                  UN_CanBeHereWallOnly      (TR_game_instance_t *gi, int size, int x, int y);
bool                  UN_Raycast                (TR_game_instance_t *gi, int x0, int y0, int x1, int y1, bool wall_only);
bool                  UN_Raycast                (TR_game_instance_t *gi, int size, int x0, int y0, int x1, int y1, bool wall_only=false);
void                  UN_GetUnitsHere           (TR_get_unit_here_t *data, TR_game_instance_t *gi, TR_unit_ptr not_this_one, TR_id_t team_id, int size, int x, int y, bool ground, bool air, TR_get_unit_here_mode_t mode=TR_get_unit_here_mode_all);
int                   UN_Overlap                (TR_game_instance_t *gi, TR_unit_ptr u, int size, int x, int y);
bool                  UN_HasOrder               (TR_game_instance_t *gi, TR_unit_ptr u, TR_id_t cmd_mask);
TR_status_effect_ptr  UN_HasStatusEffect         (TR_game_instance_t *gi, TR_unit_ptr u, int type);
void                  UN_PrintOrders            (TR_game_instance_t *gi, TR_unit_ptr u);
bool                  UN_CheckTargetMove        (TR_game_instance_t *gi, TR_unit_ptr u);
bool                  UN_IsFollowingThisTarget  (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target);
bool                  UN_CheckFollowTarget      (TR_game_instance_t *gi, TR_unit_ptr u, short level);
bool                  UN_CheckFollowTarget      (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target);
void                  UN_FindClosestPoint       (TR_game_instance_t *gi, int size, int *to_x, int *to_y, bool cannot_be_pushed_only=false);
int                   UN_GetDist                (TR_game_instance_t *gi, TR_unit_ptr u, int x, int y);
int                   UN_GetDist                (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr u2);
int                   UN_GetLight               (TR_game_instance_t *gi, TR_unit_ptr u, short pos_x, short pos_y, short u_pos_x, short u_pos_y, int opt_fact);
int                   UN_GetDamage              (TR_game_instance_t *gi, TR_unit_ptr u, int attack_damage, TR_attack_type_t attack_type);
void                  UN_DealInstantDamage      (TR_game_instance_t *gi, TR_player_ptr killer_p, TR_unit_ptr u, int attack_damage, TR_attack_type_t attack_type, bool show_hp=true);
bool                  UN_Intersect              (TR_game_instance_t *gi, TR_unit_ptr u, int x1, int y1, int x2, int y2);
TR_unit_ptr           UN_GetUnitHere            (TR_game_instance_t *gi, int x, int y);
int                   UN_GetSizeShift           (TR_game_instance_t *gi, int size);
float                 UN_GetSizeShiftFloat      (TR_game_instance_t *gi, int size);
void                  UN_LookAt                 (TR_game_instance_t *gi, TR_unit_ptr u, int lookat_x, int lookat_y);
void                  UN_Cleanse                (TR_game_instance_t *gi, TR_unit_ptr u);
void                  UN_AttachParticleSystem   (TR_game_instance_t *gi, TR_unit_ptr u, TR_particle_system_ptr ps);
void                  UN_PropagateAttack        (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, bool around_only=false);
void                  UN_AddHP                  (TR_game_instance_t *gi, TR_unit_ptr u, int hp);
void                  UN_SetMana                (TR_game_instance_t *gi, TR_unit_ptr u, int mn);
void                  UN_AddSkill               (TR_game_instance_t *gi, TR_unit_ptr u, TR_skill_type_t skill_type);
int                   UN_SkillId                (TR_game_instance_t *gi, TR_unit_ptr u, TR_skill_type_t skill_type);

/*
 * cmd
 */

#define MAX_CMD       50000
#define cmd(ptr)      (gi->cmds[(ptr)])
typedef int         TR_cmd_ptr;

enum TR_cmd_type_t {
  TR_cmd_type_none                 = 0,
  TR_cmd_type_unit_order           = 1,
  TR_cmd_type_spawn_unit           = 2,
  TR_cmd_type_stop_unit            = 3,
  TR_cmd_type_build_unit           = 4,
  TR_cmd_type_generic              = 5,
  TR_cmd_type_end_of_turn          = 6,
  TR_cmd_type_join_game            = 7,
  TR_cmd_type_seed_game            = 8,
  TR_cmd_type_leech_game           = 9,
  TR_cmd_type_ping                 = 10,
  TR_cmd_type_message              = 11,
  TR_cmd_type_disconnect           = 12,
  TR_cmd_type_game_ready           = 13,
  TR_cmd_type_port                 = 14,
  TR_cmd_type_players_connected    = 15,
  TR_cmd_type_game_closed          = 16,
  TR_cmd_type_timeout_peer_player  = 17,
};

#pragma pack(push, 1)
struct TR_unit_order_packet_t {
  TR_cmd_type_t        type;
  TR_id_t              id;
  TR_id_t              cmd_mask;
  int                   turn;
  int                   r_squared,
                        target_cx,
                        target_cy,
                        click_cx,
                        click_cy;
  TR_id_t              target,
                        target_player_id,
                        target_cmd_id;
  TR_skill_type_t       skill_type;
  bool                  skill_here,
                        push_back;
};
struct TR_spawn_unit_packet_t {
  TR_cmd_type_t         type;
  TR_id_t               id,
                        cmd_mask;
  TR_unit_type_t        unit_type;
  int                   turn,
                        target_cx,
                        target_cy;
  TR_user_data_type_t   user_data_type;
  bool                  find_pos;
};
struct TR_stop_unit_packet_t {
  TR_cmd_type_t        type;
  TR_id_t              id;
  int                   turn;
};
struct TR_build_unit_packet_t {
  TR_cmd_type_t          type;
  TR_id_t                id, /* the unit which build */
                          cmd_mask, /* the unit to build */
                          player_id,
                          target_player_id;
  int                     turn;
  TR_unit_type_t         unit_type;
  int                     target_cx,
                          target_cy;
  bool                    start_it,
                          cancel_it;
  TR_user_data_type_t    user_data_type;
};
struct TR_generic_packet_t {
  TR_cmd_type_t      type;
  TR_id_t            id,
                      player_id,
                      cmd_type,
                      cmd_mask;
  TR_unit_type_t     unit_type;
  int                 turn;
  int                 target_cx,
                      target_cy;
};
struct TR_end_of_turn_packet_t {
  TR_cmd_type_t type;
  TR_id_t      id;
  int           turn;
  short         turn_frame_count;
  TR_time_t    frame_time_step;
  unsigned int  hash;
};
struct TR_join_game_packet_t {
  TR_cmd_type_t type;
  int           team_id;
  char          player_name[MAX_MESSAGE_CHAR];
};
struct TR_seed_game_packet_t {
  TR_cmd_type_t type;
  int           player_id;
};
struct TR_leech_game_packet_t {
  TR_cmd_type_t type;
  int           player_id;
};
struct TR_ping_packet_t {
  TR_cmd_type_t type;
  TR_time_t    time;
};
struct TR_message_packet_t {
  TR_cmd_type_t  type;
  TR_id_t        team_id;
  char            username[MAX_MESSAGE_CHAR],
                  message[MAX_MESSAGE_CHAR];
};
struct TR_disconnect_packet_t {
  TR_cmd_type_t type;
  TR_id_t       player_id;
};
struct TR_game_ready_packet_t {
  TR_cmd_type_t  type;
  TR_id_t        player_id;
  unsigned int    seed;
};
struct TR_port_packet_t {
  TR_cmd_type_t  type;
  int             port;
  TR_id_t        player_id;
};
struct TR_players_connected_packet_t {
  TR_cmd_type_t  type;
  int             missing_player_count,
                  missing_player_ids[MAX_PLAYER];
};
struct TR_game_closed_packet_t {
  TR_cmd_type_t  type;
};
struct TR_timeout_peer_player_packet_t {
  TR_cmd_type_t  type;
  int             time;
  TR_id_t        player_id;
  char            player_name[MAX_MESSAGE_CHAR];
};
#pragma pack(pop)

struct TR_cmd_t {
  TR_time_t               frame_time_step;
  TR_user_data_type_t     user_data_type;
  TR_ptr_t                user_data;
  int                     turn;
  TR_cmd_type_t           type;
  TR_unit_type_t          unit_type;
  TR_id_t                 id,
                          cmd_type,
                          /* share by units with the same order */
                          cmd_mask;
  TR_cmd_ptr              fnext,
                          previous,
                          next,
                          cmd_next;
  int                     target_cx, 
                          target_cy,
                          click_cx, 
                          click_cy,
                          r_squared,
                          turn_frame_count;
  TR_id_t                 target,
                          target_player_id,
                          target_cmd_id,
                          player_id;
  TR_skill_type_t         skill_type;
  bool                    sent,
                          skill_here,
                          push_back,
                          start_it,
                          cancel_it,
                          find_pos;
  unsigned int            hash;
  TR_unit_order_cb_t      unit_order_completed_cb,
                          unit_order_failed_cb;
};

void              CMD_Init        (TR_game_instance_t *gi, int argc, char** argv);
TR_cmd_ptr       CMD_New         (TR_game_instance_t *gi, TR_id_t id);
void              CMD_Free        (TR_game_instance_t *gi, TR_cmd_ptr r);
void              CMD_FreeAll     (TR_game_instance_t *gi);

/*
 * cmd store
 */

#define SHORT_CONNECTION_TIMEOUT  5000
#define cmd_store(ptr)            (gi->cmd_stores[(ptr)])
typedef short TR_cmd_store_ptr;

struct TR_cmd_store_t {
  TR_cmd_store_ptr   fnext,
                      previous,
                      next;
  TR_cmd_ptr         cmd_head;
  list<TR_buffer_t*> cmd_buffers;
};

void                CS_Init                     (TR_game_instance_t *gi, int argc, char** argv);
TR_cmd_store_ptr   CS_New                      (TR_game_instance_t *gi);
void                CS_Free                     (TR_game_instance_t *gi, TR_cmd_store_ptr cs);
void                CS_FreeAll                  (TR_game_instance_t *gi);
void                CS_ClearCmdStore            (TR_game_instance_t *gi, TR_cmd_store_ptr cs);
void                CS_InsertCmd                (TR_game_instance_t *gi, TR_cmd_store_ptr r, TR_cmd_ptr c);
int                 CS_LeechCompressedBuffer    (TR_game_instance_t *gi, TR_cmd_store_ptr cs, int data_length, unsigned char *data);

/*
 * remote cmd store
 */

#define remote_cmd_store(ptr)           (gi->remote_cmd_stores[(ptr)])
typedef short TR_remote_cmd_store_ptr;

struct TR_remote_cmd_store_t {
  TR_cmd_store_ptr         cs;
  TR_id_t                  player_id;
  TR_remote_cmd_store_ptr  fnext,
                            previous,
                            next;
  ENetHost                  *host;
  ENetPeer                  *peer;
  ENetAddress               address;
  int                       ping;
  bool                      connected;
};

void                        RS_Init               (TR_game_instance_t *gi, int argc, char** argv);
TR_remote_cmd_store_ptr    RS_New                (TR_game_instance_t *gi, TR_cmd_store_ptr cmd_store);
void                        RS_Free               (TR_game_instance_t *gi, TR_remote_cmd_store_ptr r);
void                        RS_FreeAll            (TR_game_instance_t *gi);
void                        RS_Connect            (TR_game_instance_t *gi, TR_remote_cmd_store_ptr r);
void                        RS_WaitForConnection  (TR_game_instance_t *gi, TR_remote_cmd_store_ptr r);
void                        RS_Update             (TR_game_instance_t *gi, TR_remote_cmd_store_ptr r);
void                        RS_Leech              (TR_game_instance_t *gi, TR_remote_cmd_store_ptr r);

/*
 * camera
 */

#define MAX_CAMERA              4
#define camera(ptr)             (gi->cameras[(ptr)])
#define CAMERA_HEIGHT           13
#define CAMERA_ANGLE            90
#define MAX_SKILLS_PER_CAMERA   MAX_SKILLS_PER_UNIT

typedef short TR_camera_ptr;

struct TR_camera_t {
  Ogre::Camera         *camera;
  Ogre::Viewport       *viewport;
  Ogre::Overlay        *selection,
                       *lagometer,
                       *minimap,
                       *minimap_camera,
                       *skill_overlays[MAX_SKILLS_PER_CAMERA],
                       *skill_overlay_tex_panels[MAX_SKILLS_PER_CAMERA],
                       *skill_info_overlay,
                       *flash_overlay,
                       *win_lose_overlay;
  Ogre::TextAreaOverlayElement  
                       *skill_info_text_overlay,
                       *flash_text_overlay,
                       *win_lose_text_overlay;
  Ogre::RaySceneQuery  *ray_query;
  Ogre::AnimationState *banner_anim_state;
  Ogre::Entity         *last_hover,
                       *banner_ent;
  Ogre::SceneNode      *banner_node;
  Ogre::TexturePtr     fovmap_tex,
                       minimap_camera_tex,
                       lagometer_tex,
                       march_tex,
                       skill_overlay_texs[MAX_SKILLS_PER_CAMERA];
  int                  viewport_x,
                       viewport_y;
  TR_id_t             player_id;
  TR_camera_ptr       fnext,
                       previous,
                       next;
  /* human */ 
  int                  hud_state;
  TR_unit_type_t      selected_sub_group,
                       gs_sub_group[MAX_GROUP_SELECTION];
  TR_unit_ptr         selected_units_head,
                       gs_head[MAX_GROUP_SELECTION];
  bool                 selection_started,
                       pointing_minimap,
                       used_skill;
  int                  mp_lb_x, mp_lb_y,
                       mr_lb_x, mr_lb_y;
  bool                 skill_here,
                       window_opened,
                       window_locked,
                       typing,
                       send_to_all;
  TR_skill_type_t      skill_types[MAX_SKILLS_PER_CAMERA];
  int                  skill_type_count;
  int                  window_start_x,
                       window_start_y,
                       window_size_x,
                       window_size_y,
                       window_end_x,
                       window_end_y;
  short                selected_gs;
  TR_ptr_t            user_data;
  TR_btn_t            chat_btn;
  str                  flash;
  TR_time_t           flash_span;
  TCOD_color_t         flash_color;
};

void            CAM_Init                       (TR_game_instance_t *gi, int argc, char** argv);
TR_camera_ptr  CAM_New                        (TR_game_instance_t *gi, TR_id_t player_id);
void            CAM_Free                       (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_FreeAll                    (TR_game_instance_t *gi);
void            CAM_Update                     (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateInputs               (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateBanner               (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateSelection            (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateMouseHover           (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateFovMapTex            (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateMarchTex             (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateMinimapTex           (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateLagometer            (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateUnitRender           (TR_game_instance_t *gi, TR_camera_ptr ca);
bool            CAM_UpdateHUD                  (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateAndRenderWindow      (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateAndRenderChat        (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateFlash                (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateWinLose              (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_CloseWindow                (TR_game_instance_t *gi, TR_camera_ptr caa);
void            CAM_RenderFps                  (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_RenderPing                 (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_RenderStats                (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
bool            CAM_IsUnitSelected             (TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, short gs=-1);
short           CAM_GetSelectedUnitsCount      (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_SelectUnits                (TR_game_instance_t *gi, TR_camera_ptr ca, int s_cx, int s_cy, int e_cx, int e_cy, TR_unit_type_t type, bool extended);
bool            CAM_IsThereUnitsHere           (TR_game_instance_t *gi, TR_camera_ptr ca, int s_cx, int s_cy, int e_cx, int e_cy);
void            CAM_AddToSelection             (TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u);
void            CAM_SubToSelection             (TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, short gs=-1);
void            CAM_FreeUnitSelection          (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_AddToGroupSelection        (TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u, int gs);
void            CAM_CopySelectionTo            (TR_game_instance_t *gi, TR_camera_ptr ca, int gs);
void            CAM_RestoreGroupSelection      (TR_game_instance_t *gi, TR_camera_ptr ca, int gs, bool free_selection);
void            CAM_SelectNextSubGroup         (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_FocusOnUnits               (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_LookAt                     (TR_game_instance_t *gi, TR_camera_ptr ca, int x, int y);
void            CAM_UpdateWindowCloseButton    (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_OpenOptionWindow           (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateOptionWindow         (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_OpenWaitWindow             (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_CloseWaitWindow            (TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_UpdateWaitWindow           (SDL_Surface *surface, TR_game_instance_t *gi, TR_camera_ptr ca);
void            CAM_SetFlash                   (TR_game_instance_t *gi, TR_camera_ptr ca, const str &s, TCOD_color_t flash_color);
TR_unit_ptr    CAM_PickUnit                   (TR_game_instance_t *gi, TR_camera_ptr ca, int x, int y);
Ogre::Vector3   CAM_RayToVector                (TR_game_instance_t *gi, TR_camera_ptr ca, Ogre::Ray r);
Ogre::Ray       CAM_GetMouseRay                (TR_game_instance_t *gi, TR_camera_ptr ca);
Ogre::Vector2   CAM_WorldToScreen              (TR_game_instance_t *gi, TR_camera_ptr ca, float x, float y, float z);
Ogre::Vector3   CAM_ScreenToWorld              (TR_game_instance_t *gi, TR_camera_ptr ca, int x, int y);
int             CAM_GetRSquared                (TR_game_instance_t *gi, TR_camera_ptr ca);

/*
 * client
 */

#define client(ptr)                 (gi->clients[(ptr)])
#define HUD_STATE_OPTION_WINDOW     12345
#define HUD_STATE_WAIT_WINDOW       123456
#define CLIENT_FLASH_SPAN           7000

typedef short TR_client_ptr;

#define NULL_CLIENT             0
#define HUMAN_CLIENT            (1<<1)
#define REPLAY_CLIENT           (1<<2)

struct TR_client_t {
  int                  type;
  TR_cmd_store_ptr    cs;
  TR_id_t             player_id,
                       cmd_id;
  TR_client_ptr       fnext,
                       previous,
                       next;
  /* human */ 
  ENetHost             *host;
  ENetPeer             *peer;
  ENetAddress          address,
                       seeding_address;
  bool                 connected,
                       offline,
                       generate_eot;
  TR_ptr_t            user_data;
  unsigned char        *replay_data;
  int                  replay_size,
                       replay_data_offset,
                       leech_replay_next_turn;
};

void            CL_Init                       (TR_game_instance_t *gi, int argc, char** argv);
TR_client_ptr  CL_New                        (TR_game_instance_t *gi, TR_cmd_store_ptr cs, int type, int player_id);
void            CL_Free                       (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_FreeAll                    (TR_game_instance_t *gi);
void            CL_Connect                    (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_WaitForConnection          (TR_game_instance_t *gi, TR_client_ptr c);
bool            CL_UpdateConnection           (TR_game_instance_t *gi, TR_client_ptr c);
bool            CL_Update                     (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_Seed                       (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_EndOfTurn                  (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_UpdateInputs               (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_ClearCmdStore              (TR_game_instance_t *gi, TR_client_ptr c);
int             CL_GetRSquared                (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_CmdUnitOrderOnSelection    (TR_game_instance_t *gi, TR_client_ptr c, int to_x, int to_y, TR_unit_ptr target, bool push_back, bool skill_here=false, TR_skill_type_t skill_type=(TR_skill_type_t)-1, bool ground_only=false, TR_id_t cmd_mask=-1, int r_squared=-1, TR_ptr_t user_data=TR_null, TR_unit_order_cb_t unit_order_completed_cb=null, TR_unit_order_cb_t unit_order_failed_cb=null);
void            CL_CmdUnitOrder               (TR_game_instance_t *gi, TR_client_ptr c, TR_unit_ptr u, int to_x, int to_y, TR_unit_ptr target, bool push_back, bool skill_here=false, TR_skill_type_t skill_type=(TR_skill_type_t)-1, bool ground_only=false, TR_id_t cmd_mask=-1, int r_squared=0, TR_ptr_t user_data=TR_null, TR_unit_order_cb_t unit_order_completed_cb=null, TR_unit_order_cb_t unit_order_failed_cb=null, bool play_sound=false);
void            CL_CmdUnitStop                (TR_game_instance_t *gi, TR_client_ptr c, TR_unit_ptr u);
void            CL_CmdSpawnUnit               (TR_game_instance_t *gi, TR_client_ptr c, TR_id_t from_u, int to_x, int to_y, TR_unit_type_t unit_type, TR_user_data_type_t user_data_type=TR_null, TR_ptr_t user_data=TR_null, bool find_pos=true);
void            CL_CmdBuildOrder              (TR_game_instance_t *gi, TR_client_ptr c, TR_id_t builder_id, TR_id_t builder_player_id, TR_id_t unit_player_id, TR_unit_type_t unit_type, TR_user_data_type_t user_data_type, int target_cx, int target_cy, bool start_it);
void            CL_CmdCancelBuildOrder        (TR_game_instance_t *gi, TR_client_ptr c, TR_id_t builder_id, TR_id_t builder_player_id, TR_id_t unit_player_id, TR_id_t unit_id);
void            CL_CmdGeneric                 (TR_game_instance_t *gi, TR_client_ptr c, TR_id_t cmd_type, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy);
bool            CL_HasOrders                  (TR_game_instance_t *gi, TR_client_ptr c, TR_unit_ptr u, bool skill_here, TR_skill_type_t skill_type);
void            CL_LoadReplay                 (TR_game_instance_t *gi, TR_client_ptr c);
void            CL_LeechReplay                (TR_game_instance_t *gi, TR_client_ptr c);

/*
 * player
 */

#define player(ptr)                         (gi->players[(ptr)])
#define MAX_WORKER_TASK                     (MAX_UNIT/MAX_PLAYER)
#define MAX_WORKER                          4
#define path_point_worker_ptr(index)        (ta->path + ((index)<<1))     
#define worker(W)                           (gi->workers[p][(W)])   
#define ON_HEAVY_MAP(X, Y)                  (wo->heavy_map[MAP_SIZE_X*(Y)+(X)])
#define ON_OBSTACLE_MAP(X, Y)               (wo->obstacle_map[MAP_SIZE_X*(Y)+(X)])
typedef short TR_player_ptr;
typedef short TR_worker_ptr;

struct TR_worker_task_t {
  TR_unit_ptr        u;
  TR_id_t            id,
                      *task_id_proxy;
  TR_unit_type_t     type;
  int                 pos_x,
                      pos_y,
                      target_cx,
                      target_cy,
                      click_cx,
                      click_cy,
                      size,
                      target_size,
                      chase_cx,
                      chase_cy,
                      path_len,
                      *chase_cx_proxy,
                      *chase_cy_proxy,
                      *path_len_proxy;
  bool                *started_proxy,
                      *found_path_proxy,
                      found_path,
                      skill_here,
                      has_target,
                      look_for_unit,
                      selected,
                      flying,
                      look_for_ground,
                      look_for_air;
  TR_id_t            team_id;
  TR_get_unit_here_mode_t unit_here_mode;
  int                 range;
  int                 path[MAX_PATH_LEN * 2];
};

struct TR_path_callback_t {
  TR_game_instance_t *gi;
  TR_player_ptr      p;
  TR_worker_ptr      w;
  bool                use_heavy_map;
  short               size;
  int                 start_mark,
                      mark;
  bool                follow_mode,
                      can_follow,
                      do_first_pass;
};

struct TR_worker_t {
  TR_game_instance_t *gi;
  TR_player_ptr      p;
  TR_worker_ptr      w;
  HANDLE              handle;
  DWORD               id;
  int                 task_count,
                      task_cost,
                      close_mark;
  TR_path_callback_t callback_data;
  int                 close_map[MAP_SIZE],
                      f_map[MAP_SIZE];
  int                 path_x[MAX_PATH_LEN],
                      path_y[MAX_PATH_LEN],
                      path_i[MAX_PATH_LEN],
                      path_j[MAX_PATH_LEN],
                      path_len;
  TR_astar_node_t    pool[MAX_ASTAR_NODE];
};

struct TR_player_t {
  map<TR_id_t, TR_unit_ptr> cmd_to_mem_unit_map;
  TR_unit_class_t            unit_dic[MAX_UNIT_TYPE];
  TR_id_t                    id,
                              team_id,
                              update_frame_mark;
  TR_cmd_store_ptr           cs;
  TCOD_map_t                  tcod_map;
  TR_unit_ptr                unit_head;
  TR_player_ptr              fnext,
                              previous,
                              next;
  int                         worker_count;
  TR_worker_ptr              free_workers[MAX_WORKER];
  int                         free_worker_count;
  int                         current_worker;
  bool                        win,
                              lose,
                              inert,
                              /* atm, used only for the frame_time_step + determinism 
                                 ie. bots are not taken into account for the frame_time_step 
                                 ie. because bots are not recorded for replays */
                              human;
  TR_ptr_t                   user_data;
};

void                PL_Init                       (TR_game_instance_t *gi);
TR_player_ptr      PL_New                        (TR_game_instance_t *gi, TR_ptr_t user_data=TR_null);
void                PL_Free                       (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_FreeAll                    (TR_game_instance_t *gi);
bool                PL_NextTurn                   (TR_game_instance_t *gi, TR_player_ptr p);
int                 PL_IsInFov                    (TR_game_instance_t *gi, TR_player_ptr p, int pos_x, int pos_y);
bool                PL_IsInFov                    (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
int                 PL_IsInMagicVision            (TR_game_instance_t *gi, TR_player_ptr p, int pos_x, int pos_y);
bool                PL_IsInMagicVision            (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
int                 PL_GetLight                   (TR_game_instance_t *gi, TR_player_ptr p, short pos_x, short pos_y);
void                PL_UpdateUnitsPart1           (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_UpdateUnitsPart2           (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_UpdateUnitsPart3           (TR_game_instance_t *gi, TR_player_ptr p);
bool                PL_UpdateFollowGoal           (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
bool                PL_StartMoveOrder             (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
bool                PL_MoveAndSkills              (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_ProcessCmdStore            (TR_game_instance_t *gi, TR_player_ptr p);
TR_unit_ptr        PL_SpawnUnit                  (TR_game_instance_t *gi, TR_player_ptr p, TR_id_t cmd_id, int x, int y, TR_unit_type_t unit_type, TR_user_data_type_t user_data_type=TR_null, TR_ptr_t user_data=TR_null, bool find_pos=true);
TR_cadaver_ptr      PL_KillUnit                   (TR_game_instance_t *gi, TR_player_ptr killer_p, TR_player_ptr unit_p, TR_unit_ptr u, bool with_blood);
void                PL_KillAll                    (TR_game_instance_t *gi, TR_player_ptr p);
bool                PL_IsUnitAlive                (TR_game_instance_t *gi, TR_player_ptr p, TR_id_t cmd_id);
bool                PL_MoveUnit                   (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
bool                PL_UnitSkill                  (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u);
bool                PL_GenFollowUnit              (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_id_t cmd_mask, TR_skill_type_t skill_type, bool following_friend, bool free_front=false);
bool                PL_GenFollowTarget            (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_ptr target, TR_id_t cmd_mask, TR_skill_type_t skill_type, bool following_friend, bool free_front=false);
bool                PL_GenForcedAction            (TR_game_instance_t *gi, TR_unit_ptr u, bool skill_here, TR_skill_type_t skill_type, TR_id_t cmd_mask, int x, int y, TR_unit_ptr target, TR_unit_ptr forced_action_unit);
TR_unit_ptr        PL_GetFirstUnitInRange        (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, int range, int reachable_range, bool look_for_enemy, bool look_for_ground, bool look_for_air);
void                PL_InitWorkers                (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_StartWorkers               (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_CloseWorkers               (TR_game_instance_t *gi, TR_player_ptr p);
void                PL_CloseWorker                (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w);
DWORD WINAPI        PL_WorkerFun                  (void *params);
bool                PL_ASTAR_DoesCollide          (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, int size, int x0, int y0, int x1, int y1, bool flying);
void                PL_ASTAR_InsertSortedNode     (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr node, TR_astar_node_ptr *h);
void                PL_ASTAR_InitNode             (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr node, TR_astar_node_ptr parent, int dest_x, int dest_y, int x, int y, int i, int j, bool r, bool fast);
TR_astar_node_ptr  PL_ASTAR_PopNode              (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr *h);
bool                PL_ASTAR_ExpandNode           (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr *free_head, TR_astar_node_ptr node, TR_astar_node_ptr *open_list, int size, int dest_x, int dest_y, int di, int dj, bool fast, bool flying);
int                 PL_ASTAR_FindCollideRPlace    (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, int i, int j, int size);
void                PL_ASTAR_MakePath             (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, TR_astar_node_ptr node, TR_worker_task_t *ta);
bool                PL_IsReachable                (TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, int range);
bool                PL_SnapPoint                  (TR_game_instance_t *gi, int size, int ox, int oy, int oi, int oj, int di, int dj, int *x, int *y, bool find_nearest_origin, bool flying, bool skill_here);
int                 PL_DistFromEdge               (TR_game_instance_t *gi, int size, int x, int y, TR_edge_t *edge);
void                PL_SnapToEdge                 (TR_game_instance_t *gi, int size, int *x, int *y, TR_edge_t *edge);
void                PL_UpdateMarchTex             (TR_game_instance_t *gi, TR_player_ptr p, TR_worker_ptr w, int x0, int y0, int x1, int y1, TR_astar_node_t pool[], TR_astar_node_ptr node, bool render_ground);
void                PL_SetSkillCooldown           (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_type_t unit_type, TR_skill_type_t skill_type);
int                 PL_GetMinCooldown             (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_type_t unit_type, TR_id_t skill_id);
void                PL_CooldownAll                   (TR_game_instance_t *gi, TR_player_ptr p, TR_unit_type_t unit_type, TR_id_t skill_id);

/*
 * game instance
 */

#define     MASTER_SERVER_PING_INTERVAL 125
#define     TIMEOUT_TIMEOUT_SPAN        2000
#define     LISTEN_MASTER_SERVER \
  ENetEvent e; \
  GI_ListenMasterServer(gi, &e);

struct game_desc_t {
  /* for the replays
     we don't want to manually supply the gameserver 
     with the number of teams and players
     that's the replay which knows that 
     that's why there is an offline mode  
     also we don't want replay to take resources
     on backend ideally */
  bool              online,
                    PvE,
                    continue_game;
  str               master_server;
  str               game_name,
                    player_name;
  int               player_count_per_team,
                    team_count,
                    offline_players;
  TR_id_t          local_player_id,
                    local_team_id; 
  str               replay;  
  unsigned int      seed;        
};

typedef void            (*TR_free_all_cb_t)(TR_game_instance_t *gi);
typedef void            (*TR_init_game_cb_t)(TR_game_instance_t *gi, game_desc_t *gd);
typedef void            (*TR_start_game_cb_t)(TR_game_instance_t *gi);
typedef void            (*TR_update_client_cb_t)(TR_game_instance_t *gi, TR_client_ptr c);
typedef void            (*TR_update_player_cb_t)(TR_game_instance_t *gi, TR_player_ptr p);
typedef str             (*TR_get_skill_name_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef str             (*TR_get_skill_anim_name_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_anim_time_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_max_level_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_every_level_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef TR_skill_type_t (*TR_default_skill_type_cb_t)(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target);
typedef char            (*TR_get_skill_shortcut_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef str             (*TR_get_skill_info_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_range_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_cooldown_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_mana_cost_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
typedef str             (*TR_get_skill_material_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_cast_speed_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_direct_cast_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef TR_unit_type_t  (*TR_get_skill_invoke_type_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef int             (*TR_get_skill_invoke_count_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_can_stop_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_continue_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_is_blocking_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);       
typedef bool            (*TR_get_skill_is_available_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_is_available_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_is_passive_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_get_skill_is_unique_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_skill_look_for_air_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_skill_look_for_ground_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_skill_cast_one_by_one_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef str             (*TR_can_cast_skill_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type, TR_unit_ptr target, short cx, short cy, bool dont_check_common_cond);
typedef bool            (*TR_skill_looks_for_unit_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_skill_looks_for_enemy_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef bool            (*TR_skill_looks_for_target_cb_t)(TR_game_instance_t *gi, TR_skill_type_t skill_type);
typedef TR_time_t       (*TR_skill_speed_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_skill_type_t skill_type);
typedef bool            (*TR_cast_skill_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_order_ptr o, int x, int y);
typedef bool            (*TR_hud_info_cb_t)(TR_game_instance_t *gi, TR_camera_ptr ca);
typedef bool            (*TR_on_death_cb_t)(TR_game_instance_t *, TR_player_ptr, TR_player_ptr, TR_unit_ptr);
typedef void            (*TR_on_update_unit_cb_t)(TR_game_instance_t *, TR_player_ptr, TR_unit_ptr);
typedef int             (*TR_on_hit_unit_cb_t)(TR_game_instance_t *, TR_ball_ptr b, TR_unit_ptr, int, int attack_penetration, bool show_hp);
typedef void            (*TR_on_update_unit_render_cb_t)(TR_game_instance_t *, TR_camera_ptr, TR_player_ptr, TR_unit_ptr);
typedef void            (*TR_on_spawn_unit_cb_t)(TR_game_instance_t *gi, TR_unit_ptr from_u, TR_unit_ptr u);
typedef void            (*TR_on_cancel_build_cb_t)(TR_game_instance_t *gi, TR_build_order_ptr b);
typedef void            (*TR_on_generic_cmd_cb_t)(TR_game_instance_t *gi, TR_id_t cmd_type, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy);
typedef void            (*TR_on_unit_order_cb_t)(TR_game_instance_t *gi, TR_unit_ptr u, TR_unit_ptr target, TR_id_t id, TR_id_t player_id, TR_id_t cmd_mask, TR_unit_type_t unit_type, int target_cx, int target_cy, bool skill_here, TR_skill_type_t skill_type);
typedef bool            (*TR_draw_unit_goal_cb_t)(TR_game_instance_t *gi, TR_camera_ptr ca, TR_unit_ptr u);
typedef str             (*TR_get_cursor_cb_t)(TR_game_instance_t *gi, TR_camera_ptr ca, TR_skill_type_t skill_type, int x, int y);
typedef void            (*TR_on_select_unit_sound_cb_t)(TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y);
typedef void            (*TR_on_move_unit_sound_cb_t)(TR_game_instance_t *gi, TR_unit_type_t unit_type, int pos_x, int pos_y);
typedef void            (*TR_on_skill_sound_cb_t)(TR_game_instance_t *gi, TR_id_t skill_type, int pos_x, int pos_y, bool can_cast);
typedef TR_unit_ptr     (*TR_get_unit_in_range_cb_t)(TR_game_instance_t *gi, TR_player_ptr p, TR_unit_ptr u, TR_unit_ptr units[MAX_UNIT], int unit_count, int skill_range);
typedef int             (*TR_get_selection_skills_cb_t)(TR_game_instance_t *gi, TR_camera_ptr ca, int skill_ids[MAX_SKILLS_PER_CAMERA], TR_skill_type_t skill_types[MAX_SKILLS_PER_CAMERA], TR_unit_ptr skill_units[MAX_SKILLS_PER_CAMERA]);

enum TR_peer_message_type_t {
  TR_peer_message_cmd_pack = 1
};

enum TR_state_t {
  TR_state_game                  = 1<<2,
  TR_state_join                  = 1<<3,
  TR_state_waiting_server        = 1<<4,
  TR_state_waiting_players       = 1<<5,
};

extern TR_game_instance_t *global_gi;

#define MAX_KEY_PRESSED 1024

struct TR_ogre_class : public OIS::KeyListener, OIS::MouseListener, OgreBites::SdkTrayListener {
  bool keyPressed(const OIS::KeyEvent &e);
  bool keyReleased(const OIS::KeyEvent &keyEventRef) {
  }
  bool mouseMoved(const OIS::MouseEvent &evt) {
  }
  bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id) {
  }
  bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id) {
  }
};

// todo: split this for cache locallity 
struct TR_game_instance_t {
  str                             desynced_str;
  TCOD_map_t                      wall_map;
  gau_Manager                     *gau_manager;
  ga_Mixer                        *gau_mixer;
  ga_StreamManager                *gau_stream_manager;
  ENetAddress                     master_server_addr;
  str                             name;
  short                           env_map[RANGE_MAP_SIZE],
                                  clearance_map2[MAP_SIZE],
                                  env_art_map[MAP_SIZE];
  int                             clearance_map_base[MAP_SIZE],
                                  clearance_map_base2[MAP_SIZE],
                                  clearance_map[MAP_SIZE],
                                  flying_clearance_map[MAP_SIZE],
                                  clearance_patch[MAP_SIZE],
                                  edge_map[MAX_UNIT_SIZE][MAP_SIZE][2],
                                  flying_edge_map[MAX_UNIT_SIZE][MAP_SIZE][2],
                                  edge_patch[MAP_SIZE][2];
  short                           march_patch[MAP_SIZE],
                                  march_map[MAX_UNIT_SIZE][MAP_SIZE],
                                  flying_march_map[MAX_UNIT_SIZE][MAP_SIZE];
  TR_edge_t                      edges[MAX_UNIT_SIZE][MAX_EDGE],
                                  flying_edges[MAX_UNIT_SIZE][MAX_EDGE],
                                  patch_edges[MAX_PATCH_EDGE];
  int                             edge_count[MAX_UNIT_SIZE],
                                  flying_edge_count[MAX_UNIT_SIZE],
                                  patch_edge_count;
  int                             march_tex[MAP_SIZE*8*8];
  int                             worker_units_x[MAX_UNIT],
                                  worker_units_y[MAX_UNIT],
                                  worker_units_size[MAX_UNIT],
                                  worker_unit_count;
  bool                            worker_units_flying[MAX_UNIT];
  float                           dir_light[3];
  TR_unit_node_ptr               unit_map[MAP_SIZE];
  int                             fov_maps[MAX_TEAM][RANGE_MAP_SIZE],
                                  working_fov_maps[MAX_TEAM][RANGE_MAP_SIZE],
                                  magic_vision_maps[MAX_TEAM][RANGE_MAP_SIZE],
                                  working_magic_vision_maps[MAX_TEAM][RANGE_MAP_SIZE];
  bool                            heavy_map[MAP_SIZE],
                                  obstacle_map[MAP_SIZE];
  TCOD_path_t                     path_pool[MAX_PLAYER][MAX_WORKER],
                                  path_pool2[MAX_PLAYER][MAX_WORKER];
  int                             paths[MAX_UNIT * MAX_PATH_LEN * 2];
  TR_id_t                        cmd_id;
  TR_time_t                      game_time,
                                  frame_time,
                                  last_t,
                                  game_time_step,
                                  frame_time_step,
                                  game_time_acc,
                                  wait_time,
                                  aver_frame_times[MAX_AVER_FRAME_TIMES],
                                  aver_wait_times[MAX_AVER_WAIT_TIMES],
                                  aver_frame_time,
                                  aver_wait_time,
                                  ping_acc,
                                  turn_frame_count_dec_acc;
  int                             aver_frame_times_counter,
                                  aver_wait_times_counter,
                                  max_ping_times_counter;
  TCOD_color_t                    static_light_map[MAP_SIZE],
                                  dyn_light_map[MAP_SIZE];
  bool                            shadow_map[MAP_SIZE];
  TR_unit_class_t                unit_dic[MAX_UNIT_TYPE];
  TR_unit_order_t                orders[MAX_UNIT_ORDER];
  TR_unit_order_ptr              free_unit_order_head;
  TR_particle_system_t           particle_systems[MAX_PARTICLE_SYSTEM];
  TR_particle_system_ptr         free_particle_system_head;
  TR_particle_system_ptr         particle_system_head;
  TR_ball_t                      balls[MAX_BALL];
  TR_ball_ptr                    free_ball_head;
  TR_ball_ptr                    ball_head;
  TR_tree_t                      trees[MAX_TREE];
  TR_tree_ptr                    free_tree_head;
  TR_tree_ptr                    tree_head;
  TR_burn_trace_t                burn_traces[MAX_BURN_TRACE];
  TR_burn_trace_ptr              free_burn_trace_head;
  TR_burn_trace_ptr              burn_trace_head;
  TR_floating_text_t             floating_texts[MAX_FLOATING_TEXT];
  TR_floating_text_ptr           free_floating_text_head;
  TR_floating_text_ptr           floating_text_head;
  TR_light_t                     lights[MAX_LIGHT];
  TR_light_ptr                   free_light_head;
  TR_light_ptr                   light_head;
  TCOD_noise_t                   torch_noise,
                                 bump_noise;
  TCOD_map_t                     tcod_light_map;
  TR_sound_t                     sounds[MAX_SOUND];
  TR_sound_ptr                   free_sound_head;
  TR_sound_ptr                   sound_head;
  TR_fluid_t                     fluids[MAX_FLUID];
  TR_fluid_ptr                   free_fluid_head;
  TR_fluid_ptr                   fluid_head;
  TR_explosion_t                 explosions[MAX_EXPLOSION];
  TR_explosion_ptr               free_explosion_head;
  TR_explosion_ptr               explosion_head;
  TR_build_explosion_t           build_explosions[MAX_BUILD_EXPLOSION];
  TR_build_explosion_ptr         free_build_explosion_head;
  TR_build_explosion_ptr         build_explosion_head;
  TR_cadaver_t                   cadavers[MAX_CADAVER];
  TR_cadaver_ptr                 free_cadaver_head;
  TR_cadaver_ptr                 cadaver_head;
  TR_lightning_t                 lightnings[MAX_LIGHTNING];
  TR_lightning_ptr               free_lightning_head;
  TR_lightning_ptr               lightning_head;
  TR_status_effect_t             status_effects[MAX_STATUS_EFFECT];
  TR_status_effect_ptr           free_status_effect_head;
  TR_unit_node_t                 unit_nodes[MAX_UNIT_NODE];
  TR_unit_node_ptr               free_unit_node_head;
  TR_unit_node_ptr               unit_node_head;
  TR_animation_t                 animations[MAX_ANIMATION];
  TR_animation_ptr               free_animation_head;
  TR_unit_t                      units[MAX_UNIT];
  TR_unit_ptr                    free_unit_head;
  TR_unit_ptr                    unit_head;
  TR_cmd_store_t                 cmd_stores[MAX_PLAYER];
  TR_cmd_store_ptr               free_cmd_store_head;
  TR_cmd_store_ptr               cmd_store_head;
  TR_remote_cmd_store_t          remote_cmd_stores[MAX_PLAYER];
  TR_remote_cmd_store_ptr        free_remote_cmd_store_head;
  TR_remote_cmd_store_ptr        remote_cmd_store_head;
  TR_player_t                    players[MAX_PLAYER];
  TR_worker_t                    workers[MAX_PLAYER][MAX_WORKER];
  TR_worker_task_t               worker_tasks[MAX_PLAYER][MAX_WORKER][MAX_WORKER_TASK];
  TR_player_ptr                  free_player_head;
  TR_player_ptr                  player_head;
  TR_cmd_t                       cmds[MAX_CMD];
  TR_cmd_ptr                     free_cmd_head;
  TR_cmd_ptr                     cmd_head;
  TR_client_t                    clients[MAX_CLIENT];
  TR_client_ptr                  free_client_head;
  TR_client_ptr                  client_head;
  TR_camera_t                    cameras[MAX_CAMERA];
  TR_camera_ptr                  free_camera_head;
  TR_camera_ptr                  camera_head;
  TR_build_order_t               build_orders[MAX_BUILD_ORDER];
  TR_build_order_ptr             free_build_order_head;
  int                             turn, 
                                  ping,
                                  max_ping,
                                  pings[MAX_MAX_PING_TIMES];
  int                             game_frame_mark,
                                  close_map[MAX_UNIT_SIZE][MAP_SIZE],
                                  flying_close_map[MAX_UNIT_SIZE][MAP_SIZE],
                                  close_map3[MAX_PLAYER][MAX_WORKER][MAX_UNIT_SIZE][MAP_SIZE],
                                  task_mark,
                                  close_map_mark,
                                  close_map_mark3[MAX_PLAYER][MAX_WORKER],
                                  colision_mark,
                                  penetration_mark,
                                  square_x[8],
                                  square_y[8];                            
  short                           turn_frame_count,
                                  turn_acc;
  TCOD_mouse_t                    mouse;
  bool                            lbtn_down,
                                  lbtn_down_cache,
                                  lbtn_up,
                                  rbtn_down,
                                  rbtn_down_cache,
                                  rbtn_up;
  unsigned int                    working_seed,
                                  state,
                                  add_state[SMALL_SIZE],
                                  add_state_count,
                                  remove_state[SMALL_SIZE],
                                  remove_state_count;
  ENetHost                        *host;
  ENetPeer                        *peer;
  ENetAddress                     address;
  bool                            connected,
                                  players_connected,
                                  minimap_cleared,
                                  desync,
                                  has_processed_cmds;
  str                             send_message;
  bool                            send_to_all;
  str                             message_queue[MESSAGE_QUEUE_COUNT],
                                  username_queue[MESSAGE_QUEUE_COUNT];
  bool                            sent_to_all_queue[MESSAGE_QUEUE_COUNT];
  TR_time_t                      display_message_counter;
  bool                            is_timingout[MAX_PLAYER];
  TR_time_t                      timeouts[MAX_PLAYER],
                                  timeout_timeouts[MAX_PLAYER];
  str                             timeout_player_names[MAX_PLAYER]; 
  TR_time_t                      connection_acc;
  HANDLE                          ping_thread_handle;
  ENetHost                        *ping_thread_host;
  ENetPeer                        *ping_thread_peer;
  ENetAddress                     ping_thread_address,
                                  ping_thread_master_server_address;
  #if !RELEASE    
  bool                            show_all,
                                  show_lagometer;
  #endif    
  /* callbacks */
  TR_free_all_cb_t                     free_all_cb;        
  TR_update_client_cb_t                update_client_cb;
  TR_update_player_cb_t                update_player_cb;
  TR_on_death_cb_t                     on_death_cb;  
  TR_on_update_unit_cb_t               on_update_unit_cb;    
  TR_get_skill_name_cb_t               get_skill_name_cb;
  TR_get_skill_anim_name_cb_t          get_skill_anim_name_cb;
  TR_get_skill_anim_time_cb_t          get_skill_anim_time_cb;
  TR_get_skill_max_level_cb_t          get_skill_max_level_cb;
  TR_get_skill_every_level_cb_t        get_skill_every_level_cb;
  TR_get_skill_shortcut_cb_t           get_skill_shortcut_cb;
  TR_get_skill_info_cb_t               get_skill_info_cb;
  TR_get_skill_range_cb_t              get_skill_range_cb;
  TR_get_skill_cooldown_cb_t           get_skill_cooldown_cb;
  TR_get_skill_mana_cost_cb_t          get_skill_mana_cost_cb;
  TR_get_skill_material_cb_t           get_skill_material_cb;
  TR_get_skill_cast_speed_cb_t         get_skill_cast_speed_cb;
  TR_get_skill_direct_cast_cb_t        get_skill_direct_cast_cb;
  TR_get_skill_invoke_type_cb_t        get_skill_invoke_type_cb;
  TR_get_skill_invoke_count_cb_t       get_skill_invoke_count_cb;
  TR_get_skill_can_stop_cb_t           get_skill_can_stop_cb;
  TR_get_skill_continue_cb_t           get_skill_continue_cb;
  TR_get_skill_is_blocking_cb_t        get_skill_is_blocking_cb;
  TR_get_skill_is_available_cb_t       get_skill_is_available_cb;
  TR_get_skill_is_passive_cb_t         get_skill_is_passive_cb;
  TR_get_skill_is_unique_cb_t          get_skill_is_unique_cb;
  TR_skill_look_for_air_cb_t           skill_look_for_air_cb;
  TR_skill_look_for_ground_cb_t        skill_look_for_ground_cb;
  TR_skill_cast_one_by_one_cb_t        skill_cast_one_by_one_cb;
  TR_can_cast_skill_cb_t               can_cast_skill_cb;
  TR_skill_looks_for_unit_cb_t         skill_looks_for_unit_cb;
  TR_skill_looks_for_enemy_cb_t        skill_looks_for_enemy_cb;
  TR_skill_looks_for_target_cb_t       skill_looks_for_target_cb;
  TR_default_skill_type_cb_t           default_skill_type_cb;
  TR_cast_skill_cb_t                   cast_skill_cb;
  TR_hud_info_cb_t                     hud_info_cb;
  TR_on_hit_unit_cb_t                  on_hit_unit_cb;
  TR_on_update_unit_render_cb_t        on_update_unit_render_cb;
  TR_on_spawn_unit_cb_t                on_spawn_unit_cb;
  TR_on_cancel_build_cb_t              on_cancel_build_cb;
  TR_on_generic_cmd_cb_t               on_generic_cmd_cb;
  TR_on_unit_order_cb_t                on_unit_order_cb;
  TR_draw_unit_goal_cb_t               draw_unit_goal_cb;
  TR_get_cursor_cb_t                   get_cursor_cb;
  TR_on_select_unit_sound_cb_t         on_select_unit_sound_cb;
  TR_on_move_unit_sound_cb_t           on_move_unit_sound_cb;
  TR_on_skill_sound_cb_t               on_skill_sound_cb;
  TR_get_unit_in_range_cb_t            get_unit_in_range_cb;
  TR_get_selection_skills_cb_t         get_selection_skills_cb;
  /* ui */
  TR_btn_t                             refresh_btn,
                                        ready_btn,
                                        waiting_for_players_btn,
                                        flash_btn,
                                        close_btn;
  TR_btn_list_t                        multi_btns,
                                        join_game_btns,
                                        skills_btns,
                                        central_hud_btns;
  str                                   cursor_tex,
                                        last_cursor_tex;
  Ogre::Overlay                         *loading_overlay;
  Ogre::TextAreaOverlayElement          *loading_text_overlay;
  /* Ogre */
  Ogre::Root                            *ogre_root;
  Ogre::SceneManager                    *ogre_scene;
  Ogre::RenderWindow                    *ogre_window;
  OIS::InputManager                     *ogre_input;
  OIS::Keyboard                         *ogre_keyboard;
  OIS::KeyCode                          key_pressed[MAX_KEY_PRESSED];
  int                                   key_pressed_count;
  OIS::Mouse                            *ogre_mouse;
  TR_ogre_class                        ogre_class;
  OgreBites::SdkTrayManager             *ogre_tray;
  Ogre::FrameEvent                      ogre_frame_event;
  Ogre::DotSceneLoader                  ogre_scene_loader;
};

bool              GI_Init                           (TR_game_instance_t *gi, int argc, char** argv);
void              GI_Free                           (TR_game_instance_t *gi);
void              GI_Close                          (TR_game_instance_t *gi);
void              GI_InitEnvMap                     (TR_game_instance_t *gi);
void              GI_InitLightMap                   (TR_game_instance_t *gi);
void              GI_InitOgreResources              (TR_game_instance_t *gi);
void              GI_InitOIS                        (TR_game_instance_t *gi);
void              GI_InitGame                       (TR_game_instance_t *gi, game_desc_t *gd, TR_init_game_cb_t init_game_cb);
void              GI_StartGame                      (TR_game_instance_t *gi, TR_start_game_cb_t start_game_cb);
void              GI_StartCamera                    (TR_game_instance_t *gi, TR_camera_ptr ca);
void              GI_UpdateTime                     (TR_game_instance_t *gi);
void              GI_UpdateTurn                     (TR_game_instance_t *gi);
void              GI_Update                         (TR_game_instance_t *gi);
void              GI_UpdateGame                  (TR_game_instance_t *gi);
void              GI_UpdateMiscs                    (TR_game_instance_t *gi);
void              GI_UpdateWorkerUnits              (TR_game_instance_t *gi);
void              GI_UpdateMarchingSquares          (TR_game_instance_t *gi);
void              GI_PathfinderScanMap              (TR_game_instance_t *gi, int size, int pos_x, int pos_y, int *close_mark, int close_map[MAP_SIZE], short march_map[MAP_SIZE]);
void              GI_UpdateFOVMaps                  (TR_game_instance_t *gi);
void              GI_SnapUnits                      (TR_game_instance_t *gi);
void              GI_DrawUnitClearanceContribution  (TR_game_instance_t *gi, TR_unit_ptr u, int clearance_map[MAP_SIZE]);
void              GI_DrawTreesClearanceContribution (TR_game_instance_t *gi, int x0, int y0, int x1, int y1);
void              GI_BuildPatchEdges                (TR_game_instance_t *gi, TR_unit_ptr u);
int               GI_DistFromEdge                   (TR_game_instance_t *gi, TR_edge_t *edge, int x, int y);
void              GI_SnapToEdge                     (TR_game_instance_t *gi, TR_unit_ptr u, TR_edge_t *edge);
void              GI_SnapUnit                       (TR_game_instance_t *gi, TR_unit_ptr u);
void              GI_InitClearanceMapBase           (TR_game_instance_t *gi);
void              GI_DrawUnitClearance              (TR_game_instance_t *gi, int size, int x, int y, int *clearance_map);
void              GI_MarchingSquares                (TR_game_instance_t *gi, int clearance_map[MAP_SIZE], TR_edge_t *edges, int *counter, int edge_map[MAP_SIZE][2], short march_map[MAP_SIZE], int x0, int y0, int x1, int y1, int half_size);
void              GI_BuildReflex                    (TR_game_instance_t *gi, TR_edge_t *edges, int edge_map[MAP_SIZE][2], int edge_count);
int               GI_BuildEdge                      (TR_game_instance_t *gi, TR_edge_t *edges, int *counter, int *v, int *f, int *idx, int iso, int pos_x, int pos_y, int i, int j);
void              GI_UnitRepulsion                  (TR_game_instance_t *gi);
void              GI_FindUnitContacts               (TR_game_instance_t *gi, short colision_matrix[MAX_UNIT][MAX_UNIT], TR_contact_t pool[], TR_unit_ptr u, TR_contact_ptr *free_contacts, TR_contact_ptr *contacts);
void              GI_FindPenetrations               (TR_game_instance_t *gi, short colision_matrix[MAX_UNIT][MAX_UNIT], TR_unit_ptr u);
void              GI_InsertContact                  (TR_game_instance_t *gi, TR_contact_t pool[], TR_contact_ptr c, TR_contact_ptr *h);
void              GI_RemoveUnitContacts             (TR_game_instance_t *gi, TR_contact_t pool[], TR_contact_ptr *h, TR_unit_ptr ua, TR_unit_ptr ub);
bool              GI_HandlePotentialCollision       (TR_game_instance_t *gi, TR_contact_t pool[], TR_contact_ptr c, int dx, int dy);
void              GI_UpdateClients                  (TR_game_instance_t *gi);
void              GI_UpdateCameras                  (TR_game_instance_t *gi);
void              GI_UpdateRemoteStores             (TR_game_instance_t *gi);
void              GI_EndOfTurn                      (TR_game_instance_t *gi);
void              GI_UpdateAverFrameTime            (TR_game_instance_t *gi, TR_time_t frame_time);
void              GI_UpdateAverWaitTime             (TR_game_instance_t *gi, TR_time_t wait_time);
void              GI_UpdateMaxPing                  (TR_game_instance_t *gi);
void              GI_UpdateInputs                   (TR_game_instance_t *gi);
bool              GI_IsKeyDown                      (TR_game_instance_t *gi, OIS::KeyCode k);
bool              GI_IsKeyPressed                   (TR_game_instance_t *gi, OIS::KeyCode k);
void              GI_StopKeyPropagation             (TR_game_instance_t *gi, OIS::KeyCode k);
OIS::KeyCode      GI_CharToKeyCode                  (TR_game_instance_t *gi, char c);
void              GI_StopClicksPropagation          (TR_game_instance_t *gi);
bool              GI_InState                        (TR_game_instance_t *gi, TR_state_t st);
void              GI_AddState                       (TR_game_instance_t *gi, TR_state_t st);
void              GI_RemoveState                    (TR_game_instance_t *gi, TR_state_t st);
void              GI_UpdateState                    (TR_game_instance_t *gi);
void              GI_ConnectToMasterServer          (TR_game_instance_t *gi);
void              GI_DisconnectFromMasterServer     (TR_game_instance_t *gi);
void              GI_KillHost                       (TR_game_instance_t *gi);
bool              GI_ListenMasterServer             (TR_game_instance_t *gi, ENetEvent *e);
bool              GI_CheckForTimeout                (TR_game_instance_t *gi, ENetEvent *e, TR_state_t current_state);
bool              GI_CheckForGameClosed             (TR_game_instance_t *gi, ENetEvent *e, TR_state_t current_state);
bool              GI_CheckConnectionToMasterServer  (TR_game_instance_t *gi, ENetEvent *e, TR_state_t current_state);
bool              GI_JoinGameOnMasterServer         (TR_game_instance_t *gi);
bool              GI_WaitServer                     (TR_game_instance_t *gi, ENetEvent *e);
bool              GI_WaitPlayers                    (TR_game_instance_t *gi, ENetEvent *e);
bool              GI_Ping                           (TR_game_instance_t *gi, ENetEvent *e);
bool              GI_Message                        (TR_game_instance_t *gi, ENetEvent *e);
void              GI_InitPingThread                 (TR_game_instance_t *gi);
void              GI_ClosePingThread                (TR_game_instance_t *gi);
DWORD WINAPI      GI_PingThread                     (void *params);
void              GI_SuccessFlash                   (TR_game_instance_t *gi, const str &msg);
void              GI_FailureFlash                   (TR_game_instance_t *gi, const str &msg);
int               GI_GetPlayerCount                 (TR_game_instance_t *gi);
TR_player_ptr    GI_GetPlayerPtr                   (TR_game_instance_t *gi, TR_id_t player_id);
TR_client_ptr    GI_GetClientPtr                   (TR_game_instance_t *gi, TR_id_t player_id);
TR_camera_ptr    GI_GetCamera                      (TR_game_instance_t *gi);
void              GI_CheckListIntegrity             (TR_game_instance_t *gi);
short             GI_FindClearance                  (TR_game_instance_t *gi, int cx, int cy);
void              GI_SaveReplay                     (TR_game_instance_t *gi);
void              GI_LoadReplayDesc                 (TR_game_instance_t *gi);
void              GI_DumpDesyncedTrace              (TR_game_instance_t *gi);

extern int                          argc;
extern char                         **argv;
extern bool                         run,
                                    ping_thread_run;
extern int                          resx, resy;
extern str                          bin_dir;

extern TR_game_instance_t          client_gi;

extern LARGE_INTEGER                pfrequency;

extern TCOD_console_t               con;
extern unsigned char                zlib_in[ZLIB_CHUNK],
                                    zlib_out[ZLIB_CHUNK];

extern int                          CON_RES_X,
                                    CON_RES_Y,
                                    LAGOMETER_START_X,
                                    offx,
                                    offy;
#if THREADS
extern CRITICAL_SECTION             log_crit,
                                    debug_crit,
                                    ping_thread_crit,
                                    ping_thread_disconnect_crit;
#endif          
extern short                        halfes[MAX_UNIT_SIZE+1];

extern TCOD_color_t                 player_colors[MAX_PLAYER];
extern short                        color_shift;

extern int                          MASTER_SERVER_PORT;
extern int                          return_error,
                                    ping_thread_return_error;

extern bool                         do_debug;

extern int                          march_allow_x[16][8],
                                    march_allow_y[16][8],
                                    march_allow_point_count[16];

extern int                          attack_penetrations[TR_attack_type_count][TR_armor_type_count];

/*
 * MOBA
 */

#include "gameplay.h"

/*
 *  parsers
 */

#include "parsers.h"

/*
 * aw 
 */

int         TR_exp(int l, int x, int p);
int         TR_exp_100(int l, int x, int p);
void        TR_Quit(int error);
void        TR_SetBinDir();
TR_time_t   TR_GetTime(); 
int         TR_Rand(unsigned int *seedp);
int         TR_ISqrt(int x);
void        TR_ZErr(int ret);

#define get16bits(d) (*((unsigned short *) (d)))
unsigned int TR_Hash(unsigned char * data, int len);

void        TR_PrintString(const str &s, int x, int y);
void        TR_PrintString(int i, int x, int y);
void        TR_PrintString(float f, int x, int y);
void        TR_PrintDebug(const str &s);
void        TR_PrintDebug(int i);
void        TR_PrintDebug(float f);
      
str         f2a(double f);
str         i2a(int i);
str         u2a(unsigned int u);
str         get_date();
void        log_in_file(const char *msg);
void        exit();
bool        dbug(const str &msg, bool dummy=true);
bool        dbug(int i, bool dummy=true);
bool        dbug(float f, bool dummy=true);

#define     log dbug

#endif  /* TR_H */

