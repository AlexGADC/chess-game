#include "App.h"

#include "ChessExceptions.h"
#include "FontLoader.h"

#include <algorithm>

namespace
{
	constexpr unsigned int BOARD_PIXELS = 800;
	constexpr unsigned int STATUS_BAR_HEIGHT = 60;
	constexpr unsigned int STATUS_FONT_SIZE = 28;
	constexpr unsigned int FRAMERATE_LIMIT = 60;

	// Both fonts ship with Windows. Segoe UI Symbol has the chess glyphs.
	const char* SYMBOL_FONT_PATH = "C:/Windows/Fonts/seguisym.ttf";
	const char* MENU_FONT_PATH = "C:/Windows/Fonts/consolab.ttf";
}

App::App(IGamePtr game)
	: m_game(std::move(game))
	, m_listener(std::make_shared<UIGameListener>())
	, m_window(
		sf::VideoMode({ BOARD_PIXELS, BOARD_PIXELS + STATUS_BAR_HEIGHT }),
		"Chess",
		sf::Style::Titlebar | sf::Style::Close) // fixed size, so mouse pixels always match the layout
	, m_symbolFont(LoadFontOrThrow(SYMBOL_FONT_PATH))
	, m_menuFont(LoadFontOrThrow(MENU_FONT_PATH))
	, m_boardView(m_symbolFont)
	, m_mainMenu(
		m_menuFont,
		"Welcome to our chess game",
		{ { "Play", EMenuAction::Play }, { "Settings", EMenuAction::Settings } },
		m_window.getSize())
	, m_settingsMenu(
		m_menuFont,
		"Settings",
		{ { "Back", EMenuAction::Back } },
		m_window.getSize())
{
	m_game->AddListener(m_listener);
	m_window.setFramerateLimit(FRAMERATE_LIMIT);
}

void App::Run()
{
	while (m_window.isOpen())
	{
		HandleEvents();
		Render();
	}
}

void App::HandleEvents()
{
	while (const std::optional event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			m_window.close();
		}
		else if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>())
		{
			if (click->button == sf::Mouse::Button::Left)
			{
				HandleLeftClick(click->position);
			}
		}
		else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
		{
			HandleKeyPress(key->code);
		}
	}
}

void App::HandleLeftClick(sf::Vector2i pixel)
{
	switch (m_screen)
	{
	case EScreen::MainMenu:
		ApplyMenuAction(m_mainMenu.GetClickedAction(pixel));
		break;
	case EScreen::Settings:
		ApplyMenuAction(m_settingsMenu.GetClickedAction(pixel));
		break;
	case EScreen::Playing:
		HandleBoardClick(pixel);
		break;
	}
}

void App::HandleKeyPress(sf::Keyboard::Key key)
{
	if (key == sf::Keyboard::Key::Escape)
	{
		m_screen = EScreen::MainMenu;
		return;
	}

	if (m_screen == EScreen::Playing)
	{
		HandleGameKeyPress(key);
	}
}

void App::ApplyMenuAction(EMenuAction action)
{
	switch (action)
	{
	case EMenuAction::Play:
		m_screen = EScreen::Playing;
		break;
	case EMenuAction::Settings:
		m_screen = EScreen::Settings;
		break;
	case EMenuAction::Back:
		m_screen = EScreen::MainMenu;
		break;
	case EMenuAction::None:
		break;
	}
}

void App::HandleBoardClick(sf::Vector2i pixel)
{
	m_errorMessage.clear();

	const EGameState state = m_game->GetState();
	if (m_game->IsGameOver() || state == EGameState::WaitingForPromotion)
	{
		return;
	}

	const std::optional<Position> clicked = m_boardView.PixelToPosition(pixel);
	if (!clicked)
	{
		return;
	}

	const bool isDestination = m_selected
		&& std::find(m_possibleMoves.begin(), m_possibleMoves.end(), *clicked) != m_possibleMoves.end();

	if (isDestination)
	{
		TryMove(*m_selected, *clicked);
		ClearSelection();
	}
	else
	{
		SelectOwnPiece(*clicked);
	}
}

void App::HandleGameKeyPress(sf::Keyboard::Key key)
{
	if (key == sf::Keyboard::Key::F5)
	{
		m_game->Restart();
		m_errorMessage.clear();
		ClearSelection();
		return;
	}

	if (m_game->GetState() != EGameState::WaitingForPromotion)
	{
		return;
	}

	switch (key)
	{
	case sf::Keyboard::Key::Q: TryPromote(EPieceType::Queen); break;
	case sf::Keyboard::Key::R: TryPromote(EPieceType::Rook); break;
	case sf::Keyboard::Key::B: TryPromote(EPieceType::Bishop); break;
	case sf::Keyboard::Key::N: TryPromote(EPieceType::Knight); break;
	default: break;
	}
}

void App::SelectOwnPiece(Position position)
{
	const IPieceInfoPtr piece = m_game->GetPieceInfo(position);
	if (piece && piece->GetColor() == m_game->GetCurrentPlayer())
	{
		m_selected = position;
		m_possibleMoves = m_game->GetPossibleMoves(position);
		m_boardView.SetSelection(m_selected, m_possibleMoves);
	}
	else
	{
		ClearSelection();
	}
}

void App::ClearSelection()
{
	m_selected.reset();
	m_possibleMoves.clear();
	m_boardView.SetSelection(std::nullopt, {});
}

void App::TryMove(Position from, Position to)
{
	try
	{
		m_game->MakeMove(from, to);
	}
	catch (const ChessException& exception)
	{
		m_errorMessage = exception.what();
	}
}

void App::TryPromote(EPieceType type)
{
	try
	{
		m_game->PromotePawn(type);
	}
	catch (const ChessException& exception)
	{
		m_errorMessage = exception.what();
	}
}

void App::Render()
{
	m_window.clear(sf::Color(40, 40, 40));

	const sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);

	switch (m_screen)
	{
	case EScreen::MainMenu:
		m_mainMenu.UpdateHover(mousePosition);
		m_mainMenu.Draw(m_window);
		break;
	case EScreen::Settings:
		m_settingsMenu.UpdateHover(mousePosition);
		m_settingsMenu.Draw(m_window);
		break;
	case EScreen::Playing:
		RenderGame();
		break;
	}

	m_window.display();
}

void App::RenderGame()
{
	m_boardView.SetLastMove(m_listener->GetLastMove());
	m_boardView.Draw(m_window, *m_game);
	DrawStatusBar();
}

void App::DrawStatusBar()
{
	sf::Text text(m_symbolFont, BuildStatusText(), STATUS_FONT_SIZE);
	text.setFillColor(sf::Color::White);
	text.setPosition({ 20.f, static_cast<float>(BOARD_PIXELS) + 12.f });
	m_window.draw(text);
}

std::string App::BuildStatusText() const
{
	if (!m_errorMessage.empty())
	{
		return m_errorMessage;
	}

	const std::string player = m_game->GetCurrentPlayer() == EColor::White ? "White" : "Black";

	switch (m_game->GetState())
	{
	case EGameState::Playing:             return player + " to move";
	case EGameState::Check:               return player + " is in check";
	case EGameState::WaitingForPromotion: return "Promote pawn: Q = queen, R = rook, B = bishop, N = knight";
	case EGameState::Checkmate:           return "Checkmate. Press F5 to restart";
	case EGameState::Stalemate:           return "Stalemate. Press F5 to restart";
	case EGameState::Draw:                return "Draw. Press F5 to restart";
	}
	return {};
}