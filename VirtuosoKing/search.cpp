#include <atomic>
#include <iostream>
#include <mutex>
#include "tt.h"
#include "movepicker.h"
std::unique_ptr<TranspositionTable> transpositionTable;
static std::vector<ThreadMemory*> threadMemoryArray;
static uint64_t get64rand() {
    return (((uint64_t)rand() << 0) & 0x000000000000FFFFull) | (((uint64_t)rand() << 16) & 0x00000000FFFF0000ull) |
          (((uint64_t)rand() << 32) & 0x0000FFFF00000000ull) | (((uint64_t)rand() << 48) & 0xFFFF000000000000ull);
}
int numThreads;
uint64_t getNodes() {
    uint64_t total = 0;
    for (int i = 0; i < numThreads; i++) {
        total += threadMemoryArray[i]->searchStats.nodes;
    }
    return total;
}
int getSelectiveDepth() {
    int max = 0;
    for (int i = 0; i < numThreads; i++) {
        if (threadMemoryArray[i]->searchParams.selectiveDepth > max) {
            max = threadMemoryArray[i]->searchParams.selectiveDepth;
        }
    }
    return max;
}
void initThreadMemory() {
    threadMemoryArray.push_back(new ThreadMemory());
}
bool isPonderSearch = false;
void startPonder() {
    isPonderSearch = true;
}
void stopPonder() {
    isPonderSearch = false;
}
uint64_t transpositionRandTable[5][224][24];
void initTranspositionKeyHandler() {
    for (uint64_t i = 0; i < 5; ++i)
        for (uint64_t w = 0; w < 224; ++w)
            for (uint64_t x = 0; x < 24; ++x)
                transpositionRandTable[i][w][x] = get64rand();
    transpositionTable = std::make_unique<TranspositionTable>(TT_MAX_SIZE);
}
uint64_t getTimeElapsed(ChessTime startTime) {
    auto endTime = ChessClock::now();
    std::chrono::milliseconds timeSpan =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    return (uint64_t)timeSpan.count() + 1;
}
ChessTime startTime;
uint64_t timeLimit;
unsigned int multiPV;
std::atomic<bool> isStop(true);
std::atomic<bool> stopSignal(true);
void search(Position pos, TimeParams timeParams) {
    const int color = pos.curTurn;
    std::vector<Move> legalMoves = pos.getAllMoves(color);
    if (legalMoves.size() <= 0) {
        stopSignal = true;
        isStop = true;
        std::cout << "bestmove none" << std::endl;
        return;
    }
    for (int i = 0; i < numThreads; i++) {
        threadMemoryArray[i]->searchParams.reset();
        threadMemoryArray[i]->searchStats.reset();
        threadMemoryArray[i]->searchParams.selectiveDepth = 0;
    }
    timeLimit = (timeParams.searchMode == TIME) ? timeParams.maxAllotment
              : (timeParams.searchMode == MOVETIME) ? timeParams.allotment
              : MAX_TIME;
    startTime = ChessClock::now();
    if (numThreads > 1) {
        std::thread* threadPool = new std::thread[numThreads];
        for (int i = 0; i < numThreads; i++) {
            threadPool[i] = std::thread(searchRoot, pos, legalMoves, timeParams, i);
        }
        for (int i = 0; i < numThreads; i++) {
            threadPool[i].join();
        }
        stopSignal = false;
        delete[] threadPool;
    } else {
        searchRoot(pos, legalMoves, timeParams, 0);
    }
}
void searchRoot(Position pos, std::vector<Move> legalMoves, TimeParams timeParams, int threadID) {
    Move ponder{};
    Move bestMove{};
    uint64_t timeSoFar;
    int bestScore = -INFTY, bestMoveIndex = -1;
    int rootDepth = 1;
    Move prevBest{};
    int prevScore = -INFTY;
    double timeChangeFactor = 1.0;
    do {
        SearchPV pvLine;
        int aspValg   = bestScore,
            aspAlpha = bestScore - ASPIRATION_MARGIN,
            aspBeta  = bestScore + ASPIRATION_MARGIN;
        if (rootDepth == 1) {
            aspAlpha = -MATE_SCORE;
            aspBeta = MATE_SCORE;
        }
        threadMemoryArray[threadID]->searchParams.reset();
        pvLine.pvLength = 0;
        searchRootPVS(pos, legalMoves, rootDepth, -MATE_SCORE,
            MATE_SCORE, &bestMoveIndex, &bestScore, threadID, &pvLine);
        if (pvLine.pvLength > 1)
            ponder = pvLine.pv[1];
        else if (bestMoveIndex != 0)
            ponder = {};
        prevScore = bestScore;
        timeSoFar = getTimeElapsed(startTime);
        uint64_t nps = 1000 * getNodes() / timeSoFar;
        if (bestMoveIndex == -1) {
            if (threadID == 0) {
                std::cout << "info depth " << rootDepth - 1;
                std::cout << " seldepth " << getSelectiveDepth();
                std::cout << " time " << timeSoFar
                          << " nodes " << getNodes() << " nps " << nps
                          << std::endl;
            }
            break;
        }
        bestMove = legalMoves.at(bestMoveIndex);
        if (threadID == 0) {
            std::cout << "info depth " << rootDepth;
            std::cout << " seldepth " << getSelectiveDepth();
            std::cout << " score";
            std::cout << " cp " << bestScore;
            std::cout << " time " << timeSoFar
                      << " nodes " << getNodes() << " nps " << nps
                      << " pv " << getPvLine(&pvLine) << std::endl;
            if (bestScore > NEAR_MATE_SCORE || bestScore < -NEAR_MATE_SCORE) {
                break;
            }
        }
        rootDepth++;
    } while (!isStop && (isPonderSearch || (threadID != 0 && rootDepth <= MAX_SEARCH_DEPTH)
            || ((timeParams.searchMode == MOVETIME && timeSoFar < (uint64_t)timeParams.allotment && rootDepth <= MAX_SEARCH_DEPTH)
            || (timeParams.searchMode == DEPTH && rootDepth <= timeParams.allotment))));
    while (isPonderSearch && !isStop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (threadID == 0) {
        stopSignal = true;
        isStop = true;
        if (getUciMove(ponder) == "????") {
            std::cout << "bestmove " << getSanMove(bestMove) << std::endl;
        } else {
            std::cout << "bestmove " << getSanMove(bestMove) << " ponder " << getSanMove(ponder) << std::endl;
        }
    }
}
void searchRootPVS(Position pos, std::vector<Move> legalMoves, Depth depth, int alpha,
    int beta, int* bestMoveIndex, int* bestScore, int threadID, SearchPV* pvLine) {
    SearchStatistics* searchStats = &(threadMemoryArray[threadID]->searchStats);
    SearchParameters* searchParams = &(threadMemoryArray[threadID]->searchParams);
    SearchStack* ssi = &(threadMemoryArray[threadID]->ssInfo[0]);
    SearchPV line;
    const int color = pos.curTurn;
    int tempMove = -1;
    int score = -MATE_SCORE;
    *bestScore = -INFTY;
    std::this_thread::yield();
    bool inCheck = pos.inCheck(color);
    int legalMovesSearched = -1, index = -1;
    MovePicker movePicker(legalMoves, "", ssi->ply);
    while (movePicker.hasNext()) {
        Move move = movePicker.getNext(pos);
        History oldMoveData = pos.doMove(move);
        if (inCheck && (move.moveFlag & CASTLE_OO || move.moveFlag & CASTLE_OOO)) {
            pos.undoMove(oldMoveData, move);
            continue;
        }
        if (pos.inCheck(oldMoveData.turn) && move.capturedPiece != KING) {
            pos.undoMove(oldMoveData, move);
            continue;
        }
        legalMovesSearched++;
        index = move.curPieceIndex;
        //// uint64_t timeSoFar = getTimeElapsed(startTime);
        //// uint64_t nps = 1000 * getNodes() / timeSoFar;
        //// if (threadID == 0 && timeSoFar > 5 * ONE_SECOND) {
        ////     std::cout << "info depth " << depth
        ////               << " currmove " << getUciMove(move)
        ////               << " currmovenumber " << legalMovesSearched + 1
        ////               << " nodes " << getNodes()
        ////               << " nps " << nps << std::endl;
        //// }
        searchStats->nodes++;
        int extension, reduction = 0;
        bool givesCheck = pos.inCheck(OPPOSITES[color][0]) || pos.inCheck(OPPOSITES[color][0]);
        extension = 0;
        Depth newDepth = depth - ONE_PLY + extension;
        if (move.capturedPiece == KING) score = MATE_SCORE - ssi->ply;
        score = -pvs<PV_NODE>(pos, newDepth, -beta, -alpha, threadID, false, givesCheck, ssi + 1, &line, extension, move);
        pos.undoMove(oldMoveData, move);
        if (stopSignal.load(std::memory_order_seq_cst)) break;
        if (score > *bestScore) {
            *bestScore = score;
            if (score > alpha) {
                alpha = score;
                tempMove = (int)index;
                if (pvLine != nullptr) changePV(move, pvLine, &line);
            } else if (pvLine != nullptr && legalMovesSearched == 0) changePV(move, pvLine, &line);
        }
        if (score >= beta) break;
    }
    *bestMoveIndex = tempMove;
}
template<nodeType curNode>
int pvs(Position pos, Depth depth, int alpha, int beta, int threadID, bool isCutNode,
    bool gaveCheck, SearchStack* ssi, SearchPV* pvLine, int extended, Move lastMovePlayed) {
    SearchParameters* searchParams = &(threadMemoryArray[threadID]->searchParams);
    SearchStatistics* searchStats = &(threadMemoryArray[threadID]->searchStats);
    constexpr bool isPvNode = curNode != ALL_NODE;
    pvLine->pvLength = 0;
    if (depth <= 0 || ssi->ply >= MAX_SEARCH_DEPTH) {
        if (ssi->ply > searchParams->selectiveDepth) {
            searchParams->selectiveDepth = ssi->ply;
        }
        searchParams->ply = ssi->ply;
        return quiescence(pos, ssi->ply, alpha, beta, threadID);
    }
    int matingScore = MATE_SCORE - ssi->ply;
    if (matingScore < beta) {
        beta = matingScore;
        if (alpha >= matingScore) {
            return alpha;
        }
    }
    int matedScore = -MATE_SCORE + ssi->ply;
    if (matedScore > alpha) {
        alpha = matedScore;
        if (beta <= matedScore) {
            return beta;
        }
    }
    if (threadID == 0 && (searchStats->nodes & 1023) == 1023 && !isPonderSearch) {
        uint64_t timeSoFar = getTimeElapsed(startTime);
        if (timeSoFar > timeLimit) {
            isStop = true;
            stopSignal = true;
        }
    }
    if (stopSignal.load(std::memory_order_relaxed)) {
        return 0;
    }
    const int color = pos.curTurn;
    // searchParams->killers[ssi->ply + 1][0] = "";
    // searchParams->killers[ssi->ply + 1][1] = "";
    SearchPV line;
    std::string ttMove{};
    uint64_t posKey = computeHash(pos);
    int hashScore = -INFTY;
    Depth hashDepth = DEPTH_ZERO;
    bool ttHit = false;
    const HashTableEntry* tte = nullptr;
    tte = transpositionTable->get(posKey);
    if (tte != nullptr && tte->key == posKey) {
        if (tte->depth >= depth) {
            if (!isPvNode && (tte->bound == EXACT || (tte->bound == LOWER_BOUND
                && tte->score >= beta) || (tte->bound == UPPER_BOUND && tte->score <= alpha))) {
                int returnScore = beta < (alpha > tte->score ? alpha : tte->score) ? beta : (alpha > tte->score ? alpha : tte->score);
                return returnScore;
            }
        }
        ttMove = tte->move;
        ssi->ttPv = isPvNode || tte->isPv;
    }
    std::string toHash = "";
    int extension;
    int bestScore = -INFTY;
    int score = -INFTY;
    int legalMovesSearched = 0, quiets = 0;
    std::vector<Move> legalMoves = pos.getAllMoves(color);
    MovePicker movePicker(legalMoves, "", ssi->ply);
    while (movePicker.hasNext()) {
        Move move = movePicker.getNext(pos);
        History oldMoveData = pos.doMove(move);
        if (gaveCheck && (move.moveFlag & CASTLE_OO || move.moveFlag & CASTLE_OOO)) {
            pos.undoMove(oldMoveData, move);
            continue;
        }
        if (pos.inCheck(oldMoveData.turn) && move.capturedPiece != KING) {
            pos.undoMove(oldMoveData, move);
            continue;
        }
        ssi->moveCount++;
        legalMovesSearched++;
        bool givesCheck = pos.inCheck(OPPOSITES[color][0]) || pos.inCheck(OPPOSITES[color][0]);
        extension = 0;
        Depth newDepth = depth - ONE_PLY + extension;
        if (move.capturedPiece == KING) score = MATE_SCORE - ssi->ply;
        else score = -pvs<PV_NODE>(pos, newDepth, -beta, -alpha, threadID, false, givesCheck, ssi + 1, &line, extension, move);
        pos.undoMove(oldMoveData, move);
        if (stopSignal.load(std::memory_order_relaxed)) return 0;
        if (score > bestScore) {
            bestScore = score;
            toHash = getUciMove(move);
            if (score > alpha) {
                changePV(move, pvLine, &line);
                if (score >= beta) break;
                else alpha = score;
            }
        }
    }
    if (legalMovesSearched == 0) {
        return scoreMate(gaveCheck, ssi->ply);
    }
    ScoreBound bound = beta <= alpha ? LOWER_BOUND : ssi->ttPv && toHash != "" ? EXACT : UPPER_BOUND;
    transpositionTable->save(posKey, depth, toHash, score, bound, ssi->ttPv);
    return bestScore;
}
int quiescence(Position pos, int plies, int alpha, int beta, int threadID) {
    SearchParameters* searchParams = &(threadMemoryArray[threadID]->searchParams);
    SearchStatistics* searchStats = &(threadMemoryArray[threadID]->searchStats);
    int color = pos.curTurn;
    if (stopSignal.load(std::memory_order_relaxed)) return 0;
    int eval = pos.getStaticEval();
    if (eval >= beta) return eval;
    if (alpha < eval) alpha = eval;
    int bestScore = eval, legalMovesSearched = 0, score = 0, specials = 0;
    bool inCheck = pos.inCheck(color);
    std::vector<Move> legalMoves = pos.getAllMoves(color);
    MovePicker movePicker(legalMoves, "", plies);
    while (movePicker.hasNext()) {
        Move move = movePicker.getNext(pos);
        History oldMoveData = pos.doMove(move);
        if (inCheck && (move.moveFlag & CASTLE_OO || move.moveFlag & CASTLE_OOO)) {
            pos.undoMove(oldMoveData, move);
            continue;
        }
        if (pos.inCheck(oldMoveData.turn) && move.capturedPiece != KING) {
            pos.undoMove(oldMoveData, move);
            continue;
        }
        legalMovesSearched++;
        searchStats->nodes++;
        if (move.moveFlag & CAPTURE || move.moveFlag & EP_CAPTURE) {
            if (move.capturedPiece == KING) score = MATE_SCORE - plies;
            else score = -quiescence(pos, plies + 1, -beta, -alpha, threadID);
            if (score > bestScore) {
                bestScore = score;
                if (score > alpha) alpha = score;
            }
        }
        pos.undoMove(oldMoveData, move);
    }
    if (legalMovesSearched == 0) {
        return scoreMate(pos.inCheck(color), plies);
    }
    return bestScore;
}
std::string getPvLine(SearchPV* pvLine) {
    std::string pvLineStr = "";
    for (size_t i = 0; i < pvLine->pvLength; i++) {
        if (i != 0) pvLineStr += "|";
        pvLineStr += getUciMove(pvLine->pv[i]);
    }
    return pvLineStr;
}
void setNumThreads(int n) {
    numThreads = n;
    while ((int)threadMemoryArray.size() < n) {
        threadMemoryArray.push_back(new ThreadMemory());
    }
    while ((int)threadMemoryArray.size() > n) {
        delete threadMemoryArray.back();
        threadMemoryArray.pop_back();
    }
}
int scoreMate(bool isInCheck, int plies) {
    if (isInCheck) return (-MATE_SCORE + plies);
    return 0;
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