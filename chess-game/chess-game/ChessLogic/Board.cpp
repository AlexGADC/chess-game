// STUB TEMPORAR (Backend 1). Vezi comentariul din Board.h.
// Regulile de sah (mutari legale, sah) lipsesc intentionat: le scrie Backend 2.

#include "Board.h"

bool Board::IsInside(Position position)
{
	return position.row >= 0 && position.row < SIZE
		&& position.col >= 0 && position.col < SIZE;
}

void Board::Reset()
{
	for (auto& row : m_squares)
		row.fill(nullptr);
}

PiecePtr Board::GetPiece(Position position) const
{
	return m_squares[position.row][position.col];
}

bool Board::IsEmpty(Position position) const
{
	return GetPiece(position) == nullptr;
}

void Board::MovePiece(Position from, Position to)
{
	m_squares[to.row][to.col] = m_squares[from.row][from.col];
	m_squares[from.row][from.col] = nullptr;
}

void Board::ReplacePiece(Position position, PiecePtr piece)
{
	m_squares[position.row][position.col] = piece;
}

PositionList Board::GetLegalMoves(Position /*from*/) const
{
	return {};
}

bool Board::IsInCheck(EColor /*color*/) const
{
	return false;
}

bool Board::HasAnyLegalMove(EColor /*color*/) const
{
	return true;
}
