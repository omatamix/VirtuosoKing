#include <algorithm>
#include <array>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include "pos.h"
#include "search.h"
#include "tt.h"
#include "eval.h"
std::atomic<bool> isStop(true);
std::atomic<bool> stopSignal(true);
std::unique_ptr<TranspositionTable> transpositionTable;
void initTranspositionTable() {
    transpositionTable = std::make_unique<TranspositionTable>(MAX_TT_SIZE);
}
uint64_t transpositionRandTable[5][224][24];
void initTranspositionKeyHandler() {
    for (int i = 0; i < 5; ++i)
        for (int w = 0; w < 224; ++w)
            for (int x = 0; x < 24; ++x)
                transpositionRandTable[i][w][x] = get64rand();
}
uint64_t getTimeElapsed(ChessTime startTime) {
    auto endTime = ChessClock::now();
    std::chrono::milliseconds timeSpan = std::chrono::duration_cast
        <std::chrono::milliseconds>(endTime - startTime);
    return (uint64_t)timeSpan.count() + 1;
}
int lmrReductions[MAX_MOVES][MAX_MOVES];
int currentPly = 0;
int currentDepth = 0;
int extensionDepth = 0;
static int moveHistoryScores[224][224];
static int butterflyTable[224][224];
static uint64_t captureKillers[MAX_MOVES + 1];
static uint64_t mateKillers[MAX_MOVES + 1];
static uint64_t killers[MAX_MOVES + 1][2];
static bool moveOrderChecks[MAX_MOVES][MAX_MOVES];
static uint64_t cached_pv_move = INVALID_DATA;
void resetMoveOrderChecks() {
    for (int ply = 0; ply < MAX_MOVES; ++ply) {
        for (int index = 0; index < MAX_MOVES; ++index) {
            moveOrderChecks[ply][index] = false;
        }
    }
}
void resetKillers() {
    for (int ply = 0; ply < MAX_MOVES + 1; ++ply) {
        for (int index = 0; index < 2; ++index) {
            killers[ply][index] = INVALID_DATA;
        }
        captureKillers[ply] = mateKillers[ply] = INVALID_DATA;
    }
}
void clearHistoryScores(bool decayScores) {
    for (int from = 0; from < 224; ++from) {
        for (int to = 0; to < 224; ++to) {
            if (decayScores) {
                moveHistoryScores[from][to] *= .7;
                butterflyTable[from][to] *= .6;
            } else {
                moveHistoryScores[from][to] = PIECE_ZERO;
                butterflyTable[from][to] = PIECE_ZERO;
            }
        }
    }
}
void clearTranspositionTables() {
    transpositionTable->clear();
}
void initReductionTable() {
    for (int d = 0; d < MAX_MOVES; d++) {
        for (int m = 0; m < MAX_MOVES; m++) {
            lmrReductions[d][m] = 1.25 + log(d) * log(m) * 100 / 267;
        }
    }
}
int Search::see(Position pos, int square) {
    const int color = pos.curTurn;
    int value = 0;
    std::pair<int, int> piece = getSmallestAttacker(pos, square, color);
    if (piece.first == -1) {
        return 0;
    }
    Move captureMove{ piece.second, square, QUEEN, CAPTURE, piece.first, pos.pieceMailbox[square], pos.colorMailbox[square], -1 };
    History oldMoveData = pos.doMove(captureMove);
    value = captureMove.capturedPiece - see(pos, square);
    pos.undoMove(oldMoveData, captureMove);
    return value;
}
int Search::seeCapture(Position pos, Move captureMove) {
    History oldMoveData = pos.doMove(captureMove);
    int seeValue = captureMove.capturedPiece - see(pos, captureMove.to);
    pos.undoMove(oldMoveData, captureMove);
    return seeValue;
}
void Search::getBestMove(Position pos, TimeParams timeParams) {
    uint64_t timeSoFar, bestMove = INVALID_DATA;
    nodeCounter = curPly = 0;
    int previousScore = 0;
    int alpha, beta, bestMoveIndex = -1;
    _timeLimit = (timeParams.searchMode == TIME) ? timeParams.maxAllotment
               : (timeParams.searchMode == MOVETIME) ? timeParams.allotment
               : MAX_TIME;
    _startTime = ChessClock::now();
    clearHistoryScores(false);
    transpositionTable->newSearch();
    int score = 0, window = aspirationWidth;
    SearchStackInfo stack[MAX_MOVES + 7];
    for (int i = 0; i <= MAX_MOVES; ++i) (stack + i)->ply = i;
    for (int depth = 1; depth < MAX_MOVES; depth++) {
        _selDepth = 0;
        extensionDepth = depth;
        clearAllocatedMoves();
        SearchPV pvLineInLoop;
        pvLineInLoop.pvLength = 0;
        alpha = -MATE_SCORE, beta = MATE_SCORE;
        score = searchPVS(pos, stack, depth, alpha, beta, &pvLineInLoop, &bestMoveIndex);
        timeSoFar = getTimeElapsed(_startTime);
        uint64_t nps = 1000 * nodeCounter / timeSoFar;
        if (bestMoveIndex == -1) {
            std::cout << "info depth " << depth
                      << " nodes " << nodeCounter
                      << " nps " << nps
                      << " time " << timeSoFar
                      << " hashfull " << transpositionTable->hashfull() << std::endl;
            break;
        }
        if (!isStop) {
            getAllocatedMove(bestMove, depth, bestMoveIndex);
        }
        if (pvLineInLoop.pvLength != 0 && !isStop) {
            std::cout << "info depth " << depth;
            std::cout << " nodes " << nodeCounter
                      << " nps " << nps
                      << " score ";
            calculateDistanceToMate(score);
            std::cout << " time " << timeSoFar
                      << " hashfull " << transpositionTable->hashfull()
                      << " pv " << getPvLine(pvLineInLoop) << std::endl;
            bestMoveForPos = pvLineInLoop.pv[0];
            bestScoreOverall = score;
        }
        previousScore = score;
        if ((MATE_THRESHOLD < previousScore || -MATE_THRESHOLD > previousScore)) break;
        if (timeParams.searchMode == MOVETIME) {
            if (extensionDepth >= 16) {
                break;
            } else if (extensionDepth >= 21 && timeSoFar > (uint64_t)timeParams.allotment - 9000) {
                break;
            } else if (extensionDepth >= 17 && timeSoFar > (uint64_t)timeParams.allotment - 3000) {
                break;
            } else if (extensionDepth >= 14 && timeSoFar > (uint64_t)timeParams.allotment + 3000) {
                break;
            } else if (extensionDepth >= 8 && timeSoFar > (uint64_t)timeParams.allotment + 7000) {
                break;
            } else if (extensionDepth < 8 && timeSoFar > (uint64_t)timeParams.allotment + 11000) {
                break;
            }
        }
        // if (timeParams.searchMode == MOVETIME && timeSoFar > (uint64_t)timeParams.allotment);
        if ((timeParams.searchMode == DEPTH && depth >= timeParams.allotment) ||
            (timeParams.limitNodes && timeParams.maxNodes < nodeCounter) || isStop) {
            break;
        }
        maxDepthReached = depth;
    }
    stopSignal = true;
    isStop = true;
    Move decoded{};
    decoded.decode(bestMove);
    std::cout << "bestmove " << getUciMove(decoded) << std::endl;
    std::cout << "search complete" << std::endl;
}
int Search::searchPVS(Position pos, SearchStackInfo* stack, int depth, int alpha,
    int beta, SearchPV* pvLine, int* bestMoveIndex) {
    const int currentPlayer = pos.curTurn;
    SearchPV line;
    uint64_t posHash = computeHash(pos);
    const HashTableEntry* tte = nullptr;
    tte = transpositionTable->get(posHash);
    uint64_t pvMove = INVALID_DATA;
    if (tte != nullptr && tte->key == posHash) {
        pvMove = tte->move;
    }
    bool checked = inCheck(pos, currentPlayer);
    int movesSearched = 0, bestScore = -MATE_SCORE;
    int currScore;
    cached_pv_move = pvMove;
    Move searchedMove;
    uint64_t encodedMove;
    size_t moveSize = allocateMoves(pos, static_cast<int>(depth));
    uint64_t encodedMoves[MAX_MOVES];
    Move moves[MAX_MOVES];
    for (size_t index = 0; index < moveSize; ++index) {
        getAllocatedMove(encodedMoves[index], depth, index);
        moves[index].decode(encodedMoves[index]);
        History oldMoveData = pos.doMove(moves[index]);
        moveOrderChecks[depth][index] = inCheck(pos, currentPlayer) || inCheck(pos, PARTNERS[currentPlayer]);
        pos.undoMove(oldMoveData, moves[index]);
    }
    currentPly = stack->ply;
    currentDepth = depth;
    int quietMovesFound = 0;
    std::sort(moves, moves + moveSize, &Search::compareMoves);
    for (size_t index = 0; index < moveSize; ++index) {
        encodedMove = encodedMoves[moves[index].curPieceIndex];
        searchedMove = moves[index];
        if (encodedMove == INVALID_DATA) {
            continue;
        }
        if (checked && (searchedMove.moveFlag & CASTLE_OO || searchedMove.moveFlag & CASTLE_OOO)) {
            continue;
        }
        History oldMoveData = pos.doMove(searchedMove);
        if (inCheck(pos, oldMoveData.turn) && searchedMove.capturedPiece != KING) {
            pos.undoMove(oldMoveData, searchedMove);
            continue;
        }
        bool isCapture = (searchedMove.moveFlag & CAPTURE) ||
                         (searchedMove.moveFlag & EP_CAPTURE) ||
                         (searchedMove.moveFlag & PROMOTION_CAPTURE);
        bool givesCheck = moveOrderChecks[depth][moves[index].curPieceIndex];
        bool isQuiet = !isCapture && !checked;
        if (isQuiet) {
            quietMovesFound++;
        }
        movesSearched++;
        if (searchedMove.capturedPiece == KING) {
            currScore = MATE_SCORE - stack->ply;
        } else {
            int newDepth = depth - ONE_PLY;
            if (quietMovesFound > 2 && depth >= 2 && isQuiet) {
                int reduction = lmrReductions[depth][movesSearched];
                Depth d = std::clamp(newDepth - reduction, 1, newDepth + 1);
                currScore = -pvSearch(pos, stack + 1, d, -alpha - 1, -alpha, &line, false, true, true);
            } else if (movesSearched > 1) {
                currScore = -pvSearch(pos, stack + 1, newDepth, -alpha - 1, -alpha, &line, false, true, true);
            }
            if (movesSearched == 1 || currScore > alpha) {
                currScore = -pvSearch(pos, stack + 1, newDepth, -beta, -alpha, &line, true, false, true);
            }
        }
        pos.undoMove(oldMoveData, searchedMove);
        if (stopSignal.load(std::memory_order_relaxed)) {
            return 0;
        }
        if (currScore >= beta) break;
        if (currScore > alpha) {
            bestScore = currScore;
            alpha = currScore;
            *bestMoveIndex = searchedMove.curPieceIndex;
            if (pvLine != nullptr) {
                changePV(searchedMove, pvLine, &line);
            }
        }
    }
    return bestScore;
}
int Search::scoreMove(const Move& currentOrNext) {
    int score = 0;
    uint64_t encodedMove = currentOrNext.encode();
    if (encodedMove == cached_pv_move) {
        score += MOVE_PV;
    } else {
        if (currentOrNext.moveFlag & CAPTURE ||
            currentOrNext.moveFlag & PROMOTION_CAPTURE) {
            if (captureKillers[currentPly] == encodedMove) {
                score += MOVE_CAPTURE_KILLER;
            } else {
                score += currentOrNext.capturedPiece - currentOrNext.moved;
                score += MOVE_CAPTURE;
            }
        } else if (currentOrNext.moveFlag & PROMOTION) {
            score += MOVE_PROMOTION;
        } else if (currentOrNext.moveFlag & EP_CAPTURE) {
            score += MOVE_CAPTURE - 200;
        } else if (currentOrNext.moveFlag & CASTLE_OO ||
                   currentOrNext.moveFlag & CASTLE_OOO) {
            score += MOVE_GREAT_MOVE;
        } else if (mateKillers[currentPly] == encodedMove) {
            score += MOVE_MATE_KILLER;
        } else if (killers[currentPly][0] == encodedMove) {
            score += MOVE_KILLER;
        } else if (killers[currentPly][1] == encodedMove) {
            score += MOVE_KILLER_SECONDARY;
        } else {
            score += moveHistoryScores[currentOrNext.from][currentOrNext.to];
            score += MOVE_QUIET;
            if (score > MOVE_GREAT_MOVE - 10) {
                score = MOVE_GREAT_MOVE - 10;
            }
        }
    }
    return score;
}
bool Search::compareMoves(const Move& current, const Move& next) {
    if (scoreMove(current) != scoreMove(next)) {
        return scoreMove(current) > scoreMove(next);
    }
    return false;
}
int Search::pvSearch(Position pos, SearchStackInfo* stack, int depth, int alpha, int beta,
    SearchPV* pvLine, bool is_pv, bool is_cut, bool nullMoveAllowed) {
    nodeCounter++;
    if (stack->ply > _selDepth) _selDepth = stack->ply;
    if (MATE_SCORE - stack->ply < beta) {
        beta = MATE_SCORE - stack->ply;
        if (alpha >= MATE_SCORE - stack->ply) return MATE_SCORE - stack->ply;
    }
    if (-MATE_SCORE + stack->ply > alpha) {
        alpha = -MATE_SCORE + stack->ply;
        if (beta <= -MATE_SCORE + stack->ply) return -MATE_SCORE + stack->ply;
    }
    if ((MAX_MOVES - 1 > stack->ply) || ((nodeCounter & 1023) == 1023)) {
        uint64_t timeSoFar = getTimeElapsed(_startTime);
        if (extensionDepth > 20) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 21 && timeSoFar > _timeLimit - 9000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 17 && timeSoFar > _timeLimit - 3000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 14 && timeSoFar > _timeLimit + 3000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 8 && timeSoFar > _timeLimit + 7000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth < 8 && timeSoFar > _timeLimit + 11000) {
            isStop = true;
            stopSignal = true;
        }
        // if (timeSoFar > _timeLimit) {
        //     isStop = true;
        //     stopSignal = true;
        // }
    }
    if (stopSignal.load(std::memory_order_relaxed)) {
        return 0;
    }
    bool is_non_fixed_window = (beta - alpha != 1);
    killers[std::min(stack->ply + 2, MAX_MOVES - 2)][0] =
    killers[std::min(stack->ply + 2, MAX_MOVES - 2)][1] =
    mateKillers[std::min(stack->ply + 2, MAX_MOVES - 2)] = 
    captureKillers[std::min(stack->ply + 2, MAX_MOVES - 2)] = INVALID_DATA;
    (stack + 2)->cutoffCnt = stack->moveCount = 0;
    SearchPV line;
    bool tt_hit = false;
    bool tt_pv = is_pv;
    uint64_t pvMove = INVALID_DATA;
    uint64_t posHash = computeHash(pos);
    const HashTableEntry* tte = nullptr;
    tte = transpositionTable->get(posHash);
    if (tte != nullptr && tte->key == posHash) {
        if (tte->depth >= depth && !is_pv) {
            if (tte->bound == EXACT || (tte->bound == LOWERBOUND && tte->score >= beta)
                                    || (tte->bound == UPPERBOUND && tte->score <= alpha)) {
                if (tte->move != INVALID_DATA) {
                    Move decodedTTMove;
                    decodedTTMove.decode(tte->move);
                    bool isCapture = (decodedTTMove.moveFlag & CAPTURE) ||
                                     (decodedTTMove.moveFlag & EP_CAPTURE) ||
                                     (decodedTTMove.moveFlag & PROMOTION_CAPTURE);
                    if (isCapture) {
                        captureKillers[stack->ply] = tte->move;
                    } else {
                        if (tte->score >  MATE_THRESHOLD ||
                            tte->score < -MATE_THRESHOLD) {
                            mateKillers[stack->ply] = tte->move;
                        }
                        killers[stack->ply][1] = killers[stack->ply][0];
                        killers[stack->ply][0] = tte->move;
                    }
                }
                return std::min(beta, std::max(alpha, tte->score));
            }
        }
        tt_hit = true;
        tt_pv = tte->isPv;
        pvMove = tte->move;
    }
    if (depth <= 0) {
        return qsearch(pos, stack, alpha, beta, MAX_QSEARCH_PLY);
    }
    stack->ttPv = tt_hit ? tte->isPv : is_pv;
    const int currentPlayer = pos.curTurn;
    const bool checked = inCheck(pos, currentPlayer);
    const bool partnerChecked = inCheck(pos, PARTNERS[currentPlayer]);
    int sEval;
    stack->staticEval = sEval = getEval(pos);
    bool improving;
    if (checked || partnerChecked) {
        improving = false;
        goto movesLoop;
    } else {
        improving = (stack - 2)->staticEval != -321114 ? stack->staticEval > (stack - 2)->staticEval
                  : (stack - 4)->staticEval != -321114 ? stack->staticEval > (stack - 4)->staticEval
                  : true;
    }
    // Preform razoring to reduce search space.
    if (sEval < alpha - 400 - (250 - 200 * ((stack + 1)->cutoffCnt > 3)) * depth * depth) {
        int value = qsearch(pos, stack, alpha, beta, MAX_QSEARCH_PLY);
        if (value < alpha) return value;
    }
    // Preform futility pruning.
    if (!is_pv && !tt_pv && depth <= 8 && sEval >= beta + depth * FUTILITY_MARGIN && sEval >= beta && alpha < MATE_THRESHOLD) {
        return sEval;
    }
    // Preform null-move pruning.
    if (nullMoveAllowed && sEval >= beta) {
        // Find reduction.
        int r = std::min(depth / 3 + 2, depth);
        // Make the null move.
        HistoryNullMove nmHist = pos.doNullMove();
        int nullScore = -pvSearch(pos, stack + 1, depth - r, -beta, -beta + 1, &line, false, !is_cut, false);
        pos.undoNullMove(nmHist);
        if (nullScore >= beta) {
            transpositionTable->save(posHash, depth, INVALID_DATA, beta, sEval, LOWERBOUND, is_pv);
            return beta; // Apply null-move pruning.
        }
    }
    // Moves loop.
    movesLoop:
    int movesSearched = 0, movesFound = 0, quietMovesFound = 0;
    int bestScore = -MATE_SCORE;
    int currScore, OAR = 0;
    int extend = 0;
    bool hasImprovement = false;
    cached_pv_move = pvMove;
    Move searchedMove;
    uint64_t encodedMove, bestMove = INVALID_DATA;
    size_t moveSize = allocateMoves(pos, depth);
    uint64_t encodedMoves[MAX_MOVES];
    Move moves[MAX_MOVES];
    for (size_t index = 0; index < moveSize; ++index) {
        getAllocatedMove(encodedMoves[index], depth, index);
        moves[index].decode(encodedMoves[index]);
        History oldMoveData = pos.doMove(moves[index]);
        moveOrderChecks[depth][index] = inCheck(pos, nextColor(currentPlayer)) ||
                                        inCheck(pos, PARTNERS[nextColor(currentPlayer)]);
        pos.undoMove(oldMoveData, moves[index]);
    }
    currentPly = stack->ply;
    currentDepth = depth;
    int movesFiltered = 0, cutoffsOccured = 0;
    int nodesCut = 0;
    std::sort(moves, moves + moveSize, &Search::compareMoves);
    for (size_t index = 0; index < moveSize; ++index) {
        encodedMove = encodedMoves[moves[index].curPieceIndex];
        searchedMove = moves[index];
        if (encodedMove == INVALID_DATA) {
            continue;
        }
        if (checked && (searchedMove.moveFlag & CASTLE_OO || searchedMove.moveFlag & CASTLE_OOO)) {
            continue;
        }
        History oldMoveData = pos.doMove(searchedMove);
        if (inCheck(pos, oldMoveData.turn) && searchedMove.capturedPiece != KING) {
            pos.undoMove(oldMoveData, searchedMove);
            continue;
        }
        stack->moveCount += 1;
        movesFound++;
        bool isCapture = (searchedMove.moveFlag & CAPTURE) ||
                         (searchedMove.moveFlag & EP_CAPTURE) ||
                         (searchedMove.moveFlag & PROMOTION_CAPTURE);
        bool givesCheck = moveOrderChecks[depth][moves[index].curPieceIndex];
        bool isQuiet = !givesCheck && !isCapture;
        // Preform late-move pruning.
        if (alpha > -MATE_THRESHOLD && depth <= 8 && // Allow also on PV-nodes.
            movesSearched >= LMP_TABLE[improving][depth] + (is_pv ? depth : 0)) {
            pos.undoMove(oldMoveData, searchedMove);
            continue;
        }
        // // int seeVal = 0;
        // // SEE-Move pruning.
        // if (isCapture && depth <= 6 && !is_pv && alpha > -MATE_THRESHOLD) {
        //     int seeVal = seeCapture(pos, searchedMove);
        //     if (seeVal < 0) {
        //         pos.undoMove(oldMoveData, searchedMove);
        //         continue;
        //     }
        // }
        // // Prune moves with low-history scores.
        // if (isQuiet && !is_pv && !tt_pv && quietMovesFound > 2 && moveHistoryScores[searchedMove.from][searchedMove.to] < 0) {
        //     pos.undoMove(oldMoveData, searchedMove);
        //     continue;
        // }
        // Preform futility-pruning at the move level.
        if (isQuiet && !is_pv && !tt_pv && depth <= 7 && sEval <= alpha - 115 - 90 * depth && alpha > -MATE_THRESHOLD) {
            pos.undoMove(oldMoveData, searchedMove);
            continue;
        }
        // // Multi-cut pruning.
        // if (depth >= MULTI_CUT_REDUCTION && is_cut && movesSearched <= MULTICUT_MAX_MOVES_SEARCHING) {
        //     currScore = -pvSearch(pos, stack + 1, depth - ONE_PLY - MULTI_CUT_REDUCTION, -beta, -beta + 1, &line, false, !is_cut, true);
        //     if (currScore >= beta) {
        //         if (++cutoffsOccured == MULTICUT_CUTOFFS_NEEDED) {
        //             pos.undoMove(oldMoveData, searchedMove);
        //             transpositionTable->save(posHash, depth, encodedMove, beta, sEval, LOWERBOUND, false);
        //             return beta;
        //         }
        //     }
        // }
        // // We should extend the search if a move gives check.
        // if ((givesCheck && maxDepthReached + 4 > _selDepth) ||
        //     (checked && maxDepthReached + 2 > _selDepth && movesSearched > 3) ||
        //     (partnerChecked && maxDepthReached + 2 > _selDepth && movesSearched > 3)) {
        //     extend = 1;
        // } else extend = 0;
        if (isQuiet) {
            quietMovesFound++;
        }
        movesSearched = stack->moveCount;
        if (searchedMove.capturedPiece == KING) {
            currScore = MATE_SCORE - stack->ply;
        } else {
            int newDepth = depth - ONE_PLY + extend - OAR;
            if (movesSearched > 1 && depth >= 2 && (!isCapture || !tt_pv || (is_cut && (stack - 1)->moveCount > 1))) {
                int reduction = lmrReductions[depth][movesSearched];
                reduction -= is_pv;
                Depth d = std::clamp(newDepth - reduction, 1, newDepth + 1);
                currScore = -pvSearch(pos, stack + 1, d, -alpha - 1, -alpha, &line, false, true, true);
            } else if (!is_pv || movesSearched > 1) {
                currScore = -pvSearch(pos, stack + 1, newDepth, -alpha - 1, -alpha, &line, false, !is_cut, true);
            }
            if (is_pv && (movesSearched == 1 || currScore > alpha)) {
                currScore = -pvSearch(pos, stack + 1, newDepth, -beta, -alpha, &line, true, false, true);
            }
        }
        pos.undoMove(oldMoveData, searchedMove);
        if (stopSignal.load(std::memory_order_relaxed)) {
            return 0;
        }
        if (currScore >= beta) {
            alpha = beta;
            bestMove = encodedMove;
            stack->cutoffCnt += 1 + (pvMove == INVALID_DATA);
            changePV(searchedMove, pvLine, &line);
            if (!isCapture) {
                if (currScore >  MATE_THRESHOLD ||
                    currScore < -MATE_THRESHOLD) {
                    mateKillers[stack->ply] = encodedMove;
                }
                killers[stack->ply][1] = killers[stack->ply][0];
                killers[stack->ply][0] = encodedMove;
                moveHistoryScores[searchedMove.from][searchedMove.to] += depth * depth;
            } else {
                captureKillers[stack->ply] = encodedMove;
            }
            break;
        } else {
            // if (hasImprovement && !is_pv && depth > 2 && depth < 5 && beta < 13828 && currScore > -11369) {
            //     OAR += 2;
            // }
        }
        // We found a new best move.
        if (currScore > alpha) {
            // State we found at least one improvement.
            hasImprovement = true;
            // Update the new best move.
            alpha = currScore;
            bestMove = encodedMove;
            // Update the PV line.
            changePV(searchedMove, pvLine, &line);
        }
    }
    // Check for mate.
    if (movesFound == 0) {
        if (checked) { // If we are in check then and no moves were found then it's checkmate.
            return -MATE_SCORE + stack->ply;
        } else { // If we are not in check and no moves were found then it's stalemate.
            return 0;
        }
    }
    // Update the TT table.
    ScoreBound bound = beta <= alpha ? LOWERBOUND : is_pv && bestMove != INVALID_DATA ? EXACT : UPPERBOUND;
    transpositionTable->save(posHash, depth, bestMove, alpha, sEval, bound, is_pv);
    // Return the best score.
    return alpha;
}
int Search::qsearch(Position pos, SearchStackInfo* stack, int alpha, int beta, int depth,
    bool lastMoveWasCheck) {
    // Increase the node counter.
    nodeCounter++;
    // Preform mate-distance pruning.
    if (MATE_SCORE - stack->ply < beta) {
        beta = MATE_SCORE - stack->ply;
        if (alpha >= MATE_SCORE - stack->ply) return MATE_SCORE - stack->ply;
    }
    if (-MATE_SCORE + stack->ply > alpha) {
        alpha = -MATE_SCORE + stack->ply;
        if (beta <= -MATE_SCORE + stack->ply) return -MATE_SCORE + stack->ply;
    }
    // Check to see if we have ran out of time.
    if ((MAX_MOVES - 1 > stack->ply) || ((nodeCounter & 1023) == 1023)) {
        uint64_t timeSoFar = getTimeElapsed(_startTime);
        if (extensionDepth > 20) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 21 && timeSoFar > _timeLimit - 9000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 17 && timeSoFar > _timeLimit - 3000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 14 && timeSoFar > _timeLimit + 3000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth >= 8 && timeSoFar > _timeLimit + 7000) {
            isStop = true;
            stopSignal = true;
        } else if (extensionDepth < 8 && timeSoFar > _timeLimit + 11000) {
            isStop = true;
            stopSignal = true;
        }
        // if (timeSoFar > _timeLimit) {
        //     isStop = true;
        //     stopSignal = true;
        // }
    }
    // Acknowledge the stop signal.
    if (stopSignal.load(std::memory_order_relaxed)) {
        return 0;
    }
    // Get the current evaluation.
    int standingPat = getEval(pos);
    // Are we in check?
    const int currentPlayer = pos.curTurn;
    const bool checked = inCheck(pos, currentPlayer);
    const bool partnerChecked = inCheck(pos, PARTNERS[currentPlayer]);
    // Check for a beta cutoff.
    // Don't allow standing-pat while in check.
    if (standingPat >= beta) return beta;
    // Always update alpha.
    if (alpha < standingPat) alpha = standingPat;
    // // Terminate if we are too deep in the QSearch.
    // if (depth <= 0) {
    //     return standingPat;
    // }
    // Define more base vars.
    int movesSearched = 0, movesFound = 0;
    // Define the best score var.
    int bestScore = -MATE_SCORE;
    int currScore;
    // Define and allocate the needed moves.
    Move searchedMove;
    uint64_t encodedMove, bestMove = INVALID_DATA;
    // Allocate the moves.
    size_t moveSize = allocateMoves(pos, static_cast<int>(depth + 150));
    // Define the storage method for the moves.
    uint64_t encodedMoves[MAX_MOVES];
    Move moves[MAX_MOVES];
    for (size_t index = 0; index < moveSize; ++index) {
        // Get the allocated moves and set them.
        getAllocatedMove(encodedMoves[index], depth + 150, index);
        moves[index].decode(encodedMoves[index]);
        // Be sure to define which moves give check, helps with the move ordering.
        History oldMoveData = pos.doMove(moves[index]);
        moveOrderChecks[depth + 150][index] = inCheck(pos, nextColor(currentPlayer)) ||
                                              inCheck(pos, PARTNERS[nextColor(currentPlayer)]);
        pos.undoMove(oldMoveData, moves[index]);
    }
    // Cache the current ply for move ordering.
    currentPly = stack->ply;
    // Sort the moves.
    std::sort(moves, moves + moveSize, &Search::compareMoves);
    // Loop through the sorted moves.
    for (size_t index = 0; index < moveSize; ++index) {
        // Set the next ordered move.
        encodedMove = encodedMoves[moves[index].curPieceIndex];
        searchedMove = moves[index];
        // Prevent moves with invalid data.
        if (encodedMove == INVALID_DATA) {
            continue;
        }
        // Preform the move.
        History oldMoveData = pos.doMove(searchedMove);
        // Prevent castling while in check.
        if (checked && (searchedMove.moveFlag & CASTLE_OO || searchedMove.moveFlag & CASTLE_OOO)) {
            pos.undoMove(oldMoveData, searchedMove);
            continue;
        }
        // Prevent moves that leave us in check.
        if (inCheck(pos, oldMoveData.turn) && searchedMove.capturedPiece != KING) {
            pos.undoMove(oldMoveData, searchedMove);
            continue;
        }
        // Does this move give check.
        bool givesCheck = moveOrderChecks[depth + 150][index];
        // Increment the number of valid moves found.
        movesFound++;
        // Determine if the move is a capture move.
        bool isCapture = (searchedMove.moveFlag & CAPTURE) ||
                         (searchedMove.moveFlag & EP_CAPTURE) ||
                         (searchedMove.moveFlag & PROMOTION_CAPTURE);
        if (isCapture || (givesCheck && !lastMoveWasCheck)) { // Don't allow two consecutive checks.
            movesSearched++;
            // if (isCapture && seeCapture(pos, searchedMove) < 0) {
            //     // Prune bad captures.
            //     pos.undoMove(oldMoveData, searchedMove);
            //     continue;
            // }
            if (searchedMove.capturedPiece == KING) {
                currScore = MATE_SCORE - stack->ply; // We are mating by capturing a king.
            } else {
                currScore = -qsearch(pos, stack + 1, -beta, -alpha, depth - 1, givesCheck); // Regular QSearch function.
            }
            // Beta cutoff.
            if (currScore >= beta) {
                pos.undoMove(oldMoveData, searchedMove);
                return beta;
            }
            // Always update alpha.
            if (currScore > alpha) alpha = currScore;
        }
        // Undo the move.
        pos.undoMove(oldMoveData, searchedMove);
    }
    // Check for mate.
    if (movesFound == 0) {
        if (checked) {
            return -MATE_SCORE + stack->ply; // Position is checkmate, return mate score.
        } else {
            return 0; // Position is a stalemate, return 0.
        }
    }
    // Return alpha.
    return alpha;
}
void calculateDistanceToMate(int score) {
    bool isMateScore = MATE_THRESHOLD < score || -MATE_THRESHOLD > score;
    if (isMateScore) {
        std::cout << "mate ";
        if (MATE_THRESHOLD < score) std::cout << abs(score - MATE_SCORE);
        else std::cout << -(score + MATE_SCORE);
    } else std::cout << "cp " << score;
}
std::string getPvLine(SearchPV pvLine) {
    std::string pvLineStr = "";
    for (size_t i = 0; i < pvLine.pvLength; i++) {
        if (i != 0) pvLineStr += " ";
        pvLineStr += getUciMove(pvLine.pv[i]);
    }
    return pvLineStr;
}
uint64_t computeHash(Position pos) {
    uint64_t h = 0;
    for (int i = 4; i <= 223; i++) {
        int piece = indexOf(pos.colorMailbox[i], pos.pieceMailbox[i]);
        if (piece == -1) continue;
        h ^= transpositionRandTable[pos.curTurn][i][piece];
    }
    return h;
}
