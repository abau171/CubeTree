#include <stdio.h>

#include "cube.h"
#include "cube_internal.h"

static char stickers[] = {'B', 'O', 'W', 'R', 'Y', 'G'};

static void printFaceStickerColor(CubeFaceId faceId) {
	putchar(stickers[faceId]);
}

static void printStickerRow(Cube cube, CubeFaceId faceId, int y) {
	for (int x = 0; x < 3; x++) {
		printFaceStickerColor(getStickerFace(cube, faceId, x, y));
		if (x < 2) putchar(' ');
	}
}

void printCube(Cube cube) {
	printf("cube: \n");
	for (int y = 0; y < 3; y++) {
		printf("     ");
		printStickerRow(cube, U, y);
		putchar('\n');
	}
	CubeFaceId faces[] = {L, F, R, B};
	for (int y = 0; y < 3; y++) {
		for (int i = 0; i < 4; i++) {
			CubeFaceId faceId = faces[i];
			printStickerRow(cube, faceId, y);
		}
		putchar('\n');
	}
	for (int y = 0; y < 3; y++) {
		printf("     ");
		printStickerRow(cube, D, y);
		putchar('\n');
	}
}
