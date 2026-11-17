#pragma once

#include "BoardView.h"
#include "IGame.h"
#include "MenuScreen.h"
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
	enum class EScreen
	{
		MainMenu,
		Settings,
		Playing
	};

	void HandleEvents();
	void HandleLeftClick(sf::Vector2i pixel);
	void HandleKeyPress(sf::Keyboard::Key key);

	void ApplyMenuAction(EMenuAction action);

	void HandleBoardClick(sf::Vector2i pixel);
	void HandleGameKeyPress(sf::Keyboard::Key key);
	void SelectOwnPiece(Position position);
	void ClearSelection();
	void TryMove(Position from, Position to);
	void TryPromote(EPieceType type);

	void Render();
	void RenderGame();
	void DrawStatusBar();
	std::string BuildStatusText() const;

	// Declaration order matters: members are initialized top to bottom.
	IGamePtr m_game;
	std::shared_ptr<UIGameListener> m_listener; // IGame keeps only a weak_ptr, so we must own it.
	sf::RenderWindow m_window;
	sf::Font m_symbolFont; // chess piece glyphs and in-game text
	sf::Font m_menuFont;
	BoardView m_boardView;
	MenuScreen m_mainMenu;
	MenuScreen m_settingsMenu;

	EScreen m_screen = EScreen::MainMenu;
	std::optional<Position> m_selected;
	PositionList m_possibleMoves;
	std::string m_errorMessage;
};
