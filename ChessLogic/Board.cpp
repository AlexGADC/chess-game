#include "Board.h"

#include "ChessExceptions.h"

#include <algorithm>
#include <cstdlib>
#include <initializer_list>
#include <utility>

namespace
{
	const char* OUT_OF_BOUNDS_MESSAGE = "Position outside the board.";

	constexpr int CASTLING_KING_DISTANCE = 2;
	constexpr int PAWN_DOUBLE_STEP_DISTANCE = 2;

	constexpr char EMPTY_SQUARE_SYMBOL = '.';
	constexpr char FIRST_WHITE_PIECE_SYMBOL = 'A';
	constexpr char FIRST_BLACK_PIECE_SYMBOL = 'a';
	constexpr char CASTLING_RIGHT_SYMBOL = '1';
	constexpr char NO_CASTLING_RIGHT_SYMBOL = '0';

	// Every square of the board, so the code can use one loop instead of a nested pair.
	const PositionList& GetAllSquares()
	{
		static const PositionList squares = []
		{
			PositionList all;
			for (int row = 0; row < BOARD_SIZE; ++row)
			{
				for (int col = 0; col < BOARD_SIZE; ++col)
				{
					all.push_back(Position{ row, col });
				}
			}
			return all;
		}();
		return squares;
	}

	bool Contains(const PositionList& positions, Position position)
	{
		return std::find(positions.begin(), positions.end(), position) != positions.end();
	}

	bool IsLightSquare(Position position)
	{
		return (position.row + position.col) % 2 == 1;
	}

	bool AreAllOnSameSquareColor(const PositionList& positions)
	{
		return std::all_of(positions.begin(), positions.end(),
			[&positions](Position position) { return IsLightSquare(position) == IsLightSquare(positions.front()); });
	}

	// One character per square: a different letter for each piece type and color.
	char GetSquareSymbol(const PiecePtr& piece)
	{
		if (!piece)
		{
			return EMPTY_SQUARE_SYMBOL;
		}

		const char firstSymbol = piece->GetColor() == EColor::White ? FIRST_WHITE_PIECE_SYMBOL : FIRST_BLACK_PIECE_SYMBOL;
		return static_cast<char>(firstSymbol + static_cast<int>(piece->GetType()));
	}
}

bool Board::IsInside(Position position)
{
	return position.row >= 0 && position.row < SIZE
		&& position.col >= 0 && position.col < SIZE;
}

Board::Board(const Board& other)
	: m_enPassantTarget(other.m_enPassantTarget)
{
	for (const Position& square : GetAllSquares())
	{
		const PiecePtr& piece = other.m_squares[square.row][square.col];
		m_squares[square.row][square.col] = piece ? piece->Clone() : nullptr;
	}
}

Board& Board::operator=(const Board& other)
{
	if (this != &other)
	{
		Board copy(other);
		*this = std::move(copy);
	}
	return *this;
}

void Board::Clear()
{
	for (auto& row : m_squares)
	{
		row.fill(nullptr);
	}

	m_enPassantTarget.reset();
}

void Board::Reset()
{
	Clear();

	constexpr std::array<EPieceType, SIZE> backRowOrder{
		EPieceType::Rook, EPieceType::Knight, EPieceType::Bishop, EPieceType::Queen,
		EPieceType::King, EPieceType::Bishop, EPieceType::Knight, EPieceType::Rook };

	for (EColor color : { EColor::White, EColor::Black })
	{
		for (int col = 0; col < SIZE; ++col)
		{
			m_squares[GetBackRow(color)][col] = Piece::Produce(backRowOrder[col], color);
			m_squares[GetPawnStartRow(color)][col] = Piece::Produce(EPieceType::Pawn, color);
		}
	}
}

PiecePtr Board::GetPiece(Position position) const
{
	if (!IsInside(position))
	{
		throw OutOfBoundsException(OUT_OF_BOUNDS_MESSAGE);
	}
	return m_squares[position.row][position.col];
}

bool Board::IsEmpty(Position position) const
{
	return GetPiece(position) == nullptr;
}

void Board::PlacePiece(Position position, PiecePtr piece)
{
	if (!IsInside(position))
	{
		throw OutOfBoundsException(OUT_OF_BOUNDS_MESSAGE);
	}
	m_squares[position.row][position.col] = std::move(piece);
}

void Board::MovePiece(Position from, Position to)
{
	if (!IsInside(from) || !IsInside(to))
	{
		throw OutOfBoundsException(OUT_OF_BOUNDS_MESSAGE);
	}

	const PiecePtr piece = m_squares[from.row][from.col];
	if (!piece || from == to)
	{
		return;
	}

	const bool isPawn = piece->GetType() == EPieceType::Pawn;
	const bool isKing = piece->GetType() == EPieceType::King;

	// A pawn that moves diagonally onto an empty square is capturing en passant:
	// the captured pawn is beside the starting square, not on the destination.
	if (isPawn && from.col != to.col && IsEmpty(to))
	{
		m_squares[from.row][to.col] = nullptr;
	}

	if (isKing && std::abs(to.col - from.col) == CASTLING_KING_DISTANCE)
	{
		MoveCastlingRook(from.row, to.col);
	}

	m_enPassantTarget.reset();
	if (isPawn && std::abs(to.row - from.row) == PAWN_DOUBLE_STEP_DISTANCE)
	{
		m_enPassantTarget = Position{ (from.row + to.row) / 2, from.col };
	}

	m_squares[to.row][to.col] = piece;
	m_squares[from.row][from.col] = nullptr;
	piece->MarkMoved();
}

void Board::MoveCastlingRook(int row, int kingEndCol)
{
	for (const CastlingSide& side : CASTLING_SIDES)
	{
		const PiecePtr rook = m_squares[row][side.rookStartCol];
		if (side.kingEndCol == kingEndCol && rook)
		{
			m_squares[row][side.rookEndCol] = rook;
			m_squares[row][side.rookStartCol] = nullptr;
			rook->MarkMoved();
		}
	}
}

PositionList Board::GetLegalMoves(Position from) const
{
	if (!IsInside(from) || IsEmpty(from))
	{
		return {};
	}

	const PiecePtr piece = GetPiece(from);
	PositionList legalMoves;

	for (const Position& to : piece->GetPseudoLegalMoves(*this, from))
	{
		Board boardAfterMove(*this);
		boardAfterMove.MovePiece(from, to);
		if (!boardAfterMove.IsInCheck(piece->GetColor()))
		{
			legalMoves.push_back(to);
		}
	}

	return legalMoves;
}

bool Board::HasAnyLegalMove(EColor color) const
{
	for (const Position& square : GetAllSquares())
	{
		const PiecePtr& piece = m_squares[square.row][square.col];
		if (piece && piece->GetColor() == color && !GetLegalMoves(square).empty())
		{
			return true;
		}
	}
	return false;
}

bool Board::IsSquareAttacked(Position position, EColor attackerColor) const
{
	for (const Position& square : GetAllSquares())
	{
		const PiecePtr& piece = m_squares[square.row][square.col];
		if (piece && piece->GetColor() == attackerColor
			&& Contains(piece->GetAttackedSquares(*this, square), position))
		{
			return true;
		}
	}
	return false;
}

bool Board::IsInCheck(EColor color) const
{
	const std::optional<Position> king = FindKing(color);
	return king && IsSquareAttacked(*king, GetOpponent(color));
}

std::optional<Position> Board::FindKing(EColor color) const
{
	for (const Position& square : GetAllSquares())
	{
		const PiecePtr& piece = m_squares[square.row][square.col];
		if (piece && piece->GetType() == EPieceType::King && piece->GetColor() == color)
		{
			return square;
		}
	}
	return std::nullopt;
}

std::optional<Position> Board::GetEnPassantTarget() const
{
	return m_enPassantTarget;
}

bool Board::HasCastlingRight(EColor color, const CastlingSide& side) const
{
	const int backRow = GetBackRow(color);
	return IsUnmovedPiece(Position{ backRow, KING_START_COL }, EPieceType::King, color)
		&& IsUnmovedPiece(Position{ backRow, side.rookStartCol }, EPieceType::Rook, color);
}

bool Board::IsUnmovedPiece(Position position, EPieceType type, EColor color) const
{
	const PiecePtr& piece = m_squares[position.row][position.col];
	return piece && piece->GetType() == type && piece->GetColor() == color && !piece->HasMoved();
}

bool Board::HasInsufficientMaterial() const
{
	PositionList bishopSquares;
	int knightCount = 0;

	for (const Position& square : GetAllSquares())
	{
		const PiecePtr& piece = m_squares[square.row][square.col];
		if (!piece || piece->GetType() == EPieceType::King)
		{
			continue;
		}

		if (piece->GetType() == EPieceType::Bishop)
		{
			bishopSquares.push_back(square);
		}
		else if (piece->GetType() == EPieceType::Knight)
		{
			++knightCount;
		}
		else
		{
			// A pawn, a rook or a queen is always enough to give mate.
			return false;
		}
	}

	if (knightCount == 0)
	{
		// Only kings, or kings and bishops that all stand on the same square color.
		return AreAllOnSameSquareColor(bishopSquares);
	}

	// A single knight with nothing else cannot give mate.
	return knightCount == 1 && bishopSquares.empty();
}

std::string Board::GetPositionKey() const
{
	std::string key;

	for (const Position& square : GetAllSquares())
	{
		key += GetSquareSymbol(m_squares[square.row][square.col]);
	}

	for (EColor color : { EColor::White, EColor::Black })
	{
		for (const CastlingSide& side : CASTLING_SIDES)
		{
			key += HasCastlingRight(color, side) ? CASTLING_RIGHT_SYMBOL : NO_CASTLING_RIGHT_SYMBOL;
		}
	}

	// The en passant square only makes the position different while a pawn can really capture there.
	if (IsEnPassantCapturePossible())
	{
		key += std::to_string(m_enPassantTarget->row);
		key += std::to_string(m_enPassantTarget->col);
	}

	return key;
}

bool Board::IsEnPassantCapturePossible() const
{
	if (!m_enPassantTarget)
	{
		return false;
	}

	for (const Position& square : GetAllSquares())
	{
		// The target square is empty, so a pawn from another column can only reach it by capturing en passant.
		const PiecePtr& piece = m_squares[square.row][square.col];
		if (piece && piece->GetType() == EPieceType::Pawn && square.col != m_enPassantTarget->col
			&& Contains(GetLegalMoves(square), *m_enPassantTarget))
		{
			return true;
		}
	}
	return false;
}
