#include "stdafx.h"
#include "parser.h"
#include "distcache.h"

#include <cstring>



bool DISTCACHE::LoadFromFile(const char * filename)
{
	FILE *f=fopen(filename, "rb");
	if (f==NULL) return false;
	unsigned char dimensions[2];
	auto s = fread(dimensions, 1, 2, f);
	(void)s;
	map_dx = dimensions[0];
	map_dy = dimensions[1];
	mMap.resize(map_dx*map_dy);
	mDistMap.clear();
	mDistMap.resize(map_dx*map_dy);
	s = fread(&mMap.front(), 1, mMap.size(), f);
	(void)s;
	int x, y;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			if (mMap[x+y*map_dx])
		{
			std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
			data.resize(map_dx*map_dy);
			s = fread(&data.front(), 1, data.size(), f);
			(void)s;
		}
	fclose(f);
	return true;
}

void DISTCACHE::SaveToFile(const char * filename)
{
	FILE *f=fopen(filename, "wb");
	assert(f!=NULL);
	unsigned char dimensions[2];
	dimensions[0]=(unsigned char)map_dx;
	dimensions[1]=(unsigned char)map_dy;
	fwrite(dimensions, 1, 2, f);
	fwrite(&mMap.front(), 1, mMap.size(), f);
	int x, y;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			if (mMap[x+y*map_dx])
			{
				std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
				assert(!data.empty());
				fwrite(&data.front(), 1, data.size(), f);
			}
	fclose(f);
}


Position DISTCACHE::GetNextTowards(const Position &p0, const Position &p1)
{
	if (p0==p1) return Position(0,0);
	if (!mMap[p0.x+p0.y*map_dx] || !mMap[p1.x+p1.y*map_dx]) return Position(0,0);
	int min_dist=0xFF;
	int count = 0;
	Position ret;
	for(int dx=-1;dx<=1;dx++)
		for(int dy=-1;dy<=1;dy++)
	{
		if (dx==0 && dy==0) continue;
		Position p2(p0.x+dx, p0.y+dy);
		if (!mMap[p2.x + p2.y*map_dx]) continue;
		int d=GetDist(p2, p1);
		assert(d>=0);
		if (d<min_dist)
		{
			min_dist = d;
			count = 1;
			ret = p2;
		} else if (d==min_dist)
		{
			count++;
			if (((p0.x+p0.y)%count)==0)
			{
				ret = p2;
			}
		}
	}
	assert(ret.IsValid());
	return ret;
}

void DISTCACHE::CreateFromParser(PARSER &Parser)
{
	map_dx = Parser.w;
	map_dy = Parser.h;
	mMap.resize(map_dx*map_dy);
	mDistMap.clear();
	mDistMap.resize(map_dx*map_dy);
	std::vector<Position> open_list;
	int x, y;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			mMap[x+y*map_dx] = Parser.GetAt(Position(x, y))==PARSER::WALL?0:1;
	for(y=0;y<map_dy;y++)
		for(x=0;x<map_dx;x++)
			if (mMap[x+y*map_dx])
			{
				std::vector<unsigned char> &data = mDistMap[x+y*map_dx];
				data.resize(map_dx*map_dy);
				memset(&data.front(), 0xFF, data.size());
				data[x+y*map_dx]=0;
				open_list.clear();
				open_list.push_back(Position(x,y));
				unsigned int idx;
				for(idx=0;idx<open_list.size();idx++)
				{
					Position from = open_list[idx];
					int dist = data[from.x+from.y*map_dx];
					assert(dist!=0xFF);
					for(int dx=-1;dx<=1;dx++)
						for(int dy=-1;dy<=1;dy++)
					{
						if (dx==0 && dy==0) continue;
						Position p1(from.x+dx, from.y+dy);
						if (!mMap[p1.x + p1.y*map_dx]) continue;
						if (data[p1.x+p1.y*map_dx]==0xFF)
						{
							data[p1.x+p1.y*map_dx]=dist+1;
							open_list.push_back(p1);
						}
					}
				}
			}
}

int DISTCACHE::GetDist(const Position &p0, const Position &p1) const
{
	if (!mMap[p0.x+p0.y*map_dx] || !mMap[p1.x+p1.y*map_dx]) return -1;
	return mDistMap[p1.x+p1.y*map_dx][p0.x+p0.y*map_dx];
}
