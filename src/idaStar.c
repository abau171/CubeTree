#include <stdlib.h>
#include <stdbool.h>

#include "cube3x3/cube.h"

struct CubeMoveNode {
	CubeFaceId faceId;
	TurnType turnType;
	struct CubeMoveNode* next;
};

struct CubeMoveNode* moveStack = NULL;

static void pushMove(CubeFaceId faceId, TurnType turnType) {
	struct CubeMoveNode* newNode = malloc(sizeof(struct CubeMoveNode));
	newNode->faceId = faceId;
	newNode->turnType = turnType;
	newNode->next = moveStack;
	moveStack = newNode;
}

static bool searchDepth(Cube cube, int depth) {
	if (depth == 0) {
		return cubeIsSolved(cube);
	}
	for (TurnType turnType = 1; turnType < 4; turnType++) {
		for (CubeFaceId faceId = 0; faceId < 6; faceId++) {
			Cube clone = cloneCube(cube);
			turnCubeFace(clone, faceId, turnType);
			bool solved = searchDepth(clone, depth - 1);
			if (solved) {
				pushMove(faceId, turnType);
				return true;
			}
		}
	}
	return false;
}

void solveCube(Cube cube) {
	bool solved = false;
	int depth = 0;
	do {
		solved = searchDepth(cube, depth);
		depth++;
	} while (!solved);
	struct CubeMoveNode* curNode = moveStack;
	while (curNode != NULL) {
		printTurn(curNode->faceId, curNode->turnType, true);
		curNode = curNode->next;
	}
}