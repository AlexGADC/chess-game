#pragma once

#include "Board.h"
#include "IGame.h"

#include <vector>

class Game : public IGame
{
public:
	Game();

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
	Board m_board;
	EColor m_currentPlayer;
	EGameState m_state;
	std::vector<IGameListenerWeakPtr> m_listeners;
};
