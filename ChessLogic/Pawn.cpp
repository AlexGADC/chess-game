#include "Pawn.h"

#include "Board.h"
#include "BoardGeometry.h"

#include <initializer_list>

Pawn::Pawn(EColor color)
	: Piece(EPieceType::Pawn, color)
{
}

PositionList Pawn::GetAttackedSquares(const Board& /*board*/, Position from) const
{
	PositionList attackedSquares;
	const int rowAhead = from.row + GetPawnDirection(GetColor());

	for (int colStep : { -1, 1 })
	{
		const Position target{ rowAhead, from.col + colStep };
		if (Board::IsInside(target))
		{
			attackedSquares.push_back(target);
		}
	}

	return attackedSquares;
}

PositionList Pawn::GetPseudoLegalMoves(const Board& board, Position from) const
{
	PositionList moves = GetForwardMoves(board, from);

	for (const Position& target : GetAttackedSquares(board, from))
	{
		if (CanCaptureOn(board, from, target))
		{
			moves.push_back(target);
		}
	}

	return moves;
}

PositionList Pawn::GetForwardMoves(const Board& board, Position from) const
{
	PositionList moves;
	const int direction = GetPawnDirection(GetColor());

	const Position oneAhead{ from.row + direction, from.col };
	if (!Board::IsInside(oneAhead) || !board.IsEmpty(oneAhead))
	{
		return moves;
	}
	moves.push_back(oneAhead);

	const Position twoAhead{ from.row + 2 * direction, from.col };
	if (from.row == GetPawnStartRow(GetColor()) && board.IsEmpty(twoAhead))
	{
		moves.push_back(twoAhead);
	}

	return moves;
}

bool Pawn::CanCaptureOn(const Board& board, Position from, Position target) const
{
	const PiecePtr occupant = board.GetPiece(target);
	if (occupant)
	{
		return occupant->GetColor() != GetColor();
	}

	// En passant: the target is the square an enemy pawn has just skipped, and that pawn stands beside us.
	const Position besideUs{ from.row, target.col };
	return board.GetEnPassantTarget() == target && IsEnemyPawnOn(board, besideUs);
}

bool Pawn::IsEnemyPawnOn(const Board& board, Position position) const
{
	const PiecePtr piece = board.GetPiece(position);
	return piece && piece->GetType() == EPieceType::Pawn && piece->GetColor() != GetColor();
}
