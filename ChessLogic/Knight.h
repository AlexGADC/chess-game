#pragma once

#include "Piece.h"

// Jumps in an L shape: two squares one way and one square sideways. Other pieces do not block it.
class Knight : public Piece
{
public:
	explicit Knight(EColor color);

	PositionList GetAttackedSquares(const Board& board, Position from) const override;
};
