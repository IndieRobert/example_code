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

if(GI_InState(gi, TR_state_game)) {
  /* time. */
  GI_UpdateTime(gi);
  /* the players before the cmds/inputs */
  GI_UpdateGame(gi);
  /* cameras. */
  GI_UpdateCameras(gi);
  /* clients */
  GI_UpdateClients(gi);
  /* remote stores. */
  GI_UpdateRemoteStores(gi);
  /* update turn */
  GI_UpdateTurn(gi);
} else {
  /* join */
  if(GI_InState(gi, TR_state_join)) {
    if(GI_IsKeyPressed(gi, OIS::KC_ESCAPE)) {
      TR_Quit(0);
    } else {
      LISTEN_MASTER_SERVER
      GI_CheckConnectionToMasterServer(gi, &e, TR_state_join);
      if(GI_JoinGameOnMasterServer(gi)) {
        GI_RemoveState(gi, TR_state_join);
        GI_AddState(gi, TR_state_waiting_server);
      }
    }
  }
  /* waiting for players */
  if(GI_InState(gi, TR_state_waiting_server)) {
    if(GI_IsKeyPressed(gi, OIS::KC_ESCAPE)) {
      TR_Quit(0);
    } else {
      if(GI_WaitServer(gi, &e)) {
        GI_InitGame(gi, &game_desc, GAME_InitGame);
        GI_RemoveState(gi, TR_state_waiting_server);
        GI_AddState(gi, TR_state_waiting_players);
      }
    }
  }
  /* ready */
  if(GI_InState(gi, TR_state_waiting_players)) {
    if(GI_IsKeyPressed(gi, OIS::KC_ESCAPE)) {
      TR_Quit(0);
    } else {
      if(GI_WaitPlayers(gi, &e)) {
        GI_StartGame(gi, GAME_StartGame);
        GI_RemoveState(gi, TR_state_waiting_players);
        GI_AddState(gi, TR_state_game);
      }
    }
  }
}