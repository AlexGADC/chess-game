#pragma once

#include "BoardView.h"
#include "Button.h"
#include "IGame.h"
#include "MenuScreen.h"
#include "PlayerBarView.h"
#include "TiledBackground.h"
#include "UIGameListener.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>
#include <string>

class App // : public IGameListener
{
public:
	explicit App(IGamePtr game);

	void Run();

private:
	enum class EScreen
	{
		MainMenu,
		TimeControl,
		Settings,
		Playing
	};

	// (Re)creates the window in the current mode (windowed or fullscreen).
	void OpenWindow();
	// Centers the fixed logical layout in the window at the largest scale that fits,
	// and makes the background fill whatever is left around it.
	void ApplyLayoutView();
	void ToggleFullscreen();
	// Converts a window pixel to a point in the logical layout that all screens are drawn in.
	sf::Vector2i ToLogicalPoint(sf::Vector2i pixel) const;

	void HandleEvents();
	void HandleLeftClick(sf::Vector2i point);
	void HandleKeyPress(sf::Keyboard::Key key);

	// Returns nullptr when the current screen is not a menu.
	MenuScreen* GetActiveMenu();
	void ApplyMenuAction(EMenuAction action);

	void StartGame(int minutes);
	void RestartGame();
	void ExitToMainMenu();

	void HandleBoardClick(sf::Vector2i point);
	void HandleGameKeyPress(sf::Keyboard::Key key);
	void SelectOwnPiece(Position position);
	void ClearSelection();
	void TryMove(Position from, Position to);
	void TryPromote(EPieceType type);

	void Render();
	void RenderGame(sf::Vector2i mousePoint);
	void DrawStatusBar();
	std::string BuildStatusText() const;

	// Declaration order matters: members are initialized top to bottom.
	IGamePtr m_game;
	std::shared_ptr<UIGameListener> m_listener; // IGame keeps only a weak_ptr, so we must own it.
	sf::RenderWindow m_window;
	sf::Font m_symbolFont; // chess piece glyphs and in-game text
	sf::Font m_menuFont;
	TiledBackground m_background;
	BoardView m_boardView;
	PlayerBarView m_blackBar;
	PlayerBarView m_whiteBar;
	Button m_exitButton;
	MenuScreen m_mainMenu;
	MenuScreen m_timeControlMenu;
	MenuScreen m_settingsMenu;

	EScreen m_screen = EScreen::MainMenu;
	bool m_isFullscreen = false;
	int m_timeControlSeconds = 0;
	std::optional<Position> m_selected;
	PositionList m_possibleMoves;
	std::string m_errorMessage;
};
