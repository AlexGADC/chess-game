// STUB TEMPORAR (Backend 1). Vezi comentariul din Piece.h.

#include "Piece.h"

PiecePtr Piece::Produce(EPieceType type, EColor color)
{
	return std::make_shared<Piece>(type, color);
}

Piece::Piece(EPieceType type, EColor color)
	: m_type(type)
	, m_color(color)
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
