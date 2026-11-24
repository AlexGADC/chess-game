#include "Rook.h"

Rook::Rook(EColor color)
	: Piece(EPieceType::Rook, color)
{
}

PositionList Rook::GetAttackedSquares(const Board& board, Position from) const
{
	return GetMovesInDirections(board, from, GetStraightDirections(), UNLIMITED_STEPS);
}
