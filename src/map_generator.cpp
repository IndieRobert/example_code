#define MAP_GENERATOR_SIZE_X 32
#define MAP_GENERATOR_SIZE_Y 32
TCOD_random_t randm;
char char_map[MAP_GENERATOR_SIZE_X][MAP_GENERATOR_SIZE_Y];
static int bspDepth=8;
static int minRoomSize=4;
static bool randomRoom=true; // a room fills a random part of the node or the maximum available space ?
static bool roomWalls=false; // if true, there is always a wall on north & west side of a room
typedef	char map_t[MAP_GENERATOR_SIZE_X][MAP_GENERATOR_SIZE_Y];

// draw a vertical line
void vline(map_t *map,int x, int y1, int y2) {
	int y=y1;
	int dy=(y1>y2?-1:1);
	(*map)[x][y]=' ';
	if ( y1 == y2 ) return;
	do {
		y+=dy;
		(*map)[x][y]=' ';
	} while (y!=y2);
}

// draw a vertical line up until we reach an empty space
void vline_up(map_t *map,int x, int y) {
	while (y >= 0 && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		y--;
	}
}

// draw a vertical line down until we reach an empty space
void vline_down(map_t *map,int x, int y) {
	while (y < MAP_GENERATOR_SIZE_Y && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		y++;
	}
}

// draw a horizontal line
void hline(map_t *map,int x1, int y, int x2) {
	int x=x1;
	int dx=(x1>x2?-1:1);
	(*map)[x][y]=' ';
	if ( x1 == x2 ) return;
	do {
		x+=dx;
		(*map)[x][y]=' ';
	} while (x!=x2);
}

// draw a horizontal line left until we reach an empty space
void hline_left(map_t *map,int x, int y) {
	while (x >= 0 && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		x--;
	}
}

// draw a horizontal line right until we reach an empty space
void hline_right(map_t *map,int x, int y) {
	while (x < MAP_GENERATOR_SIZE_X && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		x++;
	}
}

// the class building the dungeon from the bsp nodes
unsigned char traverse_node(TCOD_bsp_t *node, void *userData) {
	map_t *map=(map_t *)userData;
	if ( TCOD_bsp_is_leaf(node) ) {
		// calculate the room size
		int minx = node->x+1;
		int maxx = node->x+node->w-1;
		int miny = node->y+1;
		int maxy = node->y+node->h-1;
		int x,y;
		if (! roomWalls ) {
			if ( minx > 1 ) minx--;
			if ( miny > 1 ) miny--;
		}
		if (maxx == MAP_GENERATOR_SIZE_X-1 ) maxx--;
		if (maxy == MAP_GENERATOR_SIZE_Y-1 ) maxy--;
		if ( randomRoom ) {
			minx = TCOD_random_get_int(randm,minx,maxx-minRoomSize+1);
			miny = TCOD_random_get_int(randm,miny,maxy-minRoomSize+1);
			maxx = TCOD_random_get_int(randm,minx+minRoomSize-1,maxx);
			maxy = TCOD_random_get_int(randm,miny+minRoomSize-1,maxy);
		}
		// resize the node to fit the room
//	printf("node %dx%d %dx%d => room %dx%d %dx%d\n",node->x,node->y,node->w,node->h,minx,miny,maxx-minx+1,maxy-miny+1);
		node->x=minx;
		node->y=miny;
		node->w=maxx-minx+1;
		node->h=maxy-miny+1;
		// dig the room
		for (x=minx; x <= maxx; x++ ) {
			for (y=miny; y <= maxy; y++ ) {
				(*map)[x][y]=' ';
			}
		}
	} else {
//	printf("lvl %d %dx%d %dx%d\n",node->level, node->x,node->y,node->w,node->h);
		// resize the node to fit its sons
		TCOD_bsp_t *left=TCOD_bsp_left(node);
		TCOD_bsp_t *right=TCOD_bsp_right(node);
		node->x=MIN(left->x,right->x);
		node->y=MIN(left->y,right->y);
		node->w=MAX(left->x+left->w,right->x+right->w)-node->x;
		node->h=MAX(left->y+left->h,right->y+right->h)-node->y;
		// create a corridor between the two lower nodes
		if (node->horizontal) {
			// vertical corridor
			if ( left->x+left->w -1 < right->x || right->x+right->w-1 < left->x ) {
				// no overlapping zone. we need a Z shaped corridor
				int x1=TCOD_random_get_int(randm,left->x,left->x+left->w-1);
				int x2=TCOD_random_get_int(randm,right->x,right->x+right->w-1);
				int y=TCOD_random_get_int(randm,left->y+left->h,right->y);
				vline_up(map,x1,y-1);
				hline(map,x1,y,x2);
				vline_down(map,x2,y+1);
			} else {
				// straight vertical corridor
				int minx=MAX(left->x,right->x);
				int maxx=MIN(left->x+left->w-1,right->x+right->w-1);
				int x=TCOD_random_get_int(randm,minx,maxx);
				vline_down(map,x,right->y);
				vline_up(map,x,right->y-1);
			}
		} else {
			// horizontal corridor
			if ( left->y+left->h -1 < right->y || right->y+right->h-1 < left->y ) {
				// no overlapping zone. we need a Z shaped corridor
				int y1=TCOD_random_get_int(randm,left->y,left->y+left->h-1);
				int y2=TCOD_random_get_int(randm,right->y,right->y+right->h-1);
				int x=TCOD_random_get_int(randm,left->x+left->w,right->x);
				hline_left(map,x-1,y1);
				vline(map,x,y1,y2);
				hline_right(map,x+1,y2);
			} else {
				// straight horizontal corridor
				int miny=MAX(left->y,right->y);
				int maxy=MIN(left->y+left->h-1,right->y+right->h-1);
				int y=TCOD_random_get_int(randm,miny,maxy);
				hline_left(map,right->x-1,y);
				hline_right(map,right->x,y);
			}
		}
	}
	return 1;
}

void generate_map() {
  log("Generating map with seed: " + u2a(game_desc.seed));
  randm = TCOD_random_new_from_seed(TCOD_RNG_CMWC, game_desc.seed);
  gp_gi.bsp = TCOD_bsp_new_with_size(0,0,MAP_GENERATOR_SIZE_X,MAP_GENERATOR_SIZE_Y);
  memset(char_map,'#',sizeof(char)*MAP_GENERATOR_SIZE_X*MAP_GENERATOR_SIZE_Y);
  TCOD_bsp_split_recursive(gp_gi.bsp,randm,bspDepth,minRoomSize+(roomWalls?1:0),minRoomSize+(roomWalls?1:0),1.5f,1.5f);
  TCOD_bsp_traverse_inverted_level_order(gp_gi.bsp,traverse_node,&char_map);
  DO_TIMES(MAP_GENERATOR_SIZE_X) {
    char_map[f][0] = '#';
    char_map[f][MAP_GENERATOR_SIZE_Y-1] = '#';
  }
  DO_TIMES(MAP_GENERATOR_SIZE_Y) {
    char_map[0][f] = '#';
    char_map[MAP_GENERATOR_SIZE_X-1][f] = '#';
  }
  if(!game_desc.PvE) {
	  FOR_RECT(0, MAP_GENERATOR_SIZE_X-1, 0, MAP_GENERATOR_SIZE_Y-1) {
	    if(i+(MAP_GENERATOR_SIZE_Y-j) > MAP_GENERATOR_SIZE_X)
	      char_map[i][j] = char_map[j][i];
	  }
	}
}