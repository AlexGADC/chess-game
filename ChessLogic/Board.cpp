#include "Board.h"

#include "ChessExceptions.h"

#include <array>
#include <cstdlib>
#include <utility>

namespace
{
	constexpr EColor Opposite(EColor color)
	{
		return color == EColor::White ? EColor::Black : EColor::White;
	}

	constexpr int PawnDir(EColor color)
	{
		return color == EColor::White ? 1 : -1;
	}

	constexpr int PawnStartRow(EColor color)
	{
		return color == EColor::White ? 1 : Board::SIZE - 2;
	}

	constexpr int BackRow(EColor color)
	{
		return color == EColor::White ? 0 : Board::SIZE - 1;
	}

	struct Cell
	{
		bool occupied = false;
		EPieceType type{};
		EColor color{};
		bool moved = false;
	};

	using Grid = std::array<std::array<Cell, Board::SIZE>, Board::SIZE>;

	Grid BuildGrid(const Board& board)
	{
		Grid grid{};
		for (int r = 0; r < Board::SIZE; ++r)
		{
			for (int c = 0; c < Board::SIZE; ++c)
			{
				PiecePtr piece = board.GetPiece(Position{ r, c });
				if (piece)
				{
					grid[r][c].occupied = true;
					grid[r][c].type = piece->GetType();
					grid[r][c].color = piece->GetColor();
					grid[r][c].moved = piece->HasMoved();
				}
			}
		}
		return grid;
	}

	bool Inside(int r, int c)
	{
		return r >= 0 && r < Board::SIZE && c >= 0 && c < Board::SIZE;
	}

	constexpr std::array<std::pair<int, int>, 4> kRookDirs{ { { 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 } } };
	constexpr std::array<std::pair<int, int>, 4> kBishopDirs{ { { 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } } };
	constexpr std::array<std::pair<int, int>, 8> kKnightJumps{ { { 2, 1 },{ 2, -1 },{ -2, 1 },{ -2, -1 },
																  { 1, 2 },{ 1, -2 },{ -1, 2 },{ -1, -2 } } };
	constexpr std::array<std::pair<int, int>, 8> kKingSteps{ { { 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 },
															   { 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } } };

	bool IsAttacked(const Grid& g, int r, int c, EColor by)
	{
		const int dir = PawnDir(by);
		for (int dc : { -1, 1 })
		{
			const int pr = r - dir;
			const int pc = c + dc;
			if (Inside(pr, pc) && g[pr][pc].occupied
				&& g[pr][pc].color == by && g[pr][pc].type == EPieceType::Pawn)
				return true;
		}

		for (auto [dr, dc] : kKnightJumps)
		{
			const int nr = r + dr, nc = c + dc;
			if (Inside(nr, nc) && g[nr][nc].occupied
				&& g[nr][nc].color == by && g[nr][nc].type == EPieceType::Knight)
				return true;
		}

		for (auto [dr, dc] : kKingSteps)
		{
			const int nr = r + dr, nc = c + dc;
			if (Inside(nr, nc) && g[nr][nc].occupied
				&& g[nr][nc].color == by && g[nr][nc].type == EPieceType::King)
				return true;
		}

		for (auto [dr, dc] : kRookDirs)
		{
			int nr = r + dr, nc = c + dc;
			while (Inside(nr, nc))
			{
				if (g[nr][nc].occupied)
				{
					if (g[nr][nc].color == by
						&& (g[nr][nc].type == EPieceType::Rook || g[nr][nc].type == EPieceType::Queen))
						return true;
					break;
				}
				nr += dr;
				nc += dc;
			}
		}

		for (auto [dr, dc] : kBishopDirs)
		{
			int nr = r + dr, nc = c + dc;
			while (Inside(nr, nc))
			{
				if (g[nr][nc].occupied)
				{
					if (g[nr][nc].color == by
						&& (g[nr][nc].type == EPieceType::Bishop || g[nr][nc].type == EPieceType::Queen))
						return true;
					break;
				}
				nr += dr;
				nc += dc;
			}
		}

		return false;
	}

	bool FindKing(const Grid& g, EColor color, int& kr, int& kc)
	{
		for (int r = 0; r < Board::SIZE; ++r)
			for (int c = 0; c < Board::SIZE; ++c)
				if (g[r][c].occupied && g[r][c].color == color && g[r][c].type == EPieceType::King)
				{
					kr = r;
					kc = c;
					return true;
				}
		return false;
	}

	bool IsKingInCheck(const Grid& g, EColor color)
	{
		int kr = 0, kc = 0;
		if (!FindKing(g, color, kr, kc))
			return false;
		return IsAttacked(g, kr, kc, Opposite(color));
	}

	void AddSliding(const Grid& g, int r, int c, EColor color, int dr, int dc, PositionList& out)
	{
		int nr = r + dr, nc = c + dc;
		while (Inside(nr, nc))
		{
			if (g[nr][nc].occupied)
			{
				if (g[nr][nc].color != color)
					out.push_back(Position{ nr, nc });
				break;
			}
			out.push_back(Position{ nr, nc });
			nr += dr;
			nc += dc;
		}
	}

	void AddStep(const Grid& g, int nr, int nc, EColor color, PositionList& out)
	{
		if (!Inside(nr, nc))
			return;
		if (!g[nr][nc].occupied || g[nr][nc].color != color)
			out.push_back(Position{ nr, nc });
	}

	bool IsCastlingRook(const Grid& g, int r, int c, EColor color)
	{
		return g[r][c].occupied && g[r][c].type == EPieceType::Rook
			&& g[r][c].color == color && !g[r][c].moved;
	}

	void AddCastling(const Grid& g, int r, int c, EColor color, PositionList& out)
	{
		if (g[r][c].moved || r != BackRow(color) || c != 4)
			return;

		const EColor enemy = Opposite(color);
		if (IsAttacked(g, r, c, enemy))
			return;

		if (IsCastlingRook(g, r, 7, color)
			&& !g[r][5].occupied && !g[r][6].occupied
			&& !IsAttacked(g, r, 5, enemy))
			out.push_back(Position{ r, 6 });

		if (IsCastlingRook(g, r, 0, color)
			&& !g[r][1].occupied && !g[r][2].occupied && !g[r][3].occupied
			&& !IsAttacked(g, r, 3, enemy))
			out.push_back(Position{ r, 2 });
	}

	PositionList PseudoLegalMoves(const Grid& g, int r, int c, const std::optional<Position>& enPassant)
	{
		PositionList moves;
		const Cell& cell = g[r][c];
		if (!cell.occupied)
			return moves;

		const EColor color = cell.color;

		switch (cell.type)
		{
		case EPieceType::Pawn:
		{
			const int dir = PawnDir(color);
			const int oneR = r + dir;
			if (Inside(oneR, c) && !g[oneR][c].occupied)
			{
				moves.push_back(Position{ oneR, c });
				const int twoR = r + 2 * dir;
				if (r == PawnStartRow(color) && Inside(twoR, c) && !g[twoR][c].occupied)
					moves.push_back(Position{ twoR, c });
			}
			for (int dc : { -1, 1 })
			{
				const int nc = c + dc;
				if (!Inside(oneR, nc))
					continue;
				if (g[oneR][nc].occupied)
				{
					if (g[oneR][nc].color != color)
						moves.push_back(Position{ oneR, nc });
				}
				else if (enPassant && enPassant->row == oneR && enPassant->col == nc
					&& g[r][nc].occupied && g[r][nc].type == EPieceType::Pawn && g[r][nc].color != color)
				{
					moves.push_back(Position{ oneR, nc });
				}
			}
			break;
		}
		case EPieceType::Knight:
			for (auto [dr, dc] : kKnightJumps)
				AddStep(g, r + dr, c + dc, color, moves);
			break;
		case EPieceType::Bishop:
			for (auto [dr, dc] : kBishopDirs)
				AddSliding(g, r, c, color, dr, dc, moves);
			break;
		case EPieceType::Rook:
			for (auto [dr, dc] : kRookDirs)
				AddSliding(g, r, c, color, dr, dc, moves);
			break;
		case EPieceType::Queen:
			for (auto [dr, dc] : kRookDirs)
				AddSliding(g, r, c, color, dr, dc, moves);
			for (auto [dr, dc] : kBishopDirs)
				AddSliding(g, r, c, color, dr, dc, moves);
			break;
		case EPieceType::King:
			for (auto [dr, dc] : kKingSteps)
				AddStep(g, r + dr, c + dc, color, moves);
			AddCastling(g, r, c, color, moves);
			break;
		}
		return moves;
	}

	void ApplyMove(Grid& g, Position from, Position to)
	{
		Cell moving = g[from.row][from.col];

		if (moving.type == EPieceType::Pawn && from.col != to.col && !g[to.row][to.col].occupied)
			g[from.row][to.col] = Cell{};

		if (moving.type == EPieceType::King && std::abs(to.col - from.col) == 2)
		{
			const int rookFrom = to.col > from.col ? Board::SIZE - 1 : 0;
			const int rookTo = to.col > from.col ? 5 : 3;
			g[from.row][rookTo] = g[from.row][rookFrom];
			g[from.row][rookFrom] = Cell{};
		}

		moving.moved = true;
		g[to.row][to.col] = moving;
		g[from.row][from.col] = Cell{};
	}

	PositionList LegalMoves(const Grid& g, int r, int c, const std::optional<Position>& enPassant)
	{
		PositionList result;
		if (!g[r][c].occupied)
			return result;

		const EColor color = g[r][c].color;
		for (const Position& to : PseudoLegalMoves(g, r, c, enPassant))
		{
			Grid copy = g;
			ApplyMove(copy, Position{ r, c }, to);
			if (!IsKingInCheck(copy, color))
				result.push_back(to);
		}
		return result;
	}
}

bool Board::IsInside(Position position)
{
	return position.row >= 0 && position.row < SIZE
		&& position.col >= 0 && position.col < SIZE;
}

void Board::Reset()
{
	for (auto& row : m_squares)
		row.fill(nullptr);

	m_enPassantTarget.reset();

	constexpr std::array<EPieceType, SIZE> backRow{
		EPieceType::Rook, EPieceType::Knight, EPieceType::Bishop, EPieceType::Queen,
		EPieceType::King, EPieceType::Bishop, EPieceType::Knight, EPieceType::Rook };

	for (int c = 0; c < SIZE; ++c)
	{
		m_squares[0][c] = Piece::Produce(backRow[c], EColor::White);
		m_squares[1][c] = Piece::Produce(EPieceType::Pawn, EColor::White);
		m_squares[SIZE - 2][c] = Piece::Produce(EPieceType::Pawn, EColor::Black);
		m_squares[SIZE - 1][c] = Piece::Produce(backRow[c], EColor::Black);
	}
}

PiecePtr Board::GetPiece(Position position) const
{
	if (!IsInside(position))
		throw OutOfBoundsException("Pozitie in afara tablei.");
	return m_squares[position.row][position.col];
}

bool Board::IsEmpty(Position position) const
{
	return GetPiece(position) == nullptr;
}

void Board::MovePiece(Position from, Position to)
{
	if (!IsInside(from) || !IsInside(to))
		throw OutOfBoundsException("Pozitie in afara tablei.");
	if (from == to)
		return;

	PiecePtr piece = m_squares[from.row][from.col];
	if (!piece)
		return;

	const EPieceType type = piece->GetType();

	if (type == EPieceType::Pawn && from.col != to.col && m_squares[to.row][to.col] == nullptr)
		m_squares[from.row][to.col] = nullptr;

	if (type == EPieceType::King && std::abs(to.col - from.col) == 2)
	{
		const int rookFrom = to.col > from.col ? SIZE - 1 : 0;
		const int rookTo = to.col > from.col ? 5 : 3;
		PiecePtr rook = m_squares[from.row][rookFrom];
		if (rook)
		{
			m_squares[from.row][rookTo] = rook;
			m_squares[from.row][rookFrom] = nullptr;
			rook->MarkMoved();
		}
	}

	m_enPassantTarget.reset();
	if (type == EPieceType::Pawn && std::abs(to.row - from.row) == 2)
		m_enPassantTarget = Position{ (from.row + to.row) / 2, from.col };

	m_squares[to.row][to.col] = piece;
	m_squares[from.row][from.col] = nullptr;
	piece->MarkMoved();
}

void Board::ReplacePiece(Position position, PiecePtr piece)
{
	if (!IsInside(position))
		throw OutOfBoundsException("Pozitie in afara tablei.");
	m_squares[position.row][position.col] = piece;
}

PositionList Board::GetLegalMoves(Position from) const
{
	if (!IsInside(from) || IsEmpty(from))
		return {};

	const Grid grid = BuildGrid(*this);
	return LegalMoves(grid, from.row, from.col, m_enPassantTarget);
}

bool Board::IsInCheck(EColor color) const
{
	return IsKingInCheck(BuildGrid(*this), color);
}

bool Board::HasAnyLegalMove(EColor color) const
{
	const Grid grid = BuildGrid(*this);
	for (int r = 0; r < SIZE; ++r)
		for (int c = 0; c < SIZE; ++c)
			if (grid[r][c].occupied && grid[r][c].color == color
				&& !LegalMoves(grid, r, c, m_enPassantTarget).empty())
				return true;
	return false;
}