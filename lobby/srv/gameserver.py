# coding: iso-8859-15
"""
* Conquest of Wilderness
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
"""

import enet
import struct
from threading import Thread
import sys
import logging
import time
import os

try:

  # --
  MASTER_SERVER_PORT          = int(sys.argv[1])
  GAME_NAME                   =     sys.argv[2]
  TEAM_COUNT                  = int(sys.argv[3])
  PLAYER_COUNT                = int(sys.argv[4])
  OFFLINE_TEAMS               = int(sys.argv[5])
  OFFLINE_PLAYERS             = int(sys.argv[6])

  PING_SERVER_PORT 						= MASTER_SERVER_PORT+1
  GAME_PORT                   = MASTER_SERVER_PORT+2
  GAME_PORT_COUNT             = 30
  MAX_TIMEOUT                 = 60
  MAX_GAME_TIMEOUT            = 90
  PING_TIMEOUT_THRESHOLD      = 5

  MAX_PLAYER                  = 10

  # --
  logging.basicConfig(level=logging.DEBUG,
                      format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                      datefmt='%m-%d %H:%M',
                      filename="./logs/log%s" % GAME_NAME.replace("-", "_").replace("\"", ""),
                      filemode='w+')
  console = logging.StreamHandler()
  console.setLevel(logging.INFO)
  formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
  console.setFormatter(formatter)
  logging.getLogger('').addHandler(console)

  # -- 
  logging.info("CoW gameserver starting...")
  logging.info(sys.argv)

  # --
  COW_cmd_type_none                 = 0
  COW_cmd_type_unit_order           = 1
  COW_cmd_type_spawn_unit           = 2
  COW_cmd_type_stop_unit            = 3
  COW_cmd_type_build_unit           = 4
  COW_cmd_type_generic              = 5
  COW_cmd_type_end_of_turn          = 6
  COW_cmd_type_join_game            = 7
  COW_cmd_type_seed_game            = 8
  COW_cmd_type_leech_game           = 9
  COW_cmd_type_ping                 = 10
  COW_cmd_type_message              = 11
  COW_cmd_type_disconnect           = 12
  COW_cmd_type_game_ready           = 13
  COW_cmd_type_port                 = 14
  COW_cmd_type_players_connected    = 15
  COW_cmd_type_game_closed          = 16
  COW_cmd_type_timeout_peer_player  = 17

  # --
  cmd_buffers   		 = [[] for p in xrange(MAX_PLAYER)]
  free_ports    		 = range(GAME_PORT, GAME_PORT+GAME_PORT_COUNT)
  master_host   		 = enet.Host(enet.Address(None, MASTER_SERVER_PORT), 256, 1, 0, 0)
  ping_server_host   = enet.Host(enet.Address(None, MASTER_SERVER_PORT+1), 256, 1, 0, 0)
  ping_server_players= {}
  game          		 = {'game_name':          GAME_NAME, 
                		    'player_count':       [0] * (TEAM_COUNT-OFFLINE_TEAMS), 
                		    'not_connected':      [PLAYER_COUNT-OFFLINE_PLAYERS] * (TEAM_COUNT-OFFLINE_TEAMS),
                		    'creation_time':      time.time(),
                		    'game_instances':     [],
                		    'leechers':           [],
                		    'seeders':            [],
                		    'started':            False}
  the_seed      		 = time.time()
  run           		 = True
  logging.info("Running game %s, with %s teams and %s players per team, offline teams %s, offline players %s, seed %s, waiting for connection on port %s..." % (GAME_NAME, TEAM_COUNT, PLAYER_COUNT, OFFLINE_TEAMS, OFFLINE_PLAYERS, the_seed, MASTER_SERVER_PORT))

  # --
  def game_ready(peer, player_id):
    peer.send(
      0, 
      enet.Packet(
        struct.pack("<I I I", COW_cmd_type_game_ready, player_id, the_seed), 
        enet.PACKET_FLAG_RELIABLE))

  # --
  def game_closed(peer):
    logging.info("Game closed. Move on.")
    peer.send(
      0, 
      enet.Packet(
        struct.pack("<I", COW_cmd_type_game_closed), 
        enet.PACKET_FLAG_RELIABLE))

  # --
  def players_connected():
    total_players = sum(game['player_count']) + sum(game['not_connected'])
    if((len(game['seeders']) == total_players and 
       len(game['leechers']) == total_players) or 
       (len(game['seeders']) == total_players and
       len(game['seeders']) == 1)):
      logger = logging.getLogger(GAME_NAME)
      full_players    = [j * PLAYER_COUNT + i 
                         for j in xrange(TEAM_COUNT-OFFLINE_TEAMS) 
                         for i in xrange(PLAYER_COUNT-OFFLINE_PLAYERS)]
      current_players = [gi['player_id'] for gi in game['game_instances']]
      missing_players = [i for i in full_players if i not in current_players]
      logging.info("Missing players %s" % missing_players)
      if len(current_players) == 0:
        logging.error("len(current_players) == 0")
      assert(len(current_players) != 0)
      for gi in game['game_instances']:
        gi['peer'].send(
          0, 
          enet.Packet(
            struct.pack(
              "<I I %s" % ("I " * len(missing_players)), 
              COW_cmd_type_players_connected,
              len(missing_players),
              *missing_players), 
            enet.PACKET_FLAG_RELIABLE))
        for missing_player in missing_players:
          #-- the player may have never connected
          missing_player_names = [gi['player_name'] for gi in game['game_instances'] if gi['player_id'] == missing_player]
          if len(missing_player_names) > 0:
            packet = enet.Packet(struct.pack("<I i 128s 128s", COW_cmd_type_message, -1, "", "%s has quit." % missing_player_names[0]), enet.PACKET_FLAG_RELIABLE)
            gi['peer'].send(0, packet)
      logging.info("The game has started.")
      game['started'] = True

  # -- 
  def delete_game():
    for i in xrange(len(game['game_instances'])):
      delete_game_instance(i)
    master_host.flush()
    while master_host.service(5000).type != enet.EVENT_TYPE_NONE:
      pass
    for host, _, _, _ in game['seeders']:
      logging.info("recycling port %s" % host.address.port)
      free_ports.append(host.address.port)
    # save replay
    replay = the_seed
    d = "replay/r%i/" % replay
    if not os.path.isdir("replay/"):
      os.mkdir("replay/")
    os.mkdir(d)
    fi = open("%sdesc" % d, "a+b")
    fi.write(struct.pack("< 3I", TEAM_COUNT, PLAYER_COUNT, the_seed))
    fi.close()
    for i, cmd_buffer in enumerate(cmd_buffers):
      fi = open("%sp%i" % (d, i), "a+b")
      for blob in cmd_buffer:
        fi.write(struct.pack("< I", blob[0]))
        fi.write(blob[1])
      fi.close()
    logging.info("%s game deleted" % GAME_NAME)
    sys.exit()

  # -- 
  def delete_game_instance(i):
    the_player_id = game['game_instances'][i]['player_id']
    logging.info("Player %d has disconnected." % (the_player_id))
    # -- delete leecher is any
    leecher_player_ids = [player_id for _, player_id in game['leechers']]
    if the_player_id in leecher_player_ids:
      logging.info("Leecher deletion.")
      del game['leechers'][leecher_player_ids.index(the_player_id)]
    # -- delete seeder if any
    seeder_player_ids = [player_id for _, _, _, player_id in game['seeders']]
    if the_player_id in seeder_player_ids:
      logging.info("Seeder deletion.")
      del game['seeders'][seeder_player_ids.index(the_player_id)]
    # -- send disconnection packets to other players
    for j, gi in enumerate(game['game_instances']):
      packet = enet.Packet(struct.pack("<I I", COW_cmd_type_disconnect, the_player_id), enet.PACKET_FLAG_RELIABLE)
      gi['peer'].send(0, packet)
      packet = enet.Packet(struct.pack("<I i 128s 128s", COW_cmd_type_message, -1, "", "%s has quit." % game['game_instances'][i]['player_name']), enet.PACKET_FLAG_RELIABLE)
      gi['peer'].send(0, packet)
    game['player_count'][game['game_instances'][i]['team_id']] -= 1
    remain = sum(game['player_count']) + sum(game['not_connected'])
    del game['game_instances'][i]
    if remain == 0:
      delete_game()
    elif not game['started']:
      players_connected()

  # -- 
  while run: 

    # -- timeouts
    if not game['started']:
	    if time.time() - game['creation_time'] > MAX_GAME_TIMEOUT:
	      game['not_connected'] = [0] * (TEAM_COUNT-OFFLINE_TEAMS)
	      logging.info("Connection to game has timeout, current player count is %s, seeders count %s, leecher count %s." % (sum(game['player_count']), len(game['seeders']), len(game['leechers'])))
	      if sum(game['player_count']) == 0:
	        delete_game()
	      else:
	      	players_connected()
	    time.sleep(0.5)
	    #logging.info("Game is timing out for %s." % (time.time() - game['creation_time']))
    for i, gi in enumerate(game['game_instances']):
      t = time.time()
      delta = t - gi['last_ping']
      if delta > PING_TIMEOUT_THRESHOLD:
        timeout = int(MAX_TIMEOUT - (delta + gi['timeout']))
        if timeout != gi['last_timeout']:
          gi['last_timeout'] = timeout
          logging.info("Timeout %s" % timeout)
          for gi2 in game['game_instances']:
            packet = enet.Packet(struct.pack("<I I I 120s", COW_cmd_type_timeout_peer_player, timeout, gi['player_id'], gi['player_name']), enet.PACKET_FLAG_RELIABLE)
            gi2['peer'].send(0, packet)
      if delta + gi['timeout'] > MAX_TIMEOUT:
        logging.info("Player %s has timeout." % game['game_instances'][i]['player_id'])
        delete_game_instance(i)
        break

    # -- seeders
    seeders = game['seeders']
    for host, leechers, _, seed_player_id in seeders:
      done = False
      while not done:
        e = host.service(0)
        if e.type == enet.EVENT_TYPE_RECEIVE:
          if not game['started']:
            logging.error("The assert has been thrown.")
          assert(game['started'])
          # -- todo: optimize the 2 ifs
          if e.packet.dataLength > 0:
            cmd_buffers[seed_player_id].append((e.packet.dataLength, e.packet.data))
            for leech_peer, leech_player_id in leechers:
              if leech_player_id == seed_player_id:
                leech_peer.send(0, e.packet)
        else:
          if e.type == enet.EVENT_TYPE_CONNECT:
            logging.info("%s: SEEDING CONNECT" % e.peer.address)
          elif e.type == enet.EVENT_TYPE_DISCONNECT:
            logging.info("%s: SEEDING DISCONNECT" % e.peer.address)
          else:
            done = True
      host.flush()

    # -- master host 
    done = False
    while not done:
      e = master_host.service(0)
      if e.type == enet.EVENT_TYPE_NONE:
        done = True
      elif e.type == enet.EVENT_TYPE_CONNECT:
        logging.info("%s: CONNECT" % e.peer.address)
      elif e.type == enet.EVENT_TYPE_DISCONNECT:
        logging.info("%s: DISCONNECT" % e.peer.address)
        gi_addresses = [str(gi['peer'].address) for gi in game['game_instances']]
        if str(e.peer.address) in gi_addresses:
          i = gi_addresses.index(str(e.peer.address))
          delete_game_instance(i)
      elif e.type == enet.EVENT_TYPE_RECEIVE:
        if e.packet.dataLength > 0:
          t, = struct.unpack_from("<I", e.packet.data)

          # -- ping
          if t == COW_cmd_type_ping:
            e.peer.send(0, e.packet) 
            gi_addresses = [str(gi['peer'].address) for gi in game['game_instances']]
            if str(e.peer.address) in gi_addresses:
              i = gi_addresses.index(str(e.peer.address))
              t = time.time()
              d = t - game['game_instances'][i]['last_ping']
              if d > PING_TIMEOUT_THRESHOLD:
                game['game_instances'][i]['timeout'] += d
              game['game_instances'][i]['last_ping'] = t

          # -- message
          if t == COW_cmd_type_message:
            for gi in game['game_instances']:
              gi['peer'].send(0, e.packet) 

          # -- join game
          elif t == COW_cmd_type_join_game:
            if not game['started']:
              t, team_id, player_name = struct.unpack_from("<I I 128s", e.packet.data)
              player_name = player_name[:player_name.index('\x00')]
              player_id = team_id * PLAYER_COUNT + game['player_count'][team_id]; 
              logging.info("Player's joining, team id is %s, player name is %s, hence player id is %s." % (team_id, player_name, player_id))
              game['player_count'][team_id] += 1;
              game['not_connected'][team_id] -= 1;
              game['game_instances'].append(
                {'peer' : e.peer,
                 'last_ping': time.time(),
                 'timeout': 0,
                 'last_timeout': MAX_TIMEOUT,
                 'team_id': team_id,
                 'player_id': player_id,
                 'player_name': player_name})
              game_ready(e.peer, player_id)
            else:
              game_closed(e.peer)

          # -- seed game
          elif t == COW_cmd_type_seed_game:
            if not game['started']:
              t, player_id = struct.unpack_from("<I I", e.packet.data) 
              port = free_ports.pop()
              logging.info("poping port %s." % port)
              seeder = (
                 enet.Host(enet.Address(None, port), 32, 1, 0, 0), 
                 game['leechers'], 
                 e.peer,
                 player_id)
              game['seeders'].append(seeder)
              packet = enet.Packet(struct.pack("<I I I", COW_cmd_type_port, port, player_id), enet.PACKET_FLAG_RELIABLE)
              e.peer.send(0, packet)
              logging.info("Seeder linked, player id: %s." % player_id)
              players_connected()
            else:
              game_closed(e.peer)

          # -- leech game
          elif t == COW_cmd_type_leech_game:
            if not game['started']:
              t, player_id = struct.unpack_from("<I I", e.packet.data) 
              game['leechers'].append((e.peer, player_id))
              logging.info("Leecher linked, player id: %s." % player_id)
              players_connected()
            else:
              game_closed(e.peer)

          # -- fixme
          #else:
          #  logging.info("message type unknown %s et message type %s" % (e.type, t))
        master_host.flush()

    # -- ping server
    done = False
    while not done:
      ping_server_host.flush
      e = ping_server_host.service(0)
      if e.type == enet.EVENT_TYPE_NONE:
        done = True
      elif e.type == enet.EVENT_TYPE_CONNECT:
        e.peer.timeout(0, 20000, 20000)
        logging.info("%s: PING SERVER CONNECT" % e.peer.address)
      elif e.type == enet.EVENT_TYPE_DISCONNECT:
        assert(str(e.peer.address) in ping_server_players)
        player_id = ping_server_players[str(e.peer.address)]
        logging.info("%s: PING SERVER DISCONNECT, player_id is %s" % (e.peer.address, player_id))
      	gi_player_ids = [gi['player_id'] for gi in game['game_instances']]
      	if player_id in gi_player_ids:
	        i = gi_player_ids.index(player_id)
	        delete_game_instance(i)
      elif e.type == enet.EVENT_TYPE_RECEIVE:
        if e.packet.dataLength > 0:
          t, = struct.unpack_from("<I", e.packet.data)

          # -- join game
          if t == COW_cmd_type_join_game:
          	t, team_id, player_name = struct.unpack_from("<I I 128s", e.packet.data)
          	player_id = team_id * PLAYER_COUNT + game['player_count'][team_id]
          	ping_server_players[str(e.peer.address)] = player_id
          	logging.info("PING SERVER: Player's joining, team id is %s, hence player id is %s." % (team_id, player_id))
          else:
          	logging.info("PING SERVER: an unexpected message has been received %s." % t)
          	assert(False)

except Exception as e:
  logging.info(e)
  raise
        
      