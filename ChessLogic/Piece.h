#pragma once

#include "IPieceInfo.h"
#include "Position.h"

#include <memory>
#include <vector>

class Board;
class Piece;
using PiecePtr = std::shared_ptr<Piece>;

// How far a piece travels in one step: a row offset and a column offset.
struct Direction
{
	int rowStep = 0;
	int colStep = 0;
};

using DirectionList = std::vector<Direction>;

// Base class of all chess pieces. It holds what every piece has in common;
// each derived class describes how its own piece moves.
class Piece : public IPieceInfo
{
public:
	// Factory: creates the derived class that matches `type`.
	static PiecePtr Produce(EPieceType type, EColor color);

	EPieceType GetType() const override;
	EColor GetColor() const override;

	bool HasMoved() const;
	void MarkMoved();

	// A separate piece of the same class, with the same color and "has moved" flag.
	PiecePtr Clone() const;

	// The squares this piece attacks from `from`: empty squares it reaches and enemy pieces it could capture.
	virtual PositionList GetAttackedSquares(const Board& board, Position from) const = 0;

	// The moves the piece's own rules allow from `from`. Whether the move leaves the
	// own king in check is not considered here; Board::GetLegalMoves filters that.
	virtual PositionList GetPseudoLegalMoves(const Board& board, Position from) const;

protected:
	static constexpr int SINGLE_STEP = 1;
	static const int UNLIMITED_STEPS;

	static const DirectionList& GetStraightDirections();
	static const DirectionList& GetDiagonalDirections();
	static const DirectionList& GetAllDirections();

	Piece(EPieceType type, EColor color);

	// Walks from `from` along each direction for at most `maxSteps` squares and stops at the
	// first piece it meets. An enemy piece's square is included (capture), an own piece's is not.
	PositionList GetMovesInDirections(
		const Board& board, Position from, const DirectionList& directions, int maxSteps) const;

private:
	EPieceType m_type;
	EColor m_color;
	bool m_hasMoved;
};
