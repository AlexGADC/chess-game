#include "FakeGame.h"

#include "ChessExceptions.h"

#include <algorithm>

namespace
{
	class FakePieceInfo : public IPieceInfo
	{
	public:
		FakePieceInfo(EPieceType type, EColor color)
			: m_type(type)
			, m_color(color)
		{
		}

		EPieceType GetType() const override { return m_type; }
		EColor GetColor() const override { return m_color; }

	private:
		EPieceType m_type;
		EColor m_color;
	};

	EColor Opposite(EColor color)
	{
		return color == EColor::White ? EColor::Black : EColor::White;
	}
}

FakeGame::FakeGame()
{
	SetupStartingPosition();
}

void FakeGame::AddListener(IGameListenerWeakPtr listener)
{
	m_listeners.push_back(std::move(listener));
}

void FakeGame::RemoveListener(IGameListener* listener)
{
	m_listeners.erase(
		std::remove_if(m_listeners.begin(), m_listeners.end(),
			[listener](const IGameListenerWeakPtr& weak)
			{
				const IGameListenerPtr locked = weak.lock();
				return !locked || locked.get() == listener;
			}),
		m_listeners.end());
}

void FakeGame::MakeMove(Position from, Position to)
{
	if (!IsInside(from) || !IsInside(to))
	{
		throw OutOfBoundsException("Position outside the board");
	}
	if (IsGameOver() || m_state == EGameState::WaitingForPromotion)
	{
		throw InvalidStateException("Moves are not allowed right now");
	}

	const PositionList moves = GetPossibleMoves(from);
	if (std::find(moves.begin(), moves.end(), to) == moves.end())
	{
		throw InvalidMoveException("Illegal move");
	}

	m_squares[to.row][to.col] = m_squares[from.row][from.col];
	m_squares[from.row][from.col] = nullptr;
	NotifyListeners([&](IGameListener& listener) { listener.OnMoveMade(from, to); });

	const IPieceInfoPtr& moved = m_squares[to.row][to.col];
	const int promotionRow = moved->GetColor() == EColor::White ? SIZE - 1 : 0;
	if (moved->GetType() == EPieceType::Pawn && to.row == promotionRow)
	{
		m_state = EGameState::WaitingForPromotion;
		m_promotionSquare = to;
		NotifyListeners([&](IGameListener& listener) { listener.OnPawnPromotionRequired(to); });
		return;
	}

	SwitchPlayer();
}

void FakeGame::PromotePawn(EPieceType newType)
{
	if (m_state != EGameState::WaitingForPromotion)
	{
		throw InvalidStateException("No pawn is waiting for promotion");
	}
	if (newType == EPieceType::King || newType == EPieceType::Pawn)
	{
		throw InvalidMoveException("A pawn cannot promote to this piece");
	}

	m_squares[m_promotionSquare.row][m_promotionSquare.col] =
		std::make_shared<FakePieceInfo>(newType, m_currentPlayer);
	SwitchPlayer();
}

void FakeGame::Restart()
{
	SetupStartingPosition();
	NotifyListeners([](IGameListener& listener) { listener.OnGameRestarted(); });
}

IPieceInfoPtr FakeGame::GetPieceInfo(Position position) const
{
	if (!IsInside(position))
	{
		throw OutOfBoundsException("Position outside the board");
	}
	return m_squares[position.row][position.col];
}

PositionList FakeGame::GetPossibleMoves(Position from) const
{
	PositionList moves;
	const IPieceInfoPtr piece = GetPieceInfo(from);
	if (!piece || piece->GetColor() != m_currentPlayer)
	{
		return moves;
	}

	for (int row = 0; row < SIZE; ++row)
	{
		for (int col = 0; col < SIZE; ++col)
		{
			const IPieceInfoPtr target = m_squares[row][col];
			const bool isFree = !target || target->GetColor() != m_currentPlayer;
			if (isFree && Position{ row, col } != from)
			{
				moves.push_back({ row, col });
			}
		}
	}
	return moves;
}

EColor FakeGame::GetCurrentPlayer() const
{
	return m_currentPlayer;
}

EGameState FakeGame::GetState() const
{
	return m_state;
}

bool FakeGame::IsGameOver() const
{
	return m_state == EGameState::Checkmate
		|| m_state == EGameState::Stalemate
		|| m_state == EGameState::Draw;
}

bool FakeGame::IsInside(Position position)
{
	return position.row >= 0 && position.row < SIZE && position.col >= 0 && position.col < SIZE;
}

void FakeGame::SetupStartingPosition()
{
	m_squares = {};

	const std::array<EPieceType, SIZE> backRank = {
		EPieceType::Rook, EPieceType::Knight, EPieceType::Bishop, EPieceType::Queen,
		EPieceType::King, EPieceType::Bishop, EPieceType::Knight, EPieceType::Rook
	};

	for (int col = 0; col < SIZE; ++col)
	{
		m_squares[0][col] = std::make_shared<FakePieceInfo>(backRank[col], EColor::White);
		m_squares[1][col] = std::make_shared<FakePieceInfo>(EPieceType::Pawn, EColor::White);
		m_squares[6][col] = std::make_shared<FakePieceInfo>(EPieceType::Pawn, EColor::Black);
		m_squares[7][col] = std::make_shared<FakePieceInfo>(backRank[col], EColor::Black);
	}

	m_currentPlayer = EColor::White;
	m_state = EGameState::Playing;
}

void FakeGame::SwitchPlayer()
{
	m_currentPlayer = Opposite(m_currentPlayer);
	m_state = EGameState::Playing;
}

void FakeGame::NotifyListeners(const std::function<void(IGameListener&)>& notification) const
{
	for (const IGameListenerWeakPtr& weak : m_listeners)
	{
		if (const IGameListenerPtr listener = weak.lock())
		{
			notification(*listener);
		}
	}
}