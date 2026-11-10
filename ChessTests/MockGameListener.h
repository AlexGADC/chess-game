#pragma once

#include "IGameListener.h"

#include <gmock/gmock.h>

class MockGameListener : public IGameListener
{
public:
	MOCK_METHOD(void, OnMoveMade, (Position from, Position to), (override));
	MOCK_METHOD(void, OnPawnPromotionRequired, (Position position), (override));
	MOCK_METHOD(void, OnCheck, (EColor colorInCheck), (override));
	MOCK_METHOD(void, OnGameOver, (EGameState finalState), (override));
	MOCK_METHOD(void, OnGameRestarted, (), (override));
};
