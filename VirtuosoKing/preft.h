#pragma once
#include <algorithm>
#include <chrono>
#include "defs.h"
#include "eval.h"
#include "parser.h"
#include "pos.h"
constexpr int DEFAULT_PREFT_DEPTH = 5;
class Preft {
public:
	int nodesSearched;
	int quietMoves;
	int normalPawnMoves;
	int normalPawnCaptures;
	int bigPawnMoves;
	int regularCaptures;
	int enpssantPawnCaptures;
	int normalPromotionMoves;
	int capturePromotionMoves;
	int queensideCastlingMoves;
	int kingsideCastlingMoves;
	int stalematesNum;
	int checkmatesNum;
	int checksNum;
	Preft() {
		nodesSearched = 0;
		quietMoves = 0;
		normalPawnMoves = 0;
        normalPawnCaptures = 0;
		bigPawnMoves = 0;
		regularCaptures = 0;
		enpssantPawnCaptures = 0;
		normalPromotionMoves = 0;
		capturePromotionMoves = 0;
		queensideCastlingMoves = 0;
		kingsideCastlingMoves = 0;
		stalematesNum = 0;
		checkmatesNum = 0;
		checksNum = 0;
	}
	void prettyPrintResults(int nps);
	void preftFen(Parser parser, std::string fen, Depth preftDepth);
	void iterPref(Position pos, Depth curDepth);
};
