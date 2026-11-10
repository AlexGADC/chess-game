#pragma once

// STUB TEMPORAR (Backend 1). Board apartine lui Backend 2:
// cand versiunea lui ajunge pe main, se pastreaza a lui si se sterge acest fisier.
// Contine doar metodele publice pe care le foloseste Game, cu corpuri minime.

#include "EColor.h"
#include "Piece.h"
#include "Position.h"

#include <array>

class Board
{
public:
	static constexpr int SIZE = 8;

	static bool IsInside(Position position);

	void Reset();

	PiecePtr GetPiece(Position position) const;
	bool IsEmpty(Position position) const;

	void MovePiece(Position from, Position to);
	void ReplacePiece(Position position, PiecePtr piece);

	PositionList GetLegalMoves(Position from) const;
	bool IsInCheck(EColor color) const;
	bool HasAnyLegalMove(EColor color) const;

private:
	std::array<std::array<PiecePtr, SIZE>, SIZE> m_squares;
};
