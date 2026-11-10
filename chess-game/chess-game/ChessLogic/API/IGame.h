#pragma once

#include "EColor.h"
#include "EGameState.h"
#include "EPieceType.h"
#include "IGameListener.h"
#include "IPieceInfo.h"
#include "Position.h"

#include <memory>

class IGame;
using IGamePtr = std::shared_ptr<IGame>;

// Singurul punct de intrare in logica jocului pentru UI.
class IGame
{
public:
	// Creeaza un joc nou, cu piesele in pozitia initiala. Albul muta primul.
	static IGamePtr Produce();

	virtual ~IGame() = default;

	// Subiectul din Observer.
	virtual void AddListener(IGameListenerWeakPtr listener) = 0;
	virtual void RemoveListener(IGameListener* listener) = 0;

	// Arunca OutOfBoundsException, InvalidMoveException sau InvalidStateException.
	virtual void MakeMove(Position from, Position to) = 0;

	// Valid doar in starea WaitingForPromotion; newType nu poate fi King sau Pawn.
	virtual void PromotePawn(EPieceType newType) = 0;

	virtual void Restart() = 0;

	// Returneaza nullptr daca patratul e gol.
	virtual IPieceInfoPtr GetPieceInfo(Position position) const = 0;
	virtual PositionList GetPossibleMoves(Position from) const = 0;

	virtual EColor GetCurrentPlayer() const = 0;
	virtual EGameState GetState() const = 0;
	virtual bool IsGameOver() const = 0;
};
