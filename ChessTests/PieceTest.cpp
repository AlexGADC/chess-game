#include "Bishop.h"
#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "TestHelpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

namespace
{
	const EColor White = EColor::White;
	const EColor Black = EColor::Black;

	template <typename ExpectedClass>
	bool IsInstanceOf(const PiecePtr& piece)
	{
		return dynamic_cast<const ExpectedClass*>(piece.get()) != nullptr;
	}
}

// ---------------------------------------------------------------- factory and common attributes

TEST(PieceFactoryTest, Produce_CreatesTheClassThatMatchesTheType)
{
	EXPECT_TRUE(IsInstanceOf<King>(Piece::Produce(EPieceType::King, White)));
	EXPECT_TRUE(IsInstanceOf<Queen>(Piece::Produce(EPieceType::Queen, White)));
	EXPECT_TRUE(IsInstanceOf<Rook>(Piece::Produce(EPieceType::Rook, White)));
	EXPECT_TRUE(IsInstanceOf<Bishop>(Piece::Produce(EPieceType::Bishop, White)));
	EXPECT_TRUE(IsInstanceOf<Knight>(Piece::Produce(EPieceType::Knight, White)));
	EXPECT_TRUE(IsInstanceOf<Pawn>(Piece::Produce(EPieceType::Pawn, White)));

	EXPECT_FALSE(IsInstanceOf<Rook>(Piece::Produce(EPieceType::Bishop, White)));
}

TEST(PieceFactoryTest, Produce_SetsTypeAndColor)
{
	const std::array<EPieceType, 6> allTypes{
		EPieceType::King, EPieceType::Queen, EPieceType::Rook,
		EPieceType::Bishop, EPieceType::Knight, EPieceType::Pawn };

	for (EPieceType type : allTypes)
	{
		for (EColor color : { White, Black })
		{
			const PiecePtr piece = Piece::Produce(type, color);

			ASSERT_NE(piece, nullptr);
			EXPECT_EQ(piece->GetType(), type);
			EXPECT_EQ(piece->GetColor(), color);
		}
	}
}

TEST(PieceTest, NewPiece_HasNotMoved_UntilMarked)
{
	const PiecePtr rook = Piece::Produce(EPieceType::Rook, Black);
	EXPECT_FALSE(rook->HasMoved());

	rook->MarkMoved();

	EXPECT_TRUE(rook->HasMoved());
}

TEST(PieceTest, Clone_CopiesClassColorAndMovedFlag)
{
	const PiecePtr original = Piece::Produce(EPieceType::Knight, Black);
	original->MarkMoved();

	const PiecePtr copy = original->Clone();

	EXPECT_NE(copy, original);
	EXPECT_TRUE(IsInstanceOf<Knight>(copy));
	EXPECT_EQ(copy->GetType(), EPieceType::Knight);
	EXPECT_EQ(copy->GetColor(), Black);
	EXPECT_TRUE(copy->HasMoved());
}

TEST(PieceTest, Clone_IsIndependentOfTheOriginal)
{
	const PiecePtr original = Piece::Produce(EPieceType::King, White);
	const PiecePtr copy = original->Clone();

	copy->MarkMoved();

	EXPECT_FALSE(original->HasMoved());
}

// ---------------------------------------------------------------- attacked squares vs moves

TEST(PawnTest, AttacksTheDiagonals_ButMovesStraight)
{
	const Board board = MakeBoard({ { "e4", EPieceType::Pawn, White } });
	const PiecePtr pawn = board.GetPiece(At("e4"));

	EXPECT_THAT(pawn->GetAttackedSquares(board, At("e4")), UnorderedElementsAreArray(Squares({ "d5", "f5" })));
	EXPECT_THAT(pawn->GetPseudoLegalMoves(board, At("e4")), UnorderedElementsAreArray(Squares({ "e5" })));
}

TEST(PawnTest, Black_AttacksDownward)
{
	const Board board = MakeBoard({ { "e4", EPieceType::Pawn, Black } });
	const PiecePtr pawn = board.GetPiece(At("e4"));

	EXPECT_THAT(pawn->GetAttackedSquares(board, At("e4")), UnorderedElementsAreArray(Squares({ "d3", "f3" })));
}

TEST(PawnTest, OnTheEdge_AttacksOnlyOneSquare)
{
	const Board board = MakeBoard({ { "h4", EPieceType::Pawn, White } });
	const PiecePtr pawn = board.GetPiece(At("h4"));

	EXPECT_THAT(pawn->GetAttackedSquares(board, At("h4")), UnorderedElementsAreArray(Squares({ "g5" })));
}

TEST(PawnTest, OnTheLastRow_HasNoMoves)
{
	const Board board = MakeBoard({ { "e8", EPieceType::Pawn, White } });
	const PiecePtr pawn = board.GetPiece(At("e8"));

	EXPECT_THAT(pawn->GetPseudoLegalMoves(board, At("e8")), IsEmpty());
	EXPECT_THAT(pawn->GetAttackedSquares(board, At("e8")), IsEmpty());
}

TEST(KingTest, AttackedSquares_DoNotIncludeCastling_ButMovesDo)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "a1", EPieceType::Rook, White },
		{ "h1", EPieceType::Rook, White } });
	const PiecePtr king = board.GetPiece(At("e1"));

	EXPECT_THAT(king->GetAttackedSquares(board, At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "e2", "f1", "f2" })));
	EXPECT_THAT(king->GetPseudoLegalMoves(board, At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "e2", "f1", "f2", "c1", "g1" })));
}

TEST(KingTest, PseudoLegalMoves_IgnoreCheck_LegalMovesDoNot)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "d8", EPieceType::Rook, Black } });
	const PiecePtr king = board.GetPiece(At("e1"));

	// The piece's own rules allow d1 and d2; the board removes them because the rook attacks the d-file.
	EXPECT_THAT(king->GetPseudoLegalMoves(board, At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "e2", "f1", "f2" })));
	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "e2", "f1", "f2" })));
}

TEST(RookTest, AttackedSquares_IncludeEnemyPieces_NotOwnPieces)
{
	const Board board = MakeBoard({
		{ "a1", EPieceType::Rook, White },
		{ "a3", EPieceType::Pawn, Black },
		{ "c1", EPieceType::Pawn, White } });
	const PiecePtr rook = board.GetPiece(At("a1"));

	EXPECT_THAT(rook->GetAttackedSquares(board, At("a1")), UnorderedElementsAreArray(Squares({ "a2", "a3", "b1" })));
}

TEST(KnightTest, AttackedSquaresAndMovesAreTheSame)
{
	const Board board = MakeBoard({ { "g1", EPieceType::Knight, White } });
	const PiecePtr knight = board.GetPiece(At("g1"));

	EXPECT_THAT(knight->GetAttackedSquares(board, At("g1")), UnorderedElementsAreArray(Squares({ "e2", "f3", "h3" })));
	EXPECT_THAT(knight->GetPseudoLegalMoves(board, At("g1")), UnorderedElementsAreArray(Squares({ "e2", "f3", "h3" })));
}
