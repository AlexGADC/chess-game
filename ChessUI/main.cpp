#include "App.h"
#include "FakeGame.h"

#include <exception>
#include <iostream>

int main()
{
	try
	{
		// TODO: when ChessLogic is ready, replace the line below with:
		//   IGamePtr game = IGame::Produce();
		// (and add a project reference from ChessUI to ChessLogic).
		IGamePtr game = std::make_shared<FakeGame>();

		App app(game);
		app.Run();
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Fatal error: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}