#ifndef A_STAR_H
#define A_STAR_H

#include <stdbool.h>

#include <cube.h>

typedef struct movenode_t {
	uint8_t face;
	int turn_type;
	struct movenode_t* next_node;
} movenode_t;

movenode_t* prependMoveNode(movenode_t* move_list, uint8_t face, int turn_type);

movenode_t* searchDepth(const cube_t* last_cube, int depth, uint8_t last_face, bool* cancel_flag);

movenode_t* idaStar(const cube_t* cube);

#endif
