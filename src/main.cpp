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

#include "tr.h"

#include "map_generator.cpp"
#include "tr.cpp"
#include "gameplay.cpp"
#include "parsers.cpp"
#include "DotSceneLoader.cpp"
#include "bbox.cpp"

/*
 * 
 */
int main(int _argc, char **_argv) {
	run = true;
  argc = _argc;
  argv = _argv;
  #if THREADS  
  InitializeCriticalSectionAndSpinCount(&log_crit, 4000);
  InitializeCriticalSectionAndSpinCount(&debug_crit, 4000);
  InitializeCriticalSectionAndSpinCount(&ping_thread_crit, 4000);
  InitializeCriticalSectionAndSpinCount(&ping_thread_disconnect_crit, 4000);
  #endif  
  TR_SetBinDir();
  fclose(fopen((bin_dir + "log.txt").c_str(), "w+"));
  log((str("Binary directory: ") + bin_dir + "\n").c_str());
  atexit(exit);
  log("TownRush has started.");

  //log("Parsing configs...");
	//TCOD_parser_t game_parser = TCOD_parser_new();
	//TCOD_parser_new_struct(game_parser, (char*)"game");
	//TCOD_parser_run(game_parser,"game.cfg", &game_parser_listener);
	//log("Configs parsing done.");

  client_gi.name = "TR";
  log("Arg count: " + i2a(_argc));
  if(_argc == 9) {
  	/* NORMAL CASE */
    game_desc.PvE                   = str((char*)argv[1]) == "PvE";
    game_desc.master_server 		= str((char*)argv[2]);
    MASTER_SERVER_PORT              = atoi((char*)argv[3]);
    game_desc.player_name           = (char*)argv[4];
    game_desc.local_team_id         = atoi((char*)argv[5]);
    game_desc.team_count            = atoi((char*)argv[6]);
    game_desc.player_count_per_team = atoi((char*)argv[7]);
    game_desc.offline_players 		  = atoi((char*)argv[8]);
    game_desc.online                = !game_desc.PvE;
    game_desc.continue_game         = false;
  } else
  if(_argc == 3) {
  	/* REPLAY */
    game_desc.online                = false;
    game_desc.continue_game         = false;
    assert(str((char*)argv[1]) == "replay");
    game_desc.replay = argv[2];
    game_desc.player_name = "Robert";
    game_desc.local_team_id = 0;
  } else {
  	/* LAUNCH BY HAND */
  	game_desc.master_server = "localhost";
    MASTER_SERVER_PORT 				= 40000;
    game_desc.local_team_id         = atoi((char*)argv[1]);
    game_desc.player_name           = game_desc.local_team_id == 0 ? "Bob" : "Robert";
    game_desc.team_count            = 2;
    game_desc.player_count_per_team = 2;
    game_desc.PvE                   = true;
    game_desc.online                = true;
    game_desc.continue_game         = false;
  }
  log("Master server at: " + game_desc.master_server);
  log(str("Master Server on port ") + i2a(MASTER_SERVER_PORT));
  if(game_desc.online)
    log(str("Game is online"));
  else {
  	game_desc.local_player_id = 0;
    game_desc.seed = TR_GetTime();
    log(str("Game is offline"));
  }
  if(game_desc.replay != "")
    GI_LoadReplayDesc(&client_gi);
  if(game_desc.PvE)
    log(str("It's PvE"));
  else
    log(str("It's PvP"));
  log("Starting game with " + i2a(game_desc.team_count) + " teams and " + i2a(game_desc.player_count_per_team) + " player per team and " + i2a(game_desc.offline_players) + " offline players.");
  log("Player name is " + game_desc.player_name + ".");
  log("Local team id is " + i2a(game_desc.local_team_id) + ".");

  // client game instance
  if(GI_Init(&client_gi, argc, argv)) {
    GI_ConnectToMasterServer(&client_gi);
  	GI_AddState(&client_gi, TR_state_join);
  	while(run)
  		GI_Update(&client_gi);
  }
  log("Deleting game instance...");
  GI_Close(&client_gi);

  log(str("Execution time: " 
  	+ f2a(TCOD_sys_elapsed_seconds()/60) 
  	+ " minutes."));

	return return_error;
}

