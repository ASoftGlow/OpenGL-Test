#pragma once

/*
L = Elbow corner
H = Half
C = Small corner
*/
enum TileType {
	Grass,
	Grass_L_NE,
	Grass_L_SE,
	Grass_L_SW,
	Grass_L_NW,
	Grass_H_N,
	Grass_H_E,
	Grass_H_S,
	Grass_H_W,
	Grass_C_NE,
	Grass_C_SE,
	Grass_C_SW,
	Grass_C_NW,
	Rock,
	Sea,
};

struct Tile {
	TileType type;
};

struct Choice {
	TileType type;
	bool collapsed;
};