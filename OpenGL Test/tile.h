#pragma once

struct Tile {
	unsigned short type;
	char rotation;
};

struct Choice {
	unsigned short type;
	char rotation;
	bool collapsed;
};