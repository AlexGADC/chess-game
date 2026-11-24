#include "Knight.h"

namespace
{
	const DirectionList KNIGHT_JUMPS{
		{ 2, 1 }, { 2, -1 }, { -2, 1 }, { -2, -1 },
		{ 1, 2 }, { 1, -2 }, { -1, 2 }, { -1, -2 } };
}

Knight::Knight(EColor color)
	: Piece(EPieceType::Knight, color)
{
}

PositionList Knight::GetAttackedSquares(const Board& board, Position from) const
{
	return GetMovesInDirections(board, from, KNIGHT_JUMPS, SINGLE_STEP);
}
