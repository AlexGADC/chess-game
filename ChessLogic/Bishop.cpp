#include "Bishop.h"

Bishop::Bishop(EColor color)
	: Piece(EPieceType::Bishop, color)
{
}

PositionList Bishop::GetAttackedSquares(const Board& board, Position from) const
{
	return GetMovesInDirections(board, from, GetDiagonalDirections(), UNLIMITED_STEPS);
}
