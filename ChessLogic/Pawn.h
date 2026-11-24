#pragma once

#include "Piece.h"

// Moves forward, captures diagonally forward, and can capture en passant.
class Pawn : public Piece
{
public:
	explicit Pawn(EColor color);

	// A pawn attacks the two squares diagonally in front of it, whether or not they are occupied.
	PositionList GetAttackedSquares(const Board& board, Position from) const override;

	// Forward moves onto empty squares plus the diagonal captures that are possible right now.
	PositionList GetPseudoLegalMoves(const Board& board, Position from) const override;

private:
	PositionList GetForwardMoves(const Board& board, Position from) const;
	bool CanCaptureOn(const Board& board, Position from, Position target) const;
	bool IsEnemyPawnOn(const Board& board, Position position) const;
};
