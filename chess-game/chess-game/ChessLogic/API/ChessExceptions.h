#pragma once

#include <stdexcept>
#include <string>

class ChessException : public std::runtime_error
{
public:
	explicit ChessException(const std::string& message)
		: std::runtime_error(message)
	{
	}
};

// Mutare ilegala: patrat gol, piesa adversarului sau destinatie nepermisa.
class InvalidMoveException : public ChessException
{
public:
	using ChessException::ChessException;
};

// Pozitie in afara tablei 8x8.
class OutOfBoundsException : public ChessException
{
public:
	using ChessException::ChessException;
};

// Actiune nepermisa in starea curenta (ex. mutare dupa sfarsitul jocului
// sau mutare in timp ce se asteapta promovarea pionului).
class InvalidStateException : public ChessException
{
public:
	using ChessException::ChessException;
};
