#include "King.h"

#include "Board.h"

King::King(EColor color)
	: Piece(EPieceType::King, color)
{
}

PositionList King::GetAttackedSquares(const Board& board, Position from) const
{
	return GetMovesInDirections(board, from, GetAllDirections(), SINGLE_STEP);
}

PositionList King::GetPseudoLegalMoves(const Board& board, Position from) const
{
	PositionList moves = GetAttackedSquares(board, from);

	for (const CastlingSide& side : CASTLING_SIDES)
	{
		if (CanCastle(board, from, side))
		{
			moves.push_back(Position{ from.row, side.kingEndCol });
		}
	}

	return moves;
}

bool King::CanCastle(const Board& board, Position from, const CastlingSide& side) const
{
	const EColor opponent = GetOpponent(GetColor());

	// The right is only about the king and rook never having moved; the rest depends on the position.
	const bool isOnStartSquare = from == Position{ GetBackRow(GetColor()), KING_START_COL };
	if (!isOnStartSquare || !board.HasCastlingRight(GetColor(), side) || board.IsSquareAttacked(from, opponent))
	{
		return false;
	}

	const int colStep = side.rookStartCol > from.col ? 1 : -1;

	for (int col = from.col + colStep; col != side.rookStartCol; col += colStep)
	{
		if (!board.IsEmpty(Position{ from.row, col }))
		{
			return false;
		}
	}

	// The king may not cross or land on an attacked square.
	for (int col = from.col + colStep; col != side.kingEndCol + colStep; col += colStep)
	{
		if (board.IsSquareAttacked(Position{ from.row, col }, opponent))
		{
			return false;
		}
	}

	return true;
}
