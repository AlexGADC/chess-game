#include "Piece.h"

PiecePtr Piece::Produce(EPieceType type, EColor color)
{
	return std::make_shared<Piece>(type, color);
}

Piece::Piece(EPieceType type, EColor color)
	: m_type(type)
	, m_color(color)
	, m_hasMoved(false)
{
}

EPieceType Piece::GetType() const
{
	return m_type;
}

EColor Piece::GetColor() const
{
	return m_color;
}

bool Piece::HasMoved() const
{
	return m_hasMoved;
}

void Piece::MarkMoved()
{
	m_hasMoved = true;
}