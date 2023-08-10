#pragma once
#include <cassert>
#include <vector>
#include "pos.h"
constexpr int MIN_MOVE_ORDER_BEST_SCORE = 99937;
constexpr int MVVLVA_WEIGHT = 10000;
constexpr int SEE_WEIGHT = 1000035;
constexpr int COUNTER_MOVE_BONUS = 2000;
class MovePicker {
public:
    std::string curPvMove = "";
    std::vector<Move> allPresentedMoves;
    int curNodePly;
    MovePicker(std::vector<Move> allMoves, std::string pvMove, int curPly);
    Move getNext(Position pos);
    bool hasNext();
    int scoreCaptureMVVLVA(Move move);
private:
    std::vector<size_t> indexesUsed;
};