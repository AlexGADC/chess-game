#pragma once

#include "Piece.h"

// Moves any number of squares along a diagonal.
class Bishop : public Piece
{
public:
	explicit Bishop(EColor color);

	PositionList GetAttackedSquares(const Board& board, Position from) const override;
};
