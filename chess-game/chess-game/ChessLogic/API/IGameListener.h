#pragma once

#include "EColor.h"
#include "EGameState.h"
#include "Position.h"

#include <memory>

// Observer: cine vrea sa afle ce se intampla in joc (ex. UI-ul) implementeaza interfata asta
// si se inregistreaza cu IGame::AddListener.
class IGameListener
{
public:
	virtual ~IGameListener() = default;

	virtual void OnMoveMade(Position from, Position to) = 0;
	virtual void OnPawnPromotionRequired(Position position) = 0;
	virtual void OnCheck(EColor colorInCheck) = 0;
	virtual void OnGameOver(EGameState finalState) = 0;
	virtual void OnGameRestarted() = 0;
};

using IGameListenerPtr = std::shared_ptr<IGameListener>;
using IGameListenerWeakPtr = std::weak_ptr<IGameListener>;
