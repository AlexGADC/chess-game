#include "BoardView.h"

namespace
{
	constexpr int BOARD_SIZE = 8;
	constexpr unsigned int PIECE_FONT_SIZE = 80;
	constexpr float MOVE_HINT_RADIUS = 14.f;

	const sf::Color LIGHT_SQUARE_COLOR(240, 217, 181);
	const sf::Color DARK_SQUARE_COLOR(181, 136, 99);
	const sf::Color LAST_MOVE_COLOR(205, 210, 106);
	const sf::Color SELECTED_COLOR(130, 151, 105);
	const sf::Color CHECK_COLOR(220, 70, 70);
	const sf::Color MOVE_HINT_COLOR(20, 85, 30, 130);
	const sf::Color WHITE_PIECE_COLOR(255, 255, 255);
	const sf::Color BLACK_PIECE_COLOR(30, 30, 30);
}

BoardView::BoardView(const sf::Font& font)
	: m_font(font)
{
}

void BoardView::SetSelection(std::optional<Position> selected, PositionList possibleMoves)
{
	m_selected = selected;
	m_possibleMoves = std::move(possibleMoves);
}

void BoardView::SetLastMove(std::optional<std::pair<Position, Position>> lastMove)
{
	m_lastMove = lastMove;
}

void BoardView::Draw(sf::RenderTarget& target, const IGame& game) const
{
	for (int row = 0; row < BOARD_SIZE; ++row)
	{
		for (int col = 0; col < BOARD_SIZE; ++col)
		{
			const bool isLightSquare = (row + col) % 2 == 1;
			DrawSquare(target, Position{ row, col }, isLightSquare ? LIGHT_SQUARE_COLOR : DARK_SQUARE_COLOR);
		}
	}

	if (m_lastMove)
	{
		DrawSquare(target, m_lastMove->first, LAST_MOVE_COLOR);
		DrawSquare(target, m_lastMove->second, LAST_MOVE_COLOR);
	}

	if (m_selected)
	{
		DrawSquare(target, *m_selected, SELECTED_COLOR);
	}

	const EGameState state = game.GetState();
	if (state == EGameState::Check || state == EGameState::Checkmate)
	{
		if (const auto king = FindKing(game, game.GetCurrentPlayer()))
		{
			DrawSquare(target, *king, CHECK_COLOR);
		}
	}

	for (int row = 0; row < BOARD_SIZE; ++row)
	{
		for (int col = 0; col < BOARD_SIZE; ++col)
		{
			const Position position{ row, col };
			if (const IPieceInfoPtr piece = game.GetPieceInfo(position))
			{
				DrawPiece(target, position, *piece);
			}
		}
	}

	for (const Position& move : m_possibleMoves)
	{
		DrawMoveHint(target, move);
	}
}

std::optional<Position> BoardView::PixelToPosition(sf::Vector2i pixel) const
{
	if (pixel.x < 0 || pixel.y < 0)
	{
		return std::nullopt;
	}

	const int col = static_cast<int>(pixel.x / SQUARE_SIZE);
	const int rowFromTop = static_cast<int>(pixel.y / SQUARE_SIZE);
	if (col >= BOARD_SIZE || rowFromTop >= BOARD_SIZE)
	{
		return std::nullopt;
	}

	return Position{ BOARD_SIZE - 1 - rowFromTop, col };
}

sf::Vector2f BoardView::PositionToPixel(Position position)
{
	return { position.col * SQUARE_SIZE, (BOARD_SIZE - 1 - position.row) * SQUARE_SIZE };
}

const char32_t* BoardView::GetGlyph(EPieceType type)
{
	// The filled glyphs are used for both colors; the color comes from the text fill.
	switch (type)
	{
	case EPieceType::King:   return U"\u265A";
	case EPieceType::Queen:  return U"\u265B";
	case EPieceType::Rook:   return U"\u265C";
	case EPieceType::Bishop: return U"\u265D";
	case EPieceType::Knight: return U"\u265E";
	case EPieceType::Pawn:   return U"\u265F";
	}
	return U"?";
}

void BoardView::DrawSquare(sf::RenderTarget& target, Position position, sf::Color color) const
{
	sf::RectangleShape square({ SQUARE_SIZE, SQUARE_SIZE });
	square.setPosition(PositionToPixel(position));
	square.setFillColor(color);
	target.draw(square);
}

void BoardView::DrawPiece(sf::RenderTarget& target, Position position, const IPieceInfo& piece) const
{
	const bool isWhite = piece.GetColor() == EColor::White;

	sf::Text text(m_font, sf::String(GetGlyph(piece.GetType())), PIECE_FONT_SIZE);
	text.setFillColor(isWhite ? WHITE_PIECE_COLOR : BLACK_PIECE_COLOR);
	text.setOutlineColor(isWhite ? BLACK_PIECE_COLOR : WHITE_PIECE_COLOR);
	text.setOutlineThickness(2.f);

	const sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.position + bounds.size / 2.f);
	text.setPosition(PositionToPixel(position) + sf::Vector2f(SQUARE_SIZE / 2.f, SQUARE_SIZE / 2.f));
	target.draw(text);
}

void BoardView::DrawMoveHint(sf::RenderTarget& target, Position position) const
{
	sf::CircleShape hint(MOVE_HINT_RADIUS);
	hint.setOrigin({ MOVE_HINT_RADIUS, MOVE_HINT_RADIUS });
	hint.setPosition(PositionToPixel(position) + sf::Vector2f(SQUARE_SIZE / 2.f, SQUARE_SIZE / 2.f));
	hint.setFillColor(MOVE_HINT_COLOR);
	target.draw(hint);
}

std::optional<Position> BoardView::FindKing(const IGame& game, EColor color) const
{
	for (int row = 0; row < BOARD_SIZE; ++row)
	{
		for (int col = 0; col < BOARD_SIZE; ++col)
		{
			const Position position{ row, col };
			const IPieceInfoPtr piece = game.GetPieceInfo(position);
			if (piece && piece->GetType() == EPieceType::King && piece->GetColor() == color)
			{
				return position;
			}
		}
	}
	return std::nullopt;
}