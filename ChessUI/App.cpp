#include "App.h"

#include "ChessExceptions.h"
#include "FontLoader.h"
#include "UIDrawing.h"

#include <algorithm>
#include <cmath>

namespace
{
	// Every screen is laid out in this fixed logical space; a view scales and centers it in the real window.
	constexpr float BOARD_PIXELS = 800.f;
	constexpr float PLAYER_BAR_HEIGHT = 50.f;
	constexpr float STATUS_BAR_HEIGHT = 60.f;

	constexpr float BOARD_TOP = PLAYER_BAR_HEIGHT;
	constexpr float WHITE_BAR_TOP = BOARD_TOP + BOARD_PIXELS;
	constexpr float STATUS_BAR_TOP = WHITE_BAR_TOP + PLAYER_BAR_HEIGHT;

	const sf::Vector2f LOGICAL_SIZE(BOARD_PIXELS, STATUS_BAR_TOP + STATUS_BAR_HEIGHT);
	const sf::Vector2f PLAYER_BAR_SIZE(BOARD_PIXELS, PLAYER_BAR_HEIGHT);
	const sf::Vector2f STATUS_BAR_SIZE(BOARD_PIXELS, STATUS_BAR_HEIGHT);

	constexpr float STATUS_TEXT_LEFT = 20.f;
	constexpr float STATUS_TEXT_TOP_MARGIN = 12.f;
	constexpr float EXIT_BUTTON_MARGIN = 8.f;
	const sf::Vector2f EXIT_BUTTON_SIZE(200.f, STATUS_BAR_HEIGHT - 2.f * EXIT_BUTTON_MARGIN);
	const sf::Vector2f EXIT_BUTTON_POSITION(
		BOARD_PIXELS - EXIT_BUTTON_SIZE.x - EXIT_BUTTON_MARGIN,
		STATUS_BAR_TOP + EXIT_BUTTON_MARGIN);

	// Room left for the taskbar and the title bar when the window is not fullscreen.
	constexpr float DESKTOP_VERTICAL_MARGIN = 120.f;

	constexpr unsigned int STATUS_FONT_SIZE = 28;
	constexpr unsigned int FRAMERATE_LIMIT = 60;

	constexpr int BULLET_MINUTES = 3;
	constexpr int BLITZ_MINUTES = 5;
	constexpr int RAPID_MINUTES = 10;
	constexpr int SECONDS_PER_MINUTE = 60;

	const char* WINDOW_TITLE = "Chess";
	const sf::Color STATUS_BAR_COLOR(40, 40, 40);

	// Both fonts ship with Windows. Segoe UI Symbol has the chess glyphs.
	const char* SYMBOL_FONT_PATH = "C:/Windows/Fonts/seguisym.ttf";
	const char* MENU_FONT_PATH = "C:/Windows/Fonts/consolab.ttf";

	// ("Bullet", 3) -> "Bullet - 3 min"
	std::string MakeTimeControlLabel(const std::string& name, int minutes)
	{
		return name + " - " + std::to_string(minutes) + " min";
	}

	// The logical size, shrunk if needed so the window fits on a small desktop.
	sf::Vector2u GetWindowedSize()
	{
		const float availableHeight = sf::VideoMode::getDesktopMode().size.y - DESKTOP_VERTICAL_MARGIN;
		const float scale = std::min(1.f, availableHeight / LOGICAL_SIZE.y);

		return sf::Vector2u(LOGICAL_SIZE * scale);
	}
}

App::App(IGamePtr game)
	: m_game(std::move(game))
	, m_listener(std::make_shared<UIGameListener>())
	, m_symbolFont(LoadFontOrThrow(SYMBOL_FONT_PATH))
	, m_menuFont(LoadFontOrThrow(MENU_FONT_PATH))
	, m_boardView(m_symbolFont, { 0.f, BOARD_TOP })
	, m_blackBar(m_menuFont, "Black", { 0.f, 0.f }, PLAYER_BAR_SIZE)
	, m_whiteBar(m_menuFont, "White", { 0.f, WHITE_BAR_TOP }, PLAYER_BAR_SIZE)
	, m_exitButton(m_menuFont, "Exit Game", EXIT_BUTTON_POSITION, EXIT_BUTTON_SIZE)
	, m_mainMenu(
		m_menuFont,
		"Welcome to our chess game",
		{ { "Play", EMenuAction::Play }, { "Settings", EMenuAction::Settings } },
		sf::Vector2u(LOGICAL_SIZE))
	, m_timeControlMenu(
		m_menuFont,
		"Choose time control",
		{
			{ MakeTimeControlLabel("Bullet", BULLET_MINUTES), EMenuAction::PlayBullet },
			{ MakeTimeControlLabel("Blitz", BLITZ_MINUTES), EMenuAction::PlayBlitz },
			{ MakeTimeControlLabel("Rapid", RAPID_MINUTES), EMenuAction::PlayRapid },
			{ "Back", EMenuAction::Back }
		},
		sf::Vector2u(LOGICAL_SIZE))
	, m_settingsMenu(
		m_menuFont,
		"Settings",
		{ { "Fullscreen (F11)", EMenuAction::ToggleFullscreen }, { "Back", EMenuAction::Back } },
		sf::Vector2u(LOGICAL_SIZE))
{
	m_game->AddListener(m_listener);
	OpenWindow();
}

void App::Run()
{
	while (m_window.isOpen())
	{
		HandleEvents();
		Render();
	}
}

void App::OpenWindow()
{
	if (m_isFullscreen)
	{
		m_window.create(sf::VideoMode::getDesktopMode(), WINDOW_TITLE, sf::State::Fullscreen);
	}
	else
	{
		m_window.create(
			sf::VideoMode(GetWindowedSize()),
			WINDOW_TITLE,
			sf::Style::Titlebar | sf::Style::Close,
			sf::State::Windowed);
	}

	// A newly created window loses these settings.
	m_window.setFramerateLimit(FRAMERATE_LIMIT);
	ApplyLayoutView();
}

void App::ApplyLayoutView()
{
	const sf::Vector2f windowSize(m_window.getSize());
	const float windowRatio = windowSize.x / windowSize.y;
	const float logicalRatio = LOGICAL_SIZE.x / LOGICAL_SIZE.y;

	// The view shows the whole logical layout plus the extra space of a wider (or taller) window.
	sf::Vector2f visibleSize = LOGICAL_SIZE;
	if (windowRatio > logicalRatio)
	{
		visibleSize.x = LOGICAL_SIZE.y * windowRatio;
	}
	else
	{
		visibleSize.y = LOGICAL_SIZE.x / windowRatio;
	}

	const sf::Vector2f layoutCenter = LOGICAL_SIZE / 2.f;
	m_window.setView(sf::View(layoutCenter, visibleSize));
	m_background.Cover(sf::FloatRect(layoutCenter - visibleSize / 2.f, visibleSize));
}

void App::ToggleFullscreen()
{
	m_isFullscreen = !m_isFullscreen;
	OpenWindow();
}

sf::Vector2i App::ToLogicalPoint(sf::Vector2i pixel) const
{
	const sf::Vector2f point = m_window.mapPixelToCoords(pixel);
	return { static_cast<int>(std::floor(point.x)), static_cast<int>(std::floor(point.y)) };
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
				HandleLeftClick(ToLogicalPoint(click->position));
			}
		}
		else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
		{
			HandleKeyPress(key->code);
		}
	}
}

void App::HandleLeftClick(sf::Vector2i point)
{
	if (const MenuScreen* menu = GetActiveMenu())
	{
		ApplyMenuAction(menu->GetClickedAction(point));
	}
	else if (m_exitButton.Contains(point))
	{
		ExitToMainMenu();
	}
	else
	{
		HandleBoardClick(point);
	}
}

void App::HandleKeyPress(sf::Keyboard::Key key)
{
	if (key == sf::Keyboard::Key::F11)
	{
		ToggleFullscreen();
		return;
	}

	if (key == sf::Keyboard::Key::Escape)
	{
		ExitToMainMenu();
		return;
	}

	if (m_screen == EScreen::Playing)
	{
		HandleGameKeyPress(key);
	}
}

MenuScreen* App::GetActiveMenu()
{
	switch (m_screen)
	{
	case EScreen::MainMenu:    return &m_mainMenu;
	case EScreen::TimeControl: return &m_timeControlMenu;
	case EScreen::Settings:    return &m_settingsMenu;
	case EScreen::Playing:     return nullptr;
	}
	return nullptr;
}

void App::ApplyMenuAction(EMenuAction action)
{
	switch (action)
	{
	case EMenuAction::Play:
		m_screen = EScreen::TimeControl;
		break;
	case EMenuAction::PlayBullet:
		StartGame(BULLET_MINUTES);
		break;
	case EMenuAction::PlayBlitz:
		StartGame(BLITZ_MINUTES);
		break;
	case EMenuAction::PlayRapid:
		StartGame(RAPID_MINUTES);
		break;
	case EMenuAction::Settings:
		m_screen = EScreen::Settings;
		break;
	case EMenuAction::ToggleFullscreen:
		ToggleFullscreen();
		break;
	case EMenuAction::Back:
		m_screen = EScreen::MainMenu;
		break;
	case EMenuAction::None:
		break;
	}
}

void App::StartGame(int minutes)
{
	RestartGame();
	m_timeControlSeconds = minutes * SECONDS_PER_MINUTE;
	m_screen = EScreen::Playing;
}

void App::RestartGame()
{
	m_game->Restart();
	m_errorMessage.clear();
	ClearSelection();
}

void App::ExitToMainMenu()
{
	m_errorMessage.clear();
	ClearSelection();
	m_screen = EScreen::MainMenu;
}

void App::HandleBoardClick(sf::Vector2i point)
{
	m_errorMessage.clear();

	const EGameState state = m_game->GetState();
	if (m_game->IsGameOver() || state == EGameState::WaitingForPromotion)
	{
		return;
	}

	const std::optional<Position> clicked = m_boardView.PixelToPosition(point);
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
		RestartGame();
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
	m_window.clear();
	m_background.Draw(m_window);

	const sf::Vector2i mousePoint = ToLogicalPoint(sf::Mouse::getPosition(m_window));

	if (MenuScreen* menu = GetActiveMenu())
	{
		menu->UpdateHover(mousePoint);
		menu->Draw(m_window);
	}
	else
	{
		RenderGame(mousePoint);
	}

	m_window.display();
}

void App::RenderGame(sf::Vector2i mousePoint)
{
	// The clocks show the chosen time control; counting down belongs to the game logic.
	const bool isGameRunning = !m_game->IsGameOver();
	const EColor currentPlayer = m_game->GetCurrentPlayer();
	m_blackBar.Draw(m_window, m_timeControlSeconds, isGameRunning && currentPlayer == EColor::Black);
	m_whiteBar.Draw(m_window, m_timeControlSeconds, isGameRunning && currentPlayer == EColor::White);

	m_boardView.SetLastMove(m_listener->GetLastMove());
	m_boardView.Draw(m_window, *m_game);

	DrawStatusBar();
	m_exitButton.UpdateHover(mousePoint);
	m_exitButton.Draw(m_window);
}

void App::DrawStatusBar()
{
	DrawRectangle(m_window, { 0.f, STATUS_BAR_TOP }, STATUS_BAR_SIZE, STATUS_BAR_COLOR);

	sf::Text text(m_symbolFont, BuildStatusText(), STATUS_FONT_SIZE);
	text.setFillColor(sf::Color::White);
	text.setPosition({ STATUS_TEXT_LEFT, STATUS_BAR_TOP + STATUS_TEXT_TOP_MARGIN });
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
	case EGameState::WaitingForPromotion: return "Promote pawn: Q, R, B or N";
	case EGameState::Checkmate:           return "Checkmate. Press F5 to restart";
	case EGameState::Stalemate:           return "Stalemate. Press F5 to restart";
	case EGameState::Draw:                return "Draw. Press F5 to restart";
	}
	return {};
}
