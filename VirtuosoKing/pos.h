#pragma once
#pragma warning(disable : 26812)
#include <cstdint>
#include <string>
#include <vector>
#include "defs.h"
enum Piece : int {
    PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING
};
enum Color : int {
    RED = 1, BLUE, YELLOW, GREEN
};
enum MoveFlags : int {
    QUIET, BIG_PAWN, CAPTURE, EP_CAPTURE = 4,
    PROMOTION = 8, PROMOTION_CAPTURE = 16,
    CASTLE_OO = 32, CASTLE_OOO = 64,
    PRE_GEN_MOVE = 128,
};
inline bool isInvalidSquare(int square) {
    return square < 1 || BASE_MAILBOX[square] == -1 || square > 220;
}
inline int nextColor(int color) {
    return (color % 4) + 1;
}
inline int getRank(int square) {
    return square >> 4;
};
inline int getFile(int square) {
    return square & 15;
};
inline int qualifiesForPawnExtension(int square, int color) {
    switch (color) {
    case RED:
        return getRank(square) <= 7;
        break;
    case BLUE:
        return getFile(square) >= 7;
        break;
    case YELLOW:
        return getRank(square) >= 6;
        break;
    case GREEN:
        return getFile(square) <= 8;
        break;
    default:
        return false;
    }
}
inline bool isPromotionSquare(int square, int color) {
    switch (color) {
    case RED:
        return getRank(square) == 3;
        break;
    case BLUE:
        return getFile(square) == 11;
        break;
    case YELLOW:
        return getRank(square) == 10;
        break;
    case GREEN:
        return getFile(square) == 4;
        break;
    default:
        return false;
    }
};
inline bool isLightSquared(int square) {
    return (square % 2 == 0);
}
inline bool isPawnStartSquare(int square, int team) {
    return team == 1 ? (getRank(square) == 1 || getRank(square) == 12) :
                       (getFile(square) == 2 || getFile(square) == 13);
};
struct HistoryNullMove {
    int score; int turn;
    int epSquareR; int epSquareB;
    int epSquareY; int epSquareG;
};
struct Move {
    int from;
    int to;
    int promotion;
    int moveFlag;
    int moved;
    int capturedPiece;
    int capturedColor;
    int curPieceIndex;
};
struct SmallMove {
    int from;
    int to;
    int promotion;
    int flags;
};
struct History {
    int score;
    int turn;
    int king;
    int capturedColor;
    int epSquare;
    int epSquareOT;
    int epColor;
    bool lastCastleK = false;
    bool lastCastleQ = false;
};
struct Position {
public:
    int curTurn = 1;
    int posScore = 0;
    int kingTracker[5];
    bool castleTracker[2][5];
    int colorMailbox[224];
    int pieceMailbox[224];
    int enpassants[5];
    Position() {
        initBaseGlobalVar();
    }
    void setPosData(int nxtCol, int col[224], int pie[224], int ep[5],
        bool cstSc[2][5], int kt[5], int ps, bool setMaterialScore);
    History doMove(Move move);
    void undoMove(History historyOfPosition, Move move);
    HistoryNullMove doNullMove();
    void undoNullMove(HistoryNullMove nullHistory);
    bool inCheck(int color);
    bool isSquareAttacked(int square, int color);
    std::vector<Move> getAllMoves(int color);
    int scoreMove(Move move);
    void initBaseGlobalVar();
    void placePiece(int square, int piece, int color);
    void removePiece(int square);
    bool isOpponentsPiece(int square, int color);
    int getStaticEvalNew();
    int getStaticEval();
    int getMaterialScore();
};
SmallMove decodeMove(uint64_t encoded);
uint64_t encodeMove(const Move& move);
std::string getSanMove(Move move);
std::string getUciMove(Move move);
void printBoard(Position pos);