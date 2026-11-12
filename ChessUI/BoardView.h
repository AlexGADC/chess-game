
#pragma once

#include "IGame.h"

#include <SFML/Graphics.hpp>

#include <optional>
#include <utility>

// Draws the board, pieces and highlights, and converts pixels to board positions.
// It only reads state through IGame; it never changes the game.
class BoardView
{
public:
	static constexpr float SQUARE_SIZE = 100.f;

	explicit BoardView(const sf::Font& font);

	void SetSelection(std::optional<Position> selected, PositionList possibleMoves);
	void SetLastMove(std::optional<std::pair<Position, Position>> lastMove);

	void Draw(sf::RenderTarget& target, const IGame& game) const;

	// Returns nullopt when the pixel is outside the board.
	std::optional<Position> PixelToPosition(sf::Vector2i pixel) const;

private:
	// row 0 is the white base rank, so it is drawn at the bottom of the screen.
	static sf::Vector2f PositionToPixel(Position position);
	static const char32_t* GetGlyph(EPieceType type);

	void DrawSquare(sf::RenderTarget& target, Position position, sf::Color color) const;
	void DrawPiece(sf::RenderTarget& target, Position position, const IPieceInfo& piece) const;
	void DrawMoveHint(sf::RenderTarget& target, Position position) const;
	std::optional<Position> FindKing(const IGame& game, EColor color) const;

	const sf::Font& m_font;
	std::optional<Position> m_selected;
	PositionList m_possibleMoves;
	std::optional<std::pair<Position, Position>> m_lastMove;
};