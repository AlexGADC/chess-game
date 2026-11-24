#include "Piece.h"

#include "Bishop.h"
#include "Board.h"
#include "BoardGeometry.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"

const int Piece::UNLIMITED_STEPS = BOARD_SIZE - 1;

PiecePtr Piece::Produce(EPieceType type, EColor color)
{
	switch (type)
	{
	case EPieceType::King:   return std::make_shared<King>(color);
	case EPieceType::Queen:  return std::make_shared<Queen>(color);
	case EPieceType::Rook:   return std::make_shared<Rook>(color);
	case EPieceType::Bishop: return std::make_shared<Bishop>(color);
	case EPieceType::Knight: return std::make_shared<Knight>(color);
	case EPieceType::Pawn:   return std::make_shared<Pawn>(color);
	}
	return nullptr;
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

PiecePtr Piece::Clone() const
{
	PiecePtr copy = Produce(m_type, m_color);
	copy->m_hasMoved = m_hasMoved;
	return copy;
}

PositionList Piece::GetPseudoLegalMoves(const Board& board, Position from) const
{
	return GetAttackedSquares(board, from);
}

const DirectionList& Piece::GetStraightDirections()
{
	static const DirectionList directions{ { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
	return directions;
}

const DirectionList& Piece::GetDiagonalDirections()
{
	static const DirectionList directions{ { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };
	return directions;
}

const DirectionList& Piece::GetAllDirections()
{
	static const DirectionList directions = []
	{
		DirectionList all = GetStraightDirections();
		all.insert(all.end(), GetDiagonalDirections().begin(), GetDiagonalDirections().end());
		return all;
	}();
	return directions;
}

PositionList Piece::GetMovesInDirections(
	const Board& board, Position from, const DirectionList& directions, int maxSteps) const
{
	PositionList moves;

	for (const Direction& direction : directions)
	{
		Position current = from;
		for (int step = 0; step < maxSteps; ++step)
		{
			current.row += direction.rowStep;
			current.col += direction.colStep;
			if (!Board::IsInside(current))
			{
				break;
			}

			const PiecePtr occupant = board.GetPiece(current);
			if (!occupant)
			{
				moves.push_back(current);
				continue;
			}

			if (occupant->GetColor() != m_color)
			{
				moves.push_back(current);
			}
			break;
		}
	}

	return moves;
}
