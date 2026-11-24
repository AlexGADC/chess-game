#pragma once

#include "Piece.h"

// Moves any number of squares along a row or a column.
class Rook : public Piece
{
public:
	explicit Rook(EColor color);

	PositionList GetAttackedSquares(const Board& board, Position from) const override;
};
