#include "App.h"

#include "ChessExceptions.h"

#include <algorithm>
#include <stdexcept>

namespace
{
	constexpr unsigned int BOARD_PIXELS = 800;
	constexpr unsigned int STATUS_BAR_HEIGHT = 60;
	constexpr unsigned int STATUS_FONT_SIZE = 28;
	constexpr unsigned int FRAMERATE_LIMIT = 60;

	// Segoe UI Symbol ships with Windows and contains the chess glyphs.
	// Replace with sprites later if you want custom piece art.
	const char* FONT_PATH = "C:/Windows/Fonts/seguisym.ttf";

	sf::Font LoadFont()
	{
		sf::Font font;
		if (!font.openFromFile(FONT_PATH))
		{
			throw std::runtime_error(std::string("Could not load font: ") + FONT_PATH);
		}
		return font;
	}
}

App::App(IGamePtr game)
	: m_game(std::move(game))
	, m_listener(std::make_shared<UIGameListener>())
	, m_window(sf::VideoMode({ BOARD_PIXELS, BOARD_PIXELS + STATUS_BAR_HEIGHT }), "Chess")
	, m_font(LoadFont())
	, m_boardView(m_font)
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

void App::HandleKeyPress(sf::Keyboard::Key key)
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

	m_boardView.SetLastMove(m_listener->GetLastMove());
	m_boardView.Draw(m_window, *m_game);
	DrawStatusBar();

	m_window.display();
}

void App::DrawStatusBar()
{
	sf::Text text(m_font, BuildStatusText(), STATUS_FONT_SIZE);
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