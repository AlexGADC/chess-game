#include "Game.h"

IGamePtr IGame::Produce()
{
	return std::make_shared<Game>();
}

Game::Game()
	: m_currentPlayer(EColor::White)
	, m_state(EGameState::Playing)
{
	m_board.Reset();
}

// TODO (pasul Observer): adauga listenerul in m_listeners.
void Game::AddListener(IGameListenerWeakPtr /*listener*/)
{
}

// TODO (pasul Observer): scoate listenerul din m_listeners.
void Game::RemoveListener(IGameListener* /*listener*/)
{
}

// TODO (pasul MakeMove): verificari, mutare, notificari, schimbarea jucatorului.
void Game::MakeMove(Position /*from*/, Position /*to*/)
{
}

// TODO (pasul promovare).
void Game::PromotePawn(EPieceType /*newType*/)
{
}

// TODO (pasul Restart).
void Game::Restart()
{
}

IPieceInfoPtr Game::GetPieceInfo(Position position) const
{
	return m_board.GetPiece(position);
}

PositionList Game::GetPossibleMoves(Position from) const
{
	return m_board.GetLegalMoves(from);
}

EColor Game::GetCurrentPlayer() const
{
	return m_currentPlayer;
}

EGameState Game::GetState() const
{
	return m_state;
}

bool Game::IsGameOver() const
{
	return m_state == EGameState::Checkmate
		|| m_state == EGameState::Stalemate
		|| m_state == EGameState::Draw;
}
