#pragma once

// STUB TEMPORAR (Backend 1). Piece apartine lui Backend 2:
// cand versiunea lui ajunge pe main, se pastreaza a lui si se sterge acest fisier.

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

private:
	EPieceType m_type;
	EColor m_color;
};
