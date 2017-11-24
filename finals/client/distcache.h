#pragma once

#include <vector>

#include "parser.h"
#include "Position.h"

class DISTCACHE
{
public:
	int map_dx, map_dy;
	std::vector<unsigned char> mMap;
	std::vector<std::vector<unsigned char> > mDistMap;
	bool LoadFromFile(const char *filename);
	void SaveToFile(const char *filename);
	void CreateFromParser(PARSER &Parser);
	int GetDist(const Position &p0, const Position &p1);
	Position GetNextTowards(const Position &p0, const Position &p1);
};
