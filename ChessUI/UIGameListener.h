#pragma once
#pragma once

#include "IGameListener.h"

#include <optional>
#include <utility>

// Observer used by the UI. It only remembers what the UI cannot ask the game for later.
// If you already have your own listener, merge the last-move logic into it.
class UIGameListener : public IGameListener
{
public:
	using Move = std::pair<Position, Position>;

	void OnMoveMade(Position from, Position to) override
	{
		m_lastMove = Move{ from, to };
	}

	void OnPawnPromotionRequired(Position) override {}
	void OnCheck(EColor) override {}
	void OnGameOver(EGameState) override {}

	void OnGameRestarted() override
	{
		m_lastMove.reset();
	}

	const std::optional<Move>& GetLastMove() const
	{
		return m_lastMove;
	}

private:
	std::optional<Move> m_lastMove;
};