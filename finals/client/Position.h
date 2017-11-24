#pragma once

struct Position
{
	int x, y;
	Position() { x = y = 0; }
	Position(int _x, int _y) { x = _x; y = _y; }
	bool operator== (Position const &rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!= (Position const &rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}
	bool operator< (Position const &rhs) const
	{
		if (y != rhs.y)
			return y<rhs.y;
		return x<rhs.x;
	}
	bool IsValid() const
	{
		return x != 0;
	}
	int DistSquare(Position const &rhs) const
	{
		return (x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y);
	}
};

