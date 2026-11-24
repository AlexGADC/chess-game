#pragma once

#include "BoardGeometry.h"
#include "EColor.h"
#include "Piece.h"
#include "Position.h"

#include <array>
#include <optional>
#include <string>

// The 8x8 board: where the pieces are, how a move changes it, and which moves are legal.
// It does not know whose turn it is; that belongs to Game.
class Board
{
public:
	static constexpr int SIZE = BOARD_SIZE;

	static bool IsInside(Position position);

	Board() = default;
	// Pieces remember whether they have moved, so a copied board gets its own copies of the pieces.
	Board(const Board& other);
	Board& operator=(const Board& other);
	Board(Board&& other) = default;
	Board& operator=(Board&& other) = default;

	// Puts all pieces in the starting position.
	void Reset();
	// Removes every piece, so a custom position can be built with PlacePiece.
	void Clear();

	// Throws OutOfBoundsException for a position outside the board. Returns nullptr for an empty square.
	PiecePtr GetPiece(Position position) const;
	bool IsEmpty(Position position) const;
	// Puts `piece` on the square, replacing what was there. A nullptr empties the square.
	void PlacePiece(Position position, PiecePtr piece);

	// Carries out a move without checking that it is legal, including the extra effects of
	// castling (the rook moves too) and en passant (the passed pawn is removed).
	void MovePiece(Position from, Position to);

	// The moves of the piece on `from` that do not leave its own king in check.
	PositionList GetLegalMoves(Position from) const;
	bool HasAnyLegalMove(EColor color) const;

	bool IsSquareAttacked(Position position, EColor attackerColor) const;
	bool IsInCheck(EColor color) const;
	std::optional<Position> FindKing(EColor color) const;

	// The square a pawn has just skipped with a double step, if the last move was one.
	std::optional<Position> GetEnPassantTarget() const;
	// True while neither the king nor that side's rook has moved from its starting square.
	bool HasCastlingRight(EColor color, const CastlingSide& side) const;

	// True when neither player has enough pieces left to ever give checkmate.
	bool HasInsufficientMaterial() const;
	// Text that is equal for two boards exactly when they count as the same position for the
	// repetition rule: same pieces on the same squares, same castling rights, same en passant option.
	std::string GetPositionKey() const;

private:
	void MoveCastlingRook(int row, int kingEndCol);
	bool IsUnmovedPiece(Position position, EPieceType type, EColor color) const;
	bool IsEnPassantCapturePossible() const;

	std::array<std::array<PiecePtr, SIZE>, SIZE> m_squares;
	std::optional<Position> m_enPassantTarget;
};
