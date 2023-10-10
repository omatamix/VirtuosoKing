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
	bool checked = pos.inCheck(pos.curTurn);
	if (checked) {
		checksNum++;
	}
	if (curDepth == 0) {
		return;
	}
	int movesSearched = 0;
	std::vector<Move> allMoves = pos.getAllMoves(pos.curTurn);
	for (const Move& move : allMoves) {
		History oldMoveData = pos.doMove(move);
		if (checked && (move.moveFlag & CASTLE_OO || move.moveFlag & CASTLE_OOO)) {
			pos.undoMove(oldMoveData, move);
			continue;
		}
		if (move.capturedPiece == KING) {
			movesSearched++;
			nodesSearched++;
			checkmatesNum++;
			pos.undoMove(oldMoveData, move);
			continue;
		}
		if (pos.inCheck(oldMoveData.turn)) {
			pos.undoMove(oldMoveData, move);
			continue;
		}
		movesSearched++;
		if (move.moveFlag & CAPTURE) {
			if (move.moved == PAWN) {
				normalPawnCaptures++;
			} else {
				regularCaptures++;
			}
		} else if (move.moveFlag & BIG_PAWN) {
			bigPawnMoves++;
		} else if (move.moveFlag & EP_CAPTURE) {
			enpssantPawnCaptures++;
		} else if (move.moveFlag & PROMOTION) {
			normalPromotionMoves++;
		} else if (move.moveFlag & PROMOTION_CAPTURE) {
			capturePromotionMoves++;
		} else if (move.moveFlag & CASTLE_OO) {
			kingsideCastlingMoves++;
		} else if (move.moveFlag & CASTLE_OOO) {
			queensideCastlingMoves++;
		} else if (move.moved == PAWN) {
			normalPawnMoves++;
		} else {
			quietMoves++;
		}
		iterPref(pos, curDepth-1);
		pos.undoMove(oldMoveData, move);
	}
	if (movesSearched == 0) {
		if (checked) {
			checkmatesNum++;
		} else {
			stalematesNum++;
		}
	}
}