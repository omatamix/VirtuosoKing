#include "movepicker.h"
#include "pos.h"
#include "search.h"
MovePicker::MovePicker(std::vector<Move> allMoves, std::string pvMove, int curPly) {
    allPresentedMoves = allMoves, curPvMove = pvMove, curNodePly = curPly;
}
Move MovePicker::getNext(Position pos) {
    int bestScore = -MIN_MOVE_ORDER_BEST_SCORE * 2;
    size_t bestIndex = 0;
    int score;
    for (size_t i = 0; i < allPresentedMoves.size(); i++) {
        if (std::find(indexesUsed.begin(), indexesUsed.end(), i) != indexesUsed.end()) continue;
        if (getUciMove(allPresentedMoves[i]) == curPvMove) {
            bestIndex = i;
            break;
        }
        score = scoreCaptureMVVLVA(allPresentedMoves[i]) + MVVLVA_WEIGHT;
        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }
    indexesUsed.push_back(bestIndex);
    return allPresentedMoves[bestIndex];
}
bool MovePicker::hasNext() {
    return allPresentedMoves.size() > indexesUsed.size();
}
int MovePicker::scoreCaptureMVVLVA(Move move) {
    int score = move.capturedPiece;
    return score - move.moved;
}