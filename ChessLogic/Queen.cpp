#include "Queen.h"

Queen::Queen(EColor color)
	: Piece(EPieceType::Queen, color)
{
}

PositionList Queen::GetAttackedSquares(const Board& board, Position from) const
{
	return GetMovesInDirections(board, from, GetAllDirections(), UNLIMITED_STEPS);
}
