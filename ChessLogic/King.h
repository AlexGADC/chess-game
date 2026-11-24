#pragma once

#include "BoardGeometry.h"
#include "Piece.h"

// Moves one square in any direction, and can castle with a rook.
class King : public Piece
{
public:
	explicit King(EColor color);

	PositionList GetAttackedSquares(const Board& board, Position from) const override;

	// The one-square moves plus the castling moves that are currently allowed.
	PositionList GetPseudoLegalMoves(const Board& board, Position from) const override;

private:
	bool CanCastle(const Board& board, Position from, const CastlingSide& side) const;
};
