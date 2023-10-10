#include <iostream>
#include "preft.h"
void Preft::prettyPrintResults(int nps) {
	std::cout << "+-----------------+\n";
	std::cout << "|  Preft Results  |\n";
	std::cout << "+-----------------+\n";
	std::cout << "| Nodes Ran: " << nodesSearched << "\n";
	std::cout << "| Nodes Per Second: " << nps << "\n";
	std::cout << "| Quiet Moves: " << quietMoves << "\n";
	std::cout << "| Regular Pawn Pushes: " << normalPawnMoves << "\n";
	std::cout << "| Regular Pawn Captures: " << normalPawnCaptures << "\n";
	std::cout << "| Regular Big Pawn Moves: " << bigPawnMoves << "\n";
	std::cout << "| Enpassant Pawn Captures: " << enpssantPawnCaptures << "\n";
	std::cout << "| Promotion Pawn Pushes: " << normalPromotionMoves << "\n";
	std::cout << "| Promotion Pawn Captures: " << capturePromotionMoves << "\n";
	std::cout << "| Regular Capture Moves: " << regularCaptures << "\n";
	std::cout << "| Kingside Castling Moves: " << kingsideCastlingMoves << "\n";
	std::cout << "| Queenside Castling Moves: " << queensideCastlingMoves << "\n";
	std::cout << "| Stalemates: " << stalematesNum << "\n";
	std::cout << "| Checkmates: " << checkmatesNum << "\n";
	std::cout << "| Checks: " << checksNum << "\n";
	std::cout << "+-----------------+\n";
}
void Preft::preftFen(Parser parser, std::string fen, Depth preftDepth) {
	for (int i = 0; i < 224; ++i) parser.pieceMailbox[i] = BASE_MAILBOX[i];
	for (int i = 0; i < 224; ++i) parser.colorMailbox[i] = BASE_MAILBOX[i];
	for (int i = 0; i < 5; ++i) parser.epSqrs[i] = 0;
	for (int i = 0; i < 2; ++i)
		for (int k = 0; k < 5; ++k) parser.cstSqrs[i][k] = false;
	for (int i = 0; i < 5; ++i) parser.kingsTracker[i] = 0;
	parser.parseFen(fen);
	Position basePos;
	basePos.initBaseGlobalVar();
	basePos.setPosData(parser.col2Mve, parser.colorMailbox, parser.pieceMailbox, parser.epSqrs,
		parser.cstSqrs, parser.kingsTracker, 0, false);
	printBoard(basePos);
	std::cout << "\n";
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	iterPref(basePos, preftDepth);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto diff = end - begin;
	int nps = int(nodesSearched * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(diff).count());
	prettyPrintResults(nps);
}
void Preft::iterPref(Position pos, Depth curDepth) {
	nodesSearched++;
	bool checked = inCheck(pos, pos.curTurn);
	if (checked) {
		checksNum++;
	}
	if (curDepth == 0) {
		return;
	}
	int movesSearched = 0;
	Move searchedMove;
	uint64_t encodedMove;
	size_t moveSize = allocateMoves(pos, static_cast<int>(curDepth));
	for (size_t index = 0; index < moveSize; ++index) {
		getAllocatedMove(encodedMove, static_cast<int>(curDepth), static_cast<int>(index));
		searchedMove.decode(encodedMove);
		History oldMoveData = pos.doMove(searchedMove);
		if (checked && (searchedMove.moveFlag & CASTLE_OO || searchedMove.moveFlag & CASTLE_OOO)) {
			pos.undoMove(oldMoveData, searchedMove);
			continue;
		}
		if (searchedMove.capturedPiece == KING) {
			movesSearched++;
			nodesSearched++;
			checkmatesNum++;
			pos.undoMove(oldMoveData, searchedMove);
			continue;
		}
		if (inCheck(pos, oldMoveData.turn)) {
			pos.undoMove(oldMoveData, searchedMove);
			continue;
		}
		movesSearched++;
		if (searchedMove.moveFlag & CAPTURE) {
			if (searchedMove.moved == PAWN) {
				normalPawnCaptures++;
			} else {
				regularCaptures++;
			}
		} else if (searchedMove.moveFlag & BIG_PAWN) {
			bigPawnMoves++;
		} else if (searchedMove.moveFlag & EP_CAPTURE) {
			enpssantPawnCaptures++;
		} else if (searchedMove.moveFlag & PROMOTION) {
			normalPromotionMoves++;
		} else if (searchedMove.moveFlag & PROMOTION_CAPTURE) {
			capturePromotionMoves++;
		} else if (searchedMove.moveFlag & CASTLE_OO) {
			kingsideCastlingMoves++;
		} else if (searchedMove.moveFlag & CASTLE_OOO) {
			queensideCastlingMoves++;
		} else if (searchedMove.moved == PAWN) {
			normalPawnMoves++;
		} else {
			quietMoves++;
		}
		iterPref(pos, curDepth-1);
		pos.undoMove(oldMoveData, searchedMove);
	}
	if (movesSearched == 0) {
		if (checked) {
			checkmatesNum++;
		} else {
			stalematesNum++;
		}
	}
}