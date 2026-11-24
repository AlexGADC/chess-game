#pragma once

#include "Piece.h"

// Moves any number of squares along a row, a column or a diagonal.
class Queen : public Piece
{
public:
	explicit Queen(EColor color);

	PositionList GetAttackedSquares(const Board& board, Position from) const override;
};
