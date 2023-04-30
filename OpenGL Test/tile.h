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
	Sand,
	Sand_hole,
	Sand_shrub,
	Cactas_T,
	Cactas_B,
	Sand_L_NE,
	Sand_L_SE,
	Sand_L_SW,
	Sand_L_NW,
	Sand_H_N,
	Sand_H_E,
	Sand_H_S,
	Sand_H_W,
	Sand_C_NE,
	Sand_C_SE,
	Sand_C_SW,
	Sand_C_NW,
	Sand_Grass_L_NE,
	Sand_Grass_L_SE,
	Sand_Grass_L_SW,
	Sand_Grass_L_NW,
	Sand_Grass_H_N,
	Sand_Grass_H_E,
	Sand_Grass_H_S,
	Sand_Grass_H_W,
	Sand_Grass_C_NE,
	Sand_Grass_C_SE,
	Sand_Grass_C_SW,
	Sand_Grass_C_NW,
	Sand_Grass_Sea_NE,
	Sand_Grass_Sea_SE,
	Sand_Grass_Sea_SW,
	Sand_Grass_Sea_NW,
	MISSING
};

const unsigned TILE_SIZE = sizeof(char);

struct Tile {
	TileType type = MISSING;
};

struct Choice {
	TileType type;
};