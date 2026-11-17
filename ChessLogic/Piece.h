#pragma once

#include "IPieceInfo.h"

#include <memory>

class Piece;
using PiecePtr = std::shared_ptr<Piece>;

class Piece : public IPieceInfo
{
public:
	static PiecePtr Produce(EPieceType type, EColor color);

	Piece(EPieceType type, EColor color);

	EPieceType GetType() const override;
	EColor GetColor() const override;

	bool HasMoved() const;
	void MarkMoved();

private:
	EPieceType m_type;
	EColor m_color;
	bool m_hasMoved;
};