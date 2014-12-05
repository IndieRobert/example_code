




/*if(difficulty > 160) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_orc), (3), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(skeleton), (9), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_zombie), (5), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(crate01), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(tower), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(cauldron), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(orc_house), RAND(2), 3);
} else 
if(difficulty > 150) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_orc), (1), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(skeleton), (6), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_zombie), (3), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(crate01), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(tower), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(cauldron), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(orc_house), RAND(2), 3);
} else 
if(difficulty > 140) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_orc), (1), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(skeleton), (5), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_zombie), (2), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(crate01), (1), 3);
  if(RAND(2))
    GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(tower), (1), 3);
  else
    GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(cauldron), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(orc_house), RAND(2), 3);
} else 
if(difficulty > 130) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_orc), (1), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(skeleton), (4), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(plain_zombie), (1), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(crate01), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(orc_house), RAND(2), 3);
} else */
if(difficulty > 120) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (3), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (1), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(footmen), RAND(2), 3);
} else 
if(difficulty > 110) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (2), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (1), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(footmen), RAND(2), 3);
} else 
if(difficulty > 100) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (3), 3);
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(footmen), RAND(2), 3);
} else 
if(difficulty > 90) {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (2), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(footmen), RAND(2), 3);
} else {
  GAME_CreepAt(cr_ai_c, sx, sy, UNIT(footmen), (1), 3);
  GAME_CreepAt(cr_ai_c, sx+RAND(10)-(5), sy+RAND(10)-(5), UNIT(footmen), RAND(2), 3);
}