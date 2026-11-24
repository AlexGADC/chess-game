#pragma once

#include "Board.h"

#include <initializer_list>
#include <ostream>

// Converts algebraic notation to a Position: At("e4") is row 3, col 4.
inline Position At(const char* square)
{
	return Position{ square[1] - '1', square[0] - 'a' };
}

inline PositionList Squares(std::initializer_list<const char*> squares)
{
	PositionList positions;
	for (const char* square : squares)
	{
		positions.push_back(At(square));
	}
	return positions;
}

// Makes GTest print a Position as "e4" instead of raw bytes when an assertion fails.
inline void PrintTo(const Position& position, std::ostream* output)
{
	*output << static_cast<char>('a' + position.col) << static_cast<char>('1' + position.row);
}

struct PiecePlacement
{
	const char* square;
	EPieceType type;
	EColor color;
};

// Builds a board that contains only the given pieces.
inline Board MakeBoard(std::initializer_list<PiecePlacement> placements)
{
	Board board;
	board.Clear();
	for (const PiecePlacement& placement : placements)
	{
		board.PlacePiece(At(placement.square), Piece::Produce(placement.type, placement.color));
	}
	return board;
}
