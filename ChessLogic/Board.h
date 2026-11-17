#pragma once

#include "EColor.h"
#include "Piece.h"
#include "Position.h"

#include <array>
#include <optional>

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
	std::optional<Position> m_enPassantTarget;
};