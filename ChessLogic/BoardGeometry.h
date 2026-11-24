#pragma once

#include "EColor.h"

#include <array>

// Facts about the layout of a chess board that several classes need.

constexpr int BOARD_SIZE = 8;

constexpr EColor GetOpponent(EColor color)
{
	return color == EColor::White ? EColor::Black : EColor::White;
}

// The row a color's king and rooks start on.
constexpr int GetBackRow(EColor color)
{
	return color == EColor::White ? 0 : BOARD_SIZE - 1;
}

// +1 when the color's pawns move toward higher rows, -1 otherwise.
constexpr int GetPawnDirection(EColor color)
{
	return color == EColor::White ? 1 : -1;
}

constexpr int GetPawnStartRow(EColor color)
{
	return GetBackRow(color) + GetPawnDirection(color);
}

// The row on which a color's pawn is promoted.
constexpr int GetPromotionRow(EColor color)
{
	return GetBackRow(GetOpponent(color));
}

constexpr int KING_START_COL = 4;

// The columns involved when castling toward one side of the board.
struct CastlingSide
{
	int rookStartCol;
	int rookEndCol;
	int kingEndCol;
};

constexpr CastlingSide KING_SIDE_CASTLING{ 7, 5, 6 };
constexpr CastlingSide QUEEN_SIDE_CASTLING{ 0, 3, 2 };
constexpr std::array<CastlingSide, 2> CASTLING_SIDES{ KING_SIDE_CASTLING, QUEEN_SIDE_CASTLING };
