#pragma once

#include "BoardView.h"
#include "IGame.h"
#include "UIGameListener.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>
#include <string>

class App
{
public:
	explicit App(IGamePtr game);

	void Run();

private:
	void HandleEvents();
	void HandleLeftClick(sf::Vector2i pixel);
	void HandleKeyPress(sf::Keyboard::Key key);

	void SelectOwnPiece(Position position);
	void ClearSelection();
	void TryMove(Position from, Position to);
	void TryPromote(EPieceType type);

	void Render();
	void DrawStatusBar();
	std::string BuildStatusText() const;

	// Declaration order matters: members are initialized top to bottom.
	IGamePtr m_game;
	std::shared_ptr<UIGameListener> m_listener; // IGame keeps only a weak_ptr, so we must own it.
	sf::RenderWindow m_window;
	sf::Font m_font;
	BoardView m_boardView;

	std::optional<Position> m_selected;
	PositionList m_possibleMoves;
	std::string m_errorMessage;
};