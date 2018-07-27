#ifndef GAME_BUILDABLES_H
#define GAME_BUILDABLES_H

#include <cstring>
#include <game/generated/protocol.h>

enum Buildables
{
	BUILDABLE_BLOCK1,
	BUILDABLE_BLOCK2,
	BUILDABLE_BARREL,
	BUILDABLE_POWERBARREL,
	BUILDABLE_TURRET,
	BUILDABLE_FLAMETRAP,
	BUILDABLE_LIGHTNINGWALL,
	BUILDABLE_TESLACOIL,
	BUILDABLE_GENERATOR,
	NUM_BUILDABLES
};

static const char *s_BuildTipText[NUM_BUILDABLES] = {
	"Block",
	"Hard block",
	"Barrel",
	"Power barrel",
	"Turret stand",
	"Flamer",
	"Electric wall",
	"Teslacoil",
	"Shield generator"
};

const int BuildableCost[NUM_BUILDABLES] =
{
	1, // BUILDABLE_BLOCK1,
	2, // BUILDABLE_BLOCK2,
	1, // BUILDABLE_BARREL,
	2, // BUILDABLE_POWERBARREL,
	5, // BUILDABLE_TURRET,
	4, // BUILDABLE_FLAMETRAP,
	5, // BUILDABLE_LIGHTNINGWALL,
	15, // BUILDABLE_TESLACOIL,
	20, // BUILDABLE_GENERATOR,
};

const int BuildableSize[NUM_BUILDABLES] =
{
	48, // BUILDABLE_BLOCK1,
	48, // BUILDABLE_BLOCK2,
	108, // BUILDABLE_BARREL,
	108, // BUILDABLE_BARREL,
	128, // BUILDABLE_TURRET,
	92, // BUILDABLE_FLAMETRAP,
	64, // BUILDABLE_LIGHTNINGWALL,
	170, // BUILDABLE_TESLACOIL,
	192, // BUILDABLE_GENERATOR,
};

const int BuildableOffset[NUM_BUILDABLES] =
{
	0, // BUILDABLE_BLOCK1,
	0, // BUILDABLE_BLOCK2,
	-45, // BUILDABLE_BARREL,
	-45, // BUILDABLE_BARREL,
	-49, // BUILDABLE_TURRET,
	28, // BUILDABLE_FLAMETRAP,
	-18, // BUILDABLE_LIGHTNINGWALL,
	-69, // BUILDABLE_TESLACOIL,
	-58, // BUILDABLE_GENERATOR,
};

#endif
