#define DEFINE_NODE_POINT(C, X, Y, R) \
  case C: \
    x = (X); \
    y = (Y); \
    r = (R) == 1 ? true : false; \
    break;

#define DEFINE_NODE_DEFAULT \
    default: \
      dbug(mask); \
      assert(false);

#define NODE_X ((no->i<<POS_SHIFT) + node_demi)
#define NODE_Y ((no->j<<POS_SHIFT) + node_demi)

switch(mask) {
  case 0: {
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, NODE_X+node_demi,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(3, NODE_X,            NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(4, NODE_X+node_size,  NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(5, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(6, NODE_X+node_demi,  NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(7, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 1: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, NODE_X+node_demi,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(3, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(4, NODE_X+node_size,  NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(5, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(6, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 2: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, NODE_X+node_demi,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(3, NODE_X,            NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(4, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(5, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(6, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 3: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, NODE_X+node_demi,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(3, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(4, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 4: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(2, NODE_X,            NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(3, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(4, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(5, NODE_X+node_demi,  NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(6, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 6: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(2, NODE_X,            NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(3, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(4, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 7: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(2, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 8: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(1, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(3, NODE_X+node_size,  NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(4, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(5, NODE_X+node_demi,  NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(6, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 9: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(1, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, NODE_X+node_size,  NODE_Y+node_demi, 0)
      DEFINE_NODE_POINT(3, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(4, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 11: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(1, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 12: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(1, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(2, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(3, NODE_X+node_demi,  NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(4, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 13: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_POINT(1, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(2, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 14: {
    TR_edge_t *edge = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge->x0,          edge->y0,         edge->r0)
      DEFINE_NODE_POINT(1, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(2, edge->x1,          edge->y1,         edge->r1)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 5: {
    TR_edge_t *edge0 = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    TR_edge_t *edge1 = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[1]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[1]];
    switch(f) {
      DEFINE_NODE_POINT(0, NODE_X,            NODE_Y,           0)
      DEFINE_NODE_POINT(1, edge1->x0,         edge1->y0,        edge1->r0)
      DEFINE_NODE_POINT(2, edge0->x1,         edge0->y1,        edge0->r1)
      DEFINE_NODE_POINT(3, edge1->x1,         edge1->y1,        edge1->r1)
      DEFINE_NODE_POINT(4, edge0->x0,         edge0->y0,        edge0->r0)
      DEFINE_NODE_POINT(5, NODE_X+node_size,  NODE_Y+node_size, 0)
      DEFINE_NODE_DEFAULT
    }
  } break;
  case 10: {
    TR_edge_t *edge0 = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[0]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[0]];
    TR_edge_t *edge1 = flying ? &gi->flying_edges[size-1][AT2(gi->flying_edge_map[size-1], no->i, no->j)[1]] : &gi->edges[size-1][AT2(gi->edge_map[size-1], no->i, no->j)[1]];
    switch(f) {
      DEFINE_NODE_POINT(0, edge0->x1,         edge0->y1,        edge0->r1)
      DEFINE_NODE_POINT(1, NODE_X+node_size,  NODE_Y,           0)
      DEFINE_NODE_POINT(2, edge0->x0,         edge0->y0,        edge0->r0)
      DEFINE_NODE_POINT(3, edge1->x0,         edge1->y0,        edge1->r0)
      DEFINE_NODE_POINT(4, NODE_X,            NODE_Y+node_size, 0)
      DEFINE_NODE_POINT(5, edge1->x1,         edge1->y1,        edge1->r1)
      DEFINE_NODE_DEFAULT
    }
  } break;
}