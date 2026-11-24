#include "Board.h"
#include "ChessExceptions.h"
#include "TestHelpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

namespace
{
	const EColor White = EColor::White;
	const EColor Black = EColor::Black;

	void ExpectPiece(const Board& board, const char* square, EPieceType type, EColor color)
	{
		const PiecePtr piece = board.GetPiece(At(square));
		ASSERT_NE(piece, nullptr) << "no piece on " << square;
		EXPECT_EQ(piece->GetType(), type) << "wrong type on " << square;
		EXPECT_EQ(piece->GetColor(), color) << "wrong color on " << square;
	}

	Board MakeStartingBoard()
	{
		Board board;
		board.Reset();
		return board;
	}
}

// ---------------------------------------------------------------- starting position

TEST(BoardSetupTest, Reset_PlacesBackRanksInOrder)
{
	const Board board = MakeStartingBoard();

	const std::array<EPieceType, Board::SIZE> expectedOrder{
		EPieceType::Rook, EPieceType::Knight, EPieceType::Bishop, EPieceType::Queen,
		EPieceType::King, EPieceType::Bishop, EPieceType::Knight, EPieceType::Rook };

	for (int col = 0; col < Board::SIZE; ++col)
	{
		const PiecePtr whitePiece = board.GetPiece(Position{ 0, col });
		const PiecePtr blackPiece = board.GetPiece(Position{ 7, col });

		ASSERT_NE(whitePiece, nullptr);
		ASSERT_NE(blackPiece, nullptr);
		EXPECT_EQ(whitePiece->GetType(), expectedOrder[col]);
		EXPECT_EQ(whitePiece->GetColor(), White);
		EXPECT_EQ(blackPiece->GetType(), expectedOrder[col]);
		EXPECT_EQ(blackPiece->GetColor(), Black);
	}
}

TEST(BoardSetupTest, Reset_PlacesPawnsOnSecondAndSeventhRank)
{
	const Board board = MakeStartingBoard();

	for (int col = 0; col < Board::SIZE; ++col)
	{
		const PiecePtr whitePawn = board.GetPiece(Position{ 1, col });
		const PiecePtr blackPawn = board.GetPiece(Position{ 6, col });

		ASSERT_NE(whitePawn, nullptr);
		ASSERT_NE(blackPawn, nullptr);
		EXPECT_EQ(whitePawn->GetType(), EPieceType::Pawn);
		EXPECT_EQ(whitePawn->GetColor(), White);
		EXPECT_EQ(blackPawn->GetType(), EPieceType::Pawn);
		EXPECT_EQ(blackPawn->GetColor(), Black);
	}
}

TEST(BoardSetupTest, Reset_LeavesMiddleRanksEmpty)
{
	const Board board = MakeStartingBoard();

	for (int row = 2; row <= 5; ++row)
	{
		for (int col = 0; col < Board::SIZE; ++col)
		{
			EXPECT_TRUE(board.IsEmpty(Position{ row, col }));
		}
	}
}

TEST(BoardSetupTest, Reset_AfterMoves_RestoresStartingPosition)
{
	Board board = MakeStartingBoard();
	board.MovePiece(At("e2"), At("e4"));

	board.Reset();

	ExpectPiece(board, "e2", EPieceType::Pawn, White);
	EXPECT_TRUE(board.IsEmpty(At("e4")));
}

TEST(BoardSetupTest, Clear_RemovesEveryPiece)
{
	Board board = MakeStartingBoard();

	board.Clear();

	for (int row = 0; row < Board::SIZE; ++row)
	{
		for (int col = 0; col < Board::SIZE; ++col)
		{
			EXPECT_TRUE(board.IsEmpty(Position{ row, col }));
		}
	}
}

TEST(BoardSetupTest, StartingPosition_PawnKnightAndBlockedPieces)
{
	const Board board = MakeStartingBoard();

	EXPECT_THAT(board.GetLegalMoves(At("e2")), UnorderedElementsAreArray(Squares({ "e3", "e4" })));
	EXPECT_THAT(board.GetLegalMoves(At("b1")), UnorderedElementsAreArray(Squares({ "a3", "c3" })));
	EXPECT_THAT(board.GetLegalMoves(At("g8")), UnorderedElementsAreArray(Squares({ "f6", "h6" })));
	EXPECT_THAT(board.GetLegalMoves(At("a1")), IsEmpty());
	EXPECT_THAT(board.GetLegalMoves(At("c1")), IsEmpty());
	EXPECT_THAT(board.GetLegalMoves(At("d1")), IsEmpty());
	EXPECT_THAT(board.GetLegalMoves(At("e1")), IsEmpty());
}

// ---------------------------------------------------------------- bounds and empty squares

TEST(BoardAccessTest, GetPiece_OutsideBoard_Throws)
{
	const Board board = MakeStartingBoard();

	EXPECT_THROW(board.GetPiece(Position{ -1, 0 }), OutOfBoundsException);
	EXPECT_THROW(board.GetPiece(Position{ 0, 8 }), OutOfBoundsException);
	EXPECT_THROW(board.GetPiece(Position{ 8, 0 }), OutOfBoundsException);
	EXPECT_THROW(board.GetPiece(Position{ 0, -1 }), OutOfBoundsException);
}

TEST(BoardAccessTest, IsInside_AcceptsOnlyTheEightByEightBoard)
{
	EXPECT_TRUE(Board::IsInside(Position{ 0, 0 }));
	EXPECT_TRUE(Board::IsInside(Position{ 7, 7 }));
	EXPECT_FALSE(Board::IsInside(Position{ 8, 7 }));
	EXPECT_FALSE(Board::IsInside(Position{ 7, 8 }));
	EXPECT_FALSE(Board::IsInside(Position{ -1, 3 }));
	EXPECT_FALSE(Board::IsInside(Position{ 3, -1 }));
}

TEST(BoardAccessTest, MovePiece_OutsideBoard_Throws)
{
	Board board = MakeStartingBoard();

	EXPECT_THROW(board.MovePiece(At("e2"), Position{ 8, 4 }), OutOfBoundsException);
	EXPECT_THROW(board.MovePiece(Position{ -1, 4 }, At("e4")), OutOfBoundsException);
}

TEST(BoardAccessTest, GetLegalMoves_EmptySquareOrOutsideBoard_ReturnsNothing)
{
	const Board board = MakeStartingBoard();

	EXPECT_THAT(board.GetLegalMoves(At("e4")), IsEmpty());
	EXPECT_THAT(board.GetLegalMoves(Position{ 9, 9 }), IsEmpty());
}

TEST(BoardAccessTest, MovePiece_MovesThePieceAndEmptiesTheSource)
{
	Board board = MakeStartingBoard();

	board.MovePiece(At("g1"), At("f3"));

	ExpectPiece(board, "f3", EPieceType::Knight, White);
	EXPECT_TRUE(board.IsEmpty(At("g1")));
}

TEST(BoardAccessTest, MovePiece_OntoEnemyPiece_CapturesIt)
{
	Board board = MakeBoard({
		{ "d4", EPieceType::Rook, White },
		{ "d7", EPieceType::Knight, Black } });

	board.MovePiece(At("d4"), At("d7"));

	ExpectPiece(board, "d7", EPieceType::Rook, White);
	EXPECT_TRUE(board.IsEmpty(At("d4")));
}

// ---------------------------------------------------------------- rook, bishop, queen

TEST(SlidingPieceMovesTest, Rook_OnOpenBoard_MovesAlongRankAndFile)
{
	const Board board = MakeBoard({ { "d4", EPieceType::Rook, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "d3", "d5", "d6", "d7", "d8",
		"a4", "b4", "c4", "e4", "f4", "g4", "h4" })));
}

TEST(SlidingPieceMovesTest, Rook_StopsBeforeOwnPieceAndCapturesEnemyPiece)
{
	const Board board = MakeBoard({
		{ "a1", EPieceType::Rook, White },
		{ "a3", EPieceType::Pawn, White },
		{ "c1", EPieceType::Pawn, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("a1")), UnorderedElementsAreArray(Squares({ "a2", "b1", "c1" })));
}

TEST(SlidingPieceMovesTest, Bishop_OnOpenBoard_MovesAlongDiagonals)
{
	const Board board = MakeBoard({ { "d4", EPieceType::Bishop, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({
		"c3", "b2", "a1", "e5", "f6", "g7", "h8",
		"c5", "b6", "a7", "e3", "f2", "g1" })));
}

TEST(SlidingPieceMovesTest, Bishop_StopsBeforeOwnPieceAndCapturesEnemyPiece)
{
	const Board board = MakeBoard({
		{ "c1", EPieceType::Bishop, Black },
		{ "e3", EPieceType::Pawn, Black },
		{ "a3", EPieceType::Knight, White } });

	EXPECT_THAT(board.GetLegalMoves(At("c1")), UnorderedElementsAreArray(Squares({ "b2", "a3", "d2" })));
}

TEST(SlidingPieceMovesTest, Queen_OnOpenBoard_MovesLikeRookAndBishop)
{
	const Board board = MakeBoard({ { "d4", EPieceType::Queen, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "d3", "d5", "d6", "d7", "d8",
		"a4", "b4", "c4", "e4", "f4", "g4", "h4",
		"c3", "b2", "a1", "e5", "f6", "g7", "h8",
		"c5", "b6", "a7", "e3", "f2", "g1" })));
}

TEST(SlidingPieceMovesTest, Queen_IsBlockedInEveryDirectionAtTheStart)
{
	Board board = MakeStartingBoard();
	board.MovePiece(At("e2"), At("e4"));

	// Only the diagonal opened by the e-pawn is free.
	EXPECT_THAT(board.GetLegalMoves(At("d1")), UnorderedElementsAreArray(Squares({ "e2", "f3", "g4", "h5" })));
}

// ---------------------------------------------------------------- knight

TEST(KnightMovesTest, InTheCenter_HasEightJumps)
{
	const Board board = MakeBoard({ { "d4", EPieceType::Knight, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({
		"b3", "b5", "c2", "c6", "e2", "e6", "f3", "f5" })));
}

TEST(KnightMovesTest, InTheCorner_HasTwoJumps)
{
	const Board board = MakeBoard({ { "a1", EPieceType::Knight, White } });

	EXPECT_THAT(board.GetLegalMoves(At("a1")), UnorderedElementsAreArray(Squares({ "b3", "c2" })));
}

TEST(KnightMovesTest, JumpsOverPieces_CapturesEnemy_NotOwn)
{
	const Board board = MakeBoard({
		{ "a1", EPieceType::Knight, White },
		{ "a2", EPieceType::Pawn, White },
		{ "b1", EPieceType::Pawn, White },
		{ "b2", EPieceType::Pawn, White },
		{ "b3", EPieceType::Pawn, White },
		{ "c2", EPieceType::Pawn, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("a1")), UnorderedElementsAreArray(Squares({ "c2" })));
}

// ---------------------------------------------------------------- king

TEST(KingMovesTest, InTheCenter_MovesOneSquareInEveryDirection)
{
	const Board board = MakeBoard({ { "d4", EPieceType::King, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({
		"c3", "c4", "c5", "d3", "d5", "e3", "e4", "e5" })));
}

TEST(KingMovesTest, CannotStepOntoAnAttackedSquare)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "d8", EPieceType::Rook, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "e2", "f1", "f2" })));
}

TEST(KingMovesTest, CannotStepNextToTheEnemyKing)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e3", EPieceType::King, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "d1", "f1" })));
}

TEST(KingMovesTest, CanCaptureAnUndefendedAttacker_ButNotADefendedOne)
{
	const Board undefended = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e2", EPieceType::Queen, Black } });
	EXPECT_THAT(undefended.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "e2" })));

	const Board defended = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e2", EPieceType::Queen, Black },
		{ "e8", EPieceType::Rook, Black } });
	EXPECT_THAT(defended.GetLegalMoves(At("e1")), IsEmpty());
}

// ---------------------------------------------------------------- pawn

TEST(PawnMovesTest, White_FromStartRow_MovesOneOrTwoSquaresUp)
{
	const Board board = MakeBoard({ { "c2", EPieceType::Pawn, White } });

	EXPECT_THAT(board.GetLegalMoves(At("c2")), UnorderedElementsAreArray(Squares({ "c3", "c4" })));
}

TEST(PawnMovesTest, Black_FromStartRow_MovesOneOrTwoSquaresDown)
{
	const Board board = MakeBoard({ { "c7", EPieceType::Pawn, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("c7")), UnorderedElementsAreArray(Squares({ "c6", "c5" })));
}

TEST(PawnMovesTest, AwayFromStartRow_MovesOnlyOneSquare)
{
	const Board board = MakeBoard({
		{ "c3", EPieceType::Pawn, White },
		{ "f6", EPieceType::Pawn, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("c3")), UnorderedElementsAreArray(Squares({ "c4" })));
	EXPECT_THAT(board.GetLegalMoves(At("f6")), UnorderedElementsAreArray(Squares({ "f5" })));
}

TEST(PawnMovesTest, BlockedDirectlyInFront_CannotMove)
{
	const Board board = MakeBoard({
		{ "c2", EPieceType::Pawn, White },
		{ "c3", EPieceType::Knight, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("c2")), IsEmpty());
}

TEST(PawnMovesTest, BlockedOnTheSecondSquare_MovesOnlyOne)
{
	const Board board = MakeBoard({
		{ "c2", EPieceType::Pawn, White },
		{ "c4", EPieceType::Knight, White } });

	EXPECT_THAT(board.GetLegalMoves(At("c2")), UnorderedElementsAreArray(Squares({ "c3" })));
}

TEST(PawnMovesTest, CapturesDiagonallyForward_OnlyEnemyPieces)
{
	const Board board = MakeBoard({
		{ "d4", EPieceType::Pawn, White },
		{ "c5", EPieceType::Knight, Black },
		{ "e5", EPieceType::Knight, White },
		{ "c3", EPieceType::Knight, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({ "d5", "c5" })));
}

TEST(PawnMovesTest, Black_CapturesDiagonallyDownward)
{
	const Board board = MakeBoard({
		{ "d5", EPieceType::Pawn, Black },
		{ "c4", EPieceType::Rook, White },
		{ "e4", EPieceType::Rook, White },
		{ "d4", EPieceType::Rook, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d5")), UnorderedElementsAreArray(Squares({ "c4", "e4" })));
}

TEST(PawnMovesTest, OnTheEdgeFile_DoesNotWrapAround)
{
	const Board board = MakeBoard({
		{ "a4", EPieceType::Pawn, White },
		{ "h5", EPieceType::Knight, Black },
		{ "b5", EPieceType::Knight, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("a4")), UnorderedElementsAreArray(Squares({ "a5", "b5" })));
}

// ---------------------------------------------------------------- en passant

TEST(EnPassantTest, RightAfterTheDoubleStep_CaptureIsAllowed)
{
	Board board = MakeBoard({
		{ "e5", EPieceType::Pawn, White },
		{ "d7", EPieceType::Pawn, Black } });

	board.MovePiece(At("d7"), At("d5"));

	EXPECT_THAT(board.GetLegalMoves(At("e5")), UnorderedElementsAreArray(Squares({ "e6", "d6" })));
}

TEST(EnPassantTest, Capturing_RemovesThePawnThatWasPassed)
{
	Board board = MakeBoard({
		{ "e5", EPieceType::Pawn, White },
		{ "d7", EPieceType::Pawn, Black } });
	board.MovePiece(At("d7"), At("d5"));

	board.MovePiece(At("e5"), At("d6"));

	ExpectPiece(board, "d6", EPieceType::Pawn, White);
	EXPECT_TRUE(board.IsEmpty(At("d5")));
	EXPECT_TRUE(board.IsEmpty(At("e5")));
}

TEST(EnPassantTest, Black_CanCaptureEnPassantToo)
{
	Board board = MakeBoard({
		{ "d4", EPieceType::Pawn, Black },
		{ "e2", EPieceType::Pawn, White } });

	board.MovePiece(At("e2"), At("e4"));

	EXPECT_THAT(board.GetLegalMoves(At("d4")), UnorderedElementsAreArray(Squares({ "d3", "e3" })));

	board.MovePiece(At("d4"), At("e3"));
	EXPECT_TRUE(board.IsEmpty(At("e4")));
}

TEST(EnPassantTest, AfterAnotherMove_TheRightIsLost)
{
	Board board = MakeBoard({
		{ "e5", EPieceType::Pawn, White },
		{ "d7", EPieceType::Pawn, Black },
		{ "a1", EPieceType::Rook, White },
		{ "a8", EPieceType::Rook, Black } });
	board.MovePiece(At("d7"), At("d5"));

	board.MovePiece(At("a1"), At("b1"));
	board.MovePiece(At("a8"), At("b8"));

	EXPECT_THAT(board.GetLegalMoves(At("e5")), UnorderedElementsAreArray(Squares({ "e6" })));
}

TEST(EnPassantTest, AfterASingleStep_ThereIsNoEnPassant)
{
	Board board = MakeBoard({
		{ "e5", EPieceType::Pawn, White },
		{ "d6", EPieceType::Pawn, Black } });

	board.MovePiece(At("d6"), At("d5"));

	EXPECT_THAT(board.GetLegalMoves(At("e5")), UnorderedElementsAreArray(Squares({ "e6" })));
}

// ---------------------------------------------------------------- castling

namespace
{
	Board MakeCastlingBoard()
	{
		return MakeBoard({
			{ "e1", EPieceType::King, White },
			{ "a1", EPieceType::Rook, White },
			{ "h1", EPieceType::Rook, White },
			{ "e8", EPieceType::King, Black },
			{ "a8", EPieceType::Rook, Black },
			{ "h8", EPieceType::Rook, Black },
			// Pawns keep the two sides from attacking each other along the files.
			{ "a2", EPieceType::Pawn, White }, { "e2", EPieceType::Pawn, White }, { "h2", EPieceType::Pawn, White },
			{ "a7", EPieceType::Pawn, Black }, { "e7", EPieceType::Pawn, Black }, { "h7", EPieceType::Pawn, Black } });
	}
}

TEST(CastlingTest, BothSidesAreOfferedWhenNothingPreventsIt)
{
	const Board board = MakeCastlingBoard();

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "f1", "f2", "c1", "g1" })));
	EXPECT_THAT(board.GetLegalMoves(At("e8")), UnorderedElementsAreArray(Squares({
		"d8", "d7", "f8", "f7", "c8", "g8" })));
}

TEST(CastlingTest, KingSide_MovesTheRookNextToTheKing)
{
	Board board = MakeCastlingBoard();

	board.MovePiece(At("e1"), At("g1"));

	ExpectPiece(board, "g1", EPieceType::King, White);
	ExpectPiece(board, "f1", EPieceType::Rook, White);
	EXPECT_TRUE(board.IsEmpty(At("e1")));
	EXPECT_TRUE(board.IsEmpty(At("h1")));
}

TEST(CastlingTest, QueenSide_MovesTheRookNextToTheKing)
{
	Board board = MakeCastlingBoard();

	board.MovePiece(At("e8"), At("c8"));

	ExpectPiece(board, "c8", EPieceType::King, Black);
	ExpectPiece(board, "d8", EPieceType::Rook, Black);
	EXPECT_TRUE(board.IsEmpty(At("e8")));
	EXPECT_TRUE(board.IsEmpty(At("a8")));
}

TEST(CastlingTest, NotAllowed_AfterTheKingHasMoved)
{
	Board board = MakeCastlingBoard();
	board.MovePiece(At("e1"), At("f1"));
	board.MovePiece(At("f1"), At("e1"));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "d1", "d2", "f1", "f2" })));
}

TEST(CastlingTest, NotAllowed_OnTheSideWhoseRookHasMoved)
{
	Board board = MakeCastlingBoard();
	board.MovePiece(At("h1"), At("g1"));
	board.MovePiece(At("g1"), At("h1"));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "f1", "f2", "c1" })));
}

TEST(CastlingTest, NotAllowed_WhenASquareBetweenKingAndRookIsOccupied)
{
	Board board = MakeCastlingBoard();
	board.PlacePiece(At("b1"), Piece::Produce(EPieceType::Knight, White));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "f1", "f2", "g1" })));
}

TEST(CastlingTest, NotAllowed_WhileTheKingIsInCheck)
{
	Board board = MakeCastlingBoard();
	board.PlacePiece(At("e2"), nullptr);
	board.PlacePiece(At("e7"), nullptr);
	board.PlacePiece(At("e8"), nullptr);
	board.PlacePiece(At("e5"), Piece::Produce(EPieceType::Rook, Black));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "d1", "d2", "f1", "f2" })));
}

TEST(CastlingTest, NotAllowed_ThroughAnAttackedSquare)
{
	Board board = MakeCastlingBoard();
	// The black rook on f5 attacks f1 (and f2), the square the king would pass over.
	board.PlacePiece(At("f5"), Piece::Produce(EPieceType::Rook, Black));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({ "d1", "d2", "c1" })));
}

TEST(CastlingTest, NotAllowed_IntoAnAttackedSquare)
{
	Board board = MakeCastlingBoard();
	board.PlacePiece(At("g5"), Piece::Produce(EPieceType::Rook, Black));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "f1", "f2", "c1" })));
}

TEST(CastlingTest, QueenSide_Allowed_WhenOnlyTheRookPathIsAttacked)
{
	Board board = MakeCastlingBoard();
	// b1 is attacked, but the king never crosses it, so castling long is still legal.
	board.PlacePiece(At("b5"), Piece::Produce(EPieceType::Rook, Black));

	EXPECT_THAT(board.GetLegalMoves(At("e1")), UnorderedElementsAreArray(Squares({
		"d1", "d2", "f1", "f2", "c1", "g1" })));
}

TEST(CastlingTest, NotAllowed_WhenTheKingIsNotOnItsStartingSquare)
{
	const Board board = MakeBoard({
		{ "d1", EPieceType::King, White },
		{ "a1", EPieceType::Rook, White },
		{ "h1", EPieceType::Rook, White } });

	EXPECT_THAT(board.GetLegalMoves(At("d1")), UnorderedElementsAreArray(Squares({
		"c1", "c2", "d2", "e1", "e2" })));
}

// ---------------------------------------------------------------- check, pins, endings

TEST(CheckTest, IsInCheck_DetectsEveryKindOfAttacker)
{
	EXPECT_TRUE(MakeBoard({ { "e1", EPieceType::King, White }, { "e8", EPieceType::Rook, Black } }).IsInCheck(White));
	EXPECT_TRUE(MakeBoard({ { "e1", EPieceType::King, White }, { "h4", EPieceType::Bishop, Black } }).IsInCheck(White));
	EXPECT_TRUE(MakeBoard({ { "e1", EPieceType::King, White }, { "a5", EPieceType::Queen, Black } }).IsInCheck(White));
	EXPECT_TRUE(MakeBoard({ { "e1", EPieceType::King, White }, { "f3", EPieceType::Knight, Black } }).IsInCheck(White));
	EXPECT_TRUE(MakeBoard({ { "e1", EPieceType::King, White }, { "d2", EPieceType::Pawn, Black } }).IsInCheck(White));
}

TEST(CheckTest, IsInCheck_FalseWhenTheAttackIsBlockedOrMisses)
{
	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e4", EPieceType::Pawn, White },
		{ "e8", EPieceType::Rook, Black } }).IsInCheck(White));
	EXPECT_FALSE(MakeBoard({ { "e1", EPieceType::King, White }, { "d8", EPieceType::Rook, Black } }).IsInCheck(White));
	EXPECT_FALSE(MakeBoard({ { "e1", EPieceType::King, White }, { "e3", EPieceType::Knight, Black } }).IsInCheck(White));
	EXPECT_FALSE(MakeStartingBoard().IsInCheck(White));
	EXPECT_FALSE(MakeStartingBoard().IsInCheck(Black));
}

TEST(CheckTest, PawnsAttackOnlyForward)
{
	// A white pawn attacks the rank above it, so it checks a king on d5 from e4, not from e6.
	EXPECT_TRUE(MakeBoard({ { "d5", EPieceType::King, Black }, { "e4", EPieceType::Pawn, White } }).IsInCheck(Black));
	EXPECT_FALSE(MakeBoard({ { "d5", EPieceType::King, Black }, { "e6", EPieceType::Pawn, White } }).IsInCheck(Black));
	// A pawn does not attack the square straight ahead.
	EXPECT_FALSE(MakeBoard({ { "d5", EPieceType::King, Black }, { "d4", EPieceType::Pawn, White } }).IsInCheck(Black));
}

TEST(CheckTest, IsInCheck_ReportsOnlyTheAttackedColor)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e8", EPieceType::King, Black },
		{ "a8", EPieceType::Rook, White } });

	EXPECT_TRUE(board.IsInCheck(Black));
	EXPECT_FALSE(board.IsInCheck(White));
}

TEST(PinTest, PinnedRook_MovesOnlyAlongThePin)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e2", EPieceType::Rook, White },
		{ "e8", EPieceType::Rook, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("e2")), UnorderedElementsAreArray(Squares({
		"e3", "e4", "e5", "e6", "e7", "e8" })));
}

TEST(PinTest, PinnedBishop_CannotMoveAtAll)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "e2", EPieceType::Bishop, White },
		{ "e8", EPieceType::Rook, Black } });

	EXPECT_THAT(board.GetLegalMoves(At("e2")), IsEmpty());
}

TEST(PinTest, EnPassant_NotAllowedWhenItUncoversTheKing)
{
	// Capturing en passant would remove both pawns from the fifth rank and expose the king to the rook.
	Board board = MakeBoard({
		{ "a5", EPieceType::King, White },
		{ "e5", EPieceType::Pawn, White },
		{ "d7", EPieceType::Pawn, Black },
		{ "h5", EPieceType::Rook, Black } });

	board.MovePiece(At("d7"), At("d5"));

	EXPECT_THAT(board.GetLegalMoves(At("e5")), UnorderedElementsAreArray(Squares({ "e6" })));
}

TEST(CheckTest, WhenInCheck_OnlyMovesThatAnswerItAreLegal)
{
	const Board board = MakeBoard({
		{ "e1", EPieceType::King, White },
		{ "a4", EPieceType::Rook, White },
		{ "e8", EPieceType::Rook, Black } });

	// The rook can only block on e4; every other rook move leaves the king in check.
	EXPECT_THAT(board.GetLegalMoves(At("a4")), UnorderedElementsAreArray(Squares({ "e4" })));
}

TEST(EndingTest, BackRankMate_KingInCheckAndNoLegalMove)
{
	const Board board = MakeBoard({
		{ "g8", EPieceType::King, Black },
		{ "f7", EPieceType::Pawn, Black },
		{ "g7", EPieceType::Pawn, Black },
		{ "h7", EPieceType::Pawn, Black },
		{ "a8", EPieceType::Rook, White },
		{ "e1", EPieceType::King, White } });

	EXPECT_TRUE(board.IsInCheck(Black));
	EXPECT_FALSE(board.HasAnyLegalMove(Black));
	EXPECT_TRUE(board.HasAnyLegalMove(White));
}

TEST(EndingTest, Stalemate_KingNotInCheckAndNoLegalMove)
{
	const Board board = MakeBoard({
		{ "h8", EPieceType::King, Black },
		{ "f7", EPieceType::King, White },
		{ "g6", EPieceType::Queen, White } });

	EXPECT_FALSE(board.IsInCheck(Black));
	EXPECT_FALSE(board.HasAnyLegalMove(Black));
}

TEST(EndingTest, CheckThatCanBeAnswered_IsNotMate)
{
	const Board board = MakeBoard({
		{ "g8", EPieceType::King, Black },
		{ "f7", EPieceType::Pawn, Black },
		{ "g7", EPieceType::Pawn, Black },
		{ "h7", EPieceType::Pawn, Black },
		{ "d5", EPieceType::Bishop, Black },
		{ "a8", EPieceType::Rook, White },
		{ "e1", EPieceType::King, White } });

	// The bishop can capture the checking rook.
	EXPECT_TRUE(board.IsInCheck(Black));
	EXPECT_TRUE(board.HasAnyLegalMove(Black));
	EXPECT_THAT(board.GetLegalMoves(At("d5")), UnorderedElementsAreArray(Squares({ "a8" })));
}

TEST(EndingTest, StartingPosition_BothSidesHaveMoves)
{
	const Board board = MakeStartingBoard();

	EXPECT_TRUE(board.HasAnyLegalMove(White));
	EXPECT_TRUE(board.HasAnyLegalMove(Black));
}

// ---------------------------------------------------------------- copying

TEST(BoardCopyTest, Copy_HasTheSamePiecesOnTheSameSquares)
{
	Board original = MakeStartingBoard();
	original.MovePiece(At("e2"), At("e4"));

	const Board copy(original);

	ExpectPiece(copy, "e4", EPieceType::Pawn, White);
	ExpectPiece(copy, "e8", EPieceType::King, Black);
	EXPECT_TRUE(copy.IsEmpty(At("e2")));
	EXPECT_EQ(copy.GetEnPassantTarget(), original.GetEnPassantTarget());
	EXPECT_EQ(copy.GetPositionKey(), original.GetPositionKey());
}

TEST(BoardCopyTest, MovingOnTheCopy_DoesNotChangeTheOriginal)
{
	const Board original = MakeStartingBoard();
	Board copy(original);

	copy.MovePiece(At("g1"), At("f3"));

	ExpectPiece(original, "g1", EPieceType::Knight, White);
	EXPECT_TRUE(original.IsEmpty(At("f3")));
}

TEST(BoardCopyTest, Copy_DoesNotShareTheMovedFlagOfThePieces)
{
	const Board original = MakeCastlingBoard();
	Board copy(original);

	// Moving the king on the copy must not take the castling right away from the original.
	copy.MovePiece(At("e1"), At("f1"));
	copy.MovePiece(At("f1"), At("e1"));

	EXPECT_NE(copy.GetPiece(At("e1")), original.GetPiece(At("e1")));
	EXPECT_FALSE(original.GetPiece(At("e1"))->HasMoved());
	EXPECT_TRUE(original.HasCastlingRight(White, KING_SIDE_CASTLING));
	EXPECT_FALSE(copy.HasCastlingRight(White, KING_SIDE_CASTLING));
}

TEST(BoardCopyTest, Assignment_ReplacesThePositionWithAnIndependentCopy)
{
	const Board source = MakeBoard({ { "d4", EPieceType::Queen, Black } });
	Board target = MakeStartingBoard();

	target = source;
	target.MovePiece(At("d4"), At("d8"));

	ExpectPiece(source, "d4", EPieceType::Queen, Black);
	ExpectPiece(target, "d8", EPieceType::Queen, Black);
	EXPECT_TRUE(target.IsEmpty(At("e1")));
}

// ---------------------------------------------------------------- attacks, king, en passant target

TEST(BoardQueryTest, IsSquareAttacked_ByTheGivenColorOnly)
{
	const Board board = MakeBoard({
		{ "a1", EPieceType::Rook, White },
		{ "h8", EPieceType::Bishop, Black } });

	EXPECT_TRUE(board.IsSquareAttacked(At("a8"), White));
	EXPECT_TRUE(board.IsSquareAttacked(At("h1"), White));
	EXPECT_FALSE(board.IsSquareAttacked(At("b2"), White));

	EXPECT_TRUE(board.IsSquareAttacked(At("b2"), Black));
	EXPECT_FALSE(board.IsSquareAttacked(At("a8"), Black));
}

TEST(BoardQueryTest, IsSquareAttacked_StopsAtTheFirstPieceOnTheLine)
{
	const Board board = MakeBoard({
		{ "a1", EPieceType::Rook, White },
		{ "a4", EPieceType::Pawn, Black } });

	EXPECT_TRUE(board.IsSquareAttacked(At("a3"), White));
	EXPECT_TRUE(board.IsSquareAttacked(At("a4"), White));
	EXPECT_FALSE(board.IsSquareAttacked(At("a5"), White));
}

TEST(BoardQueryTest, IsSquareAttacked_EmptySquareInFrontOfAPawnIsNotAttacked)
{
	const Board board = MakeBoard({ { "e4", EPieceType::Pawn, White } });

	EXPECT_FALSE(board.IsSquareAttacked(At("e5"), White));
	EXPECT_TRUE(board.IsSquareAttacked(At("d5"), White));
	EXPECT_TRUE(board.IsSquareAttacked(At("f5"), White));
}

TEST(BoardQueryTest, FindKing_ReturnsTheSquareOfEachKing)
{
	const Board board = MakeStartingBoard();

	EXPECT_EQ(board.FindKing(White), At("e1"));
	EXPECT_EQ(board.FindKing(Black), At("e8"));
}

TEST(BoardQueryTest, FindKing_WithoutAKing_ReturnsNothing)
{
	const Board board = MakeBoard({ { "e1", EPieceType::King, White } });

	EXPECT_EQ(board.FindKing(Black), std::nullopt);
	EXPECT_FALSE(board.IsInCheck(Black));
}

TEST(BoardQueryTest, EnPassantTarget_IsTheSkippedSquare_OnlyAfterADoubleStep)
{
	Board board = MakeStartingBoard();
	EXPECT_EQ(board.GetEnPassantTarget(), std::nullopt);

	board.MovePiece(At("e2"), At("e4"));
	EXPECT_EQ(board.GetEnPassantTarget(), At("e3"));

	board.MovePiece(At("d7"), At("d5"));
	EXPECT_EQ(board.GetEnPassantTarget(), At("d6"));

	board.MovePiece(At("g1"), At("f3"));
	EXPECT_EQ(board.GetEnPassantTarget(), std::nullopt);

	board.MovePiece(At("d5"), At("d4"));
	EXPECT_EQ(board.GetEnPassantTarget(), std::nullopt);
}

TEST(BoardQueryTest, PlacePiece_ReplacesOrEmptiesTheSquare)
{
	Board board = MakeStartingBoard();

	board.PlacePiece(At("e2"), Piece::Produce(EPieceType::Queen, Black));
	ExpectPiece(board, "e2", EPieceType::Queen, Black);

	board.PlacePiece(At("e2"), nullptr);
	EXPECT_TRUE(board.IsEmpty(At("e2")));

	EXPECT_THROW(board.PlacePiece(Position{ 8, 8 }, nullptr), OutOfBoundsException);
}

// ---------------------------------------------------------------- castling rights

TEST(CastlingRightTest, StartingPosition_AllFourRightsExist)
{
	const Board board = MakeStartingBoard();

	EXPECT_TRUE(board.HasCastlingRight(White, KING_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(White, QUEEN_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(Black, KING_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(Black, QUEEN_SIDE_CASTLING));
}

TEST(CastlingRightTest, MovingARook_RemovesOnlyThatSide)
{
	Board board = MakeCastlingBoard();

	board.MovePiece(At("a8"), At("b8"));

	EXPECT_FALSE(board.HasCastlingRight(Black, QUEEN_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(Black, KING_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(White, QUEEN_SIDE_CASTLING));
}

TEST(CastlingRightTest, MovingTheKing_RemovesBothSides)
{
	Board board = MakeCastlingBoard();

	board.MovePiece(At("e1"), At("d1"));

	EXPECT_FALSE(board.HasCastlingRight(White, KING_SIDE_CASTLING));
	EXPECT_FALSE(board.HasCastlingRight(White, QUEEN_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(Black, KING_SIDE_CASTLING));
}

TEST(CastlingRightTest, CapturedRook_RemovesThatSide)
{
	Board board = MakeCastlingBoard();
	board.PlacePiece(At("h2"), nullptr);
	board.PlacePiece(At("h7"), nullptr);

	board.MovePiece(At("h8"), At("h1"));

	EXPECT_FALSE(board.HasCastlingRight(White, KING_SIDE_CASTLING));
	EXPECT_TRUE(board.HasCastlingRight(White, QUEEN_SIDE_CASTLING));
}

TEST(CastlingRightTest, AfterCastling_TheRightIsGone)
{
	Board board = MakeCastlingBoard();

	board.MovePiece(At("e1"), At("g1"));

	EXPECT_FALSE(board.HasCastlingRight(White, KING_SIDE_CASTLING));
	EXPECT_FALSE(board.HasCastlingRight(White, QUEEN_SIDE_CASTLING));
	EXPECT_TRUE(board.GetPiece(At("f1"))->HasMoved());
}

// ---------------------------------------------------------------- insufficient material

TEST(InsufficientMaterialTest, PositionsThatCanNeverEndInMate)
{
	EXPECT_TRUE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black } }).HasInsufficientMaterial());

	EXPECT_TRUE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "c1", EPieceType::Bishop, White } }).HasInsufficientMaterial());

	EXPECT_TRUE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "b8", EPieceType::Knight, Black } }).HasInsufficientMaterial());

	// c1 and f8 are both dark squares.
	EXPECT_TRUE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "c1", EPieceType::Bishop, White }, { "f8", EPieceType::Bishop, Black } }).HasInsufficientMaterial());
}

TEST(InsufficientMaterialTest, PositionsWhereMateIsStillPossible)
{
	EXPECT_FALSE(MakeStartingBoard().HasInsufficientMaterial());

	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "a2", EPieceType::Pawn, White } }).HasInsufficientMaterial());

	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "a1", EPieceType::Rook, Black } }).HasInsufficientMaterial());

	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "d1", EPieceType::Queen, White } }).HasInsufficientMaterial());

	// c1 is a dark square, c8 a light one: opposite-colored bishops can still mate.
	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "c1", EPieceType::Bishop, White }, { "c8", EPieceType::Bishop, Black } }).HasInsufficientMaterial());

	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "b1", EPieceType::Knight, White }, { "g1", EPieceType::Knight, White } }).HasInsufficientMaterial());

	EXPECT_FALSE(MakeBoard({
		{ "e1", EPieceType::King, White }, { "e8", EPieceType::King, Black },
		{ "b1", EPieceType::Knight, White }, { "c1", EPieceType::Bishop, White } }).HasInsufficientMaterial());
}

// ---------------------------------------------------------------- position key

TEST(PositionKeyTest, SamePosition_GivesTheSameKey)
{
	Board board = MakeStartingBoard();
	const std::string startKey = board.GetPositionKey();

	// The knights go out and come back: the pieces, castling rights and en passant options are unchanged.
	board.MovePiece(At("g1"), At("f3"));
	board.MovePiece(At("g8"), At("f6"));
	EXPECT_NE(board.GetPositionKey(), startKey);

	board.MovePiece(At("f3"), At("g1"));
	board.MovePiece(At("f6"), At("g8"));
	EXPECT_EQ(board.GetPositionKey(), startKey);
}

TEST(PositionKeyTest, DifferentPieceOrColorOnASquare_GivesADifferentKey)
{
	const Board whiteRook = MakeBoard({ { "d4", EPieceType::Rook, White } });
	const Board blackRook = MakeBoard({ { "d4", EPieceType::Rook, Black } });
	const Board whiteQueen = MakeBoard({ { "d4", EPieceType::Queen, White } });
	const Board whiteRookElsewhere = MakeBoard({ { "d5", EPieceType::Rook, White } });

	EXPECT_NE(whiteRook.GetPositionKey(), blackRook.GetPositionKey());
	EXPECT_NE(whiteRook.GetPositionKey(), whiteQueen.GetPositionKey());
	EXPECT_NE(whiteRook.GetPositionKey(), whiteRookElsewhere.GetPositionKey());
}

TEST(PositionKeyTest, LostCastlingRight_GivesADifferentKey)
{
	Board board = MakeCastlingBoard();
	const std::string keyWithRights = board.GetPositionKey();

	// The rook returns to its square, but the right to castle on that side is gone for good.
	board.MovePiece(At("h1"), At("g1"));
	board.MovePiece(At("g1"), At("h1"));

	EXPECT_NE(board.GetPositionKey(), keyWithRights);
}

TEST(PositionKeyTest, EnPassantOption_GivesADifferentKey_OnlyWhileACaptureIsPossible)
{
	// Same pieces on the same squares, reached with and without the en passant option.
	Board withEnPassant = MakeBoard({
		{ "e5", EPieceType::Pawn, White },
		{ "d7", EPieceType::Pawn, Black } });
	withEnPassant.MovePiece(At("d7"), At("d5"));

	const Board withoutEnPassant = MakeBoard({
		{ "e5", EPieceType::Pawn, White },
		{ "d5", EPieceType::Pawn, Black } });

	EXPECT_NE(withEnPassant.GetPositionKey(), withoutEnPassant.GetPositionKey());

	// With no pawn able to capture, the double step leaves nothing to tell the positions apart.
	Board doubleStepNobodyCanCapture = MakeBoard({ { "d7", EPieceType::Pawn, Black } });
	doubleStepNobodyCanCapture.MovePiece(At("d7"), At("d5"));
	const Board samePawnPlaced = MakeBoard({ { "d5", EPieceType::Pawn, Black } });

	EXPECT_EQ(doubleStepNobodyCanCapture.GetPositionKey(), samePawnPlaced.GetPositionKey());
}
