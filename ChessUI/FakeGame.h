#pragma once

#include "IGame.h"

#include <array>
#include <functional>
#include <vector>

// TEMPORARY stand-in for the real backend, used only to test the UI.
// It accepts any move to a square not occupied by a piece of the same color.
// Delete it (and use IGame::Produce()) once ChessLogic works.
class FakeGame : public IGame
{
public:
	FakeGame();

	void AddListener(IGameListenerWeakPtr listener) override;
	void RemoveListener(IGameListener* listener) override;

	void MakeMove(Position from, Position to) override;
	void PromotePawn(EPieceType newType) override;
	void Restart() override;

	IPieceInfoPtr GetPieceInfo(Position position) const override;
	PositionList GetPossibleMoves(Position from) const override;

	EColor GetCurrentPlayer() const override;
	EGameState GetState() const override;
	bool IsGameOver() const override;

private:
	static constexpr int SIZE = 8;

	static bool IsInside(Position position);

	void SetupStartingPosition();
	void SwitchPlayer();
	void NotifyListeners(const std::function<void(IGameListener&)>& notification) const;

	std::array<std::array<IPieceInfoPtr, SIZE>, SIZE> m_squares;
	EColor m_currentPlayer = EColor::White;
	EGameState m_state = EGameState::Playing;
	Position m_promotionSquare;
	std::vector<IGameListenerWeakPtr> m_listeners;
};