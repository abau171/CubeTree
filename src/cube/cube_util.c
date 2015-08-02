#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "cube.h"
#include "cube_internal.h"

bool isSolved(Cube cube) {
	for (int cornerId = 0; cornerId < 8; cornerId++) {
		if (cube->corners[cornerId].id != cornerId) return false;
		if (cube->corners[cornerId].rotation != 0) return false;
	}
	return true;
}

void shuffleCube(Cube cube, int numTurns) {
	srand(time(NULL));
	CubeFaceId lastFaceId = NO_FACE;
	for (int i = 0; i < numTurns; i++) {
		CubeFaceId nextFaceId;
		do {
			nextFaceId = (rand() % 6);
		} while (nextFaceId == lastFaceId);
		TurnType type = (rand() % 3) + 1;
		printMove(nextFaceId, type);
		turnCubeFace(cube, nextFaceId, type);
		lastFaceId = nextFaceId;
	}
}
