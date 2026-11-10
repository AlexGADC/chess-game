#pragma once

#include <vector>

// Conventie coordonate (de confirmat cu toata echipa):
//   row 0 = linia 1 (randul de baza al albului), row 7 = linia 8
//   col 0 = coloana a, col 7 = coloana h
struct Position
{
	int row = 0;
	int col = 0;

	bool operator==(const Position& other) const
	{
		return row == other.row && col == other.col;
	}

	bool operator!=(const Position& other) const
	{
		return !(*this == other);
	}
};

using PositionList = std::vector<Position>;
