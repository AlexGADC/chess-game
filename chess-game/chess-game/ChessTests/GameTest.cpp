	#include "IGame.h"

#include <gtest/gtest.h>

TEST(GameTest, NewGame_WhiteMovesFirst)
{
	IGamePtr game = IGame::Produce();

	EXPECT_EQ(game->GetCurrentPlayer(), EColor::White);
}

TEST(GameTest, NewGame_StateIsPlayingAndNotOver)
{
	IGamePtr game = IGame::Produce();

	EXPECT_EQ(game->GetState(), EGameState::Playing);
	EXPECT_FALSE(game->IsGameOver());
}
